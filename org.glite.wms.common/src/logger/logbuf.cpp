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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <cstdlib>
#include <cstdio>
#include <ctime>

#include <algorithm>
#include <fstream>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

#include "utilities/streamdescriptor.h"

#include "glite/wms/common/logger/logbuf.h"

namespace fs = boost::filesystem;
using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace logger {

const char   *Logbuf::lb_s_letterLevels = "FCSEWID*VLMHU!";

bool Logbuf::checkRotationBuffer( void )
{
  bool         res = false;
  long int     flags;
  filebuf     *buffer = dynamic_cast<filebuf *>( this->lb_buffer );

  if (buffer && !lb_bad_file) {
    int   fd = utilities::bufferdescriptor( *buffer );

    if( fd > 2 ) { // Avoid truncation on stdout/stderr (cout/cerr have a filebuf as buffer !!!)
      flags = fcntl( fd, F_GETFL ) & O_ACCMODE;
      res = (flags == O_RDWR) || (flags == O_RDONLY);
    }
  }
  return res;
}

streamsize Logbuf::getBufferSize( void )
{
  iostream     str( this->lb_buffer );

  str.seekp( 0, ios::end );
  return str.tellp();
}

void Logbuf::writeBuffer( std::streamoff n )
{
  bool        first = true;
  char       *next, *prev;

  if( !this->lb_data.multiline() )
    this->lb_buffer->sputn( this->pbase(), n );
  else {
    const string        &prefix = this->lb_data.multiline_prefix();
    string::size_type    len = prefix.length();

    for( next = find(prev = this->pbase(), this->pptr(), '\n'); next != this->pptr(); next = find(prev, this->pptr(), '\n') ) {
      if( !first ) this->lb_buffer->sputn( prefix.c_str(), len );
      this->lb_buffer->sputn( prev, next - prev + 1 );

      prev = next + 1; first = false;
    }
  }

  return;
}

int Logbuf::internalSync( bool overflow )
{
  bool                         do_write, show_severity = this->lb_data.show_severity();
  int                          res = 0, next_total_level( static_cast<int>(this->lb_data.next_level()) % _last_level );
  int                          next_level( next_total_level % _last_positive );
  size_t                       datesize;
  time_t                       epoch;
  string::size_type            len;
  streamoff                    n;
  static char                  timebuf[100];

  do_write = ( next_level <= static_cast<int>(this->lb_data.buffer_level()) );
  n = static_cast<streamoff>( this->pptr() - this->pbase() );

  if( n > 0 ) {
    if( do_write && (n > 1) ) {
#if (__GNUC__<3)
      this->lb_buffer->sync();
      this->lb_buffer->seekoff( 0, ios::end );
#else
      this->lb_buffer->pubsync();
      this->lb_buffer->pubseekoff( 0, ios::end );
#endif

      if( this->lb_data.date() ) {
	epoch = time( NULL );
	datesize = strftime( timebuf, 100, this->lb_data.time_format().c_str(), localtime(&epoch) );

	this->lb_buffer->sputn( timebuf, datesize );
	this->lb_current += datesize;

	len = this->lb_data.function().size();
	if( len > 0 ) {
	  if( show_severity ) {
	    this->lb_buffer->sputn( " -", 2 );
	    this->lb_buffer->sputn( lb_s_letterLevels + next_total_level, 1 );
	    this->lb_buffer->sputn( "- ", 2 );
	    this->lb_current += 5;
	  }
	  else {
	    this->lb_buffer->sputn( " - ", 3 );
	    this->lb_current += 3;
	  }

	  this->lb_buffer->sputn( this->lb_data.function().c_str(), len );
	  this->lb_current += len;
	}
	else if( show_severity ) {
	  this->lb_buffer->sputn( " -", 2 );
	  this->lb_buffer->sputn( lb_s_letterLevels + next_total_level, 1 );
	  this->lb_current += 3;
	}
	
	this->lb_buffer->sputn( ": ", 2 );
	this->lb_current += 2;
      }

      this->writeBuffer( n );
      this->lb_current += n;
    }
    else if( !this->lb_data.date() && (n == 1) ) {
      this->writeBuffer( n );
      this->lb_current += n;
    }

    this->pbump( -n );
  }

  this->lb_data.date( !overflow );

#if (__GNUC__<3)
  res = this->lb_buffer->sync();
#else
  res = this->lb_buffer->pubsync();
#endif

  if( this->lb_rotate && this->lb_data.date() && (this->lb_current >= this->lb_maxsize) ) 
    res = this->log_rotate(); // Rotation is done *after* current write and only if we are writing an ending record (flush()ed)

  return res;
}

Logbuf::Logbuf( void ) : streambuf(), lb_remove( false ), lb_rotate( false ), lb_maxfiles( 0 ),
			 lb_current( 0 ), lb_maxsize( 0 ),
			 lb_buffer( cout.rdbuf() ),
			 lb_basename(),
			 lb_data(), lb_bad_file( false )
{
  this->setp( this->lb_data.buffer_base(), this->lb_data.buffer_base() + this->lb_data.buffer_size() );
}

Logbuf::Logbuf( const char *name, level_t lev, const char *format ) : streambuf(), lb_remove( true ), lb_rotate( false ),
								      lb_maxfiles( 0 ), lb_current( 0 ), lb_maxsize( 0 ),
								      lb_buffer( new filebuf ), lb_basename(),
								      lb_data( name, lev, format ), lb_bad_file( false )
{
  if ( utilities::create_file( name ) ) lb_bad_file=true;

  if( dynamic_cast<filebuf *>(this->lb_buffer)->open(name, ios::ate | ios::out | ios::in) ) this->lb_data.bad( false );

  this->setp( this->lb_data.buffer_base(), this->lb_data.buffer_base() + this->lb_data.buffer_size() );

  this->lb_current = this->getBufferSize();
}

Logbuf::Logbuf( streambuf *buffer, level_t lev, const char *format ) : streambuf(), lb_remove( false ), lb_rotate( false ),
								       lb_maxfiles( 0 ), lb_current( 0 ), lb_maxsize( 0 ),
								       lb_buffer( buffer ), lb_data( "", lev, format ),
								       lb_bad_file( false )
{
  this->lb_data.bad( false );

  this->setp( this->lb_data.buffer_base(), this->lb_data.buffer_base() + this->lb_data.buffer_size() );

  this->lb_current = this->getBufferSize();
}

Logbuf::~Logbuf( void )
{
  if( this->lb_remove ) delete this->lb_buffer;
}

Logbuf *Logbuf::open( const char *name, level_t lev, const char *format )
{
  Logbuf   *ret = NULL;

  this->close();

  if( this->lb_buffer == NULL ) this->lb_buffer = new filebuf();

  this->lb_remove = true;

  if (utilities::create_file( name ) ) lb_bad_file=true;
     

  if( dynamic_cast<filebuf *>(this->lb_buffer)->open(name, ios::ate | ios::out | ios::in) ) {
    this->lb_data.reset( name, lev, format );

    ret = this;

    this->lb_current = this->getBufferSize();
  }

  return ret;
}

Logbuf *Logbuf::open( streambuf *buffer, level_t lev, const char *format )
{
  this->close();

  if( (this->lb_buffer != NULL) && this->lb_remove ) delete this->lb_buffer;

  this->lb_buffer = buffer;
  this->lb_remove = false;

  this->lb_data.reset( "", lev, format );

  this->lb_current = this->getBufferSize();

  return this;
}

Logbuf *Logbuf::close( void )
{
  Logbuf    *ret = NULL;

  this->deactivate_log_rotation();

  if( this->lb_remove && dynamic_cast<filebuf *>(this->lb_buffer)->close() ) ret = this;
  else this->lb_buffer = NULL;

  this->lb_data.remove();

  return ret;
}

Logbuf *Logbuf::activate_log_rotation( streamsize maxsize, const string &basename, unsigned int maxfiles )
{
  if( (maxfiles != 0) && (maxsize != 0) && this->checkRotationBuffer() ) {
    this->lb_rotate = true;
    this->lb_maxsize = maxsize;
    this->lb_basename.assign( basename );
    this->lb_maxfiles = maxfiles;
  }

  return this;
}

Logbuf *Logbuf::deactivate_log_rotation( void )
{
  this->lb_rotate = false;

  return this;
}

int Logbuf::log_rotate( void )
try {
  int             res = 0;
  filebuf        *buffer = dynamic_cast<filebuf *>( this->lb_buffer );

  if( this->lb_rotate ) {
    if( buffer ) {
      this->lb_buffer->sputn( "****Begin log file rotation***\n", 31 );

#if (__GNUC__>=3)
      this->lb_buffer->pubsync();
      this->lb_buffer->pubseekpos( 0, (ios::in | ios::out) );
      this->lb_buffer->pubsync();
#else
      this->lb_buffer->sync();
      this->lb_buffer->seekpos( 0, (ios::in | ios::out) );
      this->lb_buffer->sync();
#endif

      unsigned int             file;
      streamsize               nread;
      char                     buf[BUFSIZ];
      string                   name1, name2;
      ofstream                 ofs;
      fs::path  path1, path2;

      for( file = (this->lb_maxfiles - 1); file >= 1; --file ) {
	name1.assign( this->lb_basename ); name2.assign( this->lb_basename );
	name1.append( 1, '.' ); name1.append( boost::lexical_cast<string>(file) );
	name2.append( 1, '.' ); name2.append( boost::lexical_cast<string>(file + 1) );

	path1 = fs::path(name1, fs::native);
	path2 = fs::path(name2, fs::native);

	if( fs::exists(path1) ) {
	  if( fs::exists(path2) ) fs::remove( path2 );
	  fs::rename( path1, path2 );
	}
      }

      ofs.open( path1.native_file_string().c_str(), ios::out ); // Open the copy file...

      while( (nread = this->lb_buffer->sgetn(buf, (BUFSIZ - 1))) != 0 )
	ofs.write( buf, nread );

      ofs.close();

      ftruncate( utilities::bufferdescriptor(*buffer), 0 );

#if (__GNUC__>=3)
      this->lb_buffer->pubseekpos( 0, (ios::in | ios::out) );
#else
      this->lb_buffer->seekpos( 0, (ios::in | ios::out) );
#endif

      this->lb_buffer->sputn( "****Log file truncated****\n", 27 );
#if (__GNUC__>=3)
      res = this->lb_buffer->pubsync();
#else
      res = this->lb_buffer->sync();
#endif

      this->lb_current = 0;
    }
    else {
      this->lb_buffer->sputn( "****Log file rotation unavailable on this stream****\n", 53 );
#if (__GNUC__>=3)
      res = this->lb_buffer->pubsync();
#else
      res = this->lb_buffer->sync();
#endif

      this->lb_current = 0;
    }
  }

  return res;
}
catch( fs::filesystem_error &err ) {
  int res;
  string      error( "****Error got: \"" );

  error.append( err.what() ); error.append( "\"****\n" );

  this->lb_buffer->sputn( "****Log file rotation failed due to filesystem error****\n", 57 );
  this->lb_buffer->sputn( error.c_str(), error.length() );
#if (__GNUC__>=3)
  res = this->lb_buffer->pubsync();
#else
  res = this->lb_buffer->sync();
#endif

  this->lb_current = 0;

  return res;
}

int Logbuf::overflow( int ch )
{
  int ret = 0;
  streamoff   n = static_cast<streamoff>( this->pptr() - this->pbase() );

  if( (n != 0) && this->internalSync(true) ) ret = EOF;

  if( ch != EOF ) {
    bool     do_write( (static_cast<int>(this->lb_data.next_level()) % _last_positive) <= static_cast<int>(this->lb_data.buffer_level()) );

    if( do_write )
      this->sputc( static_cast<char>(ch) );
  }

  return ret;
}

int Logbuf::sync( void ) { return this->internalSync( false ); }

} // logger namespace
} // common namespace
} // wms namespace
} // glite namespace
