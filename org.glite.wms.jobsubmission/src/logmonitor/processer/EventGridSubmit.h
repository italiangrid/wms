#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGRIDSUBMIT_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGRIDSUBMIT_H

#include "EventInterface.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class EventFactory;

class EventGridSubmit : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventGridSubmit( void );

  virtual void process_event( void );

private:
  EventGridSubmit( ULogEvent *event, MonitorData *data );
  void finalProcess( const std::string &edgid, const std::string &seqcode );

  GridSubmitEvent       *egs_event;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGRIDSUBMIT_H */

// Local Variables:
// mode: c++
// End:
