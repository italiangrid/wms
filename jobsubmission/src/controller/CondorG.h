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
#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_CONDORG_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_CONDORG_H

#include <string>

#include <boost/thread/mutex.hpp>



#include "jobcontrol_namespace.h"

COMMON_SUBNAMESPACE_CLASS( configuration, JCConfiguration );

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class CondorG {
public:
  enum command_t { unknown, submit, remove, release };

  CondorG( const glite::wms::common::configuration::JCConfiguration *config );

  ~CondorG( void );

  CondorG *set_command( command_t command, const std::string &parameter );
  int execute( std::string &info );

  inline static CondorG *instance( void ) { return cg_s_instance; }

  inline static const char *string_command( command_t comm ) { return cg_s_commands[(int) comm]; }

private:
  std::string   cg_submit, cg_remove, cg_release, cg_command;
  boost::mutex  cg_mutex;

  static CondorG      *cg_s_instance;
  static const char   *cg_s_commands[];
};

} // namespace controller

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_CONDORG_H */

// Local Variables:
// mode: c++
// End:
