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

#include <vector>

#include "common/filelist.h"

using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {
namespace jccommon {

/*
 *   Private/Protected methods
 *   */

_base_iterator_t &_file_sequence_t::insertData( const _base_iterator_t &position, const std::string &val )
{
  iostatus_t     status = FileContainerError::all_good;
  const string   func( "_file_sequence_t::insertData(...)" );

  status = this->fs_container.add_data( val, position.bi_iterator.position(), this->fs_last.bi_iterator );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  return this->fs_last;
}

_base_iterator_t &_file_sequence_t::erasePointer( const _base_iterator_t &position )
{
  iostatus_t      status = FileContainerError::all_good;
  streamoff       end;
  size_t          size;
  FileIterator    iter;
  const string    func( "_file_sequence_t::erasePointer(...)" );

  status = this->fs_container.read_end( end );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  status = this->fs_container.get_iterator( position.bi_iterator.position(), iter );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  status = this->fs_container.increment_iterator( iter );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  status = this->fs_container.remove_data( position.bi_iterator.position() );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  status = this->fs_container.read_size( size );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  if( size != 0 ) {
    status = this->fs_container.get_iterator( iter.position(), this->fs_last.bi_iterator );
    if( status != FileContainerError::all_good )
      throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );
  }
  else {
    status = this->fs_container.read_end( end );
    if( status != FileContainerError::all_good )
      throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

    status = this->fs_container.get_iterator( end, this->fs_last.bi_iterator );
    if( status != FileContainerError::all_good )
      throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );
  }

  return this->fs_last;
}

_base_iterator_t &_file_sequence_t::eraseInterval( const _base_iterator_t &first, const _base_iterator_t &last )
{
  iostatus_t      status = FileContainerError::all_good;
  streamoff       end;
  FileIterator    iter( first.bi_iterator );
  const string    func( "_file_sequence_t::eraseInterval(...)" );

  do {
    status = this->fs_container.get_iterator( iter.position(), iter );
    if( status != FileContainerError::all_good ) break;

    status = this->fs_container.remove_data( iter.position() );
    if( status != FileContainerError::all_good ) break;

    status = this->fs_container.increment_iterator( iter );
    if( status != FileContainerError::all_good ) break;

    status = this->fs_container.read_end( end );
    if( status != FileContainerError::all_good )
      throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );
  } while( (iter.position() != last.bi_iterator.position()) && (iter.position() != end) );

  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );
  this->fs_last.bi_iterator = iter;

  return this->fs_last;
}

void _file_sequence_t::removeData( const std::string &value )
{
  iostatus_t       status = FileContainerError::all_good;
  streamoff        begin, end;
  string           data;
  FileIterator     iter;
  const string     func( "_file_sequence_t::removeData(...)" );

  status = this->fs_container.read_begin( begin );

  if( status == FileContainerError::all_good ) {
    status = this->fs_container.get_iterator( begin, iter );

    if( status == FileContainerError::all_good ) {
      do {
	status = this->fs_container.read_data( data, iter );
	if( status != FileContainerError::all_good ) break;

	if( data == value ) {
	  status = this->fs_container.remove_data( iter.position() );
	  if( status != FileContainerError::all_good ) break;
	}

	status = this->fs_container.increment_iterator( iter );
	if( status != FileContainerError::all_good ) break;

	status = this->fs_container.read_end( end );
	if( status != FileContainerError::all_good ) break;

      } while( iter.position() != end );
    }
  }

  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  return;
}

void _file_sequence_t::swapContainer( _file_sequence_t &other )
{
  vector<string>             me, you;
  vector<string>::iterator   vit;
  _base_iterator_t           it, end;

  if( &other != this ) {
    end = this->getEnd();
    for( it = this->getBegin(); it.is_different(end); it.increment() ) {
      it.read_string();
      me.push_back( it.get_data() );
    }

    end = other.getEnd();
    for( it = other.getBegin(); it.is_different(end); it.increment() ) {
      it.read_string();
      you.push_back( it.get_data() );
    }

    this->clear(); other.clear();

    for( vit = me.begin(); vit != me.end(); ++vit )
      other.insertData( other.getEnd(), *vit );

    for( vit = you.begin(); vit != you.end(); ++vit )
      this->insertData( this->getEnd(), *vit );
  }

  return;
}

_base_iterator_t &_file_sequence_t::getBegin( void )
{
  iostatus_t       status = FileContainerError::all_good;
  streamoff        begin, end;
  const string     func( "_file_sequence_t::getBegin()" );

  status = this->fs_container.read_begin( begin );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  status = this->fs_container.read_end( end );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  status = this->fs_container.get_iterator( begin, this->fs_last.bi_iterator );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  this->fs_last.good( (begin != end) );

  return this->fs_last;
}

_base_iterator_t &_file_sequence_t::getEnd( void )
{
  iostatus_t       status = FileContainerError::all_good;
  streamoff        end;
  const string     func( "_file_sequence_t::getEnd()" );

  status = this->fs_container.read_end( end );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  status = this->fs_container.get_iterator( end, this->fs_last.bi_iterator );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  this->fs_last.good( false );

  return this->fs_last;
}

_base_iterator_t &_file_sequence_t::getStart( void )
{
  iostatus_t       status = FileContainerError::all_good;
  streamoff        end;
  const string     func( "_file_sequence_t::getStart()" );

  status = this->fs_container.read_end( end );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  status = this->fs_container.get_iterator( this->fs_container.start(), this->fs_last.bi_iterator );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  this->fs_last.good( (this->fs_container.start() != end) );

  return this->fs_last;
}

_base_iterator_t &_file_sequence_t::getLast( void )
{
  iostatus_t       status = FileContainerError::all_good;
  streamoff        end, last;
  const string     func( "_file_sequence_t::getLast()" );

  status = this->fs_container.read_end( end );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  status = this->fs_container.read_last( last );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  status = this->fs_container.get_iterator( last, this->fs_last.bi_iterator );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  this->fs_last.good( (last != end) );

  return this->fs_last;
}

_file_sequence_t::_file_sequence_t( void ) : fs_container(), fs_last( this )
{}

_file_sequence_t::_file_sequence_t( const char *filename ) : fs_container( filename ), fs_last( this )
{}

_file_sequence_t::~_file_sequence_t( void )
{}

void _file_sequence_t::clear( void )
{
  iostatus_t    status;
  const string  func( "_file_sequence_t::clear()" );

  status = this->fs_container.truncate();

  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  return;
}

void _file_sequence_t::sync( void )
{
  iostatus_t     status;
  const string   func( "_file_sequence_t::sync()" );

  status = this->fs_container.sync();

  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  return;
}

void _file_sequence_t::open( const char *filename )
{
  iostatus_t   status;
  const string func( "_file_sequence_t::open(...)" );

  status = this->fs_container.close().open( filename );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  return;
}

bool _file_sequence_t::modified( void )
{
  bool          modified;
  iostatus_t    status;
  const string  func( "_file_sequence_t::modified()" );

  status = this->fs_container.modified( modified );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  return modified;
}

bool _file_sequence_t::empty( void )
{
  iostatus_t    status;
  streamoff     begin, end;
  const string  func( "_file_sequence_t::empty()" );

  status = this->fs_container.read_begin( begin );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  status = this->fs_container.read_end( end );
  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  return( begin == end );
}

size_t _file_sequence_t::size( void )
{
  size_t        nobj = 0;
  iostatus_t    status;
  FileIterator  iter;
  const string  func( "_file_sequence_t::size()" );

  status = this->fs_container.read_size( nobj );

  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  return nobj;
}

void _file_sequence_t::compact( void )
{
  iostatus_t    status;
  const string  func( "_file_sequence_t::compact()" );

  status = this->fs_container.compact_data();

  if( status != FileContainerError::all_good )
    throwErrorAndDumpFile( this->fs_container, status, func, this->fs_container.filename(), __LINE__ );

  return;
}

} // namespace jccommon
} JOBCONTROL_NAMESPACE_END
