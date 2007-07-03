#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTINTERFACE_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTINTERFACE_H

#include <string>

#include "jobcontrol_namespace.h"
#include "logmonitor/Timer.h"

class ULogEvent;

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

struct MonitorData;
class SubmitReader;

class EventInterface {
public:
  EventInterface( ULogEvent *event, MonitorData *data );
  virtual ~EventInterface( void );

  virtual void process_event( void ) = 0;

protected:
  SubmitReader *createReader( const std::string &edgid );

  MonitorData      *ei_data;
  std::string       ei_condor;

  static const std::string  ei_s_edgideq, ei_s_subnodeof, ei_s_notsub, ei_s_dagfailed, ei_s_dagideq;
  static const std::string  ei_s_joberror, ei_s_jobwrapfail;
  static const std::string  ei_s_errremcorr, ei_s_failedinsertion;

private:
  EventInterface( const EventInterface & ); // Non copyable
  EventInterface &operator=( const EventInterface & ); // Non copyable
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTINTERFACE_H */

// Local Variables:
// mode: c++
// End:
