#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_JOBSTATUSEXTRACTOR_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_JOBSTATUSEXTRACTOR_H

#include <memory>

COMMON_SUBNAMESPACE_CLASS( utilities, LineParser );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor {

class JobWrapperOutputParser;

class JobStatusExtractor {
public:
  JobStatusExtractor( const glite::wms::common::utilities::LineParser &options );
  ~JobStatusExtractor( void );

  int get_job_status( std::string &errors );

private:
  std::auto_ptr<JobWrapperOutputParser>   jse_parser;
};

}; // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_JOBSTATUSEXTRACTOR_H */

// Local Variables:
// mode: c++
// End:
