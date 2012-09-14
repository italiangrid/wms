#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTABORTED_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTABORTED_H

#include "EventInterface.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class EventFactory;

class EventAborted : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventAborted( void );

  virtual void process_event( void );

private:
  EventAborted( ULogEvent *event, MonitorData *data, bool removeTimer );

  bool                   ea_removeTimer;
  JobAbortedEvent       *ea_event;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTABORTED_H */

// Local Variables:
// mode: c++
// End:
