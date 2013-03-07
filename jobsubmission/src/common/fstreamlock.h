/*
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://www.eu-egee.org/partners for details on the
 * copyright holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *     either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 *     */

#ifndef GLITE_WMS_JOBSUBMISSION_COMMON_FSTREAMLOCK_H
#define GLITE_WMS_JOBSUBMISSION_COMMON_FSTREAMLOCK_H

#include <fstream>

#include "jobcontrol_namespace.h"

JOBCONTROL_NAMESPACE_BEGIN {
namespace jccommon {

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

} // namespace jccommon
} JOBCONTROL_NAMESPACE_END

#endif /* GLITE_WMS_JOBSUBMISSION_COMMON_FSTREAMLOCK_H */

