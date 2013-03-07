#include <classad_distribution.h>

#include "common/filelist.h"

using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {
namespace jccommon {

/*
 *   Public methods
 *   */
_base_iterator_t::_base_iterator_t( void ) : bi_new( true ), bi_good( false ), bi_container( NULL ), bi_iterator(), bi_data()
{}

_base_iterator_t::_base_iterator_t( const _base_iterator_t &bi ) : bi_new( true ), bi_good( bi.bi_good ), bi_container( bi.bi_container ),
								   bi_iterator( bi.bi_iterator ), bi_data( bi.bi_data )
{}

_base_iterator_t::_base_iterator_t( FileIterator &fi, _file_sequence_t *sequence ) : bi_new( true ), bi_good( false ),
										     bi_container( &sequence->fs_container ),
										     bi_iterator( fi ), bi_data()
{
  this->read_string( false );
}

_base_iterator_t::_base_iterator_t( _file_sequence_t *sequence ) : bi_new( true ), bi_good( false ),
								   bi_container( &sequence->fs_container ),
								   bi_iterator(), bi_data()
{}

_base_iterator_t::~_base_iterator_t( void )
{}

_base_iterator_t &_base_iterator_t::copy( const _base_iterator_t &that )
{
  if( this != &that ) {
    this->bi_new = true;
    this->bi_good = that.bi_good;
    this->bi_container = that.bi_container;
    this->bi_iterator = that.bi_iterator;
    this->bi_data.assign( that.bi_data );
  }

  return( *this );
}

_base_iterator_t &_base_iterator_t::read_string( bool check )
{
  FileContainerError::iostatus_t     status;
  streamoff                          end;
  const string                       func( "_base_iterator_t::read_string()" );

  if( this->bi_container ) {
    status = this->bi_container->read_end( end );
    if( status != FileContainerError::all_good ) throwErrorAndDumpFile( *this->bi_container, status, func, this->bi_container->filename(), __LINE__ );

    this->bi_good = true;
    this->bi_new = true;

    if( !check && (this->bi_iterator.position() == end) ) {
      this->bi_good = false;
      this->bi_data.erase();
    }
    else {
      status = this->bi_container->read_data( this->bi_data, this->bi_iterator.position(), this->bi_iterator );

      if( status != FileContainerError::all_good ) {
	this->bi_good = false;

	throwErrorAndDumpFile( *this->bi_container, status, func, this->bi_container->filename(), __LINE__ );
      }
    }
  }
  else this->bi_good = false;

  return( *this );
}

_base_iterator_t &_base_iterator_t::increment( void )
{
  FileContainerError::iostatus_t     status;
  const string                       func( "_base_iterator_t::increment()" );

  status = this->bi_container->increment_iterator( this->bi_iterator );
  if( status != FileContainerError::all_good ) throwErrorAndDumpFile( *this->bi_container, status, func, this->bi_container->filename(), __LINE__ );

  return( *this );
}

_base_iterator_t &_base_iterator_t::decrement( void )
{
  FileContainerError::iostatus_t     status;
  const string                       func( "_base_iterator_t::decrement()" );

  status = this->bi_container->decrement_iterator( this->bi_iterator );
  if( status != FileContainerError::all_good ) throwErrorAndDumpFile( *this->bi_container, status, func, this->bi_container->filename(), __LINE__ );

  return( *this );
}

StdConverter<classad::ClassAd>::StdConverter( void ) : sc_classad( NULL )
{}

StdConverter<classad::ClassAd>::~StdConverter( void )
{
  delete this->sc_classad;
}

string StdConverter<classad::ClassAd>::operator()( const classad::ClassAd &data )
{
  string                    classad;
  classad::ClassAdUnParser  unparser;

  unparser.Unparse( classad, (classad::ClassAd *) &data );

  return classad;
}

const classad::ClassAd &StdConverter<classad::ClassAd>::operator()( const string &data )
{
  classad::ClassAdParser    parser;

  if( this->sc_classad ) {
    delete this->sc_classad;
    this->sc_classad = NULL;
  }

  this->sc_classad = parser.ParseClassAd( data.c_str() );

  if( this->sc_classad == NULL ) throw FileContainerError( FileContainerError::cannot_convert_from_string );

  return *this->sc_classad;
}

} // namespace jccommon
} JOBCONTROL_NAMESPACE_END

