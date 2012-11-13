// File: WMReal.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
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

// $Id: wm_real.h,v 1.1.2.1.2.1.2.1.2.1.2.1.4.1 2012/02/07 16:41:01 mcecchi Exp $

#ifndef GLITE_WMS_MANAGER_SERVER_WMREAL_H
#define GLITE_WMS_MANAGER_SERVER_WMREAL_H

#include <boost/shared_ptr.hpp>

#include "lb_utils.h"
#include "bulkmm_utils.h"
#include "glite/wms/common/utilities/jobdir.h"

namespace classad {
class ClassAd;
}

namespace utilities = glite::wms::common::utilities;

namespace glite {

namespace jobid {
class JobId;
}

namespace wms {
namespace manager {
namespace server {

class CannotDeliverFatal: std::exception
{
  std::string m_message;
public:
  CannotDeliverFatal(std::string const& err)
    : m_message(err) { }
  ~CannotDeliverFatal() throw() { }
  char const* what() const throw() {
    return m_message.c_str();
  }
};

class CannotDeliverRecoverable: std::exception
{
  std::string m_message;
public:
  CannotDeliverRecoverable(std::string const& err)
    : m_message(err) { }
  ~CannotDeliverRecoverable() throw() { }
  char const* what() const throw() {
    return m_message.c_str();
  }
};

class CannotCreateWM : std::exception
{
  std::string m_message;
public:
  CannotCreateWM(std::string const& reason)
    : m_message(reason) { }
  ~CannotCreateWM() throw() { }

  char const* what() const throw() {
    return m_message.c_str();
  }
};

class WMReal
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;
  void submit(std::auto_ptr<classad::ClassAd> planned_ad, ContextPtr, bool is_replan);

public:
  WMReal(
    boost::shared_ptr<utilities::JobDir> to_jc,
    boost::shared_ptr<utilities::JobDir> to_ice
  );
  void submit(
    classad::ClassAd const& jdl,
    ContextPtr context,
    boost::shared_ptr<std::string> jw_template,
    bool is_replan
  );
  void submit_collection(
    classad::ClassAd const& jdl,
    ContextPtr context,
    PendingJobs& pending,
    boost::shared_ptr<std::string> jw_template
  );
  void cancel(
    glite::jobid::JobId const& request_id,
    ContextPtr context
  );
};

}}}} // glite::wms::manager::server

#endif
