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
#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_CONDORMONITOR_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_CONDORMONITOR_H

#include <boost/shared_ptr.hpp>

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor {

struct MonitorData;
struct internal_data_s;

namespace processer {
  struct MonitorData;
}

class CondorMonitor {
public:
  enum status_t { no_events, event_read, timer_expired, event_error };

  CondorMonitor( const std::string &filename, MonitorData &data );

  ~CondorMonitor( void );

  bool file_completed( void ) const;
  bool got_last( void ) const;
  unsigned int pending_jobs( void ) const;
  const std::string &logfile_name( void ) const;

  status_t process_next_event( void );

  inline static void recycle_directory( const std::string &dir ) { cm_s_recycleDirectory.assign( dir ); }
  inline static const std::string &recycle_directory( void ) { return cm_s_recycleDirectory; }

private:
  void doRecycle( void );
  void openFile( void );
  void writeCurrentPosition( FILE *fp );
  status_t checkAndProcessTimers( void );

  boost::shared_ptr<processer::MonitorData>   cm_shared_data;
  boost::shared_ptr<internal_data_s>          cm_internal_data;

  static std::string    cm_s_recycleDirectory;
};

} // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_CONDORMONITOR_H */

// Local Variables:
// mode: c++
// End:
