#ifndef GLITE_WMS_COMMON_UTILITIES_EDGSTRSTREAM_H
#define GLITE_WMS_COMMON_UTILITIES_EDGSTRSTREAM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STRINGSTREAM
#include <sstream>
#else
#include <strstream>
#include <string>
#endif

namespace glite { 
namespace wms { 
namespace common { 
namespace utilities {

#ifdef HAVE_STRINGSTREAM
typedef std::stringstream   edgstrstream;
typedef std::istringstream  iedgstrstream;
typedef std::ostringstream  oedgstrstream;
#else

class edgstrstream : public std::strstream {
public:
  edgstrstream( int which = std::ios::out ) : std::strstream() {}
  edgstrstream( const std::string &s, int which = std::ios::in | std::ios::out ) : std::strstream() {}
  ~edgstrstream( void ) {}

  std::string str( void ) const
  {
    std::string   output( ((std::strstream*)this)->std::strstream::str() );
    ((std::strstream*)this)->freeze( 0 );
    return output;
  }
  void str( std::string &s )
  {
    s.assign( this->std::strstream::str() );
    ((std::strstream*)this)->freeze( 0 );
  }
};

class iedgstrstream : public std::istrstream {
public:
  iedgstrstream( const char *s, int which = std::ios::in ) : std::istrstream( s, which )
  {}
  iedgstrstream( const std::string &s, int which = std::ios::in ) : 
    std::istrstream( s.c_str(), which ) {}
  ~iedgstrstream( void ) {}

  std::string str( void )
  {
	  std::string   output( this->rdbuf()->str() );
	  this->rdbuf()->freeze( 0 );
	  return output;
  }
  void str( std::string &s )
  {
	  s.assign( this->rdbuf()->str() );
	  this->rdbuf()->freeze( 0 );
  }
};

class oedgstrstream : public std::ostrstream {
public:
  oedgstrstream( int which = std::ios::out ) : std::ostrstream() {}
  oedgstrstream( const std::string &s, int which = std::ios::out ) : std::ostrstream() {}
  ~oedgstrstream( void ) {}

  std::string str( void )
  {
    std::string   output( this->std::ostrstream::str() );
    this->freeze( 0 );
    return output;
  }
  void str( std::string &s )
  {
    s.assign( this->std::ostrstream::str() );
    this->freeze( 0 );
  }
};

#endif

}}}}

#endif /* GLITE_WMS_COMMON_UTILITIES_EDGSTRSTREAM_H */

// Local Variables:
// mode: c++
// End:
