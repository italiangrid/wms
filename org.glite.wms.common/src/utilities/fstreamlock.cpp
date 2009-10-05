#include <cerrno>

#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>

#include "glite/wms/common/utilities/fstreamlock.h"
#include "streamdescriptor.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace utilities {

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

FstreamLock::FstreamLock( const fstream &fs, bool lock ) : DescriptorLock( streamdescriptor(fs), lock )
{}

FstreamLock::FstreamLock( const ifstream &fs, bool lock ) : DescriptorLock( streamdescriptor(fs), lock )
{}

FstreamLock::FstreamLock( const ofstream &fs, bool lock ) : DescriptorLock( streamdescriptor(fs), lock )
{}

FstreamLock::FstreamLock( const filebuf &fb, bool lock ) : DescriptorLock( bufferdescriptor(fb), lock )
{}

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

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace
