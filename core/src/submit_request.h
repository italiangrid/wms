// File: request.h
// Authors: Marco Cecchi
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

#ifndef GLITE_WMS_MANAGER_SERVER_SUBMIT_REQUEST_H
#define GLITE_WMS_MANAGER_SERVER_SUBMIT_REQUEST_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/utility.hpp>
#include <boost/thread/mutex.hpp>

#include <openssl/pem.h>
#include <openssl/x509.h>

#include <classad_distribution.h>

#include "glite/jobid/JobId.h"

#include "wm_real.h"
#include "events.h"
#include "lb_utils.h"

typedef boost::shared_ptr<classad::ClassAd> ClassAdPtr;

namespace jobid = glite::jobid;
namespace utilities = glite::wms::common::utilities;

namespace glite {
namespace wms {
namespace manager {
namespace server {

struct SubmitState {
  enum checkpoint {
    START = 0,
    LIMBO,
    PENDING
  };

  boost::shared_ptr<classad::ClassAd> m_command_ad;
  std::string m_command;
  jobid::JobId m_id;
  std::string m_sequence_code;
  std::string m_x509_proxy;
  ContextPtr m_lb_context;
  boost::function<void()> m_cleanup;
  WMReal m_wm;
  Events& m_events;
  boost::shared_ptr<std::string> m_jw_template;
  checkpoint m_last_checkpointing;
  std::set<std::string> m_collection_pending; // pending jobs' ids after bulk mm
  boost::mutex::mutex m_request_mx;
  std::time_t m_created_at;

  boost::mutex& request_mutex()
  {
    return m_request_mx;
  }

  boost::shared_ptr<classad::ClassAd> m_ad;

  SubmitState(
    boost::shared_ptr<classad::ClassAd> cmd_ad,
    std::string const& cmd,
    jobid::JobId const& jid,
    std::string const& sc,
    std::string const& proxy,
    ContextPtr lb_c,
    boost::function<void()> cl,
    WMReal const& w,
    Events& e,
    boost::shared_ptr<std::string> jw_t,
    checkpoint last_check = START
  )
    : m_command_ad(cmd_ad),
      m_command(cmd),
      m_id(jid),
      m_sequence_code(sc),
      m_x509_proxy(proxy),
      m_lb_context(lb_c),
      m_cleanup(cl),
      m_wm(w),
      m_events(e),
      m_jw_template(jw_t),
      m_last_checkpointing(last_check),
      m_created_at(std::time(0))
  {
    if (m_command == "jobsubmit") {
      std::auto_ptr<classad::ClassAd> job_ad(
        utilities::submit_command_remove_ad(*m_command_ad)
      );
      m_ad = job_ad;
      job_ad.release();
      m_command_ad.reset();
    } else {
      m_ad.reset(new classad::ClassAd);
    }
  }

  ~SubmitState() {
    m_cleanup();
  }
};

class SubmitProcessor
{
  boost::shared_ptr<SubmitState> m_state;
public:
  SubmitProcessor(boost::shared_ptr<SubmitState> state);
  void postpone(std::string const& reason, int match_retry_period);
  void operator()();
};

}}}}
#endif
