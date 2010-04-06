/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://project.eu-egee.org/index.php?id=115 for details on the
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

#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "glite/wms/common/logger/logstream_ts.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace logger { 
namespace threadsafe {

logstream   edglog;

#ifndef HAVE_STRINGSTREAM
logstream::OutStream::OutStream( void ) : ostrstream()
{}

logstream::OutStream::~OutStream( void )
{}

string logstream::OutStream::str( void )
{
  char         *contents = this->ostrstream::str();
  string        output( contents );

  this->freeze( false );

  return output;
}
#endif

logstream::logstream( void ) : tl_multi( true ), tl_container( NULL ),
			       tl_stream(), tl_mutex(),
			       tl_buffer()
{
  this->tl_container = new DataContainerMulti( data_c::bd_s_timeFormat );
  this->tl_stream.reset_container( this->tl_container );
}

logstream::logstream( const char *name, level_t lev, const char *format ) : tl_multi( true ), tl_container( NULL ),
									    tl_stream( name, lev, format ), tl_mutex(),
									    tl_buffer()
{
  this->tl_container = new DataContainerMulti( format );
  this->tl_stream.reset_container( this->tl_container );
}

logstream::logstream( const string &name, level_t lev, const char *format ) : tl_multi( true ), tl_container( NULL ),
									      tl_stream( name.c_str(), lev, format ), tl_mutex(),
									      tl_buffer()
{
  this->tl_container = new DataContainerMulti( format );
  this->tl_stream.reset_container( this->tl_container );
}

logstream::logstream( ostream &ostr, level_t lev, const char *format ) : tl_multi( true ), tl_container( NULL ),
									 tl_stream( ostr, lev, format ), tl_mutex(),
									 tl_buffer()
{
  this->tl_container = new DataContainerMulti( format );
  this->tl_stream.reset_container( this->tl_container );
}

logstream::~logstream( void )
{
  this->tl_stream.reset_container( NULL );
  delete this->tl_container;
}

void logstream::open( const char *name, level_t lev, const char *format )
{
  boost::mutex::scoped_lock    lock( this->tl_mutex );
  this->tl_stream.open( name, lev, format );

  return;
}

void logstream::open( ostream &ostr, level_t lev, const char *format )
{
  boost::mutex::scoped_lock    lock( this->tl_mutex );
  this->tl_stream.open( ostr, lev, format );

  return;
}

void logstream::close( void )
{
  boost::mutex::scoped_lock    lock( this->tl_mutex );
  this->tl_stream.close();

  return;
}

void logstream::unsafe_attach( logger::logstream &ls )
{
  boost::mutex::scoped_lock   lock( this->tl_mutex );

  if( this->tl_multi ) this->tl_multi = false;
  this->tl_stream.attach_to( ls );

  return;
}

bool logstream::activate_log_rotation( streamsize maxsize, const string &basename, unsigned int maxfile )
{
  boost::mutex::scoped_lock   lock( this->tl_mutex );

  return this->tl_stream.activate_log_rotation( maxsize, basename, maxfile );
}

void logstream::deactivate_log_rotation( void )
{
  boost::mutex::scoped_lock   lock( this->tl_mutex );

  this->tl_stream.deactivate_log_rotation();

  return;
}

void logstream::log_rotate( void )
{
  boost::mutex::scoped_lock   lock( this->tl_mutex );

  this->tl_stream.log_rotate();

  return;
}

logstream &logstream::next_level( level_t lev )
{
  this->tl_stream.next_level( lev );

  return *this;
}

logstream &logstream::function( const char *function )
{
  this->tl_stream.function( function );

  return *this;
}

logstream &logstream::time_format( const char *format )
{
  this->tl_stream.time_format( format );

  return *this;
}

logstream &logstream::clear_function( void )
{
  this->tl_stream.clear_function();

  return *this;
}

logstream &logstream::show_severity( bool show )
{
  boost::mutex::scoped_lock   lock( this->tl_mutex );

  this->tl_stream.show_severity( show );

  return *this;
}

logstream &logstream::current_level( level_t lev )
{
  boost::mutex::scoped_lock   lock( this->tl_mutex );

  this->tl_stream.current_level( lev );

  return *this;
}

logstream &logstream::endl( void )
{
  boost::mutex::scoped_lock   lock( this->tl_mutex );

  this->checkBuffer();
#ifndef HAVE_STRINGSTREAM
  this->tl_buffer->put( '\0' );
#endif
  this->tl_stream << this->tl_buffer->str() << std::endl;

  this->tl_buffer.reset( new OutStream() );

  return *this;
}

logstream &logstream::ends( void )
{
  boost::mutex::scoped_lock   lock( this->tl_mutex );

  this->checkBuffer();
#ifndef HAVE_STRINGSTREAM
  this->tl_buffer->put( '\0' );
#endif
  this->tl_stream << this->tl_buffer->str() << std::ends;

  this->tl_buffer.reset( new OutStream() );

  return *this;
}

logstream &logstream::flush( void )
{
  boost::mutex::scoped_lock   lock( this->tl_mutex );

  this->checkBuffer();
#ifndef HAVE_STRINGSTREAM
  this->tl_buffer->put( '\0' );
#endif
  this->tl_stream << this->tl_buffer->str() << std::flush;

  this->tl_buffer.reset( new OutStream() );

  return *this;
}

} // namespace threadsafe
} // logger namespace
} // common namespace
} // wms namespace
} // glite namespace



