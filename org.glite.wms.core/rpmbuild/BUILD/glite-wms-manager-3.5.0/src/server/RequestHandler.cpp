// File: RequestHandler.cpp
// Author: Francesco Giacomini
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include "RequestHandler.h"
#include <string>
#include <ctime>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/bind.hpp>
#include <classad_distribution.h>
#include "WMReal.h"
#include "signal_handling.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"
#include "glite/wmsutils/exception/Exception.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/helper/exceptions.h"
#include "glite/lb/producer.h"
#include "Request.hpp"
#include "TaskQueue.hpp"
#include "listmatch.h"
#include "submission_utils.h"
#include "glite/lb/producer.h"

namespace fs = boost::filesystem;
namespace jobid = glite::wmsutils::jobid;
namespace task = glite::wms::common::task;
namespace exception = glite::wmsutils::exception;
namespace jdl = glite::jdl;

namespace glite {
namespace wms {
namespace manager {
namespace server {

struct RequestHandler::Impl
{
  WMReal m_wm;
};

RequestHandler::RequestHandler()
  : m_impl(new Impl)
{
}

namespace {

// returns (retry_count, shallow_count)
boost::tuple<int, int> retrieve_lb_info(RequestPtr req)
{
  // this can currently happen only in case of a resubmission
  // for a resubmission we need a few information stored in the LB:
  // - previous deep and shallow resubmissions
  // - previous matches
  // - original jdl

  if (!req->marked_resubmitted()) {
    return boost::make_tuple(0, 0);
  }

  ContextPtr context = req->lb_context();
  jobid::JobId const& jobid = req->id();

#ifndef GLITE_WMS_HAVE_LBPROXY
  // flush the lb events since we'll query the lb server
  if (!flush_lb_events(context)) {
    Warning(
      "edg_wll_LogFlush failed for " << jobid
      << " (" << get_lb_message(context) << ")"
    );
  }
#endif

  LB_Events events(get_interesting_events(context, jobid));
  if (events.empty()) {
    Warning("Cannot retrieve interesting events for " << jobid);
  }

  typedef std::vector<std::pair<std::string,int> > matches_type;
  matches_type const previous_matches = get_previous_matches(events);

  // in principle this warning should happen only if the req is a resubmit
  if (previous_matches.empty()) {
    Warning("cannot retrieve previous matches for " << jobid);
  }

  std::vector<std::string> previous_matches_simple;
  for (matches_type::const_iterator it = previous_matches.begin();
       it != previous_matches.end(); ++it) {
    previous_matches_simple.push_back(it->first);
  }

  // retrieve original jdl
  std::string const job_ad_str(get_original_jdl(context, jobid));
  if (job_ad_str.empty()) {
    throw CannotRetrieveJDL();
  }
  std::auto_ptr<classad::ClassAd> job_ad(glite::wmsutils::classads::parse_classad(job_ad_str));

  jdl::set_edg_previous_matches(*job_ad, previous_matches_simple);
  jdl::set_edg_previous_matches_ex(*job_ad, previous_matches);

  req->jdl(job_ad);

  return get_retry_counts(events);
}

bool is_request_expired(RequestPtr req)
{
  return req->expiry_time() < std::time(0);
}

void process_match(RequestPtr req)
{
  std::string filename;
  int number_of_results;
  bool include_brokerinfo;
  boost::tie(filename, number_of_results, include_brokerinfo)
    = req->match_parameters();

  Debug(
    "considering match " << req->id()
    << ' ' << filename
    << ' ' << number_of_results
    << ' ' << include_brokerinfo
  );

  if (!match(*req->jdl(), filename, number_of_results, include_brokerinfo)) {
    Info("Failed match for " << req->id());
  }

  req->state(Request::DELIVERED);
}

bool shallow_resubmission_is_enabled(RequestPtr req)
{
  return server::shallow_resubmission_is_enabled(*req->jdl());
}

void check_deep_count(RequestPtr req, int current_count)
{
  server::check_deep_count(*req->jdl(), current_count);
}

void check_shallow_count(RequestPtr req, int current_count)
{
  server::check_shallow_count(*req->jdl(), current_count);
}

fs::path get_reallyrunning_token(RequestPtr req)
{
  return server::get_reallyrunning_token(req->id());
}

void process_submit(RequestPtr req, WMReal& wm)
{
  Debug("considering (re)submit of " << req->id());

#warning race condition to be addressed
  // the race condition is about a resubmit for an already cancelled
  // job, probably we can check this in retrieve_lb_info(), retrieving
  // also cancel events and ignoring this request if there is one

  // retrieve from the LB possibly missing information needed to
  // process the request, e.g. the jdl, the previous matches, etc.

  try {

    int deep_count;
    int shallow_count;
    boost::tie(deep_count, shallow_count) = retrieve_lb_info(req);

    if (is_request_expired(req)) {

      req->state(Request::UNRECOVERABLE, "request expired");

    } else {

      if (shallow_resubmission_is_enabled(req)) {

        fs::path const token_file(get_reallyrunning_token(req));

        if (req->marked_resubmitted()) {
          bool exists = !unlink(token_file.string().c_str()); 
          if (exists) {
            check_shallow_count(req, shallow_count);
            log_resubmission_shallow(
              req->lb_context(),
              token_file.native_file_string()
            );
          } else {
            check_deep_count(req, deep_count);
            log_resubmission_deep(
              req->lb_context(),
              token_file.native_file_string()
            );
          }
        }

        create_token(token_file);

      } else {

        if (req->marked_resubmitted()) {
          check_deep_count(req, deep_count);
          log_resubmission_deep(req->lb_context());
        }

      }

      wm.submit(*req->jdl(), req->lb_context());
      req->state(Request::DELIVERED);
    }

  } catch (HitMaxRetryCount& e) {
    std::ostringstream os;
    os << "hit max retry count (" << e.count() << ')';
    req->state(Request::UNRECOVERABLE, os.str());
  } catch (HitJobRetryCount& e) {
    std::ostringstream os;
    os << "hit job retry count (" << e.count() << ')';
    req->state(Request::UNRECOVERABLE, os.str());
  } catch (HitMaxShallowCount& e) {
    std::ostringstream os;
    os << "hit max shallow retry count (" << e.count() << ')';
    req->state(Request::UNRECOVERABLE, os.str());
  } catch (HitJobShallowCount& e) {
    std::ostringstream os;
    os << "hit job shallow retry count (" << e.count() << ')';
    req->state(Request::UNRECOVERABLE, os.str());
  } catch (CannotRetrieveJDL& e) {
    req->state(Request::RECOVERABLE, "cannot retrieve jdl");
  } catch (helper::HelperError const& e) {
    req->state(Request::RECOVERABLE, e.what());
  }
}

void process_cancel(RequestPtr req, WMReal& wm)
{
  Debug("considering cancel of " << req->id());

  wm.cancel(req->id(), req->lb_context());
  req->state(Request::CANCEL_DELIVERED);
}

}

void
RequestHandler::operator()()
try {

  Info("RequestHandler: starting");

  WMReal& wm = m_impl->m_wm;

  while (!received_quit_signal()) {

    RequestPtr req(read_end().read());
    req->last_processed(std::time(0));

    try {

      if (req->jdl()) {

        // submit or match
        if (req->marked_cancelled()) {
          // shortcut submission and cancellation
          log_cancelled(req->cancel_context());

          // see transition management in the dispatcher
          req->state(Request::RECOVERABLE);
        } else {
          req->state(Request::PROCESSING);
          if (req->marked_match()) {
            process_match(req);
          } else {
            process_submit(req, wm);
          }
        }

      } else if (req->marked_resubmitted()) {

        // resubmit
        if (req->marked_cancelled()) {
          // shortcut resubmission and cancellation
          log_cancelled(req->cancel_context());

          // see the transition management in the dispatcher
          req->state(Request::RECOVERABLE);
        } else {
          req->state(Request::PROCESSING);
          process_submit(req, wm);
        }

      } else if (req->marked_cancelled()) {

        // normal cancel
        req->state(Request::PROCESSING);
        process_cancel(req, wm);

      } else {

        // what's this?
        req->state(Request::UNRECOVERABLE, "internal management error");

      }

    } catch (std::invalid_argument const& e) {
      req->state(Request::UNRECOVERABLE, e.what());
    } catch (jdl::ManipulationException const& e) {
      req->state(Request::UNRECOVERABLE, e.what());
    } catch (exception::Exception const& e) {
      req->state(Request::UNRECOVERABLE, e.what());
    }

  }

  Info("RequestHandler: exiting");

} catch (task::Eof&) {
  Info("RequestHandler: End of input. Exiting...");
} catch (std::exception const& e) {
  Error("RequestHandler: " << e.what() << ". Exiting...");
} catch (...) {
  Error("RequestHandler: caught unknown exception. Exiting...");
}

}}}} // glite::wms::manager::server
