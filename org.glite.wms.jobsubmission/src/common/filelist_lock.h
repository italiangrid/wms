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

#ifndef GLITE_WMS_COMMON_UTILITIES_FILELISTLOCK_H
#define GLITE_WMS_COMMON_UTILITIES_FILELISTLOCK_H

#include <boost/thread/mutex.hpp>

#include "common/fstreamlock.h"

#include "jobcontrol_namespace.h"

JOBCONTROL_NAMESPACE_BEGIN {
namespace jccommon {

class _file_sequence_t;
class FileListLock;

class FileListDescriptorMutex {
  friend class FileListLock;

public:
  FileListDescriptorMutex( _file_sequence_t &fs );

  ~FileListDescriptorMutex( void );

protected:
  bool         fldm_locked;
  int          fldm_descriptor;
};

class FileListMutex : private FileListDescriptorMutex {
  friend class FileListLock;

public:
  FileListMutex( _file_sequence_t &fs );

  ~FileListMutex( void );

private:
  boost::mutex   flm_mutex;
};

class FileListLock {
public:
  FileListLock( FileListDescriptorMutex &fldm);
  FileListLock( FileListMutex &flm);

  ~FileListLock( void );

  int lock( void );
  int unlock( void );

private:
  bool                       *fl_locked;
  boost::mutex::scoped_lock  *fl_mutexlock;
  DescriptorLock              fl_filelock;
};

} // namespace jccommon
} JOBCONTROL_NAMESPACE_END
#endif /* GLITE_WMS_COMMON_UTILITIES_FILELISTLOCK_H */
