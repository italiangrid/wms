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
  enum command_t { unknown, submit, remove };

  CondorG( const glite::wms::common::configuration::JCConfiguration *config );

  ~CondorG( void );

  CondorG *set_command( command_t command, const std::string &parameter );
  int execute( std::string &info );

  inline static CondorG *instance( void ) { return cg_s_instance; }

  inline static const char *string_command( command_t comm ) { return cg_s_commands[(int) comm]; }

private:
  std::string   cg_submit, cg_remove, cg_command;
  boost::mutex  cg_mutex;

  static CondorG      *cg_s_instance;
  static const char   *cg_s_commands[];
};

}; // namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_CONDORG_H */

// Local Variables:
// mode: c++
// End:
