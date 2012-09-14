#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGRIDRESOURCEDOWN_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGRIDRESOURCEDOWN_H

#include "EventInterface.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class EventFactory;

class EventGridResourceDown : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventGridResourceDown( void );

  virtual void process_event( void );

private:
  EventGridResourceDown( ULogEvent *event, MonitorData *data );

  GridResourceDownEvent       *egrd_event;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGRIDRESOURCEDOWN_H */

// Local Variables:
// mode: c++
// End:
