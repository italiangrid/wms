#include <signal.h>

sig_atomic_t   edg_wl_jobcontrol_common_received_signal = 0;

void edg_wl_jobcontrol_common_SignalHandler( int signal )
{
  edg_wl_jobcontrol_common_received_signal = signal;

  return;
}
