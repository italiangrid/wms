#ifndef GLITE_WMS_JOBSUBMISSION_COMMON_FILELIST_H
#define GLITE_WMS_JOBSUBMISSION_COMMON_FILELIST_H 

#include <iterator>

#include <boost/lexical_cast.hpp>

#include "common/file_container.h"
#include "common/filelist_iterator.h"

#include "jobcontrol_namespace.h"

JOBCONTROL_NAMESPACE_BEGIN {
namespace jccommon {

class FileListDescriptorMutex;

class _file_sequence_t {
  friend class FileListDescriptorMutex;
  friend class _base_iterator_t;

public:
  void clear( void );
  void compact( void );
  void sync( void );
  void open( const char *filename );
  bool modified( void );
  bool empty( void );
  size_t size( void );

  inline void close( void ) { this->fs_container.close(); }
  inline const std::string &filename( void ) const { return this->fs_container.filename(); }

private:
  typedef FileContainerError::iostatus_t   iostatus_t;

protected:
  _file_sequence_t( void );
  _file_sequence_t( const char *filename );

  ~_file_sequence_t( void );

  _base_iterator_t &insertData( const _base_iterator_t &position, const std::string &val );
  _base_iterator_t &erasePointer( const _base_iterator_t &position );
  _base_iterator_t &eraseInterval( const _base_iterator_t &first, const _base_iterator_t &last );
  _base_iterator_t &getBegin( void );
  _base_iterator_t &getEnd( void );
  _base_iterator_t &getStart( void );
  _base_iterator_t &getLast( void );

  void removeData( const std::string &value );
  void swapContainer( _file_sequence_t &other );

protected:
  FileContainer           fs_container;
  _base_iterator_t        fs_last;
};

template <class Type, class Converter = StdConverter<Type> >
class FileList : public _file_sequence_t {
public:
  typedef Type                                    value_type;
  typedef Type *                                  pointer;
  typedef const Type *                            const_pointer;
  typedef Type &                                  reference;
  typedef const Type &                            const_reference;
  typedef FLIterator<Type, Converter>             iterator;
  typedef FLIterator<const Type, Converter>       const_iterator;
  typedef std::reverse_iterator<iterator>         reverse_iterator;
  typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

  FileList( void );
  FileList( const std::string &filename );
  FileList( const char *filename );

  ~FileList( void );

  inline void open( const std::string &filename )
  { this->_file_sequence_t::open( filename.c_str() ); }

  inline iterator begin( void ) { return iterator( this->getBegin() ); }
  inline iterator end( void ) { return iterator( this->getEnd() ); }
  inline const_iterator begin( void ) const { return const_iterator( this->getBegin() ); }
  inline const_iterator end( void ) const { return const_iterator( this->getEnd() ); }
  inline reverse_iterator rbegin( void ) { return reverse_iterator( this->end() ); }
  inline reverse_iterator rend( void ) { return reverse_iterator( this->begin() ); }
  inline const_reverse_iterator rbegin( void ) const { return const_reverse_iterator( this->end() ); }
  inline const_reverse_iterator rend( void ) const { return const_reverse_iterator( this->begin() ); }

  inline Type front( void ) { return *(this->begin()); }
  inline Type back( void ) { return *(--this->end()); }

  inline void push_front( const Type &data ) { this->insertData( this->getBegin(), iterator::fli_s_converter(data) ); }
  inline void push_back( const Type &data ) { this->insertData( this->getEnd(), iterator::fli_s_converter(data) ); }

  inline void pop_front( void ) { this->erasePointer( this->getBegin() ); }
  inline void pop_back( void ) { this->erasePointer( this->getEnd().decrement() ); }

  template <class InIt>
  inline void insert( const iterator &position, InIt first, InIt last )
  {
    InIt                    it;
    _base_iterator_t        mylast, begin = this->getBegin(), end = this->getEnd();

    if( position.getBase().is_equal(end) ) {
      for( it = first; it != last; ++it )
	this->insertData( this->getEnd(), iterator::fli_s_converter(*it) );
    }
    else {
      for( it = first; it != last; ++it )
	this->insertData( position.getBase(), iterator::fli_s_converter(*it) );
    }

    return;
  }

  inline iterator insert( const iterator &position, const Type &val )
  { return iterator( this->insertData(position.getBase(), iterator::fli_s_converter(val)) ); }
  inline iterator erase( const iterator &position )
  { return iterator( this->erasePointer(position.getBase()) ); }
  inline iterator erase( const iterator &first, const iterator &last )
  { return iterator( this->eraseInterval(first.getBase(), last.getBase()) ); }

  inline void swap( FileList<Type, Converter> &other ) { this->swapContainer(other); }

  void remove( const Type &val ) { this->removeData( iterator::fli_s_converter(val) ); }
};

template<class Type, class Converter>
FileList<Type, Converter>::FileList( void ) : _file_sequence_t() {}

template<class Type, class Converter>
FileList<Type, Converter>::FileList( const std::string &filename ) :
  _file_sequence_t( filename.c_str() )
{}

template<class Type, class Converter>
FileList<Type, Converter>::FileList( const char *filename ) :
  _file_sequence_t( filename )
{}

template<class Type, class Converter>
FileList<Type, Converter>::~FileList() {}

template<> 
inline void FileList<std::string>::push_front( const std::string &data ) { this->insertData( this->getBegin(), data ); }

template<>
inline void FileList<std::string>::push_back( const std::string &data ) { this->insertData( this->getEnd(), data ); }

template<>
inline FileList<std::string>::iterator FileList<std::string>::insert( const FileList<std::string>::iterator &position,
								      const std::string &val )
{ return iterator( this->insertData(position.getBase(), val) ); }

template<>
inline void FileList<std::string>::remove( const std::string &val ) { this->removeData( val ); }

} // namespace jccommon
} JOBCONTROL_NAMESPACE_END

#endif /* GLITE_WMS_JOBSUBMISSION_COMMON_FILELIST_H */
