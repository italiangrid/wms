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

#ifndef GLITE_WMS_COMMON_UTILITIES_FILELOCKER_H
#define GLITE_WMS_COMMON_UTILITIES_FILELOCKER_H

#include <cstdio>

#include <cstring>
#include <string>

#include <boost/thread/mutex.hpp>

#include "glite/wms/common/utilities/fstreamlock.h"

namespace glite {
namespace wms {
namespace common {
namespace utilities {

class CannotOpenLockFile : public std::string {
public:
  CannotOpenLockFile( int );

  ~CannotOpenLockFile( void );
};

class FileMutexLocked {};

class FileLocker;

class FileMutex {
  friend class FileLocker;

public:
  FileMutex( const std::string &filename );

  ~FileMutex( void );

private:
  FileMutex( const FileMutex &fm ); // Not implemented
  FileMutex &operator=( const FileMutex &fm ); // Not implemented

  bool           fm_locked;
  int            fm_descriptor;
  boost::mutex   fm_mutex;
};

class FileLocker {
public:
  FileLocker( FileMutex &fm, bool lock = true );

  ~FileLocker();

  int lock( void );
  int unlock( void );

private:
  FileLocker( void ); // Not implemented
  FileLocker( const FileLocker &fl ); // Not implemented
  FileLocker &operator=( const FileLocker &fl ); // Not implemented

  bool                       *fl_locked;
  DescriptorLock              fl_filelock;
  boost::mutex::scoped_lock   fl_mutexlock;
};

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_UTILITIES_FILELOCKER_H */

// Local Variables:
// mode: c++
// End:
