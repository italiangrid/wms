#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_SUBMITREADER_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_SUBMITREADER_H

#include "common/files.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

class SubmitReader {
public:
  SubmitReader( const glite::wmsutils::jobid::JobId &edgid );
  SubmitReader( const glite::wmsutils::jobid::JobId &dagid, const glite::wmsutils::jobid::JobId &jobid );
  ~SubmitReader( void );

  const std::string &to_string( void ) const { return this->sr_submit; }
  const std::string &get_globus_rsl( void ) const { return this->sr_globusRsl; }

private:
  void internalRead( const glite::wmsutils::jobid::JobId &edgid );

  std::string        sr_globusRsl, sr_submit;
  jccommon::Files    sr_files;
};

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_SUBMITREADER_H */

// Local Variables:
// mode: c++
// End:
