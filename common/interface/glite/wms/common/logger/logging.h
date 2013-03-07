#ifndef GLITE_WMS_COMMON_LOGGING_H
#define GLITE_WMS_COMMON_LOGGING_H

#include "glite/wms/common/logger/logging_defs.h"
#include "glite/wms/common/logger/logging_init.h"
#include "glite/wms/common/logger/key_value.h"

namespace glite {
//namespace wms {
//namespace common {
namespace logging {

extern NullLogger null_log;
extern Logger access_log;
extern Logger security_log;
extern Logger control_log;

static Init init_;

}
//}}
}

#ifndef WITHOUT_LOGGING

#define GLITE_LOG_ACCESS_FATAL \
if (!glite::logging::access_log.native_logger().isFatalEnabled()) \
  ; \
else glite::logging::access_log.native_logger().fatalStream()

#define GLITE_LOG_ACCESS_ERROR \
if (!glite::logging::access_log.native_logger().isErrorEnabled()) \
  ; \
else glite::logging::access_log.native_logger().errorStream()

#define GLITE_LOG_ACCESS_WARN \
if (!glite::logging::access_log.native_logger().isWarnEnabled()) \
  ; \
else glite::logging::access_log.native_logger().warnStream()

#define GLITE_LOG_ACCESS_INFO \
if (!glite::logging::access_log.native_logger().isInfoEnabled()) \
  ; \
else glite::logging::access_log.native_logger().infoStream()

#define GLITE_LOG_ACCESS_DEBUG \
if (!glite::logging::access_log.native_logger().isDebugEnabled()) \
  ; \
else glite::logging::access_log.native_logger().debugStream()

#define GLITE_LOG_SECURITY_FATAL \
if (!glite::logging::security_log.native_logger().isFatalEnabled()) \
  ; \
else glite::logging::security_log.native_logger().fatalStream()

#define GLITE_LOG_SECURITY_ERROR \
if (!glite::logging::security_log.native_logger().isErrorEnabled()) \
  ; \
else glite::logging::security_log.native_logger().errorStream()

#define GLITE_LOG_SECURITY_WARN \
if (!glite::logging::security_log.native_logger().isWarnEnabled()) \
  ; \
else glite::logging::security_log.native_logger().warnStream()

#define GLITE_LOG_SECURITY_INFO \
if (!glite::logging::security_log.native_logger().isInfoEnabled()) \
  ; \
else glite::logging::security_log.native_logger().infoStream()

#define GLITE_LOG_SECURITY_DEBUG \
if (!glite::logging::security_log.native_logger().isDebugEnabled()) \
  ; \
else glite::logging::security_log.native_logger().debugStream()

#define GLITE_LOG_CONTROL_FATAL \
if (!glite::logging::control_log.native_logger().isFatalEnabled()) \
  ; \
else glite::logging::control_log.native_logger().fatalStream()

#define GLITE_LOG_CONTROL_ERROR \
if (!glite::logging::control_log.native_logger().isErrorEnabled()) \
  ; \
else glite::logging::control_log.native_logger().errorStream()

#define GLITE_LOG_CONTROL_WARN \
if (!glite::logging::control_log.native_logger().isWarnEnabled()) \
  ; \
else glite::logging::control_log.native_logger().warnStream()

#define GLITE_LOG_CONTROL_INFO \
if (!glite::logging::control_log.native_logger().isInfoEnabled()) \
  ; \
else glite::logging::control_log.native_logger().infoStream()

#define GLITE_LOG_CONTROL_DEBUG \
if (!glite::logging::control_log.native_logger().isDebugEnabled()) \
  ; \
else glite::logging::control_log.native_logger().debugStream()

#else

#define GLITE_LOG_ACCESS_FATAL if (true) ; else glite::logging::null_log
#define GLITE_LOG_ACCESS_ERROR if (true) ; else glite::logging::null_log
#define GLITE_LOG_ACCESS_WARN if (true) ; else glite::logging::null_log
#define GLITE_LOG_ACCESS_INFO if (true) ; else glite::logging::null_log
#define GLITE_LOG_ACCESS_DEBUG if (true) ; else glite::logging::null_log

#define GLITE_LOG_SECURITY_FATAL if (true) ; else glite::logging::null_log
#define GLITE_LOG_SECURITY_ERROR if (true) ; else glite::logging::null_log
#define GLITE_LOG_SECURITY_WARN if (true) ; else glite::logging::null_log
#define GLITE_LOG_SECURITY_INFO if (true) ; else glite::logging::null_log
#define GLITE_LOG_SECURITY_DEBUG if (true) ; else glite::logging::null_log

#define GLITE_LOG_CONTROL_FATAL if (true) ; else glite::logging::null_log
#define GLITE_LOG_CONTROL_ERROR if (true) ; else glite::logging::null_log
#define GLITE_LOG_CONTROL_WARN if (true) ; else glite::logging::null_log
#define GLITE_LOG_CONTROL_INFO if (true) ; else glite::logging::null_log
#define GLITE_LOG_CONTROL_DEBUG if (true) ; else glite::logging::null_log

#endif

#endif
