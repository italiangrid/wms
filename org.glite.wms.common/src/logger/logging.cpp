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

NullLogger null_log;
Logger access_log;
Logger security_log;
Logger control_log;

int Init::s_count = 0;

namespace {

void init_access_logging()
{
  log4cpp::SyslogAppender* appender(
    new log4cpp::SyslogAppender("syslog", "access")
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

void init_security_logging()
{
  log4cpp::SyslogAppender* appender(
    new log4cpp::SyslogAppender("syslog", "security")
  );
//  log4cpp::FileAppender* appender(
//    new log4cpp::FileAppender("file", "/home/giaco/tmp/log")
//  );

  appender->setLayout(new log4cpp::SimpleLayout());

  log4cpp::Category& security(
    log4cpp::Category::getInstance(std::string("security"))
  );

  security.addAppender(appender);
  security.setPriority(log4cpp::Priority::INFO);
  security_log.native_logger(&security);
}

void init_control_logging()
{
  log4cpp::SyslogAppender* appender(
    new log4cpp::SyslogAppender("syslog", "control")
  );
//  log4cpp::FileAppender* appender(
//    new log4cpp::FileAppender("file", "/home/giaco/tmp/log")
//  );

  appender->setLayout(new log4cpp::SimpleLayout());

  log4cpp::Category& control(
    log4cpp::Category::getInstance(std::string("control"))
  );

  control.addAppender(appender);
  control.setPriority(log4cpp::Priority::INFO);
  control_log.native_logger(&control);
}

}

Init::Init()
{
  if (s_count++ == 0) {
    init_access_logging();
    init_control_logging();
    init_security_logging();
  }
}

Init::~Init()
{
  if (--s_count == 0) {
    access_log.native_logger(0);
    security_log.native_logger(0);
    control_log.native_logger(0);
  }
}

}
//}}
}
