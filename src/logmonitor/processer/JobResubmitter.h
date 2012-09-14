/* Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_JOBRESUBMITTER_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_JOBRESUBMITTER_H

#include <classad_distribution.h>

#include "glite/wms/common/utilities/jobdir.h"
#include "common/id_container.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon { 
  class EventLogger;
}

namespace logmonitor { namespace processer {

class JobResubmitter {
  typedef  glite::wms::common::utilities::JobDir                       JD;

public:
  JobResubmitter( jccommon::EventLogger *logger );
  ~JobResubmitter( void );

  void resubmit( int laststatus, const std::string &edgid, const std::string &sequence_code, jccommon::IdContainer *container = NULL );

private:
  JD                      *jr_jobdir;
  jccommon::EventLogger   *jr_logger;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_JOBRESUBMITTER_H */

// Local Variables:
// mode: c++
// End:
