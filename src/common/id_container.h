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
#ifndef EDG_WORKLOAD_JOBCONTROL_COMMON_IDCONTAINER_H
#define EDG_WORKLOAD_JOBCONTROL_COMMON_IDCONTAINER_H

#include <memory>
#include <vector>

#include "common/filelist_lock.h"

#include "pointer_id.h"
#include "constants.h"
#include "jobcontrol_namespace.h"

namespace classad { class ClassAd; }

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

class RamContainer;

class IdContainer {
  friend class RamContainer;
private:
  typedef glite::wms::jobsubmission::jccommon::FileList<classad::ClassAd> FileList;
  typedef glite::wms::jobsubmission::jccommon::FileListDescriptorMutex ListMutex;
  typedef glite::wms::jobsubmission::jccommon::FileListLock ListLock;
public:
  typedef std::list<PointerId>::iterator        iterator;
  typedef std::list<PointerId>::const_iterator  const_iterator;

  IdContainer( const char *filename );
  IdContainer( const std::string &filename );
  ~IdContainer( void );

  void refresh( void );
  void compact( void );
  void clear( void );

  bool insert( const std::string &edgId, const std::string &condorId, const std::string &seqcode, int status );
  bool update_pointer( iterator position, const std::string &seqcode, int condorstatus, int laststatus = undefined_status );
  bool increment_pointer_retry_count( iterator position );
  bool remove_by_edg_id( const std::string &edgId );
  bool remove_by_condor_id( const std::string &condorId );

  std::string condor_id( const std::string &edgId );
  std::string edg_id( const std::string &condorId );

  std::list<PointerId>::iterator position_by_condor_id( const std::string &condorId );
  std::list<PointerId>::iterator position_by_edg_id( const std::string &edgId );

  inline bool remove( iterator &it ) { return this->remove_by_edg_id(it->edg_id()); }
  inline size_t inserted( void ) { return this->ic_inserted; }

  inline std::list<PointerId> *operator->( void ) { return &this->ic_pointers; }
  inline const std::list<PointerId> *operator->( void ) const { return &this->ic_pointers; }

  inline iterator last_inserted( void ) { return this->ic_last; }
  inline iterator begin( void ) { return this->ic_pointers.begin(); }
  inline iterator end( void ) { return this->ic_pointers.end(); }
  inline const_iterator begin( void ) const { return this->ic_pointers.begin(); }
  inline const_iterator end( void ) const { return this->ic_pointers.end(); }
private:
  void onConstruct( void );
  bool removePointers( std::vector<EdgId>::iterator &edgPos, std::vector<CondorId>::iterator &condorPos );

  size_t                             ic_inserted;
  FileList                           ic_container;
  ListMutex                          ic_mutex;
  std::list<PointerId>               ic_pointers;
  std::vector<EdgId>                 ic_edgs;
  std::vector<CondorId>              ic_condors;
  iterator                           ic_last;
};

} // namespace jccommon
} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_COMMON_IDCONTAINER_H */
