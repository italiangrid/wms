#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_JOBWRAPPEROUTPUTPARSER_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_JOBWRAPPEROUTPUTPARSER_H

#include <string>
#include <iostream>

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor {

class JobWrapperOutputParser {
public:
  enum status_type { unknown = -1, good, abort, resubmit };

  JobWrapperOutputParser( const std::string &edgid );
  JobWrapperOutputParser( const std::string &dagid, const std::string &jobid );
  ~JobWrapperOutputParser( void );

  status_type parse_file( int &retcode, std::string &errors, std::string &sc );

private:
  bool parseStream( std::istream &is, std::string &errors, int &retcode, status_type &stat, std::string &sc );

  std::string     jwop_dagid, jwop_edgid;
};

}; // namespace logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_JOBWRAPPEROUTPUTPARSER_H */

// Local Variables:
// mode: c++
// End:
