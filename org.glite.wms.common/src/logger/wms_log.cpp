#include <syslog.h>
#include <iostream>
#include <string>
#include "glite/wms/common/logger/wms_log.h"


namespace glite{
namespace wms{
namespace common{
namespace logger{


boost::scoped_ptr<wms_log> wms_log::wms_log_instance;
boost::mutex wms_log::mx;

wms_log::wms_log(){
   m_mode = wms_log::STDERR;
}

wms_log* 
wms_log::get_instance(){
  boost::mutex::scoped_lock lock(mx);
  if( wms_log_instance.get() == 0 )
     wms_log_instance.reset( new wms_log );
  return wms_log_instance.get();
} 

void wms_log::init(wms_log::mode m){
   m_mode = m;
}

void wms_log::debug(std::string str){
   if(m_mode == wms_log::SYSLOG)
      syslog(LOG_DEBUG, str.c_str());
   else
      std::cerr << str;
}

void wms_log::info(std::string str){
   if(m_mode == wms_log::SYSLOG)
      syslog(LOG_INFO, str.c_str());
   else
      std::cerr << str;
}

void wms_log::warning(std::string str){
   if(m_mode == wms_log::SYSLOG)
      syslog(LOG_WARNING, str.c_str());
   else
      std::cerr << str;
}

void wms_log::error(std::string str){
   if(m_mode == wms_log::SYSLOG)
      syslog(LOG_ERR, str.c_str());
   else
      std::cerr << str;
}

void wms_log::sever(std::string str){
   if(m_mode == wms_log::SYSLOG)
      syslog(LOG_CRIT, str.c_str());
   else
      std::cerr << str;
}

void wms_log::critical(std::string str){
   if(m_mode == wms_log::SYSLOG)
      syslog(LOG_ALERT, str.c_str());
   else
      std::cerr << str;
}

void wms_log::fatal(std::string str){
   if(m_mode == wms_log::SYSLOG)
      syslog(LOG_EMERG, str.c_str());
   else
      std::cerr << str;
}


}
}
}
}

