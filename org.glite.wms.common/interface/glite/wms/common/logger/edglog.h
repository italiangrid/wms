/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org\partners for details on the
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

#ifndef GLITE_WMS_COMMON_LOGGER_EDGLOG_H
#define GLITE_WMS_COMMON_LOGGER_EDGLOG_H

#include "glite/wms/common/logger/logstream_ts.h"

namespace glite {
namespace wms {
namespace common {
namespace logger { 
namespace threadsafe {
  extern logstream  edglog;
} // Namespace threadsafe 
} // logger namespace
} // common namespace
} // wms namespace
} // glite namespace

#define RenameLogStreamNS_ts( ts )  namespace ts = glite::wms::common::logger::threadsafe

#endif /* GLITE_WMS_COMMON_LOGGER_EDGLOG_H */

// Local Variables:
// mode: c++
// End:
