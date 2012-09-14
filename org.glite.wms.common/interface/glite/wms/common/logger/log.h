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
  using namespace glite::wms::common::logger; \
  if (Log::DEBUG <= Log().level()) { \
    std::ostringstream os; \
    os << DEBUG_INFO << message; \
    Log().log(Log::DEBUG, os.str()); \
  } \
} while (0)

#define Info(message) \
do { \
  using namespace glite::wms::common::logger; \
  if (Log::INFO <= Log().level()) { \
    std::ostringstream os; \
    os << message; \
    Log().log(Log::INFO, os.str()); \
  } \
} while (0)

#define Notice(message) \
do { \
  using namespace glite::wms::common::logger; \
  if (Log::NOTICE <= Log().level()) { \
    std::ostringstream os; \
    os << message; \
    Log().log(Log::NOTICE, os.str()); \
  } \
} while (0)

#define Warning(message) \
do { \
  using namespace glite::wms::common::logger; \
  if (Log::WARNING <= Log().level()) { \
    std::ostringstream os; \
    os << message; \
    Log().log(Log::WARNING, os.str()); \
  } \
} while (0)

#define Error(message) \
do { \
  using namespace glite::wms::common::logger; \
  if (Log::ERROR <= Log().level()) { \
    std::ostringstream os; \
    os << message; \
    Log().log(Log::ERROR, os.str()); \
  } \
} while (0)

#define Critical(message) \
do { \
  using namespace glite::wms::common::logger; \
  if (Log::CRITICAL <= Log().level()) { \
    std::ostringstream os; \
    os << message; \
    Log().log(Log::CRITICAL, os.str()); \
  } \
} while (0)

#define Alert(message) \
do { \
  using namespace glite::wms::common::logger; \
  if (Log::ALERT <= Log().level()) { \
    std::ostringstream os; \
    os << message; \
    Log().log(Log::ALERT, os.str()); \
  } \
} while (0)

}}}}

#endif
