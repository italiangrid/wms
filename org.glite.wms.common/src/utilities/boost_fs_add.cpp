/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
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

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <memory>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include "boost_fs_add.h"

using namespace std;
namespace fs = boost::filesystem;

namespace glite { 
namespace wms { 
namespace common {
namespace utilities {

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

void create_parents( const fs::path &dpath )
{
  string     err( "create_parent(): " );
  fs::path       branch( dpath.branch_path() );
  string     who("create_parents");

  if( dpath.empty() ) {
    err.append( "cannot create an empty path." );

    throw CannotCreateParents( err );
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

      throw CannotCreateParents( err );
    }
  }
  else if( !is_directory(dpath) ) {
    err.append( dpath.native_file_string() ); err.append( " is not a directory." );

    throw CannotCreateParents( err );
  }

  return;
}

}}}}
