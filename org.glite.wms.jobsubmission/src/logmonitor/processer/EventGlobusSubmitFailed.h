#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGLOBUSSUBMITFAILED_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGLOBUSSUBMITFAILED_H

#include "EventInterface.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class EventFactory;

class EventGlobusSubmitFailed : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventGlobusSubmitFailed( void );

  virtual void process_event( void );

private:
  EventGlobusSubmitFailed( ULogEvent *event, MonitorData *data );

  GlobusSubmitFailedEvent       *egsf_event;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGLOBUSSUBMITFAILED_H */

// Local Variables:
// mode: c++
// End:
