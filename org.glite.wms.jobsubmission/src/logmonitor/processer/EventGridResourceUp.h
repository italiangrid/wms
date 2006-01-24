#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGRIDRESOURCEUP_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGRIDRESOURCEUP_H

#include "EventInterface.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class EventFactory;

class EventGridResourceUp : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventGridResourceUp( void );

  virtual void process_event( void );

private:
  EventGridResourceUp( ULogEvent *event, MonitorData *data );

  GridResourceUpEvent       *egru_event;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGRIDRESOURCEUP_H */

// Local Variables:
// mode: c++
// End:
