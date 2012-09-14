#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGLOBUSSUBMIT_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGLOBUSSUBMIT_H

#include "EventInterface.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class EventFactory;

class EventGlobusSubmit : public EventInterface {
  friend class EventFactory;

public:
  virtual ~EventGlobusSubmit( void );

  virtual void process_event( void );

private:
  EventGlobusSubmit( ULogEvent *event, MonitorData *data );
  void finalProcess( const std::string &edgid, const std::string &seqcode );

  GlobusSubmitEvent       *egs_event;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTGLOBUSSUBMIT_H */

// Local Variables:
// mode: c++
// End:
