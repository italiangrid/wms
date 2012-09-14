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
#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_ABORTEDCONTAINER_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_ABORTEDCONTAINER_H

#include <map>
#include <string>

#include "common/filelist.h"
#include "common/filelist_iterator.h"

JOBCONTROL_NAMESPACE_BEGIN {
namespace logmonitor {

class AbortedContainer {
public:
  AbortedContainer( const char *filename );
  AbortedContainer( const std::string &filename );
  ~AbortedContainer( void );

  void compact( void );
  bool insert( const std::string &condorId );
  bool remove( const std::string &condorId );

  inline bool search( const std::string &condorId )
  { return( this->ac_pointers.find(condorId) != this->ac_pointers.end() ); }
  inline size_t inserted( void ) { return this->ac_inserted; }

private:
  void onConstruct( void );

  typedef glite::wms::jobsubmission::jccommon::FileList<std::string> FileList;
  typedef std::map<std::string, FileList::iterator> Map;

  size_t     ac_inserted;
  FileList   ac_filelist;
  Map        ac_pointers;
};

} // namespace logmonitor
} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_ABORTEDCONTAINER_H */
