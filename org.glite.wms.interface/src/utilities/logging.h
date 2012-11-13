/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

//
// File: exceptions.h
// Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
//

#ifndef _GLITE_WMS_WMPROXY_COMMANDS_LOGGING_H_
#define _GLITE_WMS_WMPROXY_COMMANDS_LOGGING_H_

// Boost
#include <boost/lexical_cast.hpp>
#include "glite/wms/common/logger/manipulators.h"

#define edglog(level) glite::wms::common::logger::threadsafe::edglog<<glite::wms::common::logger::setlevel(glite::wms::common::logger::level)
#define edglog_fn(name) glite::wms::common::logger::StatePusher pusher(glite::wms::common::logger::threadsafe::edglog, "PID: " + boost::lexical_cast<std::string>(getpid()) + " - " + #name)
#define glitelogTag(level) glite::wms::common::logger::threadsafe::edglog<<glite::wms::common::logger::setlevel(glite::wms::common::logger::level)<<"*********"
#define glitelogHead(level) glite::wms::common::logger::threadsafe::edglog<<glite::wms::common::logger::setlevel(glite::wms::common::logger::level)<<"* Error *"
#define glitelogBody(level) glite::wms::common::logger::threadsafe::edglog<<glite::wms::common::logger::setlevel(glite::wms::common::logger::level)<<"*       *"

#endif
