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

#ifndef GLITE_WMS_COMMON_UTILITIES_FILELISTITERATOR_H
#define GLITE_WMS_COMMON_UTILITIES_FILELISTITERATOR_H

#ifndef GLITE_WMS_COMMON_UTILITIES_FILECONTAINER_H
#include "gltie/wms/common/utilities/filecontainer.h"
#endif

#include <boost/shared_ptr.hpp>

namespace classad { class ClassAd; } // Forward declaration

namespace glite {
namespace wms {
namespace common {
namespace utilities {

class _file_sequence_t;

class _base_iterator_t {
  friend class _file_sequence_t;

public:
  _base_iterator_t( void );
  _base_iterator_t( const _base_iterator_t &bt );

  ~_base_iterator_t( void );

  inline bool is_equal( const _base_iterator_t &bi ) const
  { return( (this->bi_container == bi.bi_container) && (this->bi_iterator.position() == bi.bi_iterator.position()) ); }
  inline bool is_different( const _base_iterator_t &bi ) const
  { return( (this->bi_container != bi.bi_container) || (this->bi_iterator.position() != bi.bi_iterator.position()) ); }

  _base_iterator_t &copy( const _base_iterator_t &bi );
  _base_iterator_t &read_string( bool check = true );
  _base_iterator_t &increment( void );
  _base_iterator_t &decrement( void );

  inline void check_status( void ) const
  {
    if( !this->bi_good )
      throwErrorAndDumpFile( *this->bi_container, FileContainerError::unavailable_position, "_base_iterator_t::check_status()",
			     this->bi_container->filename(), __LINE__ );
  }
  inline bool good( void ) const { return this->bi_good; }
  inline const std::string &get_data( void ) const { return( this->bi_data ); }

protected:
  _base_iterator_t( FileIterator &fi, _file_sequence_t *sequence );
  _base_iterator_t( _file_sequence_t *sequence );

  inline void good( bool gd ) { this->bi_good = gd; return; }

  mutable bool        bi_new;
  bool                bi_good;
  FileContainer      *bi_container;
  FileIterator        bi_iterator;
  std::string         bi_data;
};

template <class Type>
class StdConverter {
public:
  inline StdConverter( void ) {}

  inline std::string operator()( const Type &data )
  try { return boost::lexical_cast<std::string>( data ); }
  catch( boost::bad_lexical_cast &e ) { throw FileContainerError( FileContainerError::cannot_convert_to_string ); }

  inline Type operator()( const std::string &data ) 
  try { return boost::lexical_cast<Type>( data ); }
  catch( boost::bad_lexical_cast &e ) { throw FileContainerError( FileContainerError::cannot_convert_from_string ); }
};

template <> class StdConverter<std::string> {};

template <> class StdConverter<classad::ClassAd> {
public:
  StdConverter( void );
  ~StdConverter( void );

  std::string operator()( const classad::ClassAd &data );
  const classad::ClassAd &operator()( const std::string &data );

private:
  classad::ClassAd    *sc_classad;
};

template <class Type, class Converter>
class FileList;

template <class Type, class Converter = StdConverter<Type> >
class FLIterator : private _base_iterator_t {
  friend class FileList<Type, Converter>;

public:
  typedef Type                             value_type;
  typedef Type *                           pointer;
  typedef Type &                           reference;
  typedef ptrdiff_t                        difference_type;
  typedef std::bidirectional_iterator_tag  iterator_category;

  FLIterator( void );
  FLIterator( const FLIterator & );

  ~FLIterator( void );

  inline FLIterator &operator=( const FLIterator &that )
  {
    if( this != &that ) {
      this->copy( that );
      this->fli_object = that.fli_object;
    }

    return *this;
  }
  inline const Type &operator*( void ) const
  {
    this->check_status();
    if( this->bi_new ) {
      this->fli_object.reset( new Type(fli_s_converter(this->bi_data)) );
      this->bi_new = false;
    }
    return *this->fli_object;
  }
  inline const Type *operator->( void ) const
  {
    this->check_status();
    if( this->bi_new ) {
      this->fli_object.reset( new Type(fli_s_converter(this->bi_data)) );
      this->bi_new = false;
    }
    return this->fli_object.get();
  }

  // Prefix operators
  inline FLIterator &operator++( void ) { this->increment(); this->read_string( false ); return *this; }
  inline FLIterator &operator--( void ) { this->decrement(); this->read_string( false ); return *this; }
  // Postfix operators
  inline FLIterator operator++( int ) { FLIterator tmp( *this ); this->increment(); this->read_string( false ); return tmp; }
  inline FLIterator operator--( int ) { FLIterator tmp( *this ); this->decrement(); this->read_string( false ); return tmp; }

  inline bool operator==( const FLIterator &fli ) { return this->is_equal( fli ); }
  inline bool operator!=( const FLIterator &fli ) { return this->is_different( fli ); }

  inline void reset( void ) { this->good( false ); return; }

  static Converter    fli_s_converter;

protected:
  FLIterator( const _base_iterator_t &bli );
  FLIterator( const FileIterator &it, _file_sequence_t &seq );

  inline _base_iterator_t &getBase( void ) { return( (_base_iterator_t &)(*this) ); }
  inline const _base_iterator_t &getBase( void ) const { return( (const _base_iterator_t &)(*this) ); }

  mutable boost::shared_ptr<Type>      fli_object;
};

template <class Type, class Converter>
Converter  FLIterator<Type, Converter>::fli_s_converter;

template <class Type, class Converter>
FLIterator<Type, Converter>::FLIterator( void ) : _base_iterator_t(), fli_object() {}

template <class Type, class Converter>
FLIterator<Type, Converter>::FLIterator( const FLIterator<Type, Converter> &fli ) :
  _base_iterator_t( fli ), fli_object()
{}

template <class Type, class Converter>
FLIterator<Type, Converter>::FLIterator( const _base_iterator_t &bi ) : _base_iterator_t( bi ), fli_object()
{ this->read_string( false ); }

template <class Type, class Converter>
FLIterator<Type, Converter>::FLIterator( const FileIterator &it, _file_sequence_t &seq ) : _base_iterator_t( it, seq ),
													    fli_object()
{ this->read_string( false ); }

template <class Type, class Converter>
FLIterator<Type, Converter>::~FLIterator( void ) {}

template<> inline const std::string &FLIterator<std::string>::operator*( void ) const 
{ 
  this->check_status();
  return this->bi_data;
}
template<> inline const std::string *FLIterator<std::string>::operator->( void ) const 
{
  this->check_status();
  return &this->bi_data;
}

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_UTILITIES_FILELISTITERATOR_H */

// Local Variables:
// mode: c++
// End:

