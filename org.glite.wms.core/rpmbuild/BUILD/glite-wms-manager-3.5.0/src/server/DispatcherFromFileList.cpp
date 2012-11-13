// File: DispatcherFromFileList.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include "DispatcherFromFileList.h"
#include <algorithm>
#include <cctype>
#include <ctime>
#include <cstdio>
#include <boost/thread/xtime.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include "signal_handling.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wmsutils/exception/Exception.h"
#include "glite/wms/common/utilities/wm_commands.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/common/utilities/scope_guard.h"
#include "TaskQueue.hpp"
#include "Request.hpp"
#include "glite/security/proxyrenewal/renewal.h"
#include "glite/wms/purger/purger.h"
#include "glite/lb/producer.h"
#include "lb_utils.h"
#include "submission_utils.h"
#include "filelist_utils.h"
#include "filelist_recovery.h"

namespace task = glite::wms::common::task;
namespace configuration = glite::wms::common::configuration;
namespace utilities = glite::wms::common::utilities;
namespace jobid = glite::wmsutils::jobid;
namespace ca = glite::wmsutils::classads;

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {

bool
is_recovery_enabled()
{
#ifdef GLITE_WMS_HAVE_LBPROXY
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );

  return config.wm()->enable_recovery();
#else
  return false;
#endif
}

bool older_than(RequestPtr const& req, std::time_t threshold)
{
  return req->last_processed() < threshold;
}

class UnregisterProxy
{
  std::string m_id;
public:
  UnregisterProxy(jobid::JobId const& id)
    : m_id(id.toString())
  {
  }
  void operator()() const
  {
    glite_renewal_UnregisterProxy(m_id.c_str(), "");
  }
};

void do_transitions_for_cancel(
  RequestPtr const& req,
  std::time_t current_time,
  pipe_type::write_end_type& write_end
)
{
  std::time_t threshold = current_time - 300; // 5 minutes
  Request::State state = req->state();

  if (req->jdl() || req->marked_resubmitted() || older_than(req, threshold)) {
    // new cancel or old one
    // manage immediately, if applicable

    Info("considering cancel of " << req->id());

    switch (state) {
    case Request::WAITING:
    case Request::RECOVERABLE:
      req->clear_jdl();
      req->state(Request::CANCELLED, "cancelled by user");
      req->add_cleanup(UnregisterProxy(req->id()));
      req->add_cleanup(
        boost::bind(purger::purgeStorage, req->id(), std::string())
      );
      break;
    case Request::DELIVERED:
      req->clear_jdl();         // this, together with marked_cancelled(),
                                // tells the RequestHandler that it should
                                // process the cancel request
      req->state(Request::READY);
      write_end.write(req);
      break;
    case Request::READY:
    case Request::PROCESSING:
      // do nothing; these are unstable states, subject to race conditions
      // wait for a stable one; in particular don't clear the jdl because
      // it may be in use (or shortly be) by the RequestHandler
      req->clear_jdl();         // this, together with marked_cancelled(),
                                // tells the RequestHandler that it should
                                // process the cancel request
      req->marked_cancelled();
      req->state(Request::READY);
      write_end.write(req);
      break;
    case Request::UNRECOVERABLE:
      // don't change state; this will cause the job to be aborted rather than
      // cancelled, so the user knows that there is something wrong with this
      // job
      req->add_cleanup(UnregisterProxy(req->id()));
      req->add_cleanup(
        boost::bind(purger::purgeStorage, req->id(), std::string())
      );
      break;
    case Request::CANCELLED:
      // do nothing; the job has already been cancelled
      break;
    case Request::CANCEL_DELIVERED:
      break;
    }
  }
}

int get_match_retry_period()
{
  return configuration::Configuration::instance()->wm()->match_retry_period();
}

void do_transitions_for_submit(
  RequestPtr const& req,
  std::time_t current_time,
  pipe_type::write_end_type& write_end
)
{
  std::time_t threshold = current_time - get_match_retry_period();
  Request::State state = req->state();

  switch (state) {

  case Request::WAITING:

    if (older_than(req, threshold)) {

      Info("considering (re)submit of " << req->id());

      bool proxy_expired = true;
      try {
        proxy_expired = is_proxy_expired(req->proxy(), req->id());
      } catch (MissingProxy&) {
        req->state(Request::UNRECOVERABLE, "X509 proxy not found or I/O error");
        break;
      } catch (InvalidProxy&) {
        req->state(Request::UNRECOVERABLE, "invalid X509 proxy");
        break;
      }
      if (proxy_expired) {
        req->state(Request::UNRECOVERABLE, "proxy expired");
        break;
      }
      req->state(Request::READY);
      write_end.write(req);
    }
    break;

  case Request::RECOVERABLE:

    log_pending(req->lb_context(), req->message());
    Info("postponing " << req->id() << " (" << req->message() << ')');
    req->state(Request::WAITING);
    break;

  case Request::DELIVERED:
    // keep this informational message here because in ~Request it may be
    // "overriden" by the CANCELLED state, i.e. a DELIVERED request may
    // become later a CANCELLED one and be destroyed as such

    Info(req->id() << " delivered");
    break;

  case Request::UNRECOVERABLE:
    Info(req->id() << " failed (" << req->message() << ')');
    break;

  case Request::READY:
  case Request::PROCESSING:
    // do nothing; the job is in the hands of the request handler, wait for
    // the job to enter a stable state
    break;

  case Request::CANCELLED:
    Info(req->id() << " cancelled");
    break;

  }
}

void do_transitions(
  TaskQueue const& tq,
  pipe_type::write_end_type& write_end
)
{
  std::time_t current_time = std::time(0);

  TaskQueue::const_iterator const tq_end = tq.end();
  for (TaskQueue::const_iterator it = tq.begin(); it != tq_end; ++it) {
    RequestPtr req(it->second);

    assert(
      !req->marked_match() && "match requests shouldn't appear in the TQ"
    );
    if (req->marked_cancelled()) {
      do_transitions_for_cancel(req, current_time, write_end);
    } else {
      do_transitions_for_submit(req, current_time, write_end);
    }
  }
}

bool is_done(std::pair<std::string, RequestPtr> const& id_req)
{
  RequestPtr const& req = id_req.second;
  return
    req->state() == Request::DELIVERED && !req->marked_cancelled()
    || req->state() == Request::UNRECOVERABLE
    || req->state() == Request::CANCELLED
    || req->state() == Request::CANCEL_DELIVERED;
}

void remove_done(TaskQueue& tq)
{
  TaskQueue::iterator cur = tq.begin();
  TaskQueue::iterator const last = tq.end();
  while ((cur = std::find_if(cur, last, is_done)) != last) {
    TaskQueue::iterator tmp = cur++;
    tq.erase(tmp);
  }
}

class is_in_state
{
  RequestPtr m_req;
  int m_states;
public:
  is_in_state(RequestPtr const& req, int states)
    : m_req(req), m_states(states)
  {
  }
  bool operator()() const
  {
    return m_req->state() & m_states;
  }
};

bool get_check_status()
{
#ifdef GLITE_WMS_HAVE_LBPROXY
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.wm()->enable_status_check();
#else
  return false;
#endif
}

void
get_new_requests(
  ExtractorPtr extractor,
  std::string const& input,
  TaskQueue& tq,
  pipe_type::write_end_type& write_end
)
{
  requests_type new_requests(extractor->get_all_available());

  requests_type::iterator it = new_requests.begin();
  requests_type::iterator const end = new_requests.end();
  for ( ; it != end; ++it) {

    extractor_type::iterator request_it = *it;
    boost::function<void()> cleanup(FLCleanUp(extractor, request_it));
    // if the request is not valid, clean it up automatically
    utilities::scope_guard cleanup_guard(cleanup);

    std::string const command_ad_str = *request_it;

    try {

      classad::ClassAd command_ad;
      classad::ClassAdParser parser;
      if (!parser.ParseClassAd(command_ad_str, command_ad)) {
        Info("Invalid request: " << command_ad_str);
        continue;
      }

      std::string command;
      jobid::JobId id;
      std::string sequence_code;
      std::string x509_proxy;
      if (utilities::command_is_valid(command_ad)) {
        boost::tie(command, id, sequence_code, x509_proxy)
          = parse_request(command_ad);
      } else {
        Info("Invalid command: "
          << command_ad_str <<
          " (doesn't match requirements...)"
        );
        continue;
      }

      bool const status_check_is_enabled = get_check_status();

      JobStatusPtr status;

      if (status_check_is_enabled) {
        status = job_status(id);
      }

      // TODO
      // for a match, since it doesn'to go anymore to the TQ, probably it does
      // not make sense to get its status and look for it in the TQ

      TaskQueue::iterator it = tq.find(id.toString());

      if (it == tq.end()) {

        if (command == "jobsubmit" || command == "jobresubmit") {

          bool is_acceptable = true;
          if (status_check_is_enabled) {
            is_acceptable = is_waiting(status) || !status;
          }

          if (is_acceptable) {

            Info("new " << command << " for " << id);

            cleanup_guard.dismiss();
            RequestPtr request(new Request(command_ad, command, id, cleanup));

            log_dequeued(request->lb_context(), input);

            if (command == "jobsubmit") {
              bool proxy_expired = true;
              try {
                proxy_expired = is_proxy_expired(request->proxy(), id);
              } catch(MissingProxy&) {
                request->state(
                  Request::UNRECOVERABLE,
                  "X509 proxy not found or I/O error"
                );
                Info(request->id() << " failed (" << request->message() << ')');
                continue;
              } catch(InvalidProxy&) {
                request->state(
                  Request::UNRECOVERABLE,
                  "invalid X509 proxy"
                );
                Info(request->id() << " failed (" << request->message() << ')');
                continue;
              }
              if (proxy_expired) {
                request->state(Request::UNRECOVERABLE, "proxy expired");
                Info(request->id() << " failed (" << request->message() << ')');
              } else {
                request->state(Request::READY);
                write_end.write(request);
                tq.insert(std::make_pair(id.toString(), request));
              }
            } else { // jobresubmit
              // can it be forwarded immediately to the RH?
              tq.insert(std::make_pair(id.toString(), request));
            }
          } else {
            Info(
              "ignoring " << command << " for " << id
              << " because not compatible with state "
              << status_to_string(status)
            );
          }
        } else if (command == "jobcancel") {

          bool is_acceptable = true;
          if (status_check_is_enabled) {
            is_acceptable = !is_cancelled(status);
          }

          if (is_acceptable) {

            Info("new jobcancel for " << id);

            cleanup_guard.dismiss();
            RequestPtr request(new Request(command_ad, command, id, cleanup));
            // TODO can the request be written immediately into the pipe?
            tq.insert(std::make_pair(id.toString(), request));

            log_cancel_req(request->lb_context());

          } else {

            Info(
              "ignoring jobcancel for " << id
              << " because not compatible with state "
              << status_to_string(status)
            );

          }

        } else if (command == "match") {

          cleanup_guard.dismiss();
          RequestPtr request(new Request(command_ad, command, id, cleanup));
          // the request is processed immediately
          // the request is not inserted in the TQ because in case of
          // failure it won't be retried
          Info("considering match of " << request->id());
          request->state(Request::READY);
          write_end.write(request);

        } else {

          Info("don't know what to do with " << command << " for " << id);

        }

      } else { //request found in tq

        Request& existing_request(*it->second);

        if (command == "jobsubmit") {

          if (existing_request.marked_match()) {
            // abort the new submit
            std::string const message("already existing match request");
            Info(message << ' ' << id);
            // create a fake request to perform the proper actions
            Request r(command_ad, command, id, cleanup);
            cleanup_guard.dismiss();
            r.state(Request::UNRECOVERABLE, message);
          } else {
            // ignore
            Info("ignoring submit " << id);
          }

        } else if (command == "jobresubmit") {

          if (existing_request.marked_match()) {
            // ignore the resubmit
            Info("ignoring resubmit, already existing match request " << id);
          } else {
            // during normal running a resubmit can be seen at this point only
            // if the status of this job is either DELIVERED, PROCESSING (this
            // case is possible because the submit request can be passed to
            // the JC, fail and be resubmitted by the LM before the
            // RequestHandler thread could mark it as DELIVERED) or CANCELLED
            // (this case is possible because the cancel request can be
            // processed - and passed to the JC, moving the request to the
            // CANCELLED state - while the job failed and was resubmitted by
            // the LM)

            // but during start up, w/o the appropriate checks on the status
            // of a job, still to be done) also an initial state
            // (i.e. WAITING) is possible because all the requests are read in
            // one shot from the input

            Debug(
              "jobresubmit " << id << " when in state " << existing_request.state()
            );
            log_dequeued(existing_request.lb_context(), input);
            existing_request.mark_resubmitted();
            existing_request.add_cleanup(cleanup);
            cleanup_guard.dismiss();
          }

        } else if (command == "jobcancel") {

          if (existing_request.marked_match()) {
            // ignore the cancel
            Info("ignoring cancel, already existing match request " << id);
          } else {
            ContextPtr cancel_context(
              create_context(id, x509_proxy, sequence_code)
            );
            log_cancel_req(cancel_context);
            existing_request.mark_cancelled(cancel_context);
            existing_request.add_cleanup(cleanup);
            cleanup_guard.dismiss();
          }

        } else if (command == "match") {

          // ignore the match, independently if the existing request with the
          // same id is a (re)submit, cancel or match
          Info("ignoring match " << id);

        }

      }

    } catch (ca::ClassAdError& e) {
      Info(e.what() << " for " << command_ad_str);
    } catch (CannotCreateLBContext& e) {
      Info("Cannot create LB context (error code = " << e.error_code() << ')');
    } catch (InvalidRequest& e) {
      Info("Invalid request: " << command_ad_str);
    }
  }
}

} // {anonymous}

struct DispatcherFromFileList::Impl
{
  Impl(std::string const& input, TaskQueue& tq)
    : m_input(input), m_tq(tq)
  {}
  std::string m_input;
  TaskQueue& m_tq;
};

DispatcherFromFileList::DispatcherFromFileList(
  std::string const& input,
  TaskQueue& tq
)
  : m_impl(new Impl(input, tq))
{
}

void DispatcherFromFileList::operator()()
try {

  Info("Dispatcher: starting");

  std::string const& input_file(m_impl->m_input);
  TaskQueue& tq(m_impl->m_tq);

  ExtractorPtr extractor(new extractor_type(input_file));

  if (extractor) {
    Info("Dispatcher: reading from " << input_file);
  } else {
    Error("Dispatcher: cannot read from " << input_file << ". Exiting...");
    return;
  }

  if (is_recovery_enabled()) {
    Info("Dispatcher: doing recovery");
    recovery(extractor, tq);
  } else {
    Info("Dispatcher: recovery disabled");
  }

  while (!received_quit_signal()) {

    do_transitions(tq, write_end());
    remove_done(tq);
    get_new_requests(extractor, input_file, tq, write_end());

    unsigned int const one_second = 1;
    ::sleep(one_second);

  }

  Info("Dispatcher: exiting");

} catch (task::SigPipe&) {
  Info("Dispatcher: no RequestHandler listening. Exiting...");
} catch (LB_Unavailable&) {
  Info("Dispatcher: LB unavailable. Exiting...");
} catch (std::exception const& e) {
  Error("Dispatcher: " << e.what() << ". Exiting...");
} catch (...) {
  Error("Dispatcher: caught unknown exception. Exiting...");
}

}}}} // glite::wms::manager::server
