/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#ifndef GLITE_WMS_MANAGER_SERVER_EVENTS_H
#define GLITE_WMS_MANAGER_SERVER_EVENTS_H

#include <map>
#include <string>
#include <ctime>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include "glite/wms/common/utilities/input_reader.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/common/utilities/scope_guard.h"
#include "glite/wms/common/utilities/wm_commands.h"

namespace utilities = glite::wms::common::utilities;

namespace glite {
namespace wms {
namespace manager {
namespace server {

// the lower the number, the higher the priority
int const submit_priority = 60;
int const cancel_priority = 50;
int const replanner_priority = 40;
int const match_priority = 30;
int const dispatcher_priority = 20;
int const ism_priority = 10;
int const debug_priority = 0;

class Events: boost::noncopyable
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  Events();

  void schedule(boost::function<void()>, int priority = 0);
  void schedule_at(boost::function<void()>, std::time_t t, int priority = 0);
  void run();
  void stop();
  int ready_size();
  int waiting_size();
};

}}}}

#endif
