#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTSUBMIT_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTSUBMIT_H

#include "EventInterface.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class EventFactory;

class EventSubmit : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventSubmit( void );

  virtual void process_event( void );

private:
  EventSubmit( ULogEvent *event, MonitorData *data );
  void finalProcess( const std::string &edgid, const std::string &seqcode );

  SubmitEvent       *es_event;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTSUBMIT_H */

// Local Variables:
// mode: c++
// End:
