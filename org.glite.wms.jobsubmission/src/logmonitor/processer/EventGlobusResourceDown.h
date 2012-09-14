#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGLOBUSRESOURCEDOWN_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGLOBUSRESOURCEDOWN_H

#include "EventInterface.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class EventFactory;

class EventGlobusResourceDown : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventGlobusResourceDown( void );

  virtual void process_event( void );

private:
  EventGlobusResourceDown( ULogEvent *event, MonitorData *data );

  GlobusResourceDownEvent       *egrd_event;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGLOBUSRESOURCEDOWN_H */

// Local Variables:
// mode: c++
// End:
