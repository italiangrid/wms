#ifndef GLITE_WMS_COMMON_UTILITIES_FILELISTLOCK_H
#define GLITE_WMS_COMMON_UTILITIES_FILELISTLOCK_H

#include <boost/thread/mutex.hpp>

#include "fstreamlock.h"

namespace glite {
namespace wms {
namespace common {
namespace utilities {

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
  FileListLock( FileListDescriptorMutex &fldm, bool lock = true );
  FileListLock( FileListMutex &flm, bool lock = true );

  ~FileListLock( void );

  int lock( void );
  int unlock( void );

private:
  bool                       *fl_locked;
  boost::mutex::scoped_lock  *fl_mutexlock;
  DescriptorLock              fl_filelock;
};

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_UTILITIES_FILELISTLOCK_H */

// Local Variables:
// mode: c++
// End:
