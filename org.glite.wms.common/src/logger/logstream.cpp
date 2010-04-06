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

#include "glite/wms/common/logger/logstream.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace logger {

logstream cedglog;

logbase_c::logbase_c( void ) : ostream( NULL ), lb_buffer()
{
  this->setstate( ios::badbit );
}

logbase_c::logbase_c( const char *name, level_t lev, const char *format ) : ostream( NULL ), lb_buffer( name, lev, format )
{
  this->clear();
  if( this->lb_buffer.bad() ) this->setstate( ios::badbit );
}

logbase_c::logbase_c( const string &name, level_t lev, const char *format ) : ostream( NULL ), lb_buffer( name.c_str(), lev, format )
{
  this->clear();
  if( this->lb_buffer.bad() ) this->setstate( ios::badbit );
}

logbase_c::logbase_c( ostream &ostr, level_t lev, const char *format ) : ostream( NULL ), lb_buffer( ostr.rdbuf(), lev, format )
{
  this->clear();
  if( this->lb_buffer.bad() ) this->setstate( ios::badbit );
}

logbase_c::~logbase_c( void ) {}

void logbase_c::open( const char *name, level_t lev, const char *format )
{
  this->clear();
  if( !this->lb_buffer.open(name, lev, format) )
    this->setstate( ios::badbit );

  return;
}

void logbase_c::open( const string &name, level_t lev, const char *format )
{
  this->clear();
  if( !this->lb_buffer.open(name.c_str(), lev, format) )
    this->setstate( ios::badbit );

  return;
}

void logbase_c::open( ostream &ostr, level_t lev, const char *format )
{
  this->clear();
  if( !this->lb_buffer.open(ostr.rdbuf(), lev, format) )
    this->setstate( ios::badbit );

  return;
}

void logbase_c::close( void )
{
  if( !this->lb_buffer.close() ) this->setstate( ios::failbit );

  return;
}

logstream::logstream( void ) : logbase_c()
{ this->init( &this->lb_buffer ); }

logstream::logstream( const char *name, level_t lev, const char *format ) : logbase_c( name, lev, format )
{ this->init( &this->lb_buffer ); }

logstream::logstream( const string &name, level_t lev, const char *format ) : logbase_c( name, lev, format )
{ this->init( &this->lb_buffer ); }

logstream::logstream( ostream &ostr, level_t lev, const char *format ) : logbase_c( ostr, lev, format )
{ this->init( &this->lb_buffer ); }

logstream::~logstream( void ) {}

logstream &logstream::attach_to( logstream &ts )
{
  this->reset_container( ts.container() );
  this->init( &ts.lb_buffer );

  return *this;
}

} // logger namespace
} // common namespace
} // wms namespace
} // glite namespace
