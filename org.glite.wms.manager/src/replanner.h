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

// Authors: Marco Cecchi

#ifndef GLITE_WMS_MANAGER_SERVER_REPLANNER_H
#define GLITE_WMS_MANAGER_SERVER_REPLANNER_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "wm_real.h"
#include "events.h"
#include "lb_utils.h"

namespace glite {
namespace wms {
namespace manager {
namespace server {

struct ReplannerState {

  ContextPtr m_lb_context;
  WMReal m_wm;
  Events& m_events;
  boost::shared_ptr<std::string> m_jw_template;

  ReplannerState(
    WMReal const& w,
    Events& e,
    boost::shared_ptr<std::string> jw_t
  ) :
      m_wm(w),
      m_events(e),
      m_jw_template(jw_t)
  {
  }
};

class Replanner
{
  boost::shared_ptr<ReplannerState> m_state;
public:
  Replanner(boost::shared_ptr<ReplannerState> state);
  void operator()();
};

}}}}
#endif
