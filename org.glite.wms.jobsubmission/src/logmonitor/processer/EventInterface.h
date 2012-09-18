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
#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTINTERFACE_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTINTERFACE_H

#include <string>

#include "jobcontrol_namespace.h"
#include "logmonitor/Timer.h"

class ULogEvent;

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class SubmitReader;

class EventInterface {
public:
  EventInterface( ULogEvent *event, MonitorData *data );
  virtual ~EventInterface( void );

  virtual void process_event( void ) = 0;

protected:
  SubmitReader *createReader( const std::string &edgid );

  MonitorData      *ei_data;
  std::string       ei_condor;

  static const std::string  ei_s_edgideq, ei_s_subnodeof, ei_s_notsub;
  static const std::string  ei_s_joberror, ei_s_jobwrapfail;
  static const std::string  ei_s_errremcorr, ei_s_failedinsertion;

private:
  EventInterface( const EventInterface & ); // Non copyable
  EventInterface &operator=( const EventInterface & ); // Non copyable
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTINTERFACE_H */

// Local Variables:
// mode: c++
// End:
