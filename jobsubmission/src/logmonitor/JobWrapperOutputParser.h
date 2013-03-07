/* Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
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
  ~JobWrapperOutputParser( void );

  status_type parse_file( int &retcode, std::string &errors, std::string &sc, std::string& reason);

private:
  bool parseStream( std::istream &is, std::string &errors, int &retcode, status_type &stat, std::string &sc, std::string& reason);

  std::string     jwop_edgid;
};

} // namespace logmonitor

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_JOBWRAPPEROUTPUTPARSER_H */

// Local Variables:
// mode: c++
// End:
