#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "glite/wms/common/logger/common.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace logger {

const char    *data_c::bd_s_timeFormat = "%d %b, %H:%M:%S";

DataContainerImpl::DataContainerImpl( void ) {}

DataContainerImpl::~DataContainerImpl( void ) {}

void DataContainerImpl::copy( const DataContainerImpl &that )
{
  this->date( that.date() );
  this->multiline( that.multiline(), that.multiline_prefix().c_str() );
  this->next_level( that.next_level() );
  this->time_format( that.time_format().c_str() );
  this->function( that.function().c_str() );
}

DataContainerSingle::DataContainerSingle( const char *format ) : DataContainerImpl(),
								 dcs_date( true ), dcs_multiline( false ), dcs_next( null ),
								 dcs_format( format ), dcs_function(), dcs_multiprefix( "* " )
{}

DataContainerSingle::DataContainerSingle( void ) : DataContainerImpl(),
						   dcs_date( true ), dcs_multiline( false ), dcs_next( null ),
						   dcs_format( data_c::bd_s_timeFormat ), dcs_function(), dcs_multiprefix( "* " )
{}

DataContainerSingle::~DataContainerSingle( void )
{}

void DataContainerSingle::date( bool d )
{ this->dcs_date = d; return; }

void DataContainerSingle::multiline( bool m, const char *prefix )
{
  this->dcs_multiline = m;
  this->dcs_multiprefix.assign( (prefix ? prefix : "* ") );

  return;
}

void DataContainerSingle::next_level( level_t lev )
{
  if( static_cast<int>(lev) < static_cast<int>(_first_level) ) lev = _first_level;
  else if( static_cast<int>(lev) >= static_cast<int>(_last_level) )
    lev = static_cast<level_t>( static_cast<int>(_last_level) - 1 );

  this->dcs_next = lev; 

  return; 
}

void DataContainerSingle::time_format( const char *format )
{ this->dcs_format.assign( format ); return; }

void DataContainerSingle::function( const char *func )
{ this->dcs_function.assign( func ); return; }

void DataContainerSingle::clear_function( void )
{ this->dcs_function.erase(); return; }

bool DataContainerSingle::date( void ) const
{ return this->dcs_date; }

bool DataContainerSingle::multiline( void ) const
{ return this->dcs_multiline; }

level_t DataContainerSingle::next_level( void ) const
{ return this->dcs_next; }

const string &DataContainerSingle::time_format( void ) const
{ return this->dcs_format; }

const string &DataContainerSingle::function( void ) const
{ return this->dcs_function; }

const string &DataContainerSingle::multiline_prefix( void ) const
{ return this->dcs_multiprefix; }

bool DataContainerSingle::date( void )
{ return this->dcs_date; }

bool DataContainerSingle::multiline( void )
{ return this->dcs_multiline; }

level_t DataContainerSingle::next_level( void )
{ return this->dcs_next; }

const string &DataContainerSingle::time_format( void )
{ return this->dcs_format; }

const string &DataContainerSingle::function( void )
{ return this->dcs_function; }

const string &DataContainerSingle::multiline_prefix( void )
{ return this->dcs_multiprefix; }

data_c::data_c( void ) : bd_bad( false ), bd_remove( true ), bd_showSeverity( false ),
			 bd_current( null ), bd_maxSize( bd_s_maxSize ), bd_total( 0 ),
			 bd_data( NULL ), bd_filename(), bd_buffer()
{
  memset( this->bd_buffer, 0, bd_s_bufsize );
  this->bd_data = new DataContainerSingle( bd_s_timeFormat );
}

data_c::data_c( const char *name, level_t level, const char *format ) : bd_bad( true ), bd_remove( true ), bd_showSeverity( true ),
									bd_current( level ),
									bd_maxSize( bd_s_maxSize ), bd_total( 0 ),
									bd_data( NULL ),
									bd_filename( name ),
									bd_buffer()

{
  memset( this->bd_buffer, 0, bd_s_bufsize );
  this->bd_data = new DataContainerSingle( format );
}

data_c::~data_c( void )
{
  if( this->bd_remove ) delete this->bd_data;
}

void data_c::reset_container( DataContainerImpl *dc )
{
  if( dc != NULL ) {
    if( this->bd_remove ) delete this->bd_data;

    this->bd_data = dc;
    this->bd_remove = false;
  }
  else {
    if( this->bd_remove ) delete this->bd_data;

    this->bd_data = new DataContainerSingle( bd_s_timeFormat );
    this->bd_remove = true;
  }

  return;
}

void data_c::reset( const char *name, level_t level, const char *format )
{
  this->bd_bad = false;
  this->bd_current = level;
  this->bd_showSeverity = true;
  this->bd_maxSize = bd_s_maxSize;
  this->bd_total = 0;
  this->bd_filename.assign( name );

  this->bd_data->date( true );
  this->bd_data->next_level( null );
  this->bd_data->time_format( format );
  this->bd_data->clear_function();

  memset( this->bd_buffer, 0, bd_s_bufsize );

  return;
}

void data_c::remove( void )
{
  this->bd_total = 0;
  this->bd_current = null;

  return;
}

} // logger namespace
} // common namespace
} // wms namespace
} // glite namespace
