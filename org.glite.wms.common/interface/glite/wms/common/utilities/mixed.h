/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org\partners for details on the
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

#ifndef GLITE_WMS_COMMON_UTILITIES_MIXED_H
#define GLITE_WMS_COMMON_UTILITIES_MIXED_H

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <iostream>
#include <vector>
#include <string>

namespace glite {
namespace wms {
namespace common {
namespace utilities {

class Mixed {
  friend std::ostream &operator<<( std::ostream &os, const Mixed &val );
  friend bool operator==( const Mixed &a, const Mixed &b );
  inline friend bool operator!=( const Mixed &a, const Mixed &b )
  { return( !operator==(a, b) ); }

public:
  enum mixed_type_t { unk = -1, log, intg, real, chr, vgen, vlog, vint, vreal, vchr };

  // Constructor
  Mixed( void );
  Mixed( bool b );
  Mixed( int i );
  Mixed( double d );
  Mixed( const char *c );
  Mixed( const char *begin, const char *end );
  Mixed( const std::string &s );
  Mixed( const Mixed &m );

  template<class type_t>
  inline Mixed( int n, const type_t *t ) : pbuf( NULL ) { this->setVector( n, t ); }
  template<class type_t>
  inline Mixed( const std::vector<type_t> &v ) : pbuf( NULL ) { this->setVector( v ); }

  // Destructor
  ~Mixed( void );

  // Extractors
  inline char                 *getStringType( void ) { return( stringTypes[(int)this->type + 1] ); }
  inline char                 *getStringValue( void ) { return( this->castString() ); }
  inline std::vector<Mixed>   &getVectorValue( void ) { return( this->castVector() ); }

  // Constant extractors
  inline mixed_type_t      getType( void ) const { return( this->type ); }
  inline bool              getLogicalValue( void ) const { return( ((Mixed *)this)->castBool() ); }
  inline int               getIntegerValue( void ) const { return( ((Mixed *)this)->castInteger() ); }
  inline double            getDoubleValue( void ) const { return( ((Mixed *)this)->castReal() ); }

  inline const char       *getStringValue( void ) const { return( (const char *) ((Mixed *)this)->castString() ); }
  inline const char       *getStringType( void ) const { return( (const char *) stringTypes[(int)this->type + 1] ); }

  inline const std::vector<Mixed> &getVectorValue( void ) const { return( (const std::vector<Mixed> &) ((Mixed *)this)->castVector() ); }

  inline bool defined( void ) const { return( this->type != unk ); }
  inline bool isVector( void ) const  { return( ((int)this->type >= (int)vgen) && ((int)this->type <= (int)vchr) ); }

  // Methods
  inline Mixed &setLogicalValue( bool b )
  {
    this->deletePointer(); this->type = log; this->bval = b;
    return( *this );
  }
  inline Mixed &setIntegerValue( int i )
  {
    this->deletePointer(); this->type = intg; this->ival = i;
    return( *this );
  }
  inline Mixed &setDoubleValue( double d )
  {
    this->deletePointer(); this->type = real; this->rval = d;
    return( *this );
  }
  inline Mixed &setStringValue( const char *s )
  {
    this->setString( s );
    return( *this );
  }
  inline Mixed &setStringValue( const char *begin, const char *end )
  {
    this->setString( begin, end );
    return( *this );
  }
  template<class type_t>
  inline Mixed &setVectorValue( int n, const type_t *t )
  {
    this->deletePointer(); this->setVector( n, t );
    return( *this );
  }
  inline Mixed &setVectorValue( const std::vector<Mixed> &m, enum mixed_type_t t )
  {
    this->deletePointer(); 
    this->setVector( m ); this->type = t;
    return( *this );
  }
  template<class type_t>
  inline Mixed &setVectorValue( const std::vector<type_t> &v )
  {
    this->deletePointer(); this->setVector( v );
    return( *this );
  }

  inline Mixed &undefine( void )
  {
    this->deletePointer(); this->type = unk;
    return( *this );
  }
    
  // Operators
  inline operator bool( void ) { return( this->castBool() ); }
  inline operator int( void ) { return( this->castInteger() ); }
  inline operator double( void ) { return( this->castReal() ); }
  inline operator char *( void ) { return( this->castString() ); }
  inline operator const char *( void ) { return( (const char *) this->castString() ); }
  inline operator std::vector<Mixed>( void ) { return( this->castVector() ); }

  inline operator const char *( void ) const { return( (const char *) ((Mixed *)this)->castString() ); }

  inline std::vector<Mixed> *operator->( void ) { return( this->castPointerVector() ); }
  inline bool operator=( bool b )
  {
    this->deletePointer(); this->type = log; this->bval = b;
    return( b );
  }
  inline int operator=( int i )
  {
    this->deletePointer(); this->type = intg; this->ival = i;
    return( i );
  }
  inline double operator=( double d )
  {
    this->deletePointer(); this->type = real; this->rval = d;
    return( d );
  }
  inline char *operator=( char *s )
  {
    this->setString( s );
    return( s );
  }
  template<class type_t>
  inline std::vector<type_t> &operator=( std::vector<type_t> &v )
  {
    this->deletePointer(); this->setVector( v );
    return( v );
  }
  inline Mixed &operator=( const Mixed &m )
  { this->copyValue( m ); return( *this ); }

  // Class Methods
  inline static char *getType( enum mixed_type_t type ) { return( stringTypes[(int) type + 1] ); }
  inline static void setVerbose( bool v = true ) { verbose = v; }
  inline static Mixed &zero( void ) { return( __zero ); }
  inline static bool getVerbose( void ) { return( verbose ); }

private:
  inline void deletePointer( void )
  { 
    if( this->pbuf ) {
      delete [] this->pbuf;
      this->pbuf = NULL;
    }

    if( this->type == chr ) delete [] this->sval.value; 
    else if( this->isVector() ) delete this->vval;
  }
  inline void setString( const char *begin, const char *end = NULL )
  {
    size_t    len = (size_t) -1;

    if( end == NULL ) len = strlen( begin );
    else if( end > begin ) len = end - begin;

    if( len != (size_t) -1 ) {
      this->deletePointer();

      this->sval.value = new char[len + 1];
      this->sval.length = len;
      memcpy( this->sval.value, begin, len ); this->sval.value[len] = '\0';

      this->type = chr;
    }
  }
  inline void setVector( int max, const bool *b )
  {
    int               n;

    this->vval = new std::vector<Mixed>;
    for( n = 0; n < max; n++ ) this->vval->push_back( b[n] );
    this->type = vlog;
  }
  inline void setVector( int max, const int *i )
  {
    int               n;

    this->vval = new std::vector<Mixed>;
    for( n = 0; n < max; n++ ) this->vval->push_back( i[n] );
    this->type = vint;
  }
  inline void setVector( int max, const double *d )
  {
    int               n;

    this->vval = new std::vector<Mixed>;
    for( n = 0; n < max; n++ ) this->vval->push_back( d[n] );
    this->type = vreal;
  }
  inline void setVector( int max, const char **c )
  {
    int               n;

    this->vval = new std::vector<Mixed>;
    for( n = 0; n < max; n++ ) this->vval->push_back( c[n] );
    this->type = vchr;
  }
  inline void setVector( int max, const std::string *s )
  {
    int               n;

    this->vval = new std::vector<Mixed>;
    for( n = 0; n < max; n++ ) this->vval->push_back( s[n].c_str() );
    this->type = vchr;
  }
  inline void setVector( int max, const Mixed *m )
  {
    int               n;

    this->vval = new std::vector<Mixed>;
    for( n = 0; n < max; n++ ) this->vval->push_back( m[n] );
    this->type = vgen;
  }
  inline void setVector( const std::vector<bool> &b )
  {
    size_t            n, max = b.size();

    this->vval = new std::vector<Mixed>;
    for( n = 0; n < max; n++ ) this->vval->push_back( b[n] );
    this->type = vlog;
  }
  inline void setVector( const std::vector<int> &i )
  {
    size_t            n, max = i.size();

    this->vval = new std::vector<Mixed>;
    for( n = 0; n < max; n++ ) this->vval->push_back( i[n] );
    this->type = vint;
  }
  inline void setVector( const std::vector<double> &r )
  {
    size_t            n, max = r.size();

    this->vval = new std::vector<Mixed>;
    for( n = 0; n < max; n++ ) this->vval->push_back( r[n] );
    this->type = vreal;
  }
  inline void setVector( const std::vector<char *> &c )
  {
    size_t            n, max = c.size();

    this->vval = new std::vector<Mixed>;
    for( n = 0; n < max; n++ ) this->vval->push_back( c[n] );
    this->type = vchr;
  }
  inline void setVector( const std::vector<std::string> &s )
  {
    size_t            n, max = s.size();

    this->vval = new std::vector<Mixed>;
    for( n = 0; n < max; n++ ) this->vval->push_back( s[n].c_str() );
    this->type = vchr;
  }
  inline void setVector( const std::vector<Mixed> &b )
  {
    this->vval = new std::vector<Mixed>( b );
    this->type = vgen;
  }

  inline void copyValue( const Mixed &m )
  {
    if( m.type == chr ) this->setString( m.sval.value, m.sval.value + m.sval.length );
    else if( m.type == log ) this->setLogicalValue( m.bval );
    else if( m.type == intg ) this->setIntegerValue( m.ival );
    else if( m.type == real ) this->setDoubleValue( m.rval );
    else if( m.isVector() ) this->setVectorValue( *m.vval, m.type );
    else if( m.type == unk ) this->undefine();
  }
  inline bool castBool( void )
  {
    switch( this->type ) {
    case log:
      return( bval );
      break;
    case intg:
      return( (bool) ival );
      break;
    case real:
      return( (bool) rval );
      break;
    case chr:
      if( this->sval.value ) {
	if( !memcmp(this->sval.value, "true", 4) || !memcmp(this->sval.value, "TRUE", 4) || 
	    !memcmp(this->sval.value, "yes", 3) || !memcmp(this->sval.value, "YES", 3) )
	  return( true );
	else {
	  int    i = atoi( this->sval.value );
	  return( (bool) i );
	}
      }
      else return( false );
      break;
    case unk:
    case vgen:
    case vlog:
    case vint:
    case vreal:
    case vchr:
    default:
      return( false );
      break;
    }
  }
  inline int castInteger( void )
  {
    switch( this->type ) {
    case log:
      return( (int) bval );
      break;
    case intg:
      return( ival );
      break;
    case real:
      return( (int) rval );
      break;
    case chr:
      if( this->sval.value ) {
	int    val = atoi( this->sval.value );
	return( val );
      }
      else return( 0 );
      break;
    case unk:
    case vgen:
    case vlog:
    case vint:
    case vreal:
    case vchr:
    default:
      return( 0 );
      break;
    }
  }
  inline double castReal( void )
  {
    switch( this->type ) {
    case log:
      return( (double) bval );
      break;
    case intg:
      return( (int) ival );
      break;
    case real:
      return( rval );
      break;
    case chr:
      if( this->sval.value ) {
	double val = atof( this->sval.value );
	return( val );
      }
      else return( 0.0 );
      break;
    case unk:
    case vgen:
    case vlog:
    case vint:
    case vreal:
    case vchr:
    default:
      return( 0.0 );
      break;
    }
  }
  inline char *castString( void )
  {
    if( this->pbuf == NULL ) this->pbuf = new char[30];

    switch( this->type ) {
    case log:
      strcpy( this->pbuf, (this->bval ? "true" : "false") );
      return( this->pbuf );
      break;
    case intg:
      sprintf( this->pbuf, "%d", ival );
      return( this->pbuf );
      break;
    case real:
      sprintf( this->pbuf, "%f", rval );
      return( this->pbuf );
      break;
    case chr:
      return( this->sval.value );
      break;
    case unk:
    case vgen:
    case vlog:
    case vint:
    case vreal:
    case vchr:
    default:
      return( NULL );
      break;
    }
  }
  inline std::vector<Mixed> &castVector( void )
  {
    switch( this->type ) {
    case vgen:
    case vlog:
    case vint:
    case vreal:
    case vchr:
      return( *vval );
      break;
    case unk:
    case log:
    case intg:
    case real:
    case chr:
    default:
      return( *empty );
      break;
    }
  }
  inline std::vector<Mixed> *castPointerVector( void )
  {
    switch( this->type ) {
    case vgen:
    case vlog:
    case vint:
    case vreal:
    case vchr:
      return( vval );
      break;
    case unk:
    case log:
    case intg:
    case real:
    case chr:
    default:
      return( empty );
      break;
    }
  }

  // Object data...
  enum mixed_type_t   type;
  union {
    bool                  bval;
    int                   ival;
    double                rval;
    struct {
      char      *value;
      size_t     length;
    }                     sval;
    std::vector<Mixed>   *vval;
  };
  char               *pbuf;

  // Class data...
  static bool                 verbose;
  static char                *stringTypes[];
  static std::vector<Mixed>  *empty;
  static Mixed                __zero;
};

typedef Mixed::mixed_type_t   mixed_type_t;

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_UTILITIES_MIXED_H */

// Local Variables:
// mode: c++
// End:
