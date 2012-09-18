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

#include <cerrno>

#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>

#include "fstreamlock.h"
#include "glite/wms/common/utilities/streamdescriptor.h"

JOBCONTROL_NAMESPACE_BEGIN {
namespace jccommon {

using namespace std;

namespace {

inline int signal_aware_fcntl( int fd, int cmd, struct flock *fl )
{
  int    res;

  do {
    res = fcntl( fd, cmd, fl );
  } while( (res == -1) && (errno == EINTR) ); // We have been interrupted by a signal, just retry...

  return res;
}

} // Anonymous namespace

DescriptorLock::DescriptorLock( int fd, bool lock ) : dl_locked( false ), dl_fd( fd )
{
  if( lock ) this->lock();
}

DescriptorLock::~DescriptorLock( void )
{
  if( this->dl_locked ) this->unlock();
}

FstreamLock::FstreamLock(const fstream &fs, bool lock)
  : DescriptorLock(glite::wms::common::utilities::streamdescriptor(fs), lock) { }

FstreamLock::FstreamLock(const ifstream &fs, bool lock)
  : DescriptorLock(glite::wms::common::utilities::streamdescriptor(fs), lock) { }

FstreamLock::FstreamLock(const ofstream &fs, bool lock)
  : DescriptorLock(glite::wms::common::utilities::streamdescriptor(fs), lock) { }

FstreamLock::FstreamLock(const filebuf &fb, bool lock)
  : DescriptorLock(glite::wms::common::utilities::bufferdescriptor(fb), lock) { }

FstreamLock::~FstreamLock( void )
{}

int DescriptorLock::lock( void )
{
  int             res = 0;
  struct flock    fc;

  if( !this->dl_locked ) {
    fc.l_whence = SEEK_SET;
    fc.l_start = 0;
    fc.l_len = 0;

    fc.l_type = F_WRLCK;
    res = signal_aware_fcntl( this->dl_fd, F_SETLKW, &fc );

    this->dl_locked = (res == 0);
  }

  return( res );
}

int DescriptorLock::unlock( void )
{
  int              res = 0;
  struct flock     fc;

  if( this->dl_locked ) {
    fc.l_whence = SEEK_SET;
    fc.l_start = fc.l_len = 0;

    fc.l_type = F_UNLCK;
    res = signal_aware_fcntl( this->dl_fd, F_SETLKW, &fc );

    this->dl_locked = (res != 0);
  }

  return( res );
}

} // namespace jccommon
} JOBCONTROL_NAMESPACE_END
