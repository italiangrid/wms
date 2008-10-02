#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_MONITORDATA_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_MONITORDATA_H

#include <memory>
#include <boost/shared_ptr.hpp>

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {
  class EventLogger;
  class IdContainer;
}

namespace logmonitor { 

namespace processer {
  class JobResubmitter;
}
class AbortedContainer;
class Timer;
class SizeFile;

struct MonitorData {
  MonitorData(
    boost::shared_ptr<AbortedContainer> aborted,
    boost::shared_ptr<jccommon::IdContainer> container,
    boost::shared_ptr<processer::JobResubmitter> resubmitter,
    boost::shared_ptr<jccommon::EventLogger> logger);

  boost::shared_ptr<AbortedContainer> md_aborted;
  boost::shared_ptr<jccommon::IdContainer> md_container;
  boost::shared_ptr<processer::JobResubmitter> md_resubmitter;
  boost::shared_ptr<jccommon::EventLogger> md_logger;
};

namespace processer {

struct MonitorData {
  MonitorData( const char *filename, logmonitor::MonitorData &data );
  MonitorData( const std::string &filename, logmonitor::MonitorData &data );

  bool md_isDagLog;
  boost::shared_ptr<jccommon::EventLogger> md_logger;
  boost::shared_ptr<jccommon::IdContainer> md_container;
  boost::shared_ptr<AbortedContainer> md_aborted;
  boost::shared_ptr<JobResubmitter> md_resubmitter;
  std::string md_logfile_name, md_dagId;
  boost::shared_ptr<Timer> md_timer;
  boost::shared_ptr<SizeFile> md_sizefile;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_PROCESSER_MONITORDATA_H */

// Local Variables:
// mode: c++
// End:
