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
#ifndef EDG_WORKLOAD_JOBCONTROL_DAEMONS_MONITORLOOP_H
#define EDG_WORKLOAD_JOBCONTROL_DAEMONS_MONITORLOOP_H

#include <memory>
#include <iosfwd>

#include "glite/wms/common/logger/logstream.h"
#include "jobcontrol_namespace.h"

COMMON_SUBNAMESPACE_CLASS( utilities, LineParser );

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {
  class EventLogger;
  class IdContainer;
};

namespace logmonitor { 
class AbortedContainer;

namespace processer {
  class JobResubmitter;
}

};

namespace daemons {

class MonitorLoop {
public:
  enum run_code_t { do_nothing, reload, shutdown };

  MonitorLoop( const glite::wms::common::utilities::LineParser &options );
  ~MonitorLoop( void );

  run_code_t run( void );

  inline run_code_t operator()( void ) { return this->run(); }

  inline static MonitorLoop *instance( void ) { return ml_s_instance; }
  inline static const char *compile_time( void ) { return ml_s_time; }
  inline static const char *compile_date( void ) { return ml_s_date; }
  inline static const char *version( void ) { return ml_s_version; }
  inline static const char *build_host( void ) { return ml_s_buildHost; }
  inline static const char *build_user( void ) { return ml_s_buildUser; }

private:
  MonitorLoop( const MonitorLoop &loop ); // Not implemented
  MonitorLoop &operator=( const MonitorLoop &loop ); // Not implemented

  void createDirectories( void );
  void activateSignalHandling( void );
  bool checkSignal( run_code_t &return_code );

  bool                                                  ml_verbose;
  glite::wms::common::logger::logstream                            &ml_stream;
  std::auto_ptr<jccommon::EventLogger>                  ml_logger;
  std::auto_ptr<jccommon::IdContainer>                  ml_idContainer;
  std::auto_ptr<logmonitor::AbortedContainer>           ml_abContainer;
  std::auto_ptr<logmonitor::processer::JobResubmitter>  ml_resubmitter;

  const glite::wms::common::utilities::LineParser         &ml_options;

  static const char       *ml_s_version, *ml_s_time, *ml_s_date, *ml_s_abortedFileName;
  static const char       *ml_s_buildUser, *ml_s_buildHost;
  static const int         ml_s_signals[];
  static MonitorLoop      *ml_s_instance;
};

} // Namespace daemons

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_DAEMONS_MONITORLOOP_H */

// Local Variables:
// mode: c++
// End:
