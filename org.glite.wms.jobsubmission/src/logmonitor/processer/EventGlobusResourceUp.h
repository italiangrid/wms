#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGLOBUSRESOURCEUP_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGLOBUSRESOURCEUP_H

#include "EventInterface.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class EventFactory;

class EventGlobusResourceUp : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventGlobusResourceUp( void );

  virtual void process_event( void );

private:
  EventGlobusResourceUp( ULogEvent *event, MonitorData *data );

  GlobusResourceUpEvent       *egru_event;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGLOBUSRESOURCEUP_H */

// Local Variables:
// mode: c++
// End:
