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

#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_SUBMITREADER_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_SUBMITREADER_H

#include "common/files.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

class SubmitReader {
public:
  SubmitReader( const glite::jobid::JobId &edgid );
  SubmitReader( const glite::jobid::JobId &dagid, const glite::jobid::JobId &jobid );
  ~SubmitReader( void );

  inline const std::string &to_string( void ) const { return this->sr_submit; }
  inline const std::string &get_globus_rsl( void ) const { return this->sr_globusRsl; }

private:
  void internalRead( const glite::jobid::JobId &edgid );

  std::string        sr_submit, sr_globusRsl;
  jccommon::Files    sr_files;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_SUBMITREADER_H */

// Local Variables:
// mode: c++
// End:
