#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTPOSTTERMINATED_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTPOSTTERMINATED_H

#include "EventInterface.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class EventFactory;

class EventPostTerminated : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventPostTerminated( void );

  virtual void process_event( void );

private:
  EventPostTerminated( ULogEvent *event, MonitorData *data );

  PostScriptTerminatedEvent       *ept_event;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTPOSTTERMINATED_H */

// Local Variables:
// mode: c++
// End:
