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

#ifndef GLITE_WMS_MANAGER_SERVER_REQUEST_H
#define GLITE_WMS_MANAGER_SERVER_REQUEST_H

#include <string>
#include <vector>
#include <ctime>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/utility.hpp>
#include <boost/thread/mutex.hpp>

#include <classad_distribution.h>

#include "glite/jobid/JobId.h"

#include "wm_real.h"
#include "events.h"
#include "submission_utils.h"
#include "lb_utils.h"

typedef boost::shared_ptr<classad::ClassAd> ClassAdPtr;

namespace jobid = glite::jobid;
namespace utilities = glite::wms::common::utilities;

namespace glite {
namespace wms {
namespace manager {
namespace server {

struct CancelState {
  jobid::JobId m_id;
  ContextPtr m_lb_context;
  boost::function<void()> m_cleanup;
  WMReal m_wm;
  Events& m_events;

  CancelState(
    jobid::JobId const& jid,
    ContextPtr lb_c,
    boost::function<void()> cl,
    WMReal const& w,
    Events& e
  )
    : m_id(jid),
      m_lb_context(lb_c),
      m_cleanup(cl),
      m_wm(w),
      m_events(e)
  {
    boost::filesystem::path const cancel_wm_token(get_cancel_token(jid));
    try {
      create_token(cancel_wm_token);
    } catch (CannotCreateToken&) {
      Error("Cannot create token for cancel request " << jid.toString());
    }
  }

  ~CancelState() {
    m_cleanup();
  }
};

class CancelProcessor
{
  boost::shared_ptr<CancelState> m_state;
public:
  CancelProcessor(boost::shared_ptr<CancelState> state);
  void operator()();
};

}}}}
#endif
