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
#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTJOBRELEASED_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTJOBRELEASED_H

#include "EventInterface.h"
#include <user_log.c++.h>

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor {
namespace processer {

struct MonitorData;
class EventFactory;

class EventJobReleased : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventJobReleased() { }
  virtual void process_event();

private:
  EventJobReleased(ULogEvent *event, MonitorData *data);
  JobReleasedEvent *event_;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTJOBHELD_H */

// Local Variables:
// mode: c++
// End:
