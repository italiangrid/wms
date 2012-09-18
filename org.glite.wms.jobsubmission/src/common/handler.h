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
#ifndef EDG_WORKLOAD_JOBCONTROL_COMMON_HANDLER_H
#define EDG_WORKLOAD_JOBCONTROL_COMMON_HANDLER_H

#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

extern sig_atomic_t edg_wl_jobcontrol_common_received_signal;

void edg_wl_jobcontrol_common_SignalHandler( int signal );

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* EDG_WORKLOAD_JOBCONTROL_COMMON_HANDLER_H */

// Local Variables:
// mode: c
// End:
