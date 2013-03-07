/* Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://www.eu-egee.org/partners/ for details on the copyright
 * holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License. */
#ifndef EDG_WORKLOAD_JOBCONTROL_COMMON_RAMCONTAINER_H
#define EDG_WORKLOAD_JOBCONTROL_COMMON_RAMCONTAINER_H

#include <vector>

#include "pointer_id.h"

namespace classad { class ClassAd; }

JOBCONTROL_NAMESPACE_BEGIN {
namespace jccommon {

class IdContainer;

class RamContainer {
public:
  RamContainer( void );
  RamContainer( IdContainer &ic );
  ~RamContainer( void );

  inline int inserted( void ) const { return this->rc_inserted; }

  void copy( IdContainer &ic );

  std::string condor_id( const std::string &edgId );
  std::string edg_id( const std::string &condorId );

  void insert( const std::string &edgId, const std::string &condorId );
  bool remove_by_condor_id( const std::string &condorid );

private:
  RamContainer( const RamContainer &rc ); // Not implemented
  RamContainer &operator=( const RamContainer &rc ); // Not implemented

  void internalCopy( IdContainer &ic );

  int                     rc_inserted;
  std::list<PointerId>    rc_pointers;
  std::vector<EdgId>      rc_edgs;
  std::vector<CondorId>   rc_condors;
};

} // namespace jccommon
} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_COMMON_RAMCONTAINER_H */
