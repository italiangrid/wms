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
/*************************************************************************
 *                                                                       *
 * filename  : mixed.cpp                                                 *
 * begin     : Fri 12 Oct 2001                                           *
 * author    : Rosario Peluso                                            *
 * email     : Rosario.Peluso@pd.infn.it                                 *
 * copyright : (C) 2001 by INFN                                          *
 *                                                                       *
 *************************************************************************/

#include "glite/wms/common/utilities/mixed.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace utilities {

bool    Mixed::verbose = false;
char   *Mixed::stringTypes[] = { "unknown", "boolean", "integer", "real", "string",
				 "generic vector", "boolean vector", "integer vector", "real vector",
				 "string vector" };
Mixed   Mixed::__zero;

vector<Mixed>   *Mixed::empty = new vector<Mixed>( 1, Mixed::__zero );

Mixed::Mixed( void ) : type( unk ), pbuf( NULL )
{}

Mixed::Mixed( bool b ) : type( log ), bval( b ), pbuf( NULL )
{}

Mixed::Mixed( int i ) : type( intg ), ival( i ), pbuf( NULL )
{}

Mixed::Mixed( double d ) : type( real ), rval( d ), pbuf( NULL )
{}

Mixed::Mixed( const char *c ) : type( unk ), pbuf( NULL )
{
  this->setString( c );
}

Mixed::Mixed( const char *begin, const char *end ) : type( unk ), pbuf( NULL )
{
  this->setString( begin, end );
}

Mixed::Mixed( const string &s ) : type( unk ), pbuf( NULL )
{
  this->setString( s.c_str() ); 
}

Mixed::Mixed( const Mixed &m ) : type( unk ), pbuf( NULL )
{
  this->copyValue( m );
}

Mixed::~Mixed( void )
{
  this->deletePointer();
}

using namespace std;

ostream &operator<<( ostream &os, const Mixed &val )
{
  if( val.verbose )
    os << " (" << Mixed::stringTypes[(int)val.type + 1] << ") ";

  if( val.type == Mixed::unk ) os << "(null)";
  else if( val.type == Mixed::log ) os << (val.bval ? "true" : "false");
  else if( val.type == Mixed::intg ) os << val.ival;
  else if( val.type == Mixed::real ) os << val.rval;
  else if( val.type == Mixed::chr ) os.write( val.sval.value, val.sval.length );
  else if( val.isVector() ) {
    size_t   max = val.vval->size();
    for( size_t nn = 0; nn < max; nn++ ) 
      os << val.vval->operator[](nn) << ( (nn < (max - 1)) ? ", " : "" );
  }

  return( os );
}

bool operator==( const Mixed &a, const Mixed &b )
{
  if( &a == &b ) return( true );
  else if( a.type == b.type ) {
    bool answ = false;

    switch( a.type ) {
    case Mixed::log:   answ = (a.bval && b.bval); break;
    case Mixed::intg:  answ = (a.ival == b.ival); break;
    case Mixed::real:  answ = (a.rval == b.rval); break;
    case Mixed::chr:   answ = (strncmp(a.sval.value, b.sval.value, a.sval.length) == 0); break;
    case Mixed::vlog:
    case Mixed::vint:
    case Mixed::vreal: answ = (a.vval == b.vval); break;
    case Mixed::unk:   answ = true; break;
    case Mixed::vchr:
    case Mixed::vgen:
    default:    answ = false; break;
    }

    return( answ );
  }
  else return( false );
}

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace
