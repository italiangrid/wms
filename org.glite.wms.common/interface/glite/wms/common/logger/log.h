#ifndef GLITE_WMS_COMMON_LOGGER_LOG_H
#define GLITE_WMS_COMMON_LOGGER_LOG_H

#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>

namespace glite {
namespace wms {
namespace common {
namespace logger {

class Log
{
public:
  enum Level {
    ALERT,
    CRITICAL,
    ERROR,
    WARNING,
    NOTICE,
    INFO,
    DEBUG
  };

private:

  static std::ostream* s_log_stream;
  static Level s_level;

public:

  static void init(std::ostream& log_stream, Level l);
  static void init(int syslog_facility, Level l);

  Log();

  Level level() const { return s_level; }
  std::ostream& log_stream();

  void log(Level l, std::string const& s);
};

#define DEBUG_INFO \
  __FUNCTION__ \
  << '(' << __FILE__ << ':' \
  << boost::lexical_cast<std::string>(__LINE__) << ") "

#define Debug(message) \
do { \
  if (Log::DEBUG <= Log().level()) { \
    std::ostringstream os; \
    os << DEBUG_INFO << message; \
    Log().log(Log::DEBUG, os.str()); \
  } \
} while (0)

#define Info(message) \
do { \
  if (Log::INFO <= Log().level()) { \
    Log().log(Log::INFO, message); \
  } \
} while (0)

#define Notice(message) \
do { \
  if (Log::NOTICE <= Log().level()) { \
    Log().log(Log::NOTICE, message); \
  } \
} while (0)

#define Warning(message) \
do { \
  if (Log::WARNING <= Log().level()) { \
    Log().log(Log::WARNING, message); \
  } \
} while (0)

#define Error(message) \
do { \
  if (Log::Error <= Log().level()) { \
    Log().log(Log::ERROR, message); \
  } \
} while (0)

#define Critical(message) \
do { \
  if (Log::CRITICAL <= Log().level()) { \
    Log().log(Log::CRITICAL, message); \
  } \
} while (0)

#define Alert(message) \
do { \
  if (Log::ALERT <= Log().level()) { \
    Log().log(Log::ALERT, message); \
  } \
} while (0)

}}}}

#endif
