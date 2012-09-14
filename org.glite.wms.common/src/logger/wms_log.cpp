#include <syslog.h>
#include <iostream>
#include <string>
#include "glite/wms/common/logger/wms_log.h"


namespace glite{
namespace wms{
namespace common{
namespace logger{

namespace {
   boost::mutex mx;
}

wms_log::wms_log(){
   m_mode = wms_log::STDERR;
   m_init_flag = 0;
   m_init_level = wms_log::DEBUG;
}

void wms_log::init(wms_log::mode m, wms_log::level l){
   boost::mutex::scoped_lock lock(mx);
   if( ! m_init_flag )
      m_mode = m;
      m_init_level = l;
   
   m_init_flag = 1;
}

void wms_log::debug(const std::string& str){
   if( m_init_level >= wms_log::DEBUG ) {
      if(m_mode == wms_log::SYSLOG)
         syslog(LOG_DEBUG, str.c_str());
      else if(m_mode == wms_log::STDERR)
         std::cerr << str;
      else if(m_mode == wms_log::STDOUT)
         std::cout << str;
   }
}

void wms_log::info(const std::string& str){
   if( m_init_level >= wms_log::INFO ) {
      if(m_mode == wms_log::SYSLOG)
         syslog(LOG_INFO, str.c_str());
      else if(m_mode == wms_log::STDERR)
         std::cerr << str;
      else if(m_mode == wms_log::STDOUT)
         std::cout << str;
   }
}

void wms_log::warning(const std::string& str){
   if( m_init_level >= wms_log::WARNING ) {
      if(m_mode == wms_log::SYSLOG)
         syslog(LOG_WARNING, str.c_str());
      else if(m_mode == wms_log::STDERR)
         std::cerr << str;
      else if(m_mode == wms_log::STDOUT)
         std::cout << str;
   }
}

void wms_log::error(const std::string& str){
   if( m_init_level >= wms_log::ERROR ) {
      if(m_mode == wms_log::SYSLOG)
         syslog(LOG_ERR, str.c_str());
      else if(m_mode == wms_log::STDERR)
         std::cerr << str;
      else if(m_mode == wms_log::STDOUT)
         std::cout << str;
   }
}

void wms_log::sever(const std::string& str){
   if ( m_init_level >= wms_log::SEVER ) {
      if(m_mode == wms_log::SYSLOG)
         syslog(LOG_CRIT, str.c_str());
      else if(m_mode == wms_log::STDERR)
         std::cerr << str;
      else if(m_mode == wms_log::STDOUT)
         std::cout << str;
   }
}

void wms_log::critical(const std::string& str){
   if( m_init_level >= wms_log::CRITICAL ) {
      if(m_mode == wms_log::SYSLOG)
         syslog(LOG_ALERT, str.c_str());
      else if(m_mode == wms_log::STDERR)
         std::cerr << str;
      else if(m_mode == wms_log::STDOUT)
         std::cout << str;
   }
}

void wms_log::fatal(const std::string& str){
   if( m_init_level >= wms_log::FATAL ) {
      if(m_mode == wms_log::SYSLOG)
         syslog(LOG_EMERG, str.c_str());
      else if(m_mode == wms_log::STDERR)
         std::cerr << str;
      else if(m_mode == wms_log::STDOUT)
         std::cout << str;
   }
}



}
}
}
}

