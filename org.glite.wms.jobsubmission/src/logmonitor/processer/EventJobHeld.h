#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTJOBHELD_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTJOBHELD_H

#include "EventInterface.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class EventFactory;

class EventJobHeld : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventJobHeld( void );

  virtual void process_event( void );

private:
  EventJobHeld( ULogEvent *event, MonitorData *data );

  JobHeldEvent       *ejh_event;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTJOBHELD_H */

// Local Variables:
// mode: c++
// End:
