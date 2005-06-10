#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <memory>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

using namespace std;

namespace boost { namespace filesystem {

string normalize_path( const string &fpath )
{
  string                   modified;
  string::const_iterator   last, next;
  string::reverse_iterator check;

  last = fpath.begin();
  do {
    next = find( last, fpath.end(), '/' );

    if( next != fpath.end() ) {
      modified.append( last, next + 1 );

      for( last = next; *last == '/'; ++last );
    }
    else modified.append( last, fpath.end() );
  } while( next != fpath.end() );

  check = modified.rbegin();
  if( *check == '/' ) modified.assign( modified.begin(), modified.end() - 1 );

  return modified;
}

void create_parents( const path &dpath )
{
  string     err( "create_parent(): " );
  path       branch( dpath.branch_path() );
  string     who("create_parents");

  if( dpath.empty() ) {
    err.append( "cannot create an empty path." );

    throw filesystem_error( who, err );
  }
  else if( !exists(dpath) ) {
    if( branch.empty() ) create_directory( dpath );
    else if( !exists(branch) ) {
      create_parents( branch );
      create_directory( dpath );
    }
    else if( is_directory(branch) ) create_directory( dpath );
    else {
      err.append( branch.native_file_string() ); err.append( " is not a directory." );

      throw filesystem_error( who, err );
    }
  }
  else if( !is_directory(dpath) ) {
    err.append( dpath.native_file_string() ); err.append( " is not a directory." );

    throw filesystem_error( err, who );
  }

  return;
}

}};
