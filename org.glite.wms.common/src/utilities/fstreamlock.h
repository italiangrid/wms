#ifndef GLITE_WMS_COMMON_UTILITIES_FSTREAMLOCK_H
#define GLITE_WMS_COMMON_UTILITIES_FSTREAMLOCK_H

#include <fstream>

namespace glite {
namespace wms {
namespace common {
namespace utilities {

class DescriptorLock {
public:
  DescriptorLock( int fd, bool lock = true );

  ~DescriptorLock( void );

  int lock( void );
  int unlock( void );

  inline bool locked( void ) { return( this->dl_locked ); }

protected:
  bool     dl_locked;
  int      dl_fd;
};

class FstreamLock : public DescriptorLock {
public:
  FstreamLock( const std::fstream &fs, bool lock = true );
  FstreamLock( const std::ifstream &ifs, bool lock = true );
  FstreamLock( const std::ofstream &ofs, bool lock = true );
  FstreamLock( const std::filebuf &fb, bool lock = true );

  ~FstreamLock( void );

  inline int descriptor( void ) { return( this->dl_fd ); }
};

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_UTILITIES_FSTREAMLOCK_H */

// Local Variables:
// mode: c++
// End:
