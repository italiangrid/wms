// File: dispatcher_utils.h
// Authors: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//          Marco Cecchi
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

// $Id: dispatcher_utils.h,v 1.1.2.3.4.1.2.1 2012/02/07 16:41:01 mcecchi Exp $

#ifndef GLITE_WMS_MANAGER_SERVER_DISPATCHER_H
#define GLITE_WMS_MANAGER_SERVER_DISPATCHER_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include "glite/jobid/JobId.h"

#include "events.h"

namespace jobid = glite::jobid;

namespace glite {
namespace wms {

namespace common {
namespace utilities {
class InputReader;
}}

namespace manager {
namespace server {

boost::tuple<
  jobid::JobId,                 // jobid
  std::string,                  // sequence code
  std::string                   // x509_proxy
>
check_request(
  classad::ClassAd const& command_ad,
  std::string const& command
);

class Dispatcher
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  Dispatcher(
    Events& m_events,
    boost::shared_ptr<utilities::InputReader> m_input,
    WMReal const& wm_real,
    boost::shared_ptr<std::string> jw_template
  );
  void operator()();
};

}}}} // glite::wms::manager::server

#endif
