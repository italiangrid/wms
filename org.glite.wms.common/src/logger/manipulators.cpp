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

#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/logger/logbuf.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace logger {

#ifndef LOGGER_FUTURE_IMPLEMENTATION
setfunction::setfunction( const char *func ) : sf_function( func ) {}
setfunction::setfunction( const string &func ) : sf_function( func ) {}
#endif /* !LOGGER_FUTURE_IMPLEMENTATION */

void StatePusher::setState( const char *func )
{
  this->fp_data.copy( *this->fp_buffer->container() ); // Store the old values
  if( func != NULL ) this->fp_buffer->function( func );
}

StatePusher::StatePusher( ostream &os, const char *func ) : fp_buffer( dynamic_cast<Logbuf *>(os.rdbuf()) ), fp_data()
{
  if( this->fp_buffer ) this->setState( func );
}

StatePusher::StatePusher( ostream &os, const string &func ) : fp_buffer( dynamic_cast<Logbuf *>(os.rdbuf()) ), fp_data()
{
  if( this->fp_buffer ) this->setState( func.c_str() );
}

StatePusher::~StatePusher( void )
{
  if( this->fp_buffer ) this->fp_buffer->container()->copy( this->fp_data );
}

settimeformat::settimeformat( const char *format ) : stf_format( format ) {}
settimeformat::settimeformat( const string &format ) : stf_format( format ) {}

setlevel::setlevel( level_t lev ) : sl_level( lev ) {}

setcurrent::setcurrent( level_t lev ) : sc_level( lev ) {}

setmultiline::setmultiline( bool multi, const char *prefix ) : sm_multi( multi ), sm_prefix( prefix ) {}

setshowseverity::setshowseverity( bool show ) : ss_show( show ) {}

#ifndef LOGGER_FUTURE_IMPLEMENTATION
ostream &operator<<( ostream &os, const logger::setfunction &sf )
{
  Logbuf     *buffer = dynamic_cast<Logbuf *>( os.rdbuf() );

  if( buffer != NULL ) buffer->function( sf.sf_function.c_str() );

  return os;
}
#endif /* !LOGGER_FUTURE_IMPLEMENTATION */

ostream &operator<<( ostream &os, const logger::settimeformat &stf )
{
  Logbuf     *buffer = dynamic_cast<Logbuf *>( os.rdbuf() );

  if( buffer != NULL ) buffer->time_format( stf.stf_format.c_str() );

  return os;
}

ostream &operator<<( ostream &os, const logger::setlevel &sl )
{
  Logbuf     *buffer = dynamic_cast<Logbuf *>( os.rdbuf() );

  if( buffer != NULL ) buffer->next_level( sl.sl_level );

  return os;
}

ostream &operator<<( ostream &os, const logger::setcurrent &sc )
{
  Logbuf     *buffer = dynamic_cast<Logbuf *>( os.rdbuf() );

  if( buffer != NULL ) buffer->buffer_level( sc.sc_level );

  return os;
}

ostream &operator<<( ostream &os, const logger::setmultiline &sm )
{
  Logbuf     *buffer = dynamic_cast<Logbuf *>( os.rdbuf() );

  if( buffer != NULL ) buffer->multiline( sm.sm_multi, sm.sm_prefix );

  return os;
}

ostream &operator<<( ostream &os, const logger::setshowseverity &ss )
{
  Logbuf    *buffer = dynamic_cast<Logbuf *>( os.rdbuf() );

  if( buffer != NULL ) buffer->show_severity( ss.ss_show );

  return os;
}

} // logger namespace
} // common namespace
} // wms namespace
} // glite namespace

