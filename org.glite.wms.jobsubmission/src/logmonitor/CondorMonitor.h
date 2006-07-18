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

}; // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_CONDORMONITOR_H */

// Local Variables:
// mode: c++
// End:
