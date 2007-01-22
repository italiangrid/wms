#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGENERIC_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGENERIC_H

#include "EventInterface.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class EventFactory;

class EventGeneric : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventGeneric( void );

  virtual void process_event( void );

private:
  EventGeneric( ULogEvent *event, MonitorData *data );

  void finalProcess( int code );

  GenericEvent       *eg_event;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGENERIC_H */

// Local Variables:
// mode: c++
// End:
