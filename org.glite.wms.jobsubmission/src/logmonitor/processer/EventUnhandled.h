#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTUNHANDLED_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTUNHANDLED_H

#include "EventInterface.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class EventFactory;

class EventUnhandled : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventUnhandled( void );

  virtual void process_event( void );

private:
  EventUnhandled( ULogEvent *event, MonitorData *data );

  ULogEvent    *eu_event;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTUNHANDLED_H */

// Local Variables:
// mode: c++
// End:
