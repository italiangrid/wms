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
#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_MONITORDATA_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_MONITORDATA_H

#include <memory>
#include <string>

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {
  class EventLogger;
  class IdContainer;
}

namespace logmonitor { 

namespace processer {
  class JobResubmitter;
}
class AbortedContainer;
class Timer;
class SizeFile;

struct MonitorData {
  MonitorData( AbortedContainer *aborted, jccommon::IdContainer *container, processer::JobResubmitter *resubmitter, jccommon::EventLogger *logger = NULL );

  jccommon::EventLogger       *md_logger;
  jccommon::IdContainer       *md_container;
  AbortedContainer            *md_aborted;
  processer::JobResubmitter   *md_resubmitter;
};

namespace processer {

struct MonitorData {
  MonitorData( const char *filename, logmonitor::MonitorData &data );
  MonitorData( const std::string &filename, logmonitor::MonitorData &data );

  jccommon::EventLogger   *md_logger;
  jccommon::IdContainer   *md_container;
  AbortedContainer        *md_aborted;
  JobResubmitter          *md_resubmitter;
  std::string              md_logfile_name, md_dagId;
  std::auto_ptr<Timer>     md_timer;
  std::auto_ptr<SizeFile>  md_sizefile;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_MONITORDATA_H */

// Local Variables:
// mode: c++
// End:
