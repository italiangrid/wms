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

// File: recovery.h
// Author: Francesco Giacomini
// Author: Salvatore Monforte

// $Id: recovery.h,v 1.1.2.2.2.1.2.2 2010/04/07 14:02:46 mcecchi Exp $

#ifndef GLITE_WMS_MANAGER_SERVER_RECOVERY_H
#define GLITE_WMS_MANAGER_SERVER_RECOVERY_H

#include "events.h"

namespace glite {
namespace wms {

namespace common {
namespace utilitites {
class InputReader;
}
}

namespace manager {
namespace server {

void
recovery(
  boost::shared_ptr<utilities::InputReader> input,
  Events& events_queue,
  WMReal const& wm,
  boost::shared_ptr<std::string> jw_template
);

}}}} // glite::wms::manager::server

#endif
