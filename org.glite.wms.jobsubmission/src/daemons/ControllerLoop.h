#ifndef EDG_WORKLOAD_JOBCONTROL_DAEMONS_CONTROLLERLOOP_H
#define EDG_WORKLOAD_JOBCONTROL_DAEMONS_CONTROLLERLOOP_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "glite/wms/common/logger/logstream.h"
#include "jobcontrol_namespace.h"

COMMON_SUBNAMESPACE_CLASS( utilities, LineParser );

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {
  class JobController;
  class JobControllerClient;
  class CondorG;
  class Request;
}

namespace jccommon { class EventLogger; }
namespace daemons {

class ControllerLoop {
public:
  enum run_code_t { do_nothing, reload, shutdown };

  ControllerLoop(glite::wms::common::utilities::LineParser const& options);

  run_code_t run();
  run_code_t operator()( void ) { return this->run(); }

  static ControllerLoop *instance( void ) { return cl_s_instance; }

private:
  void createDirectories( void );
  void activateSignalHandling( void );
  bool checkSignal( run_code_t &return_code );

  bool cl_verbose;
  glite::wms::common::logger::logstream &cl_stream;
  boost::shared_ptr<jccommon::EventLogger> cl_logger;
  std::string cl_queuefilename;
  boost::shared_ptr<controller::CondorG> cl_executer;
  boost::shared_ptr<controller::JobControllerClient> cl_client;

  const glite::wms::common::utilities::LineParser &cl_options;

  static const int cl_s_signals[];
  static ControllerLoop *cl_s_instance;
};

}; // Namespace daemons

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_DAEMONS_CONTROLLERLOOP_H */

// Local Variables:
// mode: c++
// End:
