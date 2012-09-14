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
