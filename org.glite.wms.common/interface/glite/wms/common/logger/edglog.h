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
