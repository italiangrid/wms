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
#ifndef EDG_WORKLOAD_JOBCONTROL_DAEMONS_CONTROLLERLOOP_H
#define EDG_WORKLOAD_JOBCONTROL_DAEMONS_CONTROLLERLOOP_H

#include <string>

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

  ControllerLoop( const glite::wms::common::utilities::LineParser &options );
  ~ControllerLoop( void );

  run_code_t run( void );

  inline run_code_t operator()( void ) { return this->run(); }

  inline static ControllerLoop *instance( void ) { return cl_s_instance; }
  inline static const char *compile_time( void ) { return cl_s_time; }
  inline static const char *compile_date( void ) { return cl_s_date; }
  inline static const char *version( void ) { return cl_s_version; }
  inline static const char *build_host( void ) { return cl_s_buildHost; }
  inline static const char *build_user( void ) { return cl_s_buildUser; }

private:
  ControllerLoop( const ControllerLoop &loop ); // Not implemented
  ControllerLoop &operator=( const ControllerLoop &loop ); // Not implemented

  void createDirectories( void );
  void activateSignalHandling( void );
  bool checkSignal( run_code_t &return_code );

  bool                                                           cl_verbose;
  glite::wms::common::logger::logstream                                     &cl_stream;
  std::auto_ptr<jccommon::EventLogger>                           cl_logger;
  std::string                                                    cl_queuefilename;
  std::auto_ptr<controller::CondorG>                             cl_executer;
  std::auto_ptr<controller::JobControllerClient>                 cl_client;

  const glite::wms::common::utilities::LineParser                            &cl_options;

  static const char         *cl_s_version, *cl_s_time, *cl_s_date;
  static const char         *cl_s_buildUser, *cl_s_buildHost;
  static const int           cl_s_signals[];
  static ControllerLoop     *cl_s_instance;
  bool               cl_have_lbproxy;
};

} // Namespace daemons

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_DAEMONS_CONTROLLERLOOP_H */

// Local Variables:
// mode: c++
// End:
