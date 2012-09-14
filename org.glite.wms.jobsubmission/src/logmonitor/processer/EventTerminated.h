#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTTERMINATED_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTTERMINATED_H

#include "EventInterface.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class EventFactory;

class EventTerminated : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventTerminated( void );

  virtual void process_event( void );

private:
  EventTerminated( ULogEvent *event, MonitorData *data );
  void processNormalJob( jccommon::IdContainer::iterator &position );

  JobTerminatedEvent       *et_event;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTTERMINATED_H */

// Local Variables:
// mode: c++
// End:
