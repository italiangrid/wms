// File: dispatcher_utils.cpp
// Authors:
//           Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//           Marco Cecchi
//
// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

// $Id: dispatcher_utils.cpp,v 1.1.2.10.2.7.2.1.4.1.4.3 2012/02/19 16:53:43 pandreet Exp $

#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <cstdio>

#include <boost/thread/xtime.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem/exception.hpp>

#include <classad_distribution.h>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/common/utilities/input_reader.h"
#include "glite/wms/common/utilities/jobdir_reader.h"
#include "glite/wms/common/utilities/scope_guard.h"
#include "glite/wms/common/utilities/wm_commands.h"

#include "glite/jobid/JobId.h"
#include "glite/wmsutils/classads/classad_utils.h"

#include "glite/lb/producer.h"

#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/DAGAd.h"
#include "glite/jdl/JDLAttributes.h"

#include "glite/wms/ism/ism.h"
#include "glite/wms/ism/purchaser/ism-purchaser.h"

#include "signal_handling.h"
#include "wm_real.h"
#include "lb_utils.h"
#include "submission_utils.h"
#include "dispatcher_utils.h"
#include "submit_request.h"
#include "match_request.h"
#include "cancel_request.h"
#include "bulkmm_utils.h"

namespace ca = glite::wmsutils::classads;
namespace configuration = glite::wms::common::configuration;
namespace fs = boost::filesystem;
namespace jdl = glite::jdl;
namespace jobid = glite::jobid;
namespace utilities = glite::wms::common::utilities;
namespace ism = glite::wms::ism;

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {

std::string
aux_get_sequence_code(
  classad::ClassAd const& command_ad,
  std::string const& command
)
{
  std::string sequence_code;

  if (command == "jobsubmit") {
    classad::ClassAd const* job_ad = utilities::submit_command_get_ad(command_ad);
    sequence_code = jdl::get_lb_sequence_code(*job_ad);
  } else if (command == "jobcancel") {
    sequence_code = utilities::cancel_command_get_lb_sequence_code(command_ad);
  } else if (command == "jobresubmit") {
    sequence_code = utilities::resubmit_command_get_lb_sequence_code(command_ad);
  }

  return sequence_code;
}

std::string
aux_get_x509_proxy(
  classad::ClassAd const& command_ad,
  std::string const& command,
  jobid::JobId const& id
)
{
  std::string x509_proxy;

  if (command == "jobsubmit") {

    classad::ClassAd const* job_ad
      = utilities::submit_command_get_ad(command_ad);
    x509_proxy = jdl::get_x509_user_proxy(*job_ad);
  } else if (command == "jobcancel") {

    x509_proxy = get_user_x509_proxy(id);
  } else if (command == "jobresubmit") {

    x509_proxy = get_user_x509_proxy(id);
  }

  return x509_proxy;
}

glite::jobid::JobId
aux_get_id(classad::ClassAd const& command_ad, std::string const& command)
{
  jobid::JobId id;
  if (command == "jobsubmit") {

    id = jobid::JobId(
      jdl::get_edg_jobid(
        *utilities::submit_command_get_ad(command_ad)
      )
    );
  } else if (command == "jobresubmit") {

    id = jobid::JobId(utilities::resubmit_command_get_id(command_ad));
  } else if (command == "jobcancel") {

    id = jobid::JobId(utilities::cancel_command_get_id(command_ad));
  } else if (command == "match") {

    bool id_exists;

    std::string jobidstr(
      jdl::get_edg_jobid(
        *utilities::match_command_get_ad(command_ad),
        id_exists
      )
    );

    if (id_exists) {

      id = jobid::JobId(jobidstr);
    } else {

      id = jobid::JobId(jobid::JobId::Hostname("localhost"), 6000, "");
    }
  }

  return id;
}

class Dispatch {
  boost::shared_ptr<utilities::InputReader> m_input;
  Events& m_events;
  WMReal const& m_wm;
  boost::shared_ptr<std::string> m_jw_template_ptr;
public:
  Dispatch(
    boost::shared_ptr<utilities::InputReader> input,
    Events& events,
    WMReal const& wm,
    boost::shared_ptr<std::string> jw_template_ptr
  );
  void operator()(utilities::InputItemPtr item);
};

Dispatch::Dispatch(
  boost::shared_ptr<utilities::InputReader> input,
  Events& events,
  WMReal const& wm,
  boost::shared_ptr<std::string> jw_template_ptr
)
  : m_input(input), m_events(events), m_wm(wm), m_jw_template_ptr(jw_template_ptr)
{ }

void Dispatch::operator()(utilities::InputItemPtr item)
{
  boost::function<void()> cleanup(
    boost::bind(&utilities::InputItem::remove_from_input, item)
  );

  // if the request is not valid, cleanup automatically
  utilities::scope_guard cleanup_guard(cleanup);

  std::string const command_ad_str = item->value();
  try
  {
    boost::shared_ptr<classad::ClassAd> command_ad(new classad::ClassAd);
    classad::ClassAdParser parser;
    if (!parser.ParseClassAd(command_ad_str, *command_ad)) {
      Info("Invalid request " << command_ad_str);
      return;
    }

    std::string command = utilities::command_get_command(*command_ad);

    if (
      command == "jobsubmit"
      || command == "jobresubmit"
      || command == "jobcancel"
    ) {
      jobid::JobId id;
      std::string sequence_code;
      std::string x509_proxy;
      boost::tie(id, sequence_code, x509_proxy)
        = check_request(*command_ad, command);

      if (command == "jobsubmit" || command == "jobresubmit") {
        cleanup_guard.dismiss();

        Info("new " << command << " for " << id.toString());
        ContextPtr lb_context
          = create_context(id, x509_proxy, sequence_code);

        log_dequeued(
          lb_context,
          m_input->source()
        );
        boost::shared_ptr<SubmitState> state(
          new SubmitState(
            command_ad,
            command,
            id,
            sequence_code,
            x509_proxy,
            lb_context,
            cleanup,
            m_wm,
            m_events,
            m_jw_template_ptr
          )
        );
        m_events.schedule(
          SubmitProcessor(state),
          submit_priority
        );
      } else { // if (command == "jobcancel")

        cleanup_guard.dismiss();

        ContextPtr lb_context
          = create_context(id, x509_proxy, sequence_code);
        log_cancel_req(lb_context);

        boost::shared_ptr<CancelState> state(
          new CancelState(
            id,
            lb_context,
            cleanup,
            m_wm,
            m_events
          )
        );
        m_events.schedule(
          CancelProcessor(state),
          cancel_priority
        );
      }
    } else if (command == "match") {

      Info("new " << command);
      cleanup_guard.dismiss();
      boost::shared_ptr<MatchState> state(
        new MatchState(
          command_ad,
          aux_get_id(*command_ad, command),
          cleanup
        )
      );
      m_events.schedule(
        MatchProcessor(state),
        match_priority
      );
    } else if (command == "ism_dump") {

      Info("new " << command);
      ism::call_dump_ism_entries dump_ism;
      m_events.schedule(
        boost::bind(&ism::call_dump_ism_entries::operator(), dump_ism),
        debug_priority
      );
    } else if (command == "wm_debug") {

      Info("WM queue: " << m_events.waiting_size() << "waiting, "
        << m_events.ready_size() << "ready");
    } else {

      Info("unrecognized command " << command);
    }
  } catch (ca::ClassAdError& e) {
    Info(e.what() << " for " << command_ad_str);
  } catch (CannotCreateLBContext& e) {
    Info(e.what() << " for " << command_ad_str);
  }
}

} // {anonymous}

boost::tuple<
  jobid::JobId,  // jobid
  std::string,   // sequence code
  std::string    // x509_proxy
>
check_request(classad::ClassAd const& command_ad, std::string const& command)
{
  jobid::JobId id;
  std::string sequence_code;
  std::string x509_proxy;
  
  if (utilities::command_is_valid(command_ad)) {

    id = aux_get_id(command_ad, command);
    sequence_code = aux_get_sequence_code(command_ad, command);
    x509_proxy = aux_get_x509_proxy(command_ad, command, id);
  } 
    
  return boost::make_tuple(id, sequence_code, x509_proxy);
}   

struct Dispatcher::Impl {
  Events& m_events;
  boost::shared_ptr<utilities::InputReader> m_input;
  WMReal const& m_wm;
  boost::shared_ptr<std::string> m_jw_template_ptr;

  Impl(
    Events& events,
    boost::shared_ptr<utilities::InputReader> input,
    WMReal const& wm,
    boost::shared_ptr<std::string> jw_template_ptr
  ) : m_events(events),
      m_input(input),
      m_wm(wm),
      m_jw_template_ptr(jw_template_ptr)
  { }
};

Dispatcher::Dispatcher(
  Events& events,
  boost::shared_ptr<utilities::InputReader> input,
  WMReal const& wm,
  boost::shared_ptr<std::string> jw_template_ptr
)
  : m_impl(new Impl(events, input, wm, jw_template_ptr))
{ }

void Dispatcher::operator()()
try {

  int const one_second = 1;
  int const one_minute = 60;
  int next_schedule(one_second);
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  int const max_ready_events = config.wm()->queue_size();

  while (!received_quit_signal()) {
    if (m_impl->m_events.ready_size() < max_ready_events) {

      try {
        utilities::InputReader::InputItems new_requests(m_impl->m_input->read());
        Dispatch dispatch(
          m_impl->m_input,
          m_impl->m_events,
          m_impl->m_wm,
          m_impl->m_jw_template_ptr
        );
        std::for_each(new_requests.begin(), new_requests.end(), dispatch);
      } catch (utilities::JobDirError const& e) {
        Error("dispatcher: " << e.what() << " reading from jobdir");
      }
    } else {

      next_schedule = one_minute;
      Debug(
        "Events queue full (size " <<
        boost::lexical_cast<std::string>(m_impl->m_events.ready_size()) <<
        ") rescheduling dispatcher in " << next_schedule << " seconds "
      );
    }

    m_impl->m_events.schedule_at(
      *this,
      std::time(0) + next_schedule,
      dispatcher_priority
    );
    return;
  }


  m_impl->m_events.stop();
  Info("Dispatcher: exiting");
} catch (std::exception const& e) {
  Error("Dispatcher: " << e.what() << ". Exiting...");
  m_impl->m_events.stop();
} catch (...) {
  Error("Dispatcher: caught unknown exception. Exiting...");
  m_impl->m_events.stop();
}

}}}} // glite::wms::manager::server
