/*
Copyright (c) Members of the EGEE Collaboration. 2004. 
See http://www.eu-egee.org/partners/ for details on the copyright
holders.  

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

    http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
See the License for the specific language governing permissions and 
limitations under the License.
*/
// $Id:

#include <cmath>
#include <ctime>
#include <cstdio>
#include <climits>

#include <iomanip>
#include <algorithm>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "glite/wms/common/utilities/streamdescriptor.h"

#include "common/file_container.h"

#ifdef FILELIST_HAS_DEBUG_CODE
#include <unistd.h>
#include <cstdarg>
#endif

using namespace std;
namespace fs = boost::filesystem;

JOBCONTROL_NAMESPACE_BEGIN {
namespace jccommon {

#ifdef FILELIST_HAS_DEBUG_CODE
void logMessage( const char *function, const string &message, const string &filename )
{
  static char timebuf[100];
  size_t      datesize;
  time_t      epoch;
  string      logfile( filename ), date;
  ofstream    ofs;

  logfile.append( ".log" );
  glite::wms::common::utilities::create_file( logfile.c_str() );

  ofs.open( logfile.c_str(), ios::out | ios::app );

  if( ofs.good() ) {
    epoch = time( NULL );
    datesize = strftime( timebuf, 100, "%d %b, %H:%M:%S", localtime(&epoch) );

    date.assign( timebuf, datesize );

    ofs << date << " - " << function << ": " << message << endl;
  }

  return;
}

void throwErrorAndDumpFile( FileContainer &container, FileContainerError::iostatus_t status,
			    const string &func, const string &filename, int line, bool doThrow )
{
  string     wrongname( filename );

  wrongname.append( 1, '.' );
  wrongname.append( boost::lexical_cast<string>(time(NULL)) );
  wrongname.append( 1, '.' );
  wrongname.append( boost::lexical_cast<string>(getpid()) );
  wrongname.append( ".wrong" );

  container.force_backup( wrongname.c_str() );
  container.dump_status( wrongname.c_str(), status, func, line );

  if( doThrow )
    throw FileContainerError( status, func, filename.c_str(), line );
}

FileContainer::StackPusher::StackPusher( vector<string> &callstack, const char *format, ... ) : sp_callstack( callstack )
{
  char      buffer[BUFSIZ * 8];
  va_list   vl;

  va_start( vl, format );
  vsprintf( buffer, format, vl );

  this->sp_callstack.push_back( string(buffer) );
}

FileContainer::StackPusher::~StackPusher( void )
{}

#define  cast_iterator( it )   static_cast<int>(it.position()), static_cast<int>(it.get_prev()), static_cast<int>(it.get_next())

#endif

class FileContainer::TimeStamp {
  inline friend std::ostream &operator<<( std::ostream &os, const TimeStamp &ts ) { return ts.write( os ); }
  inline friend std::istream &operator>>( std::istream &is, TimeStamp &ts ) { return ts.read( is ); }

public:
  TimeStamp( void );

  ~TimeStamp( void );

  std::ostream &write( std::ostream &os ) const;
  std::istream &read( std::istream &is );
  TimeStamp &update_stamp( TimeStamp &ts );

  inline bool operator>( const TimeStamp &ts )
  { return( (this->ts_epoch > ts.ts_epoch) || ((this->ts_epoch == ts.ts_epoch) && (this->ts_counter > ts.ts_counter)) ); }
  inline bool operator<( const TimeStamp &ts )
  { return( (this->ts_epoch < ts.ts_epoch) || ((this->ts_epoch == ts.ts_epoch) && (this->ts_counter < ts.ts_counter)) ); }
  inline bool operator==( const TimeStamp &ts )
  { return( (this->ts_epoch == ts.ts_epoch) && (this->ts_counter == ts.ts_counter) ); }
  inline bool operator!=( const TimeStamp &ts )
  { return( (this->ts_epoch != ts.ts_epoch) || (this->ts_counter != ts.ts_counter) ); }

  inline static int size( void ) { return( ts_s_initialized ? (ts_s_swidth + ts_s_twidth + 1) : 0 ); }

  inline operator bool( void ) const { return this->ts_good; }

private:
  static void initialize( void );

  bool                 ts_good;
  time_t               ts_epoch;
  unsigned short int   ts_counter;

  static bool          ts_s_initialized;
  static int           ts_s_twidth, ts_s_swidth;
};

bool FileContainer::TimeStamp::ts_s_initialized = false;
int  FileContainer::TimeStamp::ts_s_twidth = 0, FileContainer::TimeStamp::ts_s_swidth = 0;

int            FileIterator::fi_s_width = 0;
FileIterator   FileIterator::fi_s_iterator;

const char *FileContainerError::fce_s_errors[] = {
  "Unknown status", "OK", "Cannot open file", "File unavailable for operations",
  "Syntax error on file", "Input/Output error", "Data format error",
  "Position not currently available", "Container modified since last access",
  "Data recovering in progress", "File contains unrecoverable data",
  "Trying to decrementing a zero sized file.", "Not removing last pointer.",
  "Cannot convert string to given data type.",
  "Cannot convert data from string type."
};

int FileContainer::fc_s_stampSize = 0, FileContainer::fc_s_sizeSize = 0;
int FileContainer::fc_s_headerSize = 0, FileContainer::fc_s_backupSize = 0;
int FileContainer::fc_s_iteratorBackupSize = 0, FileContainer::fc_s_limitsBackupSize = 0;
int FileContainer::fc_s_numberSize = 0, FileContainer::fc_s_statusPosition = 0, FileContainer::fc_s_positionPosition = 0;
int FileContainer::fc_s_listBackupSize = 0, FileContainer::fc_s_listPosition = 0;

const int FileContainer::fc_s_iteratorBackupNumber = 4;

namespace {

const int        un_dead = 0xdead, un_beef = 0xbeef;

inline bool isGood( FileContainerError::iostatus_t answer ) 
{
  return( answer == FileContainerError::all_good );
}

inline bool notGood( FileContainerError::iostatus_t answer ) 
{
  return( answer != FileContainerError::all_good );
}

inline streamoff calculateDataSize( string::size_type size, int sizeSize )
{
  return( (4 * FileIterator::size()) + sizeSize + size + 8 );
}

inline streamoff calculateDataSize( const string &data, int sizeSize )
{
  return calculateDataSize( data.size(), sizeSize );
}

int integer_size( size_t size, int basen = 10 )
{
  int         bitnumber = static_cast<int>(log((double)(UCHAR_MAX + 1)) / log(2.0)), answer;
  double      partial = size / sizeof(unsigned char), total = (bitnumber * partial), largest, rapporto;

  largest = pow( 2, total ) - 1;
  rapporto = log( largest ) / log( static_cast<double>(basen) );
  answer = static_cast<int>( rapporto );

  if( (static_cast<double>(answer)) != rapporto ) answer += 1;

  return answer;
}

} // Unnamed namespace closure

/*
  Private methods
*/
void FileContainer::TimeStamp::initialize( void ) // static
{
  ts_s_twidth = integer_size( sizeof(time_t) );
  ts_s_swidth = integer_size( sizeof(unsigned short) );

  ts_s_initialized = true;

  return;
}

void FileIterator::initialize( void ) // static
{
  fi_s_width = integer_size( sizeof(long long), 16 );

  return;
}

void FileContainer::staticInitialize( void ) // static
{
  fc_s_stampSize = TimeStamp::size() + 1;
  fc_s_sizeSize = integer_size( sizeof(string::size_type), 10 ) + 3;
  fc_s_numberSize = integer_size( sizeof(size_t), 10 ) + 1;
  fc_s_statusPosition = fc_s_stampSize + fc_s_numberSize + 1;
  fc_s_iteratorBackupSize = ((fc_s_iteratorBackupNumber * 3) * FileIterator::size()) + (fc_s_iteratorBackupNumber * 3);
  fc_s_listBackupSize = fc_s_limitsBackupSize = (2 * FileIterator::size()) + 2;
  fc_s_backupSize = fc_s_iteratorBackupSize + fc_s_limitsBackupSize + fc_s_listBackupSize + 3;
  fc_s_headerSize = fc_s_stampSize + fc_s_numberSize + fc_s_backupSize + (4 * FileIterator::size()) + 4;
  fc_s_positionPosition = fc_s_stampSize + fc_s_numberSize + fc_s_backupSize;
  fc_s_listPosition = fc_s_positionPosition + fc_s_limitsBackupSize;
}

FileContainerError::iostatus_t FileContainer::openFile( void )
{
  iostatus_t        answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "openFile()" );
#endif

  this->fc_stream = new fstream( this->fc_filename.c_str(), (ios::in | ios::out) );

  if( this->fc_stream->bad() || !this->fc_stream->good() ) { // bad() != !good() !!!!!!!
    delete this->fc_stream; this->fc_stream = NULL;
    answer = FileContainerError::cannot_open;
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::createFile( void )
{
  iostatus_t   answer = FileContainerError::all_good;
  ofstream( this->fc_filename.c_str() ); // Creates the file if it doesn'e exist (gcc 3.2)
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "createFile()" );
#endif

  this->fc_stream = new fstream( this->fc_filename.c_str(), (ios::in | ios::out) );

  if( this->fc_stream->bad() || !this->fc_stream->good() ) { // bad() != !good() !!!
    delete this->fc_stream; this->fc_stream = NULL;
    answer = FileContainerError::cannot_open;
  }
  else answer = this->createEmptyFile();

  return answer;
}

FileContainerError::iostatus_t FileContainer::syncData( bool stamp )
{
  iostatus_t     answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "syncData( stamp = %d )", static_cast<int>(stamp) );
#endif

//  cerr << getpid() << " syncData()" << endl;

  answer = this->readInitialPosition( this->fc_limits );

  if( isGood(answer) ) {
    answer = this->readInitialPosition( this->fc_removed, true );

    if( isGood(answer) ) {
      answer = this->readSize();

      if( stamp && (isGood(answer)) ) answer = this->readStamp();
    }
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::createEmptyFile( void )
{
  iostatus_t   answer = FileContainerError::all_good;
  TimeStamp    stamp;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "createEmptyFile()" );
#endif

  *this->fc_stamp = stamp;
  answer = this->writeStamp();

  if( isGood(answer) ) {
    answer = this->writeAndSetSize( 0 );

    if( isGood(answer) ) {
      answer = this->writeFileStatus( opened );

      if( isGood(answer) ) {
	this->fc_limits.reset( fc_s_headerSize, fc_s_headerSize );
	this->fc_removed.reset( un_dead, un_beef );

	if( isGood(answer) ) {
	  answer = this->createEmptyBackup( empty );

	  if( isGood(answer) ) {
	    answer = this->writeInitialPosition( this->fc_limits );

	    if( isGood(answer) ) 
	      answer = this->writeInitialPosition( this->fc_removed, true );
	  }
	}

	if( isGood(answer) )
	  answer = this->writeFileStatus( closed );
      }
    }
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::truncateFile( off_t size )
{
  int                       fd;
  iostatus_t                answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  const char       *function = "FileContainer::truncateFile(...)";
  string            message( "Asked a truncation at size: " );
  StackPusher       stack_pusher( this->fc_callStack, "truncateFile( size = %d )", static_cast<int>(size) );

  message.append( boost::lexical_cast<string>(size) );
  logMessage( function, message, this->fc_filename );
#endif

  this->fc_stream->seekg( 0 ); this->fc_stream->seekp( 0 );

  if( this->fc_stream->good() ) {
    fd = glite::wms::common::utilities::streamdescriptor( *this->fc_stream );

    if( ftruncate(fd, size) ) answer = FileContainerError::io_error;
    else if( size == 0 ) answer = this->createEmptyFile();	

    if( isGood(answer) ) answer = this->writeFileStatus( ft_end );
  }
  else answer = FileContainerError::io_error;

  return answer;
}

FileContainerError::iostatus_t FileContainer::eraseFile( off_t size )
{
  iostatus_t                answer = FileContainerError::all_good;
  FileIterator              sizeIt( size, size, size );
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "eraseFile( size = %d )", static_cast<int>(size) );
#endif

  answer = this->writeIteratorBackup( 0, sizeIt, ft_begin );

  if( isGood(answer) ) answer = this->truncateFile( size );

  return answer;
}

FileContainerError::iostatus_t FileContainer::writeFileStatus( filestatus_t status )
{
  char        stat = static_cast<char>( status );
  iostatus_t  answer = FileContainerError::all_good;
  streamoff   old = this->fc_stream->tellp();
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "writeFileStatus( status = %d )", static_cast<int>(status) );
#endif

  this->fc_stream->sync();
  if( this->fc_stream->bad() ) answer = FileContainerError::io_error;
  else {
    this->fc_stream->seekp( fc_s_statusPosition );
    this->fc_stream->put( stat ).put( '\n' ).flush();
    this->fc_stream->seekp( old );

    this->fc_stream->sync();

    if( this->fc_stream->bad() ) answer = FileContainerError::io_error;
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::readFileStatus( filestatus_t &status )
{
  char                 buffer[2];
  iostatus_t           answer = FileContainerError::all_good;
  streamoff            old = this->fc_stream->tellg();
  string               sbuf;
  static boost::regex  expression( "^[ 01a-y]\n$" );
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "readFileStatus( status = %d )", static_cast<int>(status) );
#endif

  this->fc_stream->sync();

  this->fc_stream->seekg( fc_s_statusPosition );
  this->fc_stream->read( buffer, 2 );
  sbuf.assign( buffer, 2 );
  this->fc_stream->seekg( old );

  if( this->fc_stream->bad() ) answer = FileContainerError::io_error;
  else {
    if( boost::regex_match(sbuf, expression) ) status = static_cast<filestatus_t>( buffer[0] );
    else answer = FileContainerError::syntax_error;
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::readSize( void )
{
  iostatus_t     answer = FileContainerError::all_good;
  streamoff      old = this->fc_stream->tellg();
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "readSize()" );
#endif

  this->fc_size = 0;

  this->fc_stream->seekg( fc_s_stampSize );
  *this->fc_stream >> this->fc_size;
  this->fc_stream->seekg( old );

  if( this->fc_stream->bad() ) answer = FileContainerError::io_error;

  return answer;
}

FileContainerError::iostatus_t FileContainer::writeAndSetSize( size_t size )
{
  iostatus_t      answer = FileContainerError::all_good;
  streamoff       old = this->fc_stream->tellp();
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "writeAndSetSize( size = %d )", static_cast<int>(size) );
#endif

  this->fc_stream->seekp( fc_s_stampSize );
  *this->fc_stream << setfill( '0' ) << setw( fc_s_numberSize ) << size << endl;
  this->fc_stream->seekp( old );

  if( this->fc_stream->bad() ) answer = FileContainerError::io_error;

  if( isGood(answer) ) this->fc_size = size;

  return answer;
}

FileContainerError::iostatus_t FileContainer::createEmptyBackup( filestatus_t state )
{
  int             i;
  iostatus_t      answer = FileContainerError::all_good;
  streamoff       old = this->fc_stream->tellp();
  FileIterator    vuoto;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "createEmptyBackup()" );
#endif

  this->fc_stream->seekp( fc_s_statusPosition + 2 );

  for( i = 0; i < fc_s_iteratorBackupNumber; ++i )
    *this->fc_stream << setfill( '0' ) << hex << setw( FileIterator::size() ) << 0 
		     << ' ' << dec << vuoto << '\n';

  *this->fc_stream << vuoto << '\n' << vuoto << '\n';
  this->fc_stream->flush();
  this->fc_stream->seekp( old );

  if( this->fc_stream->bad() ) answer = FileContainerError::io_error;
  else answer = this->writeFileStatus( state );

  return answer;
}

FileContainerError::iostatus_t FileContainer::writeIteratorBackup( int what, const FileIterator &it, filestatus_t status )
{
  iostatus_t    answer = FileContainerError::all_good;
  streamoff     position = fc_s_statusPosition + 2, toadd = (3 * FileIterator::size()) + 3, old = this->fc_stream->tellp();
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, 
				  "writeIteratorBackup( what = %d, it = (%d, %d, %d), status = %d )",
				  what, cast_iterator(it), static_cast<int>(status) );
#endif

  what %= fc_s_iteratorBackupNumber; position += what * toadd;

  this->fc_stream->seekp( position );
  *this->fc_stream << setfill( '0' ) << setw( FileIterator::size() )
		   << hex << it.position() << dec << ' '
		   << it << endl;
  this->fc_stream->seekp( old );

  if( this->fc_stream->bad() ) answer = FileContainerError::io_error;
  else answer = this->writeFileStatus( status );

  return answer;
}

FileContainerError::iostatus_t FileContainer::readIteratorBackup( int what, FileIterator &it )
{
  iostatus_t            answer = FileContainerError::all_good;
  streamoff             pointer;
  streamoff             position = fc_s_statusPosition + 2, toadd = (3 * FileIterator::size()) + 3, old = this->fc_stream->tellg();
  string                buffer;
  static boost::regex   expression( "^[0-9a-fA-F]+ [0-9a-fA-F]+ [0-9a-fA-F]+$" );
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, 
				  "readIteratorBackup( what = %d, it = (%d, %d, %d) )",
				  what, cast_iterator(it) );
#endif

  what %= fc_s_iteratorBackupNumber; position += what * toadd;

  this->fc_stream->seekg( position );
  getline( *this->fc_stream, buffer );
  this->fc_stream->seekg( position );

  if( this->fc_stream->good() ) {
    if( boost::regex_match(buffer, expression) ) {
      *this->fc_stream >> hex >> pointer >> dec >> it;
      this->fc_stream->seekg( old );

      if( this->fc_stream->good() ) it.set_current( pointer );
      else answer = FileContainerError::io_error;
    }
    else answer = FileContainerError::syntax_error;
  }
  else answer = FileContainerError::io_error;

  return answer;
}

FileContainerError::iostatus_t FileContainer::writeLimitsBackup( filestatus_t status, const FileIterator &limits, bool isList )
{
  iostatus_t    answer = FileContainerError::all_good; 
  streamoff     position = fc_s_statusPosition + fc_s_iteratorBackupSize + 2 + (isList * (fc_s_limitsBackupSize));
  streamoff     old = this->fc_stream->tellp();
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, 
				  "writeLimitsBackup( status = %d, limits = (%d, %d, %d), isList = %d )",
				  static_cast<int>(status), cast_iterator(limits), static_cast<int>(isList) );
#endif

  this->fc_stream->seekp( position );
  *this->fc_stream << limits << endl;
  this->fc_stream->seekp( old );

  if( this->fc_stream->bad() ) answer = FileContainerError::io_error;
  else answer = this->writeFileStatus( status );

  return answer;
}

FileContainerError::iostatus_t FileContainer::readLimitsBackup( FileIterator &limits, bool isList )
{
  iostatus_t    answer = FileContainerError::all_good;
  streamoff     position = fc_s_statusPosition + fc_s_iteratorBackupSize + 2 + (isList * (fc_s_limitsBackupSize));
  streamoff     old = this->fc_stream->tellg();
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, 
				  "readLimitsBackup( limits = (%d, %d, %d), isList = %d )",
				  cast_iterator(limits), static_cast<int>(isList) );
#endif

  this->fc_stream->seekg( position );
  *this->fc_stream >> limits;
  this->fc_stream->seekg( old );

  if( this->fc_stream->bad() ) answer = FileContainerError::io_error;
  else if( !limits ) answer = FileContainerError::syntax_error;

  return answer;
}

FileContainerError::iostatus_t FileContainer::writeStamp( void )
{
  iostatus_t    answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "writeStamp()" );
#endif

  this->fc_stream->seekp( 0 );
  *this->fc_stream << *this->fc_stamp << endl;
  this->fc_stream->sync();

  if( this->fc_stream->bad() ) answer = FileContainerError::file_closed;

  return answer;
}

FileContainerError::iostatus_t FileContainer::readStamp( void )
{
  iostatus_t    answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "readStamp()" );
#endif

  this->fc_stream->sync();
  this->fc_stream->seekg( 0 );
  *this->fc_stream >> *this->fc_stamp;

  if( this->fc_stream->bad() ) answer = FileContainerError::file_closed;

  return answer;
}

FileContainerError::iostatus_t FileContainer::writeInitialPosition( const FileIterator &position, bool isList )
{
  iostatus_t    answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "writeInitialPosition( position = (%d, %d, %d), isList = %d )",
				  cast_iterator(position), static_cast<int>(isList) );
#endif

  this->fc_stream->sync();
  this->fc_stream->seekp( isList ? fc_s_listPosition : fc_s_positionPosition );
  *this->fc_stream << position << endl;

  if( this->fc_stream->bad() ) answer = FileContainerError::io_error;

  return answer;
}

FileContainerError::iostatus_t FileContainer::readInitialPosition( FileIterator &limits, bool isList )
{
  iostatus_t    answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "readInitialPosition( limits = (%d, %d, %d), isList = %d )",
				  cast_iterator(limits), static_cast<int>(isList) );
#endif

  this->fc_stream->sync();
  this->fc_stream->seekg( isList ? fc_s_listPosition : fc_s_positionPosition );
  *this->fc_stream >> limits;

  if( limits ) {
    if( this->fc_stream->bad() ) answer = FileContainerError::io_error;
  }
  else answer = FileContainerError::syntax_error;

  return answer;
}

FileContainerError::iostatus_t FileContainer::updateTimeStamp( void )
{
  iostatus_t   answer = FileContainerError::all_good;
  TimeStamp   stamp;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "updateTimeStamp()" );
#endif

  this->fc_stream->sync();
  this->fc_stream->seekg( 0 );
  *this->fc_stream >> stamp;

//   cerr << getpid() << ' ' << stamp << ' ' << *this->fc_stamp << endl;

  if( stamp ) {
    this->fc_stamp->update_stamp( stamp );

    this->fc_stream->seekp( 0 );
    *this->fc_stream << *this->fc_stamp << endl;

    if( this->fc_stream->bad() ) answer = FileContainerError::file_closed;
  }
  else answer = FileContainerError::syntax_error;

  return answer;
}

FileContainerError::iostatus_t FileContainer::checkStream( bool recover )
{
  iostatus_t    answer = FileContainerError::all_good;
  filestatus_t  status;
#ifdef FILELIST_HAS_DEBUG_CODE
  const char   *function = "FileContainer::checkStream(...)";
  StackPusher       stack_pusher( this->fc_callStack, "checkStream( recover = %d )", static_cast<int>(recover) );
#endif

  if( (this->fc_stream == NULL) || this->fc_stream->bad() ) answer = FileContainerError::file_closed;
  else {
    answer = this->readFileStatus( status );

    if( recover && (isGood(answer)) && (status != closed) ) {
#ifdef FILELIST_HAS_DEBUG_CODE
      string      message( "Wrong file status found, was: \'" );
      message.append( 1, static_cast<char>(status) );
      message.append( "\'. Going to recover." );

      logMessage( function, message, this->fc_filename );

      if( !this->fc_callStack.empty() ) {
	message.assign( "Current call stack:" );

	for( vector<string>::iterator vit = this->fc_callStack.begin();
	     vit != this->fc_callStack.end(); ++vit ) {
	  message.append( " -> " );
	  message.append( *vit );
	}

	logMessage( function, message, this->fc_filename );
      }
#endif

      answer = this->recover_data( status );
    }
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::checkStreamAndStamp( bool recover )
{
  bool         modified;
  iostatus_t   answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "checkStreamAndStamp( recover = %d )", static_cast<int>(recover) );
#endif

  answer = this->checkStream( recover );
  if( isGood(answer) ) {
    answer = this->checkStamp( modified );

    if( (isGood(answer)) && modified ) answer = FileContainerError::container_modified;
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::checkStamp( bool &modified )
{
  iostatus_t    answer = FileContainerError::all_good;
  TimeStamp     stamp;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "checkStamp( &modified = %d )", static_cast<int>(modified) );
#endif

  this->fc_stream->sync();
  this->fc_stream->seekg( 0 );
  *this->fc_stream >> stamp;

  if( stamp ) {
    if( this->fc_stream->good() ) modified = (stamp > *this->fc_stamp);
    else answer = FileContainerError::file_closed;
  }
  else answer = FileContainerError::syntax_error;

  return answer;
}

FileContainerError::iostatus_t FileContainer::readIteratorHere( FileIterator &it )
{
  iostatus_t    answer = FileContainerError::all_good;
  streamoff     here = this->fc_stream->tellg();
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "readIteratorHere( it = (%d, %d, %d) )", cast_iterator(it) );
#endif

  *this->fc_stream >> it;

  if( it ) {
    if( this->fc_stream->get() != '\n' ) answer = FileContainerError::syntax_error;
    else if( this->fc_stream->good() ) it.set_current( here );
    else answer = FileContainerError::io_error;
  }
  else answer = FileContainerError::syntax_error;

  return answer;
}

FileContainerError::iostatus_t FileContainer::readIterator( streamoff where, FileIterator &it, bool fileorder )
{
  char               state;
  iostatus_t         answer = FileContainerError::all_good;
  streamoff          end = this->getEnd();
  string::size_type  size;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "readIterator( where = %d, it = (%d, %d, %d), fileorder = %d )",
				  static_cast<int>(where), cast_iterator(it), static_cast<int>(fileorder) );
#endif

  if( (where >= fc_s_headerSize) && (where < end) ) {
    this->fc_stream->seekg( where );

    answer = this->readIteratorHere( it );

    if( isGood(answer) && fileorder ) {
      answer = this->readSizeAndState( size, state );

      if( isGood(answer) ) {
	this->fc_stream->seekg( size + 1, ios::cur );

	answer = this->readIteratorHere( it );

	if( isGood(answer) ) it.set_current( where );
      }
    }
  }
  else if( where == end ) it.reset( 0, 0, end );
  else if( where == 0 ) it.reset();
  else answer = FileContainerError::unavailable_position;

  return answer;
}

FileContainerError::iostatus_t FileContainer::writeIterator( streamoff where, const FileIterator &it )
{
  iostatus_t    answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "writeIterator( where = %d, it = (%d, %d, %d) )",
				  static_cast<int>(where), cast_iterator(it) );
#endif

  if( (where >= fc_s_headerSize) || (where < this->getEnd()) ) {
    this->fc_stream->seekp( where );
    *this->fc_stream << it << endl;
    this->fc_stream->sync();

    if( this->fc_stream->bad() ) answer = FileContainerError::io_error;
  }
  else answer = FileContainerError::unavailable_position;

  return answer;
}

FileContainerError::iostatus_t FileContainer::resetNextOfLast( filestatus_t status )
{
  iostatus_t        answer = FileContainerError::all_good;
  FileIterator      iter;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "resetNextOfLast( status = %d )", static_cast<int>(status) );
#endif

  answer = this->readIterator( this->fc_limits.get_next(), iter );
  if( isGood(answer) ) {
    answer = this->writeIteratorBackup( 2, iter, status );

    if( isGood(answer) ) {

      iter.set_next( this->getEnd() );

      answer = this->writeIterator( this->fc_limits.get_next(), iter );
    }
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::readSizeAndState( string::size_type &size, char &state )
{
  iostatus_t             answer = FileContainerError::all_good;
  streamoff              position;
  string                 buffer;
  static boost::regex    expression( "^[0-9]+ +(g|i)$" );
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "readSizeAndState( size = %d, state = %c )",
				  static_cast<int>(size), static_cast<char>(state) );
#endif

  position = this->fc_stream->tellg();

  getline( *this->fc_stream, buffer );

  if( boost::regex_match(buffer, expression) ) {
    this->fc_stream->seekg( position );

    *this->fc_stream >> size >> state;

    if( this->fc_stream->bad() ) answer = FileContainerError::io_error;
    else if( (this->fc_stream->get() != '\n') ) answer = FileContainerError::syntax_error;
  }
  else answer = FileContainerError::syntax_error;

  return answer;
}

FileContainerError::iostatus_t FileContainer::removeDataPointer( const FileIterator &iter, string::size_type size )
{
  iostatus_t          answer = FileContainerError::all_good;
  streamoff           end = this->getEnd();
  FileIterator        temp, prev, next;
#ifdef FILELIST_HAS_DEBUG_CODE
  const char       *function = "FileContainer::removeDataPointer(...)";
  StackPusher       stack_pusher( this->fc_callStack, "removeDataPointer( iter = (%d, %d, %d), size = %d )",
				  cast_iterator(iter), static_cast<int>(size) );
#endif

  if( (iter.get_next() == end) && (iter.get_prev() == 0) ) { // Removing the last object in the file...
    if( this->fc_size != 1 ) {
#ifdef FILELIST_HAS_DEBUG_CODE
      string   message( "Removing last object, but container size is: " );
      message.append( boost::lexical_cast<string>(this->fc_size) );
      logMessage( function, message, this->fc_filename );

      throwErrorAndDumpFile( *this, answer, string("FileContainer::removeDataPointer(...)"), this->fc_filename, __LINE__, false );
#endif
      answer = this->checkConsistency( 1 - this->fc_size );

      if( notGood(answer) ) answer = FileContainerError::not_removing_last;
    }

    if( isGood(answer) ) {
      answer = this->eraseFile( fc_s_headerSize );

      if( isGood(answer) ) {
	this->fc_limits.reset( fc_s_headerSize, fc_s_headerSize );
	this->fc_removed.reset( un_dead, un_beef );

	answer = this->writeInitialPosition( this->fc_limits );

	if( isGood(answer) ) answer = this->writeInitialPosition( this->fc_removed, true );
      }
    }
    else answer = FileContainerError::not_removing_last;
  }
  else if( iter.get_next() == end ) { // Removing the last object in the list...
    answer = this->writeLimitsBackup( rd_limits, this->fc_limits );

    if( isGood(answer) ) {
      this->fc_limits.set_next( iter.get_prev() );

      answer = this->writeInitialPosition( this->fc_limits );

      if( isGood(answer) ) {
	answer = this->readIterator( iter.get_prev(), temp );

	if( isGood(answer) ) {
	  answer = this->writeIteratorBackup( 0, temp, rd_iterator );

	  if( isGood(answer) ) {
	    temp.set_next( end );

	    answer = this->writeIterator( temp.position(), temp );

	    if( isGood(answer) ) answer = this->markDataAsErased( iter, size, rd_mark1 );
	  }
	}
      }
    }
  }
  else if( iter.get_prev() == 0 ) { // Removing the first object in the list...
    answer = this->writeLimitsBackup( rd_limits, this->fc_limits );

    if( isGood(answer) ) {
      this->fc_limits.set_prev( iter.get_next() );

      answer = this->writeInitialPosition( this->fc_limits );

      if( isGood(answer) ) {
	answer = this->readIterator( iter.get_next(), temp );

	if( isGood(answer) ) {
	  answer = this->writeIteratorBackup( 0, temp, rd_iterator );

	  if( isGood(answer) ) {
	    temp.set_prev( 0 );

	    answer = this->writeIterator( temp.position(), temp );

	    if( isGood(answer) ) answer = this->markDataAsErased( iter, size, rd_mark1 );
	  }
	}
      }
    }
  }
  else { // Removing an object in the middle
    answer = this->readIterator( iter.get_next(), next );

    if( isGood(answer) ) {
      answer = this->readIterator( iter.get_prev(), prev );

      if( isGood(answer) ) {
	answer = this->writeIteratorBackup( 0, next, rd_iterator2 );

	if( isGood(answer) ) {
	  answer = this->writeIteratorBackup( 1, prev, rd_iterator3);

	  if( isGood(answer) ) {
	    next.set_prev( prev.position() );
	    prev.set_next( next.position() );

	    answer = this->writeIterator( next.position(), next );

	    if( isGood(answer) ) {
	      answer = this->writeIterator( prev.position(), prev );

	      if( isGood(answer) ) answer = this->markDataAsErased( iter, size, rd_mark2 );
	    }
	  }
	}
      }
    }
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::markDataAsErased( const FileIterator &iter, string::size_type size, filestatus_t status )
{
  iostatus_t    answer = FileContainerError::all_good;
  FileIterator  fIt( iter );
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "markDataAsErased( iter = (%d, %d, %d), size = %d, status = %d )",
				  cast_iterator(iter), static_cast<int>(size), static_cast<int>(status) );
#endif

  answer = this->writeIteratorBackup( 2, iter, status );

  if( isGood(answer) ) {
    this->fc_stream->seekp( iter.position() );
    if( this->fc_stream->good() ) {
      answer = this->writeDataHeader( iter, size, 'i' );
  
      if( isGood(answer) ) {
	this->fc_stream->sync();

	if( this->fc_stream->bad() ) answer = FileContainerError::io_error;
      }
    }
    else answer = FileContainerError::io_error;
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::markDataAsUnerased( const FileIterator &it )
{
  char               state;
  iostatus_t         answer = FileContainerError::all_good;
  string::size_type  size;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "markDataAsUnrased( it = (%d, %d, %d) )", cast_iterator(it) );
#endif

  this->fc_stream->seekg( it.position() + (2 * FileIterator::size() + 2) );
  if( this->fc_stream->good() ) { 
    answer = this->readSizeAndState( size, state );

    if( isGood(answer) ) {
      this->fc_stream->seekp( it.position() );
      if( this->fc_stream->good() ) answer = this->writeDataHeader( it, size, 'g' );
      else answer = FileContainerError::io_error;
    }
  }
  else answer = FileContainerError::io_error;

  return answer;
}

FileContainerError::iostatus_t FileContainer::writeDataHeader( const FileIterator &it, string::size_type size, char state )
{
  iostatus_t    answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "writeDataHeader( it = (%d, %d, %d), size = %d, state = %c",
				  cast_iterator(it), static_cast<int>(size), state );
#endif

  *this->fc_stream << it << endl;
  *this->fc_stream << setfill( '0' ) << setw( fc_s_sizeSize ) << size << ' ' << state << endl;

  if( this->fc_stream->bad() ) answer = FileContainerError::io_error;

  return answer;
}

FileContainerError::iostatus_t FileContainer::writeDataHere( const FileIterator &it, const FileIterator &lit, 
							     const std::string &data, filestatus_t status )
{
  iostatus_t    answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, 
				  "writeDataHere( it = (%d, %d, %d), lit = (%d, %d, %d), data = \"%s\", status = %d )",
				  cast_iterator(it), cast_iterator(lit), data.substr(0,BUFSIZ).c_str(), static_cast<int>(status) );
#endif

  if( data.size() > 0 ) {
    this->fc_stream->seekp( 0, ios::end );

    answer = this->writeFileStatus( status );

    if( isGood(answer) ) {
      answer = this->writeDataHeader( it, data.size(), 'g' );

      if( isGood(answer) ) {
	this->fc_stream->write( data.c_str(), data.size() ) << '\n' << lit << endl;
	this->fc_stream->sync();

	if( this->fc_stream->bad() ) answer = FileContainerError::io_error;
      }
    }
  }
  else answer = FileContainerError::data_error;

  if( isGood(answer) ) answer = this->writeFileStatus( static_cast<filestatus_t>(static_cast<int>(status) + 1) );

  return answer;
}

FileContainerError::iostatus_t FileContainer::addDataAtBegin( const string &data, FileIterator &it, streamoff end )
{
  iostatus_t    answer = FileContainerError::all_good;
  streamoff     prev = 0, next = this->getBegin();
  streamoff     previous = this->fc_limits.get_next(), last = end + calculateDataSize( data, fc_s_sizeSize );
  FileIterator  temp;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, 
				  "addDataAtBegin( data = \"%s\", it = (%d, %d, %d), end = %d )",
				  data.substr(0,BUFSIZ).c_str(), cast_iterator(it), static_cast<int>(end) );
#endif

  answer = this->readIterator( next, temp );

  if( isGood(answer) ) {
    answer = this->writeIteratorBackup( 0, temp, ab_iterator );

    if( isGood(answer) ) {
      temp.set_prev( end );

      answer = this->writeIterator( next, temp );

      if( isGood(answer) ) {
	answer = this->writeLimitsBackup( ab_limits, this->fc_limits );

	if( isGood(answer) ) {
	  this->fc_limits.set_prev( end );
	  answer = this->writeInitialPosition( this->fc_limits );

	  if( isGood(answer) ) {
	    it.reset( prev, next, end );
	    temp.reset( previous, last, end );

	    answer = this->writeDataHere( it, temp, data, ab_writing );

	    if( isGood(answer) ) answer = this->resetNextOfLast( ab_last );
	  }
	}
      }
    }
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::addDataAtEnd( const string &data, FileIterator &it, streamoff end )
{
  iostatus_t    answer = FileContainerError::all_good;
  streamoff     prev = 0, next = end + calculateDataSize( data, fc_s_sizeSize );
  streamoff     previous = this->fc_limits.get_next();
  FileIterator  temp;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, 
				  "addDataAtEnd( data = \"%s\", it = (%d, %d, %d), end = %d )",
				  data.substr(0,BUFSIZ).c_str(), cast_iterator(it), static_cast<int>(end) );
#endif

  if( this->fc_limits.get_next() == end ) prev = 0;
  else {
    answer = this->readIterator( this->fc_limits.get_next(), temp );

    if( isGood(answer) ) {
      prev = this->fc_limits.get_next();

      answer = this->writeIteratorBackup( 0, temp, ae_iterator );

      if( isGood(answer) ) {
	temp.set_next( end );
	answer = this->writeIterator( this->fc_limits.get_next(), temp );
      }
    }
  }

  if( isGood(answer) ) {
    answer = this->writeLimitsBackup( ae_limits, this->fc_limits );

    if( isGood(answer) ) {
      this->fc_limits.set_next( end );
      answer = this->writeInitialPosition( this->fc_limits );

      if( isGood(answer) ) {
	it.reset( prev, next, end );
	temp.reset( previous, next, end );

	if( isGood(answer) ) answer = this->writeDataHere( it, temp, data, ae_writing );
      }
    }
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::addDataInMiddle( const string &data, FileIterator &it, streamoff where, streamoff end )
{
  iostatus_t      answer = FileContainerError::all_good;
  streamoff       prev, next, previous = this->fc_limits.get_next(), last = end + calculateDataSize( data, fc_s_sizeSize );
  FileIterator    temp1, temp2;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, 
				  "addDataAtInMiddle( data = \"%s\", it = (%d, %d, %d), where = %d, end = %d )",
				  data.substr(0,BUFSIZ).c_str(), cast_iterator(it), static_cast<int>(where), static_cast<int>(end) );
#endif

  answer = this->readIterator( where, temp1 );

  if( isGood(answer) ) {
    answer = this->readIterator( temp1.get_prev(), temp2 );

    if( isGood(answer) ) {
      prev = temp1.get_prev();
      next = where;

      answer = this->writeIteratorBackup( 0, temp1, am_iterator1 );
      if( isGood(answer) ) answer = this->writeIteratorBackup( 1, temp2, am_iterator2 );

      if( isGood(answer) ) {
	temp1.set_prev( end );
	temp2.set_next( end );

	answer = this->writeIterator( where, temp1 );
	if( isGood(answer) ) {

	  answer = this->writeIterator( temp2.position(), temp2 );

	  if( isGood(answer) ) {
	    it.reset( prev, next, end );
	    temp1.reset( previous, last, end );

	    answer = this->writeDataHere( it, temp1, data, am_writing );

	    if( isGood(answer) )
	      answer = this->resetNextOfLast( am_last );
	  }
	}
      }
    }
  }

  return answer;    
}

FileContainerError::iostatus_t FileContainer::readDataHere( FileIterator &it, FileIterator &itList, string &data, bool force )
{
  char                   state, cbuf[BUFSIZ];
  iostatus_t             answer = FileContainerError::all_good;
  streamsize             read, toread;
  string::size_type      size;
  FileIterator           tmp;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, 
				  "readDataHere( it = (%d, %d, %d), itList = (%d, %d, %d ), data, force = %d )",
				  cast_iterator(it), cast_iterator(itList), static_cast<int>(force) );
#endif

  data.erase();

  answer = this->readIteratorHere( it );

  if( isGood(answer) ) {
    answer = this->readSizeAndState( size, state );

    if( isGood(answer) ) {
      if( (state == 'g') || (force && (state == 'i')) ) {
	while( size > 0 ) {
	  toread = ( (size > BUFSIZ) ? BUFSIZ : size );
	  read = this->fc_stream->read( cbuf, toread ).gcount();

	  data.append( cbuf, read );

	  if( read == toread ) size -= toread;
	  else if( (read > 0) && (read < toread) ) size = 0;
	}

	if( this->fc_stream->get() != '\n' ) {
	  data.erase();
	  answer = FileContainerError::syntax_error;
	}
	else {
	  *this->fc_stream >> itList;

	  if( this->fc_stream->bad() || !this->fc_stream->good() ) answer = FileContainerError::io_error;
	  else if( !itList ) answer = FileContainerError::syntax_error;
	}
      }
      else if( state == 'i' ) answer = FileContainerError::unavailable_position;
      else answer = FileContainerError::syntax_error;
    }
  }
  else answer = FileContainerError::syntax_error;

  return answer;
}

FileContainerError::iostatus_t FileContainer::backupFile( const char *backupfile )
{
  iostatus_t    answer = FileContainerError::all_good;
  streamsize    nread;
  char          buffer[BUFSIZ];
  string        backup( backupfile ? backupfile : this->fc_filename );
  ofstream      bakstr;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "backupFile( backupfile = \"%s\" )", backupfile );
#endif
  if( backupfile == NULL ) backup.append( ".bak" );

  bakstr.open( backup.c_str() );
  this->fc_stream->seekg( 0 );

  while( this->fc_stream->good() && !this->fc_stream->eof() ) {
    nread = this->fc_stream->read( buffer, BUFSIZ ).gcount();

    if( nread > 0 ) {
      bakstr.write( buffer, nread );
      if( bakstr.bad() ) break;
    }
  }

  if( bakstr.bad() || (this->fc_stream->bad() && !this->fc_stream->eof()) )
    answer = FileContainerError::io_error;
  else {
    bakstr.close();

    this->fc_stream->clear();
    this->fc_stream->seekp( 0 ); this->fc_stream->seekg( 0 );
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::fillVector( vector<string> &vec )
{
  iostatus_t          answer = FileContainerError::all_good;
  streamoff           where, end = this->getEnd();
  FileIterator        it, unused;
  string              data;
#ifdef FILELIST_HAS_DEBUG_CODE
  StackPusher       stack_pusher( this->fc_callStack, "fillVector( vec )" );
#endif

  answer = this->checkStreamAndStamp();
  if( answer == FileContainerError::container_modified ) answer = this->syncData();

  if( isGood(answer) ) {
    answer = this->readIterator( this->fc_limits.get_prev(), it );
    if( isGood(answer) ) {
      vec.clear();

      where = it.position();
      while( isGood(answer) && (where < end) ) {
	this->fc_stream->seekg( where );

	answer = this->readDataHere( it, unused, data );
	if( isGood(answer) ) {
	  vec.push_back( data );

	  where = it.get_next();
	}
      }
    }
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::initContainer( void )
{
  iostatus_t      answer = FileContainerError::all_good;
  filestatus_t    status;
#ifdef FILELIST_HAS_DEBUG_CODE
  const char     *function = "FileContainer::initContainer()";
  StackPusher       stack_pusher( this->fc_callStack, "initContainer()" );
#endif

  if( (this->fc_stream == NULL) || (this->fc_stream->bad()) ) answer = FileContainerError::file_closed;
  else {
    answer = this->readStamp();

    if( isGood(answer) ) {
      answer = this->readInitialPosition( this->fc_limits );

      if( isGood(answer) ) {
	answer = this->readInitialPosition( this->fc_removed, true );

	if( isGood(answer) ) {
	  answer = this->readSize();

	  if( isGood(answer) ) {
	    answer = this->readFileStatus( status );

	    if( isGood(answer) ) {
	      if( status != closed ) {
#ifdef FILELIST_HAS_DEBUG_CODE
		string    message( "Wrong file status found, was: \'" );
		message.append( 1, static_cast<char>(status) );
		message.append( "\'. Going to recover." );

		logMessage( function, message, this->fc_filename );

		if( !this->fc_callStack.empty() ) {
		  message.assign( "Current call stack:" );

		  for( vector<string>::iterator vit = this->fc_callStack.begin();
		       vit != this->fc_callStack.end(); ++vit ) {
		    message.append( " -> " );
		    message.append( *vit );
		  }

		  logMessage( function, message, this->fc_filename );
		}
#endif

		answer = this->recover_data( status, false );
	      }

	      if( isGood(answer) ) this->fc_initialized = true;
	    }
	  }
	}
      }
    }
  }

  return answer;
}

/*
 *  Thanks to the courtesy of Francesco Prelz
 *  This function will make a consistency check on the container
 *  after a recovery operation.
 */
FileContainerError::iostatus_t FileContainer::checkConsistency( int allowable_size_offset )
{
  iostatus_t     answer = FileContainerError::all_good;
  streamoff      max_reached_offset = 0;
  // We need to know the physical end of the file, as of now.
  streamoff      end = this->getEnd();
  size_t         detected_size = 0;
#ifdef FILELIST_HAS_DEBUG_CODE
  const char     *function = "FileContainer::checkConsistency(...)";
  string          message( "Called with allowable_size_offset = " );
  StackPusher     stack_pusher( this->fc_callStack, "checkConsistency( allowable_size_offset = %d )", allowable_size_offset );

  message.append( boost::lexical_cast<string>(allowable_size_offset) );
  logMessage( function, message, this->fc_filename );
#endif
 
  // Make sure the filelist size (# of elements) as found in the file.
  // is up-to-date in fc_size;
  // Rosario: maybe this is not needed...
  answer = this->readSize();
  if( !isGood(answer) ) return answer;
  
  // Traverse filelist starting from current 'top' limit. 
  // File access is established and recovery is supposedly complete.
  FileIterator   it;

  // Main element loop.
  for( answer = this->readIterator(this->fc_limits.get_prev(), it);
       isGood(answer);
       answer = this->readIterator(it.get_next(), it) ) {

    // Let's not assume we are positioned at the end of
    // the current list element, and try to get that figure explicitely
    char               state;
    string::size_type  size;
    streamoff          current_maximum_offset;

    //    this->fc_stream->seekg( it.position() );
    if( this->fc_stream->good() ) answer = this->readSizeAndState( size, state );
    else answer = FileContainerError::io_error;

    if( isGood(answer) ) {
      current_maximum_offset = it.position() + calculateDataSize ( size, fc_s_sizeSize );

      if( current_maximum_offset > max_reached_offset) max_reached_offset = current_maximum_offset;
      detected_size += 1;

      // Is this the last element as defined in the FL limits ?
      if( it.position() == this->fc_limits.get_next() ) {
#ifdef FILELIST_HAS_DEBUG_CODE
	message.assign( "Reached the last element" );
	logMessage( function, message, this->fc_filename );

	message.assign( "current_maximum_offset = " ); message.append( boost::lexical_cast<string>(current_maximum_offset) );
	message.append( ", max_reached_offset = " ); message.append( boost::lexical_cast<string>(max_reached_offset) );
	logMessage( function, message, this->fc_filename );

	message.assign( "detected_size = " ); message.append( boost::lexical_cast<string>(detected_size) );
	message.append( ", this->fc_size = " ); message.append( boost::lexical_cast<string>(this->fc_size) );
	logMessage( function, message, this->fc_filename );
#endif
        // Last element. Check (and possibly adjust) size.
        if( detected_size != this->fc_size ) {
          int detected_offset = (detected_size - this->fc_size);

#ifdef FILELIST_HAS_DEBUG_CODE
	  message.assign( "detected_offset = " ); message.append( boost::lexical_cast<string>(detected_offset) );
	  message.append( ", allowable_size_offset = " ); message.append( boost::lexical_cast<string>(allowable_size_offset) );
	  logMessage( function, message, this->fc_filename );
#endif

          // Found offset is within allowed boundary ?
          // (taking sign into account)
	  // If allowable_size_offset is zero, any offset is got...
          if( ((allowable_size_offset > 0) && (detected_offset > 0) && (detected_offset <= allowable_size_offset)) ||
              ((allowable_size_offset < 0) && (detected_offset < 0) && (detected_offset >= allowable_size_offset)) ||
	      (allowable_size_offset == 0) ) {

            // If this is the case, we can fix the offset to the.
            // figure we found.
            answer = this->writeAndSetSize( detected_size );
#ifdef FILELIST_HAS_DEBUG_CODE
	    message.assign( "New size written on the file, status of the operation \"" );
	    message.append( FileContainerError::code_to_string(answer) );
	    message.append( "\"" );
	    logMessage( function, message, this->fc_filename );
#endif
            if( !isGood(answer) ) break;
          }
        }

        // We can now check whether the physical end of the file is beyond
        // the end of this last element (according to the limits).
        if( (current_maximum_offset <= max_reached_offset) &&
            (max_reached_offset < end) ) {
          // So we can try to truncate the file...
	  answer = this->truncateFile( static_cast<off_t>(max_reached_offset) );

#ifdef FILELIST_HAS_DEBUG_CODE
	  message.assign( "Result of truncation \"" );
	  message.append( FileContainerError::code_to_string(answer) );
	  message.append( "\"" );
	  logMessage( function, message, this->fc_filename );
#endif

	  if( isGood(answer) ) {
	    // ...and make sure this last element points to the end.
	    end = this->getEnd();

	    if( it.get_next() != end ) {
	      it.set_next( end );
	      answer = this->writeIterator( it.position(), it);
	    }
	  }
        }

        // The 'last element' case ends here. We have little left to do.
        break;
      }
    }

    // We need to make sure the start of our next element
    // is not exceeding physical boundaries.
    streamoff next_record_start = it.get_next();
    if( (next_record_start < fc_s_headerSize) || (next_record_start >= end) ) {
      answer = FileContainerError::unavailable_position;
      break;
    }

    // End of the main element loop.
  }
    
  return answer;
}

/*
  Public methods
*/
FileContainer::TimeStamp::TimeStamp( void ) : ts_good( true ), ts_epoch( time(NULL) ), ts_counter( 0 )
{
  if( !ts_s_initialized ) initialize();
}

FileContainer::TimeStamp::~TimeStamp( void ) {}

ostream &FileContainer::TimeStamp::write( ostream &os ) const
{
  os << setfill( '0' ) << setw( ts_s_twidth ) << this->ts_epoch << ' ' << setw( ts_s_swidth ) << this->ts_counter;

  return os;
}

istream &FileContainer::TimeStamp::read( istream &is )
{
  streamoff             position;
  string                buffer;
  static boost::regex   expression( "^\\d+ +\\d+$" );

  position = is.tellg();
  getline( is, buffer );

  if( (this->ts_good = boost::regex_match(buffer, expression)) ) {
    is.seekg( position );
    is >> this->ts_epoch >> this->ts_counter;
  }

  return is;
}

FileContainer::TimeStamp &FileContainer::TimeStamp::update_stamp( TimeStamp &ts )
{
  time_t   now = time( NULL );

  if( this->ts_epoch == now ) this->ts_counter += 1;
  else {
    this->ts_epoch = now;
    this->ts_counter = 0;
  }

  if( this->ts_epoch < ts.ts_epoch ) {
    this->ts_epoch = ts.ts_epoch;
    this->ts_counter = ts.ts_counter + 1;
  }
  else if( (this->ts_epoch == ts.ts_epoch) && (this->ts_counter <= ts.ts_counter) )
    this->ts_counter = ts.ts_counter + 1;

  return *this;
}

FileIterator::FileIterator( streamoff prev, streamoff next, streamoff current ) : fi_good( true ), fi_prev( prev ), fi_next( next ),
										  fi_current( current )
{
  if( fi_s_width == 0 ) initialize();
}

FileIterator::~FileIterator( void )
{}

ostream &FileIterator::write( ostream &os ) const
{
  os << hex << setfill( '0' ) << setw( fi_s_width ) 
     << this->fi_prev << ' ' << setw( fi_s_width ) << this->fi_next
     << dec;

  return os;
}

istream &FileIterator::read( istream &is )
{
  streamoff             position;
  string                buffer;
  static boost::regex   expression( "^\\s*[0-9a-fA-F]+ +[0-9a-fA-F]+" );

  position = is.tellg();
  getline( is, buffer );

  if( (this->fi_good = boost::regex_match(buffer, expression)) ) {
    is.seekg( position );
    is >> hex >> this->fi_prev >> this->fi_next >> dec;
  }

  return is;
}

const char *FileContainerError::code_to_string( iostatus_t code )
{
  int    istatus = static_cast<int>( code );

  if( (istatus <= static_cast<int>(unknown)) || (istatus >= static_cast<int>(_last_error)) )
    istatus = static_cast<int>(unknown);

  istatus += 1;

  return fce_s_errors[istatus];
}

FileContainerError::FileContainerError( iostatus_t code ) : exception(),  fce_line( -1 ), fce_code( code ), fce_func(), fce_file()
{}

FileContainerError::FileContainerError( iostatus_t code, const string &func, const char *file, int line ) : exception(), fce_line( line ),
													    fce_code( code ),
													    fce_func( func ), fce_file(),
													    fce_what()
{
  if( file != NULL ) this->fce_file.assign( file );
}

FileContainerError::~FileContainerError( void ) throw()
{}

const char *FileContainerError::what( void ) const throw()
{
  this->fce_what.assign( this->string_error() );

  return this->fce_what.c_str();
}

string FileContainerError::string_error( void ) const
{
  int        code = static_cast<int>( this->fce_code ) + 1;
  string     answer;

  if( (code < 0) || (code >= (static_cast<int>(_last_error) + 1)) ) code = 0;

  answer.assign( fce_s_errors[code] );
  if( this->fce_file.size() != 0 ) {
    answer.append( " \"" );
    answer.append( this->fce_file );
    answer.append( "\"" );
  }
  if( this->fce_func.size() != 0 ) {
    answer.append( " (" );
    answer.append( this->fce_func );

    if( this->fce_line > 0 ) {
      answer.append( "[" );
      answer.append( boost::lexical_cast<string>(this->fce_line) );
      answer.append( "]" );
    }

    answer.append( ")" );
  }

  return answer;
}

FileContainer::FileContainer( void ) : fc_initialized( false ), fc_size( 0 ), fc_stream( NULL ), fc_stamp( new TimeStamp() ),
				       fc_filename(), fc_limits( 0, 0 ), fc_removed( un_dead, un_beef )
#ifdef FILELIST_HAS_DEBUG_CODE
  , fc_callStack()
#endif
{}

FileContainer::FileContainer( const char *filename ) : fc_initialized( false ), fc_size( 0 ), fc_stream( NULL ),
						       fc_stamp( new TimeStamp() ), fc_filename(), fc_limits( 0, 0 ),
						       fc_removed( un_dead, un_beef )
#ifdef FILELIST_HAS_DEBUG_CODE
  , fc_callStack()
#endif
{
  iostatus_t   answer = this->open( filename );

  if( notGood(answer) ) throw FileContainerError( answer, "FileContainer::FileContainer(...)", filename, __LINE__ );
}

FileContainer::~FileContainer( void )
{
  if( this->fc_stream ) delete this->fc_stream;

  delete this->fc_stamp;
}

FileContainerError::iostatus_t FileContainer::open( const char *filename )
{
  iostatus_t                answer;
  fs::path   container(filename, fs::native);
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "open( filename = \"%s\" )", filename );
#endif

  this->close(); this->fc_filename.assign( filename );

  if( fc_s_stampSize == 0 ) staticInitialize();

  if( fs::exists(container) ) {
    this->fc_initialized = false;
    answer = this->openFile();
  }
  else {
    this->fc_initialized = false;
    answer = this->createFile();

    if( isGood(answer) ) {
      answer = this->updateTimeStamp();

      if( isGood(answer) ) this->fc_initialized = true;
    }
  }

  return answer;
}

FileContainer &FileContainer::close( void )
{
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "close()" );
#endif

  if( this->fc_stream ) {
    delete this->fc_stream; this->fc_stream = NULL;

    this->fc_filename.assign( "" );
  }

  this->fc_initialized = false;

  return *this;
}

FileContainerError::iostatus_t FileContainer::add_data( const std::string &data, streamoff where, FileIterator &it )
{
  iostatus_t      answer = FileContainerError::all_good;
  streamoff       end = this->getEnd();
#ifdef FILELIST_HAS_DEBUG_CODE
  const char     *function = "FileContainer::add_data(...)";
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "add_data( data = \"%s\",\n\t\twhere = %d, it = (%d, %d, %d) )",
				  data.substr(0,BUFSIZ).c_str(), static_cast<int>(where), cast_iterator(it) );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else {
    answer = this->checkStreamAndStamp();
    if( answer == FileContainerError::container_modified ) {
      answer = this->syncData( false );
      if( isGood(answer) ) end = this->getEnd();

#ifdef FILELIST_HAS_DEBUG_CODE
      string   message( "Container was modified. Syncing returned status \"" );
      message.append( FileContainerError::code_to_string(answer) );
      message.append( 1, '\"' );
      logMessage( function, message, this->fc_filename );

      message.assign( "Size is now: " ); message.append( boost::lexical_cast<string>(this->fc_size) );
      logMessage( function, message, this->fc_filename );
#endif
    }
  }

  if( isGood(answer) ) {
    answer = this->updateTimeStamp();

    if( isGood(answer) ) {
      answer = this->writeFileStatus( opened );

      if( isGood(answer) ) {
	if( where == end ) answer = this->addDataAtEnd( data, it, end );
	else if( where == this->getBegin() ) answer = this->addDataAtBegin( data, it, end );
	else answer = this->addDataInMiddle( data, it, where, end );

	if( isGood(answer) ) {
	  answer = this->createEmptyBackup( ad_empty );

	  if( isGood(answer) ) {
	    answer = this->writeAndSetSize( this->fc_size + 1 );

	    if( isGood(answer) ) answer = this->writeFileStatus( closed );
	  }
	}
      }
    }
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::read_data( string &data, streamoff where, FileIterator &it, bool fileorder )
{
  iostatus_t     answer = FileContainerError::all_good;
  streamoff      end = this->getEnd();
  FileIterator   fileIt;
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "read_data( data, where = %d, it = (%d, %d, %d), fileorder = %d )",
				  static_cast<int>(where), cast_iterator(it), static_cast<int>(fileorder) );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else {
    answer = this->checkStreamAndStamp();
    if( answer == FileContainerError::container_modified ) {
      answer = this->syncData();
      if( isGood(answer) ) end = this->getEnd();
    }
  }

  if( isGood(answer) ) {
    if( where <= end ) {
      this->fc_stream->seekg( where );
      answer = this->readDataHere( it, fileIt, data, fileorder );

      if( fileorder ) it.reset( fileIt.get_prev(), fileIt.get_next(), where );
    }
    else answer = FileContainerError::unavailable_position;
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::remove_data( streamoff where )
{
  char                state;
  iostatus_t          answer = FileContainerError::all_good;
  string::size_type   size;
  FileIterator        iter, temp;
#ifdef FILELIST_HAS_DEBUG_CODE
  const char         *function = "remove_data(...)";
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "remove_data( where = %d )", static_cast<int>(where) );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else {
    answer = this->checkStreamAndStamp();
    if( answer == FileContainerError::container_modified ) answer = this->syncData( false );
  }

  if( isGood(answer) ) {
    answer = this->updateTimeStamp();

    if( isGood(answer) ) answer = this->writeFileStatus( opened );
  }

  if( isGood(answer) ) {
    answer = this->readIterator( where, iter );

    if( isGood(answer) ) {
      answer = this->readSizeAndState( size, state );

      if( isGood(answer) ) {
	if( state == 'g' ) answer = this->removeDataPointer( iter, size );
	else answer = FileContainerError::unavailable_position;
      }
    }
  }

  if( isGood(answer) ) {
    answer = this->createEmptyBackup( rd_empty );

    if( isGood(answer) ) {
      if( this->fc_size == 0 ) {
#ifdef FILELIST_HAS_DEBUG_CODE
	string   message( "Zero size inside the container. Going to check consistency" );
	logMessage( function, message, this->fc_filename );
#endif
  	answer = this->checkConsistency( 0 );
#ifdef FILELIST_HAS_DEBUG_CODE
	if( notGood(answer) ) {
	  message.assign( "Consistency check returned \"" );
	  message.append( FileContainerError::code_to_string(answer) );
	  message.append( "\"" );
	  logMessage( function, message, this->fc_filename );
	}
#endif
	if( isGood(answer) ) this->fc_size += 1; // This number does not count the just removed field.
	else answer = FileContainerError::decrementing_from_zero;
      }

      if( isGood(answer) ) {
	answer = this->writeAndSetSize( this->fc_size - 1 );

	if( isGood(answer) ) answer = this->writeFileStatus( closed );
      }
    }
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::increment_iterator( FileIterator &it )
{
  iostatus_t     answer = FileContainerError::all_good;
  streamoff      end = this->getEnd();
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "increment_iterator( it = (%d, %d, %d) )", cast_iterator(it) );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else {
    answer = this->checkStreamAndStamp();
    if( answer == FileContainerError::container_modified ) {
      answer = this->syncData();
      if( isGood(answer) ) end = this->getEnd();
    }
  }

  if( isGood(answer) ) {
    if( it.position() == 0 ) answer = this->readIterator( this->fc_limits.get_prev(), it );
    else if( it.get_next() == end ) it.reset( 0, 0, end );
    else {
      answer = this->readIterator( it.position(), it );

      if( isGood(answer) ) answer = this->readIterator( it.get_next(), it );
    }
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::increment_fileorder_iterator( FileIterator &it )
{
  iostatus_t     answer = FileContainerError::all_good;
  streamoff      end = this->getEnd();
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "increment_fileorder_iterator( it = (%d, %d, %d) )", cast_iterator(it) );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else {
    answer = this->checkStreamAndStamp();
    if( answer == FileContainerError::container_modified ) {
      answer = this->syncData();
      if( isGood(answer) ) end = this->getEnd();
    }
  }

  if( isGood(answer) ) {
    if( it.position() == 0 ) answer = this->readIterator( this->fc_limits.get_prev(), it, true );
    else if( it.get_next() == end ) it.reset( 0, 0, end );
    else {
      answer = this->readIterator( it.position(), it, true );

      if( isGood(answer) ) answer = this->readIterator( it.get_next(), it, true );
    }
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::decrement_iterator( FileIterator &it )
{
  iostatus_t    answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "decrement_iterator( it = (%d, %d, %d) )", cast_iterator(it) );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else {
    answer = this->checkStreamAndStamp();
    if( answer == FileContainerError::container_modified ) answer = this->syncData();
  }

  if( isGood(answer) ) {
    if( it.position() == this->getEnd() ) answer = this->readIterator( this->fc_limits.get_next(), it );
    else if( it.get_prev() == 0 ) it.reset();
    else {
      answer = this->readIterator( it.position(), it );

      if( isGood(answer) ) answer = this->readIterator( it.get_prev(), it );
    }
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::decrement_fileorder_iterator( FileIterator &it )
{
  iostatus_t    answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "decrement_fileorder_iterator( it = (%d, %d, %d) )", cast_iterator(it) );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else {
    answer = this->checkStreamAndStamp();
    if( answer == FileContainerError::container_modified ) answer = this->syncData();
  }

  if( isGood(answer) ) {
    if( it.position() == this->getEnd() ) answer = this->readIterator( this->fc_limits.get_next(), it, true );
    else if( it.get_prev() == 0 ) it.reset();
    else {
      answer = this->readIterator( it.position(), it, true );

      if( isGood(answer) ) answer = this->readIterator( it.get_prev(), it, true );
    }
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::truncate( void )
{
  iostatus_t    answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "truncate()" );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else answer = this->checkStreamAndStamp();

  if( isGood(answer) ) {
    answer = this->eraseFile();

    if( isGood(answer) ) this->fc_size = 0;
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::read_size( size_t &size )
{
  iostatus_t         answer = FileContainerError::all_good;
  streamoff          end = this->getEnd();
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "read_size( size )" );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else {
    answer = this->checkStreamAndStamp();
    if( isGood(answer) ) end = this->getEnd();
  }

  if( (answer == FileContainerError::container_modified) || ((this->getBegin() != end) && (this->fc_size == 0)) )
    answer = this->syncData();

  if( isGood(answer) ) size = this->fc_size;

  return answer;
}

FileContainerError::iostatus_t FileContainer::compact_data( void )
{
  iostatus_t                answer = FileContainerError::all_good;
  streamoff                 end;
  FileIterator              it;
  vector<string>            copy;
  vector<string>::iterator  copyIt;
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "compact_data()" );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();

  if( isGood(answer) ) answer = this->fillVector( copy );

  if( isGood(answer) ) {
    answer = this->backupFile();

    if( isGood(answer) ) {
      answer = this->eraseFile();

      if( isGood(answer) ) {
	answer = this->writeFileStatus( opened );

	if( isGood(answer) ) {
	  end = this->getEnd();

	  for( copyIt = copy.begin(); isGood(answer) && (copyIt != copy.end()); ++copyIt ) {
	    answer = this->addDataAtEnd( *copyIt, it, end );
	    end = it.get_next();
	  }

	  if( isGood(answer) ) {
	    answer = this->createEmptyBackup( empty );

	    if( isGood(answer) ) {
	      answer = this->writeAndSetSize( copy.size() );

	      if( isGood(answer) ) answer = this->writeFileStatus( closed );
	    }
	  }
	}
      }
    }
  }

  return answer;
}

FileContainerError::iostatus_t FileContainer::recover_data( filestatus_t status, bool check )
{
  iostatus_t     answer = FileContainerError::all_good;
  FileIterator   limits, temp;
#ifdef FILELIST_HAS_DEBUG_CODE
  const char    *function = "FileContainer::recover_data(...)";
  string         message;
  StackPusher       stack_pusher( this->fc_callStack, "recover_data( status = %d, check = %d )",
				  static_cast<int>(status), static_cast<int>(check) );
#endif

  if( check && !this->fc_initialized ) answer = this->initContainer();
  else if( check ) {
    answer = this->checkStreamAndStamp( false );
    if( answer == FileContainerError::container_modified ) {
#ifdef FILELIST_HAS_DEBUG_CODE
      message.assign( "Container modified, going to sync data. Old size = " );
      message.append( boost::lexical_cast<string>(this->fc_size) );

      logMessage( function, message, this->fc_filename );
#endif
      answer = this->syncData();

#ifdef FILELIST_HAS_DEBUG_CODE
      message.assign( "New size read = " );
      message.assign( boost::lexical_cast<string>(this->fc_size) );

      logMessage( function, message, this->fc_filename );
#endif
    }
  }

  if( isGood(answer) ) answer = this->backupFile();

  if( isGood(answer) ) {
    if( status == closed ) answer = this->readFileStatus( status );

    if( isGood(answer) ) {
      this->fc_stream->sync();

      switch( status ) {
      case closed:
	break;
      case empty:
	answer = this->writeFileStatus( closed );

	if( isGood(answer) ) {
	  answer = this->syncData();

	  if( isGood(answer) && (this->fc_size == 0) && (this->getBegin() != this->getEnd()) ) {
	    answer = this->eraseFile();

	    if( isGood(answer) ) this->fc_size = 0;
	  }
	}

	break;
      case opened:
      case ft_end:
	answer = this->writeFileStatus( closed );

	break;
      case ae_writing:
      case ae_written:
	answer = this->readInitialPosition( limits );

	if( isGood(answer) ) answer = this->truncateFile( static_cast<off_t>(limits.get_next()) );

	if( notGood(answer) ) break; // Exit on error
      case ae_limits:
	answer = this->readLimitsBackup( this->fc_limits );

	if( isGood(answer) ) answer = this->writeInitialPosition( this->fc_limits );

	if( notGood(answer) ) break; // Exit on error
      case ae_iterator:
	answer = this->readIteratorBackup( 0, temp );

	if( isGood(answer) ) {
	  answer = this->writeIterator( temp.position(), temp );

	  if( isGood(answer) ) answer = this->checkConsistency( 1 );
	}

	break;
      case ab_last:
	answer = this->readIteratorBackup( 2, temp );

	if( isGood(answer) ) answer = this->writeIterator( temp.position(), temp );

	if( notGood(answer) ) break; // Exit on error
      case ab_writing:
      case ab_written:
	answer = this->readInitialPosition( limits );

	if( isGood(answer) ) answer = this->truncateFile( static_cast<off_t>(limits.get_prev()) );

	if( notGood(answer) ) break; // Exit on error
      case ab_limits:
	answer = this->readLimitsBackup( this->fc_limits );

	if( isGood(answer) ) answer = this->writeInitialPosition( this->fc_limits );

	if( notGood(answer) ) break; // Exit on error
      case ab_iterator:
	answer = this->readIteratorBackup( 0, temp );

	if( isGood(answer) ) {
	  answer = this->writeIterator( temp.position(), temp );

	  if( isGood(answer) ) answer = this->checkConsistency( 1 );
	}

	break;
      case am_last:
	answer = this->readIteratorBackup( 2, temp );

	if( isGood(answer) ) answer = this->writeIterator( temp.position(), temp );

	if( notGood(answer) ) break; // Exit on error
      case am_writing:
      case am_written:
	answer = this->readIteratorBackup( 0, temp );

	if( isGood(answer) ) {
	  answer = this->readIterator( temp.position(), temp );

	  if( isGood(answer) ) answer = this->truncateFile( static_cast<off_t>(temp.get_prev()) );
	}

	if( notGood(answer) ) break; // Exit on error
      case am_iterator2:
	answer = this->readIteratorBackup( 1, temp );

	if( isGood(answer) ) answer = this->writeIterator( temp.position(), temp );

	if( notGood(answer) ) break; // Exit on error
      case am_iterator1:
	answer = this->readIteratorBackup( 0, temp );

	if( isGood(answer) ) {
	  answer = this->writeIterator( temp.position(), temp );

	  if( isGood(answer) ) answer = this->checkConsistency( 1 );
	}

	break;
      case ad_empty:
	answer = this->checkConsistency( 1 );

	break;
      case ft_begin:
	answer = this->readIteratorBackup( 0, temp );

	if( isGood(answer) ) answer = this->truncateFile( static_cast<off_t>(temp.position()) );

	break;
      case rd_mark1:
	answer = this->readIteratorBackup( 2, temp );

	if( isGood(answer) ) answer = this->markDataAsUnerased( temp );

	if( notGood(answer) ) break; // Exit on error
      case rd_iterator:
	answer = this->readIteratorBackup( 0, temp );

	if( isGood(answer) ) answer = this->writeIterator( temp.position(), temp );

	if( notGood(answer) ) break; // Exit on error
      case rd_limits:
	answer = this->readLimitsBackup( this->fc_limits );

	if( isGood(answer) ) {
	  answer = this->writeInitialPosition( this->fc_limits );

	  if( isGood(answer) ) answer = this->checkConsistency( -1 );
	}

	break;
      case rd_mark2:
	answer = this->readIteratorBackup( 2, temp );

	if( isGood(answer) ) answer = this->markDataAsUnerased( temp );

	if( notGood(answer) ) break; // Exit on error
      case rd_iterator3:
	answer = this->readIteratorBackup( 1, temp );

	if( isGood(answer) ) answer = this->writeIterator( temp.position(), temp );

	if( notGood(answer) ) break; // Exit on error
      case rd_iterator2:
	answer = this->readIteratorBackup( 0, temp );

	if( isGood(answer) ) {
	  answer = this->writeIterator( temp.position(), temp );

	  if( isGood(answer) ) answer = this->checkConsistency( -1 );
	}

	break;
      case rd_empty:
	answer = this->checkConsistency( -1 );

	break;
      default:
	answer = FileContainerError::unrecoverable_data;

	break;
      }

      if( isGood(answer) && (status != closed) ) answer = this->writeFileStatus( closed );
    }
  }

#ifdef FILELIST_HAS_DEBUG_CODE
  message.assign( "Return status is \"" );
  message.append( FileContainerError::code_to_string(answer) );
  message.append( "\"" );

  logMessage( function, message, this->fc_filename );
#endif

  return answer;
}

FileContainerError::iostatus_t FileContainer::sync( void )
{
  iostatus_t    answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "sync()" );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else answer = this->checkStreamAndStamp();

  if( answer == FileContainerError::container_modified ) this->syncData();

  return answer;
}

FileContainerError::iostatus_t FileContainer::get_iterator( streamoff where, FileIterator &it )
{
  iostatus_t  answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "get_iterator( where = %d, it = (%d, %d, %d) )",
				  static_cast<int>(where), cast_iterator(it) );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else {
    answer = this->checkStreamAndStamp();
    if( answer == FileContainerError::container_modified ) answer = this->syncData();
  }

  if( isGood(answer) ) answer = this->readIterator( where, it );

  return answer;
}

FileContainerError::iostatus_t FileContainer::get_fileorder_iterator( streamoff where, FileIterator &it )
{
  iostatus_t  answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "get_fileorder_iterator( where = %d, it = (%d, %d, %d) )",
				  static_cast<int>(where), cast_iterator(it) );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else {
    answer = this->checkStreamAndStamp();
    if( answer == FileContainerError::container_modified ) answer = this->syncData();
  }

  if( isGood(answer) ) answer = this->readIterator( where, it, true );

  return answer;
}

FileContainerError::iostatus_t FileContainer::modified( bool &mod )
{
  iostatus_t   answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "modified( mod )" );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();

  if( isGood(answer) ) answer = this->checkStamp( mod );

  return answer;
}

FileContainerError::iostatus_t FileContainer::read_begin( streamoff &begin )
{
  iostatus_t   answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "read_begin( begin )" );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else {
    answer = this->checkStreamAndStamp();
    if( answer == FileContainerError::container_modified ) answer = this->syncData();
  }

  if( isGood(answer) ) begin = this->getBegin();

  return answer;
}

FileContainerError::iostatus_t FileContainer::read_end( streamoff &end )
{
  iostatus_t   answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "read_end( end )" );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else {
    answer = this->checkStreamAndStamp();
    if( answer == FileContainerError::container_modified ) answer = this->syncData();
  }

  if( isGood(answer) ) end = this->getEnd();

  return answer;
}

FileContainerError::iostatus_t FileContainer::read_last( streamoff &last )
{
  iostatus_t   answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "read_last( last )" );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else {
    answer = this->checkStreamAndStamp();
    if( answer == FileContainerError::container_modified ) answer = this->syncData();
  }

  if( isGood(answer) ) last = this->fc_limits.get_next();

  return answer;
}

FileContainerError::iostatus_t FileContainer::read_file_end( streamoff &fileend )
{
  iostatus_t   answer = FileContainerError::all_good;
#ifdef FILELIST_HAS_DEBUG_CODE
  this->fc_callStack.clear();
  StackPusher       stack_pusher( this->fc_callStack, "read_file_end( fileend )" );
#endif

  if( !this->fc_initialized ) answer = this->initContainer();
  else {
    answer = this->checkStreamAndStamp();
    if( answer == FileContainerError::container_modified ) answer = this->syncData();
  }

  if( isGood(answer) ) {
    this->fc_stream->seekp( 0, ios::end );
    fileend = this->fc_stream->tellp();
  }

  return answer;
}

#ifdef FILELIST_HAS_DEBUG_CODE
FileContainerError::iostatus_t FileContainer::dump_status( const char *filename, iostatus_t status, const string &caller, int line )
{
  iostatus_t                  answer = FileContainerError::all_good;
  int                         istat = static_cast<int>( status );
  ofstream                    df( filename, ios::out | ios::app );
  vector<string>::iterator    vIt;

  if( df.good() ) {
    df << endl << endl << endl << endl
       << "************************************************************************" << endl
       << "*                                                                      *" << endl
       << "*    FileContainer internal status.                                    *" << endl
       << "*                                                                      *" << endl
       << "************************************************************************" << endl
       << endl
       << "Filename = " << this->fc_filename << endl
       << "Initialized = " << this->fc_initialized << endl
       << "Internal Size = " << this->fc_size << endl
       << "Stream pointer = " << static_cast<void *>(this->fc_stream) << endl
       << "\tgood() = " << this->fc_stream->good() << ", bad() = " << this->fc_stream->bad()
       << ",eof() = " << this->fc_stream->eof() << endl
       << "Timestamp = " << static_cast<void *>(this->fc_stamp);

    if( this->fc_stamp == NULL )
      df << " - Value not available." << endl;
    else
      df << " - Value: " << *this->fc_stamp << endl;

    df << "Limits = " << this->fc_limits << endl
       << "Removed = " << this->fc_removed << endl
       << "Current end pointer = " << this->getEnd() << endl << endl;

    df << "Last known (wrong) status was: " << FileContainerError::code_to_string(status)
       << " (" << istat << ")" << endl << endl;

    df << "Caller: " << caller << ":" << line << endl;

    if( this->fc_callStack.empty() )
      df << "Internal stack trace is empty." << endl;
    else {
      df << "Internal stack trace dump follows:" << endl;

      for( vIt = this->fc_callStack.begin(); vIt != this->fc_callStack.end(); ++vIt )
	df << '\t' << *vIt << endl;
    }
  }
  else answer = FileContainerError::io_error;

  return answer;
}
#endif

} // namespace jccommon
} JOBCONTROL_NAMESPACE_END

