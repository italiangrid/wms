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
#ifndef EDG_WORKLOAD_JOBCONTROL_COMMON_CONSTANTS_H
#define EDG_WORKLOAD_JOBCONTROL_COMMON_CONSTANTS_H

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

enum generic_event_t {
  null_event,
  cancelled_event,
  retry_remove,
  cannot_cancel_event,
  user_cancelled_event
};

enum job_statuses_t {
  undefined_status = -2,
  no_resubmission
};

extern const char *generic_events_string[];

} // namespace jccommon

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_COMMON_CONSTANTS_H */

// Local Variables:
// mode: c++
// End:
