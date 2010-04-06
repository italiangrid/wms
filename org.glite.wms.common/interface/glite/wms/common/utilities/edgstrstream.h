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
