#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTEXECUTE_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTEXECUTE_H

#include "EventInterface.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class EventFactory;

class EventExecute : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventExecute( void );

  virtual void process_event( void );

private:
  EventExecute( ULogEvent *event, MonitorData *data );

  ExecuteEvent       *ee_event;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTEXECUTE_H */

// Local Variables:
// mode: c++
// End:
