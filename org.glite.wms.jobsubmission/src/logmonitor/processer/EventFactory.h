#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTFACTORY_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTFACTORY_H

#include <boost/shared_ptr.hpp>

class ULogEvent;

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

class EventInterface;
struct MonitorData;

class EventFactory {
public:
  EventFactory( boost::shared_ptr<MonitorData> &data );
  ~EventFactory( void );

  EventInterface *create_processor( ULogEvent *event, bool removeTimer = true );

private:
  boost::shared_ptr<MonitorData>     ef_data;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_EVENTFACTORY_H */

// Local Variables:
// mode: c++
// End:
