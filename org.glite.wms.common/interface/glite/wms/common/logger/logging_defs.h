#ifndef GLITE_WMS_COMMON_LOGGING_DEFS_H
#define GLITE_WMS_COMMON_LOGGING_DEFS_H

#include <log4cpp/Category.hh>

namespace glite {
//namespace wms {
//namespace common {
namespace logging {

struct NullLogger
{
  template<typename T>
  NullLogger& operator<<(T const&) { return *this; }
};

class Logger
{
  log4cpp::Category* m_logger;
public:
  void native_logger(log4cpp::Category* l)
  {
    m_logger = l;
  }
  log4cpp::Category& native_logger()
  {
    return *m_logger;
  }
};

}
//}}
}

#endif
