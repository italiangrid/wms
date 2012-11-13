// File: dispatcher.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#include "dispatcher.h"
#include <algorithm>
#include <cctype>
#include <ctime>
#include <cstdio>
#include <boost/thread/xtime.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include "input_reader.h"
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
// #include "filelist_utils.h"
// #include "filelist_recovery.h"

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

      if (is_proxy_expired(req->id())) {
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

  default:
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

class dispatch
{
  std::string const* m_input;
  TaskQueue* m_tq;
  pipe_type::write_end_type* m_write_end;
public:
  dispatch(
    std::string const& input,
    TaskQueue& tq,
    pipe_type::write_end_type& write_end
  )
    : m_input(&input), m_tq(&tq), m_write_end(&write_end)
  {
  }
  void operator()(InputReader::requests_type::value_type const& v);
};

void dispatch::operator()(InputReader::requests_type::value_type const& v)
{
  std::string const& input = *m_input;
  TaskQueue& tq = *m_tq;
  pipe_type::write_end_type& write_end = *m_write_end;

  ClassAdPtr command_ad_ptr;
  InputReader::cleanup_type cleanup;
  boost::tie(command_ad_ptr, cleanup) = v;

  classad::ClassAd& command_ad = *command_ad_ptr;

  utilities::scope_guard cleanup_guard(cleanup);

  try {

    std::string command;
    jobid::JobId id;
    std::string sequence_code;
    std::string x509_proxy;
    if (utilities::command_is_valid(command_ad)) {
      boost::tie(command, id, sequence_code, x509_proxy)
        = parse_request(command_ad);
    } else {
      Info("Invalid request");
      return;
    }

    bool const status_check_is_enabled = get_check_status();

    JobStatusPtr status;

    if (status_check_is_enabled) {
      status = job_status(id);
    }

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
            if (is_proxy_expired(id)) {
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

    } else {

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
    Info(e.what());
  } catch (InvalidRequest& e) {
    Info("Invalid request");
  }
}

} // {anonymous}

struct Dispatcher::Impl
{
  InputReader& reader;
  TaskQueue& tq;
};

Dispatcher::Dispatcher(
  InputReader& reader,
  TaskQueue& tq
)
  : m_impl(new Impl)
{
  m_impl->reader = reader;
  m_impl->tq = tq;
}

void Dispatcher::operator()()
try {

  Info("Dispatcher: starting");

  InputReader& reader = m_impl->reader;
  TaskQueue& tq = m_impl->tq;

  Info(
    "Dispatcher: reading from " << reader.source() << " as " << reader.name()
  );

//   if (is_recovery_enabled()) {
//     Info("Dispatcher: doing recovery");
//     recovery(extractor, tq);
//   } else {
//     Info("Dispatcher: recovery disabled");
//   }

  while (!received_quit_signal()) {

    do_transitions(tq, write_end());
    remove_done(tq);
    InputReader::requests_type new_requests(reader.read());
    std::for_each(
      new_requests.begin(),
      new_requests.end(),
      dispatch(reader.source(), tq, write_end())
    );

    unsigned int const one_second = 1;
    ::sleep(one_second);

  }

  Info("Dispatcher: exiting");

} catch (task::SigPipe&) {
  Info("Dispatcher: no RequestHandler listening. Exiting...");
} catch (std::exception const& e) {
  Error("Dispatcher: " << e.what() << ". Exiting...");
} catch (...) {
  Error("Dispatcher: caught unknown exception. Exiting...");
}

}}}} // glite::wms::manager::server
