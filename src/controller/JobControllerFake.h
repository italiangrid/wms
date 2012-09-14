// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERFAKE_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERFAKE_H

#include "jobcontrol_namespace.h"
#include "JobControllerImpl.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon { class RamContainer; }

namespace controller {

class JobControllerFake : public JobControllerImpl {
public:
  JobControllerFake( void );
  virtual ~JobControllerFake( void );

  virtual int submit( const classad::ClassAd *ad );
  virtual bool cancel( const glite::jobid::JobId &id, const char *logfile );
  virtual bool cancel( int condorid, const char *logfile );
  virtual bool release(int condorid, char const* logfile);
  virtual size_t queue_size( void );

private:
  JobControllerFake( const JobControllerFake & ); // Not implemented
  JobControllerFake &operator=( const JobControllerFake & ); // Not implemented
};

} // namespace controller

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERFAKE_H */

// Local Variables:
// mode: c++
// End:
