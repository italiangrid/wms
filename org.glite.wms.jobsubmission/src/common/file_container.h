/*
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://www.eu-egee.org/partners for details on the
 * copyright holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *     either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 *     */

#ifndef GLITE_WMS_JOBSUBMISSION_COMMON_FILECONTAINER_H
#define GLITE_WMS_JOBSUBMISSION_COMMON_FILECONTAINER_H

#include <unistd.h>

#include <exception>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

/*
 *   Hack to activate wrong filelist dumping...
 *     Remove when no more needed...
 *     */
#define FILELIST_HAS_DEBUG_CODE

#include "jobcontrol_namespace.h"

JOBCONTROL_NAMESPACE_BEGIN {
namespace jccommon {

class FileIterator {
  inline friend std::ostream &operator<<( std::ostream &os, const FileIterator &fi ) { return fi.write( os ); }
  inline friend std::istream &operator>>( std::istream &is, FileIterator &fi ) { return fi.read( is ); }

public:
  FileIterator( std::streamoff prev = 0, std::streamoff next = 0, std::streamoff current = 0 );

  ~FileIterator( void );

  std::ostream &write( std::ostream &os ) const;
  std::istream &read( std::istream &is );

  inline std::streamoff position( void ) const { return this->fi_current; }
  inline std::streamoff get_prev( void ) const { return this->fi_prev; }
  inline std::streamoff get_next( void ) const { return this->fi_next; }
  inline FileIterator &set_prev( std::streamoff prev ) { this->fi_prev = prev; return *this; }
  inline FileIterator &set_next( std::streamoff next ) { this->fi_next = next; return *this; }
  inline FileIterator &set_current( std::streamoff current ) { this->fi_current = current; return *this; }
  inline FileIterator &reset( std::streamoff prev = 0, std::streamoff next = 0, std::streamoff current = 0 )
  { this->fi_prev = prev; this->fi_next = next; this->fi_current = current; return *this; }

  inline operator bool( void ) const { return this->fi_good; }
  inline bool operator!( void ) const { return !this->fi_good; }

  inline static int size( void ) { return fi_s_width; }

private:
  void initialize( void );

  bool             fi_good;
  std::streamoff   fi_prev, fi_next, fi_current;

  static int           fi_s_width;
  static FileIterator  fi_s_iterator;
};

class FileContainerError : public std::exception {
public:
  enum iostatus_t { unknown = -1, 
		    all_good, 
		    cannot_open,
		    file_closed,
		    syntax_error,
		    io_error,
		    data_error,
		    unavailable_position,
		    container_modified,
		    try_recover,
		    unrecoverable_data,
		    decrementing_from_zero,
		    not_removing_last,
		    cannot_convert_from_string,
		    cannot_convert_to_string,
		    _last_error
  };

  FileContainerError( iostatus_t code = all_good );
  FileContainerError( iostatus_t code, const std::string &func, const char *file = NULL, int line = -1 );

  ~FileContainerError( void ) throw();

  std::string string_error( void ) const;
  virtual const char *what( void ) const throw();

  inline iostatus_t code( void ) { return this->fce_code; }
  inline FileContainerError &set_code( iostatus_t code ) { this->fce_code = code; return *this; }

  static const char *code_to_string( iostatus_t status );

private:
  int                   fce_line;
  iostatus_t            fce_code;
  std::string           fce_func, fce_file;
  mutable std::string   fce_what;

  static const char    *fce_s_errors[];
};

class FileContainer {
public:
  enum filestatus_t { empty        = ' ',
		      opened       = '1',
		      closed       = '0',
		      ae_iterator  = 'a',
		      ae_limits,
		      ae_writing,
		      ae_written,
		      ab_iterator,
		      ab_writing,
		      ab_written,
		      ab_last,
		      ab_limits,
		      am_iterator1,
		      am_iterator2,
		      am_writing,
		      am_written,
		      am_last,
		      rd_limits,
		      rd_iterator,
		      rd_iterator2,
		      rd_iterator3,
		      rd_mark1,
		      rd_mark2,
		      ft_begin,
		      ft_end,        /* 'v' */
		      ad_empty,
		      rd_empty       /* 'x' */
  };

  typedef FileContainerError::iostatus_t iostatus_t;

  FileContainer( void );
  FileContainer( const char *filename );

  ~FileContainer( void );

  iostatus_t open( const char *filename );
  FileContainer &close( void );

  iostatus_t add_data( const std::string &data, std::streamoff where, FileIterator &it );
  iostatus_t read_data( std::string &data, std::streamoff where, FileIterator &it, bool fileorder = false );
  iostatus_t remove_data( std::streamoff where );
  iostatus_t recover_data( filestatus_t status = closed, bool check = true );
  iostatus_t increment_iterator( FileIterator &it );
  iostatus_t decrement_iterator( FileIterator &it );
  iostatus_t increment_fileorder_iterator( FileIterator &it );
  iostatus_t decrement_fileorder_iterator( FileIterator &it );
  iostatus_t truncate( void );
  iostatus_t read_size( size_t &size );
  iostatus_t compact_data( void );
  iostatus_t sync( void );
  iostatus_t get_iterator( std::streamoff where, FileIterator &it );
  iostatus_t get_fileorder_iterator( std::streamoff where, FileIterator &it );
  iostatus_t modified( bool &mod );
  iostatus_t read_begin( std::streamoff &begin );
  iostatus_t read_end( std::streamoff &end );
  iostatus_t read_last( std::streamoff &last );
  iostatus_t read_file_end( std::streamoff &fileend );
#ifdef FILELIST_HAS_DEBUG_CODE
  iostatus_t dump_status( const char *filename, iostatus_t status, const std::string &caller, int line );
#endif

  inline iostatus_t read_data( std::string &data, FileIterator &it ) { return this->read_data( data, it.position(), it ); }
  inline iostatus_t read_fileorder_data( std::string &data, FileIterator &it )
  { return this->read_data( data, it.position(), it, true ); }
  inline iostatus_t add_data( const std::string &data, FileIterator &it ) { return this->add_data( data, it.position(), it ); }
  inline iostatus_t remove_data( const FileIterator &it ) { return this->remove_data( it.position() ); }
  inline iostatus_t initialize( void ) { return this->initContainer(); }
  inline iostatus_t force_backup( const char *backupfile = NULL ) { return this->backupFile( backupfile ); }
  inline std::streamoff file_begin( void ) { return fc_s_headerSize; }
  inline std::streamoff start( void ) { return 0; }
  inline std::fstream &get_stream( void ) { return *this->fc_stream; }

  inline const std::string &filename( void ) const { return this->fc_filename; }

private:
  const FileContainer &operator=( const FileContainer & ); // Not implemented
  FileContainer( const FileContainer & ); // Not implemented

  class TimeStamp;

  iostatus_t openFile( void );
  iostatus_t createFile( void );
  iostatus_t syncData( bool stamp = true );
  iostatus_t createEmptyFile( void );
  iostatus_t eraseFile( off_t size = 0 );
  iostatus_t truncateFile( off_t size );
  iostatus_t readStamp( void );
  iostatus_t writeStamp( void );
  iostatus_t writeFileStatus( filestatus_t fs );
  iostatus_t readFileStatus( filestatus_t &fs );
  iostatus_t readSize( void );
  iostatus_t writeAndSetSize( size_t num );
  iostatus_t createEmptyBackup( filestatus_t status );
  iostatus_t writeIteratorBackup( int what, const FileIterator &it, filestatus_t status );
  iostatus_t readIteratorBackup( int what, FileIterator &it );
  iostatus_t writeLimitsBackup( filestatus_t status, const FileIterator &it, bool isList = false );
  iostatus_t readLimitsBackup( FileIterator &it, bool isList = false );
  iostatus_t writeInitialPosition( const FileIterator &limits, bool isList = false );
  iostatus_t readInitialPosition( FileIterator &limits, bool isList = false );
  iostatus_t updateTimeStamp( void );
  iostatus_t checkStream( bool recover = true );
  iostatus_t checkStreamAndStamp( bool recover = true );
  iostatus_t checkStamp( bool &modified );
  iostatus_t readIteratorHere( FileIterator &it );
  iostatus_t readIterator( std::streamoff where, FileIterator &it, bool fileorder = false );
  iostatus_t writeIterator( std::streamoff where, const FileIterator &it );
  iostatus_t resetNextOfLast( filestatus_t status );
  iostatus_t writeDataHeader( const FileIterator &it, std::string::size_type size, char state );
  iostatus_t writeDataHere( const FileIterator &it, const FileIterator &lit, const std::string &data, filestatus_t status );
  iostatus_t addDataAtBegin( const std::string &data, FileIterator &it, std::streamoff end );
  iostatus_t addDataAtEnd( const std::string &data, FileIterator &it, std::streamoff end );
  iostatus_t addDataInMiddle( const std::string &data, FileIterator &it, std::streamoff where, std::streamoff end );
  iostatus_t readDataHere( FileIterator &it, FileIterator &itList, std::string &data, bool force = false );
  iostatus_t readSizeAndState( std::string::size_type &size, char &state );
  iostatus_t removeDataPointer( const FileIterator &it, std::string::size_type size );
  iostatus_t markDataAsErased( const FileIterator &it, std::string::size_type size, filestatus_t status );
  iostatus_t markDataAsUnerased( const FileIterator &it );
  iostatus_t backupFile( const char *backupfile = NULL );
  iostatus_t fillVector( std::vector<std::string> &vec );
  iostatus_t initContainer( void );
  iostatus_t checkConsistency( int allowable_size_offset );

  inline std::streamoff getEnd( void ) { this->fc_stream->seekp( 0, std::ios::end ); return this->fc_stream->tellp(); }
  inline std::streamoff getBegin( void ) { return this->fc_limits.get_prev(); }

  static void staticInitialize( void );

  bool               fc_initialized;
  size_t             fc_size;
  std::fstream      *fc_stream;
  TimeStamp         *fc_stamp;
  std::string        fc_filename;
  FileIterator       fc_limits, fc_removed;
#ifdef FILELIST_HAS_DEBUG_CODE
  std::vector<std::string>   fc_callStack;

  class StackPusher {
  public:
    StackPusher( std::vector<std::string> &callstack, const char *format, ... );
    ~StackPusher( void );

  private:
    std::vector<std::string>     &sp_callstack;
  };
#endif

  static int         fc_s_stampSize, fc_s_sizeSize, fc_s_headerSize, fc_s_backupSize;
  static int         fc_s_iteratorBackupSize, fc_s_limitsBackupSize, fc_s_numberSize;
  static int         fc_s_statusPosition, fc_s_positionPosition;
  static int         fc_s_listBackupSize, fc_s_listPosition;
  static const int   fc_s_iteratorBackupNumber;
};

#ifdef FILELIST_HAS_DEBUG_CODE
void throwErrorAndDumpFile( FileContainer &cont, FileContainerError::iostatus_t status,
			    const std::string &func, const std::string &filename, int line, bool doThrow = true );
#else
#define throwErrorAndDumpFile( container, status, func, filename, line )  \
           throw FileContainerError( (status), (func), (filename).c_str(), (line) )
#endif

} // namespace jccommon
} JOBCONTROL_NAMESPACE_END

#endif /* GLITE_WMS_JOBSUBMISSION_COMMON_FILECONTAINER_H */
