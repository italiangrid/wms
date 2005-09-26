#ifndef EDG_WORKLOAD_JOBCONTROL_COMMON_EVENTLOGGER_H
#define EDG_WORKLOAD_JOBCONTROL_COMMON_EVENTLOGGER_H

#include <exception>

#ifdef ENABLE_LOGGING
#include "glite/lb/context.h"
#endif

#include "../jobcontrol_namespace.h"

typedef  struct _edg_wll_Context  *edg_wll_Context;

namespace classad { class ClassAd; }

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

struct ProxySet {
  const char  *ps_x509Proxy, *ps_x509Key, *ps_x509Cert;
};

class LoggerException : public std::exception {
public:
  LoggerException( const char *reason );
  LoggerException( const std::string &reason );
  virtual ~LoggerException( void ) throw();

  virtual const char *what( void ) const throw();

private:
  std::string     le_reason;
};

class EventLogger {
public:
  EventLogger( void );
#ifdef ENABLE_LOGGING
  EventLogger( edg_wll_Context *cont, int flag = EDG_WLL_SEQ_NORMAL );
#else
  EventLogger( edg_wll_Context *cont, int flag = 0 );
#endif
  ~EventLogger( void );

  EventLogger &initialize_jobcontroller_context( ProxySet *ps = NULL );
  EventLogger &initialize_logmonitor_context( ProxySet *ps = NULL );

  EventLogger &reset_context( const std::string &edgId, const std::string &sequenceCode );
  EventLogger &reset_context( const std::string &edgId, const std::string &sequenceCode, int flag );

  EventLogger &reset_user_proxy( const std::string &proxyfile );

#ifdef ENABLE_LBPROXY
  EventLogger &set_LBProxy_context( const std::string &jobid, const std::string &sequence, const std::string &proxyfile);
#endif

  /*
    LogMonitor events
  */
  void unhandled_event( const char *descr );
  void condor_submit_event( const std::string &condorid, const std::string &rsl );
  void globus_submit_event( const std::string &ce, const std::string &rsl, const std::string &logfile );
  void execute_event( const char *host );
  void terminated_event( int retcode );
  void failed_on_error_event( const std::string &cause );
  void abort_on_error_event( const std::string &cause );
  void aborted_by_system_event( const std::string &cause );
  void aborted_by_user_event( void );
  void globus_submit_failed_event( const std::string &rsl, const char *reason, const std::string &logfile );
 // void globus_resource_down_event( void );
  void job_held_event( const std::string &reason );
  void job_really_run_event( const std::string &sc ); 

  /*
    ControllerLoop events
  */
  void job_dequeued_event( const std::string &filename );
  void job_cancel_requested_event( const std::string &source );

  /*
    JobControllerProxy events
  */
  void job_enqueued_start_event( const std::string &filename, const classad::ClassAd *ad );
  void job_enqueued_ok_event( const std::string &filename, const classad::ClassAd *ad );
  void job_enqueued_failed_event( const std::string &filename, const std::string &error, const classad::ClassAd *ad );

  /*
    JobControllerReal events
  */
  void condor_submit_start_event( const std::string &logfile );
  void condor_submit_failed_event( const std::string &rsl, const std::string &reason, const std::string &logfile );
  void condor_submit_ok_event( const std::string &rsl, const std::string &condorid, const std::string &logfile );
  void job_cancel_refused_event( const std::string &info );
  void job_abort_classad_invalid_event( const std::string &logfile, const std::string &reason );
  void job_abort_cannot_write_submit_file_event( const std::string &logfile, const std::string &filename, const std::string &moreinfo );

  /*
    Events during resubmission (will go away ???)
  */
  void job_resubmitting_event( void );
  void job_wm_enqueued_start_event( const std::string &filename, const classad::ClassAd &ad );
  void job_wm_enqueued_ok_event( const std::string &filename, const classad::ClassAd &ad );
  void job_wm_enqueued_failed_event( const std::string &filename, const classad::ClassAd &ad, const std::string &error );

  /*
    Extractors
  */
  std::string sequence_code( void );

  inline operator edg_wll_Context *( void ) { return this->el_context; }

  inline static void set_lb_retries( unsigned int r ) { el_s_retries = r; return; }
  inline static void set_lb_interval( unsigned int sec ) { el_s_sleep = sec; return; }

private:
  inline void startLogging( void ) { this->el_count = 0; this->el_hostProxy = false; }

  void testCode( int &code, bool retry = false );
  std::string getLoggingError( const char *preamble );

  bool               el_remove, el_hostProxy;
  int                el_flag;
  unsigned int       el_count;
  edg_wll_Context   *el_context;
  std::string        el_proxy;

  static unsigned int         el_s_retries, el_s_sleep;
  static const char          *el_s_notLogged, *el_s_unavailable, *el_s_OK, *el_s_failed;
};

}; // Namespace common

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_COMMON_EVENTLOGGER_H */

// Local Variables:
// mode: c++
// End:
