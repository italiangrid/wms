#include <cctype>

#include <string>
#include <algorithm>

#include "glite/wmsutils/jobid/JobId.h"

using namespace std;

namespace glite {
namespace wmsutils {
namespace jobid {

namespace {

class HexInt {
public:
  HexInt( unsigned int i = 0 );
  HexInt( const string &str );
  HexInt( string::const_iterator begin, string::const_iterator end );
  ~HexInt( void );

  inline operator unsigned int( void ) const { return this->hi_int; }
  inline operator const string &( void ) const { return this->hi_str; }

  static unsigned int least( void ) { return hi_s_least; }
  static void least( unsigned int least ) { hi_s_least = least; }

private:
  void parseString( void );

  unsigned int    hi_int;
  string          hi_str;

  static unsigned int  hi_s_least;
  static const char   *hi_s_map;
};

class BadChar {
public:
  BadChar( void );
  ~BadChar( void );

  inline bool operator()( char c )
  { return( !(((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) ||
  	      (c == '.') || (c == '-') || (c == ' ')) ); }
};

unsigned int  HexInt::hi_s_least = 2;
const char   *HexInt::hi_s_map = "0123456789abcdef";

HexInt::HexInt( unsigned int ui ) : hi_int( ui ), hi_str( hi_s_least, '0' )
{
  int                       n;
  string::reverse_iterator  pos = this->hi_str.rbegin();

  while( ui != 0 ) {
    n = ui % 16;
    if( pos != this->hi_str.rend() ) {
      *pos = hi_s_map[n];
      pos += 1;
    }
    else this->hi_str.insert( this->hi_str.begin(), hi_s_map[n] );

    ui /= 16;
  }

  if( this->hi_str.length() < hi_s_least )
    this->hi_str.insert( this->hi_str.begin(), (hi_s_least - this->hi_str.length()), '0' );
}

HexInt::HexInt( const string &str ) : hi_int( 0 ), hi_str( str )
{
  this->parseString();
}

HexInt::HexInt( string::const_iterator begin, string::const_iterator end ) : hi_int( 0 ), hi_str( begin, end )
{
  this->parseString();
}

void HexInt::parseString( void )
{
  int                        hexbase;
  char                      *pos, *end = (char *) hi_s_map + 16;
  string::reverse_iterator   it;

  for( it = this->hi_str.rbegin(), hexbase = 1; it != this->hi_str.rend(); ++it, hexbase *= 16 ) {
    pos = find( (char *) hi_s_map, end, (char) tolower(*it) );

    if( pos != end ) this->hi_int += hexbase * (pos - hi_s_map);
    else {
      this->hi_int = 0;
      break;
    }
  }

  return;
}

HexInt::~HexInt( void ) {}

BadChar::BadChar( void ) {}

BadChar::~BadChar( void ) {}

/*
  Helper function for the get_reduced_part(...)
*/
string get_reduced_part_internal( const string &unique, int level )
{
  string::size_type  length = unique.length();
  string             piece( unique.substr(0, 2) ), answer;

  if( (level == 0) || (length <= 2) ) answer.assign( piece );
  else if( length != 0 ) {
    answer.assign( piece );
    answer.append( 1, '/' );
    answer.append( get_reduced_part_internal(unique.substr(2, length - 2),  level - 1) );
  }

  return answer;
}

}; // Unnamed namespace

string get_reduced_part( const JobId &id, int level )
{
  return get_reduced_part_internal( id.getUnique(), level );
}

string to_filename( const JobId &id )
{
  string            sid( id.toString() ), coded;
  string::iterator  last, next;

  last = sid.begin();
  do {
    next = find_if( last, sid.end(), BadChar() );

    if( next != sid.end() ) {
      if( last != next ) coded.append( last, next );
      coded.append( 1, '_' );
      coded.append( HexInt(*next) );

      last = next + 1;
    }
    else coded.append( last, sid.end() );
  } while( next != sid.end() );

  return coded;
}

JobId from_filename( const string &filename )
{
  char                     c;
  string                   decoded;
  string::const_iterator   last, next;

  last = filename.begin();
  do {
    next = find( last, filename.end(), '_' );

    if( next != filename.end() ) {
      c = HexInt( next + 1, next + 3 );

      if( last != next ) decoded.append( last, next );
      decoded.append( 1, c );

      last = next + 3;
    }
    else decoded.append( last, filename.end() );
  } while( next != filename.end() );

  return JobId( decoded );
}

} // namespace jobid
} // namespace wmsutils
} // namespace glite
