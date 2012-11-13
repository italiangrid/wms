// File: match_request.h
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

#ifndef GLITE_WMS_MANAGER_SERVER_MATCH_REQUEST_H
#define GLITE_WMS_MANAGER_SERVER_MATCH_REQUEST_H

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
#include "lb_utils.h"

typedef boost::shared_ptr<classad::ClassAd> ClassAdPtr;

namespace jobid = glite::jobid;
namespace utilities = glite::wms::common::utilities;

namespace glite {
namespace wms {
namespace manager {
namespace server {

struct MatchState {
  boost::shared_ptr<classad::ClassAd> command_ad;
  jobid::JobId id;
  boost::function<void()> cleanup;

  MatchState(
    boost::shared_ptr<classad::ClassAd> cmd_ad,
    jobid::JobId const& jid,
    boost::function<void()> cl
  )
    : command_ad(cmd_ad), id(jid), cleanup(cl)
  { }

  ~MatchState() {
    cleanup();
  }
};

class MatchProcessor
{
  boost::shared_ptr<MatchState> m_state;
public:
  MatchProcessor(boost::shared_ptr<MatchState> state);
  void operator()();
};

}}}}
#endif
