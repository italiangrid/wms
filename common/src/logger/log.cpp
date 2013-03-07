#include <syslog.h>
#include <iostream>
#include <string>
#include "glite/wms/common/logger/log.h"

namespace {
bool initialized = false;

int syslog_level[] = {
  LOG_ALERT,
  LOG_CRIT,
  LOG_ERR,
  LOG_WARNING,
  LOG_NOTICE,
  LOG_INFO,
  LOG_DEBUG
};

char level_string[] = {
  'A',
  'C',
  'E',
  'W',
  'N',
  'I',
  'D'
};

}

namespace glite {
namespace wms {
namespace common {
namespace logger {

std::ostream* Log::s_log_stream;
Log::Level Log::s_level;

void Log::init(std::ostream& log_stream, Level l)
{
  assert(initialized == false);
  assert(log_stream);
  s_log_stream = &log_stream;
  s_level = l;
  initialized = true;
}

void Log::init(int syslog_facility, Level l)
{
  assert(initialized == false);
  ::openlog("WM", LOG_CONS | LOG_NDELAY, syslog_facility);
  s_level = l;
  initialized = true;
}

Log::Log()
{
  assert(initialized == true);
}

void Log::log(Log::Level l, std::string const& s)
{
  if (s_log_stream) {
    *s_log_stream << '[' << level_string[l] << "] " << s << std::endl;
  } else {
    syslog(syslog_level[l], "[%c] %s", level_string[l], s.c_str());
  }
}

}}}}
