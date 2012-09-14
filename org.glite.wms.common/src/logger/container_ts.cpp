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

#include "glite/wms/common/logger/container_ts.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace logger {

DataContainerMulti::data_s::data_s( const DataContainerSingle &dcs ) : d_date( dcs.date() ), d_multiline( dcs.multiline() ),
								       d_next( dcs.next_level() ),
								       d_format( dcs.time_format() ), d_function( dcs.function() ),
								       d_multiprefix( dcs.multiline_prefix() )
{}

DataContainerMulti::DataContainerMulti( const char *format ) : dcm_data(), dcm_single( format )
{
  this->dcm_data.reset( new data_s(this->dcm_single) );
}

DataContainerMulti::~DataContainerMulti( void )
{}

void DataContainerMulti::date( bool d )
{
  this->createData();
  this->dcm_data->d_date = d;

  return;
}

void DataContainerMulti::multiline( bool d, const char *prefix )
{
  this->createData();
  this->dcm_data->d_multiline = d;
  this->dcm_data->d_multiprefix.assign( prefix ? string(prefix) : this->dcm_single.multiline_prefix() );

  return;
}

void DataContainerMulti::next_level( level_t lev )
{
  if( static_cast<int>(lev) < static_cast<int>(_first_level) ) lev = _first_level;
  else if( static_cast<int>(lev) >= static_cast<int>(_last_level) )
    lev = static_cast<level_t>( static_cast<int>(_last_level) - 1 );

  this->createData();
  this->dcm_data->d_next = lev;

  return;
}

void DataContainerMulti::time_format( const char *format )
{
  this->createData();
  this->dcm_data->d_format.assign( format ? string(format) : this->dcm_single.time_format() );

  return;
}

void DataContainerMulti::function( const char *function )
{
  this->createData();
  this->dcm_data->d_function.assign( function ? string(function) : this->dcm_single.function() );

  return;
}

void DataContainerMulti::clear_function( void )
{
  this->createData();
  this->dcm_data->d_function.erase();

  return;
}

bool DataContainerMulti::date( void ) const
{
  this->createData();

  return this->dcm_data->d_date;
}

bool DataContainerMulti::multiline( void ) const
{
  this->createData();

  return this->dcm_data->d_multiline;
}

level_t DataContainerMulti::next_level( void ) const
{
  this->createData();

  return this->dcm_data->d_next;
}

const string &DataContainerMulti::time_format( void ) const
{
  this->createData();

  return this->dcm_data->d_format;
}

const string &DataContainerMulti::function( void ) const
{
  this->createData();

  return this->dcm_data->d_function;
}

const string &DataContainerMulti::multiline_prefix( void ) const
{
  this->createData();

  return this->dcm_data->d_multiprefix;
}

bool DataContainerMulti::date( void )
{
  this->createData();

  return this->dcm_data->d_date;
}

bool DataContainerMulti::multiline( void )
{
  this->createData();

  return this->dcm_data->d_multiline;
}

level_t DataContainerMulti::next_level( void )
{
  this->createData();

  return this->dcm_data->d_next;
}

const string &DataContainerMulti::time_format( void )
{
  this->createData();

  return this->dcm_data->d_format;
}

const string &DataContainerMulti::function( void )
{
  this->createData();

  return this->dcm_data->d_function;
}

const string &DataContainerMulti::multiline_prefix( void )
{
  this->createData();

  return this->dcm_data->d_multiprefix;
}

} // logger namespace
} // common namespace
} // wms namespace
} // glite namespace
