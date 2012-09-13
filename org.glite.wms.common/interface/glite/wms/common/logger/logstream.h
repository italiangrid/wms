/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef GLITE_WMS_COMMON_LOGGER_LOGSTREAM_H
#define GLITE_WMS_COMMON_LOGGER_LOGSTREAM_H

#include <iostream>
#include <string>

#include "glite/wms/common/logger/logbuf.h"

namespace glite {
namespace wms {
namespace common {
namespace logger {
namespace threadsafe { 
  class logstream; 
} // threadsafe namespace

class logbase_c : public std::ostream {
  friend class threadsafe::logstream;

public:
  logbase_c( void );
  logbase_c( const char *name, level_t lev, const char *format );
  logbase_c( const std::string &name, level_t lev, const char *format );
  logbase_c( std::ostream &ostr, level_t lev, const char *format );

  virtual ~logbase_c( void );

  // Setters
  inline logbase_c &show_severity( bool show ) { this->lb_buffer.show_severity( show ); return *this; }
  inline logbase_c &next_level( level_t lev ) { this->lb_buffer.next_level( lev ); return *this; }
  inline logbase_c &current_level( level_t lev ) { this->lb_buffer.buffer_level( lev ); return *this; }
  inline logbase_c &function( const char *func ) { this->lb_buffer.function( func ); return *this; }
  inline logbase_c &time_format( const char *format ) { this->lb_buffer.time_format( format ); return *this; }
  inline logbase_c &clear_function( void ) { this->lb_buffer.clear_function(); return *this; }
  inline logbase_c &reset_container( DataContainerImpl *dc ) { this->lb_buffer.reset_container( dc ); return *this; }

  void close( void );
  void open( const char *name, level_t lev = medium, const char *format = data_c::bd_s_timeFormat );
  void open( const std::string &name, level_t lev = medium, const char *format = data_c::bd_s_timeFormat );
  void open( std::ostream &name, level_t lev = medium, const char *format = data_c::bd_s_timeFormat );

  inline bool activate_log_rotation( std::streamsize maxsize, const std::string &basename, unsigned int maxfiles )
  { this->lb_buffer.activate_log_rotation( maxsize, basename, maxfiles ); return this->lb_buffer.bad_file(); }
  inline void deactivate_log_rotation( void ) { this->lb_buffer.deactivate_log_rotation(); }
  inline void log_rotate( void ) { this->lb_buffer.log_rotate(); }

protected:
  inline DataContainerImpl *container( void ) { return this->lb_buffer.container(); }

  mutable   Logbuf    lb_buffer;

private:
  logbase_c &operator=( const logbase_c &lb ); // Not implemented
  logbase_c( const logbase_c &lb ); // Not implemented
};

class logstream : public logbase_c {
  friend class threadsafe::logstream;

public:
  logstream( void );
  logstream( const char *name, level_t lev = medium, const char *format = data_c::bd_s_timeFormat );
  logstream( const std::string &name, level_t lev = medium, const char *format = data_c::bd_s_timeFormat );
  logstream( std::ostream &ostr, level_t lev = medium, const char *format = data_c::bd_s_timeFormat );

  virtual ~logstream( void );

  inline void reset( std::ostream &ostr, level_t lev = medium, const char *format = data_c::bd_s_timeFormat )
  { this->open( ostr, lev, format ); }

  logstream &attach_to( logstream &ls );
};

extern logstream cedglog;

#define RenameLogStreamNS( log ) namespace log = glite::wms::common::logger

} // logger namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_LOGGER_LOGSTREAM_H */

// Local Variables:
// mode: c++
// End:
