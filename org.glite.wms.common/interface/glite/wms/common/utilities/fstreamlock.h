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
