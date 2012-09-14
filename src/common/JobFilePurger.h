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

#ifndef EDG_WORKLOAD_JOBCONTROL_COMMON_JOBFILEPURGE_H
#define EDG_WORKLOAD_JOBCONTROL_COMMON_JOBFILEPURGE_H

#include "glite/jobid/JobId.h"
#include "jobcontrol_namespace.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

class JobFilePurger {
public:
  JobFilePurger(
    glite::jobid::JobId const& jobid,
    bool have_lbproxy
  );
  void do_purge(bool everything = false);

private:
  bool                   jfp_have_lbproxy;
  glite::jobid::JobId    jfp_jobId;
  glite::jobid::JobId    jfp_dagId;
};

} // jccommon namespace

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_COMMON_JOBFILEPURGE_H */
