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

#include <cstdio>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "glite/wms/common/utilities/FileLocker.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace utilities {

CannotOpenLockFile::CannotOpenLockFile( int error ) : string( strerror(error) ) {}

CannotOpenLockFile::~CannotOpenLockFile( void ) {}

FileMutex::FileMutex( const string &filename ) : fm_locked( false ), fm_descriptor( -1 ), fm_mutex()
{
  string   lockfile( filename );

  lockfile.append( ".lock" );

  this->fm_descriptor = ::open( lockfile.c_str(), (O_RDONLY | O_CREAT) );
  if( this->fm_descriptor == -1 ) throw CannotOpenLockFile( errno );
}

FileMutex::~FileMutex( void )
{
  if( this->fm_descriptor != -1 ) ::close( this->fm_descriptor );
}

FileLocker::FileLocker( FileMutex &fm, bool lock ) : fl_locked( &fm.fm_locked ), fl_filelock( fm.fm_descriptor, false ),
						     fl_mutexlock( fm.fm_mutex, false )
{
  if( lock ) {
    if( *this->fl_locked ) throw FileMutexLocked();

    this->lock();
  }
}

FileLocker::~FileLocker( void )
{
  if( *this->fl_locked ) this->unlock();
}

int FileLocker::lock( void )
{
  int     answer = 0;

  if( *this->fl_locked ) throw FileMutexLocked();
  else {
    answer = this->fl_filelock.lock();
    if( !answer ) {
      this->fl_mutexlock.lock();

      *this->fl_locked = true;
    }
  }

  return( answer );
}

int FileLocker::unlock( void )
{
  int    answer = 0;

  if( *this->fl_locked ) {
    this->fl_mutexlock.unlock();

    answer = this->fl_filelock.unlock();
    if( !answer ) *this->fl_locked = false;
  }

  return( answer );
}

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace
