#include "glite/wms/common/logger/logging_defs.h"
#include "glite/wms/common/logger/logging_init.h"
#include <log4cpp/SyslogAppender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/SimpleLayout.hh>
#include <log4cpp/Priority.hh>

#ifndef LOG4CPP_HAVE_SYSLOG
#error no syslog support
#endif

namespace glite {
//namespace wms {
//namespace common {
namespace logging {

NullLogger null_logger;
Logger access_log;

// extern Logger security_log;
// extern Logger control_log;

int Init::s_count = 0;

namespace {

void init_logging()
{
  log4cpp::SyslogAppender* appender(
    new log4cpp::SyslogAppender("syslog", "log4cpp")
  );
//  log4cpp::FileAppender* appender(
//    new log4cpp::FileAppender("file", "/home/giaco/tmp/log")
//  );

  appender->setLayout(new log4cpp::SimpleLayout());

  log4cpp::Category& access(
    log4cpp::Category::getInstance(std::string("access"))
  );

  access.addAppender(appender);
  access.setPriority(log4cpp::Priority::INFO);
  access_log.native_logger(&access);
}

}

Init::Init()
{
  if (s_count++ == 0) {
    init_logging();
  }
}

Init::~Init()
{
  if (--s_count == 0) {
    access_log.native_logger(0);
  }
}

}
//}}
}
