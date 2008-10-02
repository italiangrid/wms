#include <cstdlib>
#include <cstdio>
#include <cerrno>

#include <iostream>
#include <string>

#include <classad_distribution.h>

#ifdef GLITE_WMS_HAVE_LOGGING
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace fs = boost::filesystem;

#include <openssl/pem.h>
#include <openssl/x509.h>

#include "glite/wms/common/utilities/boost_fs_add.h"
#include "glite/lb/producer.h"
#include "glite/lb/consumer.h"
#include "glite/lb/context.h"
#endif
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/logger/edglog.h"

#include "jobcontrol_namespace.h"

#include "SignalChecker.h"
#include "EventLogger.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS_ts( ts );
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

namespace {
                                                                                
  unsigned int  EventLogger::el_s_retries = 3, EventLogger::el_s_sleep = 10;
  const char   *EventLogger::el_s_notLogged = "Event not logged, context unset.", *EventLogger::el_s_unavailable = "unavailable";
  const char   *EventLogger::el_s_OK = "OK", *EventLogger::el_s_failed = "Failed";

  // retrieve the subject_name from a given x509_proxy (thx to giaco)
  std::string get_proxy_subject(std::string const& x509_proxy)
  {
    static std::string const null_string;
                                                                                
    std::FILE* fd = std::fopen(x509_proxy.c_str(), "r");
    if (!fd) {
      return null_string;
    }
    boost::shared_ptr<std::FILE> fd_(fd, std::fclose);
                                                                                
    X509* const cert = PEM_read_X509(fd, 0, 0, 0);
    if (!cert) {
      return null_string;
    }
    boost::shared_ptr<X509> cert_(cert, X509_free);
                                                                                
    char* const s = X509_NAME_oneline(::X509_get_subject_name(cert), 0, 0);
    if (!s) {
      return null_string;
    }
    boost::shared_ptr<char> s_(s, free);
                                                                                
    return std::string(s);
  }
                                                                                
}

LoggerException::LoggerException( const char *reason ) : le_reason( reason ? reason : "" )
{ }

LoggerException::LoggerException( const string &reason ) : le_reason( reason )
{ }

LoggerException::~LoggerException() throw() { }

const char *LoggerException::what() const throw()
{
  return this->le_reason.c_str();
}

string EventLogger::getLoggingError(const char *preamble)
{
  string       cause( preamble ? preamble : "" );

  if( preamble ) cause.append( 1, ' ' ); 

#ifdef GLITE_WMS_HAVE_LOGGING
  char        *text, *desc;

  edg_wll_Error( *this->el_context, &text, &desc );
  cause.append( text );
  cause.append( " - " ); cause.append( desc );

  free( desc ); free( text );
#else
  cause.append( "Logging disabled !!! Are you sure there was an error ???" );
#endif

  return cause;
}

void EventLogger::testCode( int &code, bool retry )
{
  const configuration::CommonConfiguration     *conf = configuration::Configuration::instance()->common();
  int          ret;
  string       cause, host_proxy;

  if (code) {
    cause = this->getLoggingError( NULL );

    switch (code) {
      case EINVAL:
        ts::edglog << logger::setlevel( logger::critical )
          << "Critical error in L&B calls: EINVAL." << '\n'
          << "Cause = \"" << cause << "\"." << '\n';

      code = 0; // Don't retry...
      break;
#ifdef GLITE_WMS_HAVE_LOGGING
    case EDG_WLL_ERROR_GSS:
      ts::edglog << logger::setlevel( logger::severe )
		 << "Severe error in GSS layer while communicating with L&B daemons." << '\n'
		 << "Cause = \"" << cause << "\"." << '\n';

      if( this->el_hostProxy ) {
	ts::edglog << "The log with the host certificate has just been done. Giving up." << '\n';

	code = 0; // Don't retry...
      }
      else {
	ts::edglog << logger::setlevel( logger::info )
		   << "Retrying using host proxy certificate..." << '\n';

	host_proxy = conf->host_proxy_file();

	if( host_proxy.length() == 0 ) {
	  ts::edglog << logger::setlevel( logger::warning )
		     << "Host proxy file not set inside configuration file." << '\n'
		     << "Trying with a default NULL and hoping for the best." << '\n';

	  ret = edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_X509_PROXY, NULL );
	}
	else {
	  ts::edglog << logger::setlevel( logger::info )
		     << "Host proxy file found = \"" << host_proxy << "\"." << '\n';

	  ret = edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_X509_PROXY, host_proxy.c_str() );
	}

	if( ret ) {
	  ts::edglog << logger::setlevel( logger::severe )
		     << "Cannot set the host proxy inside the context. Giving up." << '\n';

	  code = 0; // Don't retry.
	}
	else this->el_hostProxy = true; // Set and retry (code is still != 0)
      }

      break;
#endif
    default:
      if( ++this->el_count > el_s_retries ) {
        ts::edglog << logger::setlevel( logger::error )
          << "L&B call retried " << this->el_count << " times always failed." << '\n'
          << "Ignoring." << '\n';

        code = 0; // Don't retry anymore
      } else {
        ts::edglog << logger::setlevel( logger::warning )
          << "L&B call got a transient error (" << code << "). Waiting "
          << el_s_sleep << " seconds and trying again." << '\n'
          << logger::setlevel( logger::info )
          << "Try n. " << this->el_count << "/" << el_s_retries << '\n';

        sleep(el_s_sleep);
      }
      break;
    }
  }
  else // The logging call worked fine, do nothing
    ts::edglog << logger::setlevel( logger::debug ) << "L&B call succeeded." << '\n';

  SignalChecker::instance()->throw_on_signal();

  return;
}

EventLogger::EventLogger( void ) : el_remove( true ),
#ifdef GLITE_WMS_HAVE_LOGGING
				   el_flag( EDG_WLL_SEQ_NORMAL ),
#else
				   el_flag( 0 ),
#endif
				   el_count( 0 ), el_context( NULL ), el_proxy()
{
  this->el_context = new edg_wll_Context;

#ifdef GLITE_WMS_HAVE_LOGGING
  if( edg_wll_InitContext(this->el_context) )
    throw LoggerException( "Cannot initialize logging context" );
#endif
}

EventLogger::EventLogger( edg_wll_Context *cont, int flag ) : el_remove( false ), el_hostProxy( false ),
							      el_flag( flag ), el_count( 0 ), el_context( cont ),
							      el_proxy()
{ }

EventLogger::~EventLogger( void )
{
  if( this->el_context && this->el_remove ) {
#ifdef GLITE_WMS_HAVE_LOGGING
    edg_wll_FreeContext( *this->el_context );
#endif

    delete this->el_context;
  }
}

EventLogger &EventLogger::initialize_jobcontroller_context( ProxySet *ps )
{
#ifdef GLITE_WMS_HAVE_LOGGING
  int   res = 0;

  if( this->el_context ) {
    res = edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_SOURCE, EDG_WLL_SOURCE_JOB_SUBMISSION );
    res |= edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_INSTANCE, "unique" );

    if( ps ) {
      res |= edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_X509_PROXY, ps->ps_x509Proxy );
      res |= edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_X509_KEY, ps->ps_x509Key );
      res |= edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_X509_CERT, ps->ps_x509Cert );
    }

    if( res ) throw LoggerException( "Invalid context parameter setting." );
  }
#endif

  return *this;
}

EventLogger &EventLogger::initialize_logmonitor_context( ProxySet *ps )
{
#ifdef GLITE_WMS_HAVE_LOGGING
  int    res = 0;

  if( this->el_context ) {
    res = edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_SOURCE, EDG_WLL_SOURCE_LOG_MONITOR );
    res |= edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_INSTANCE, "unique" );

    if( ps ) {
      res |= edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_X509_PROXY, ps->ps_x509Proxy );
      res |= edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_X509_KEY, ps->ps_x509Key );
      res |= edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_X509_CERT, ps->ps_x509Cert );
    }

    if( res ) throw LoggerException( "Invalid context parameter setting." );
  }
#endif

  return *this;
}

EventLogger &EventLogger::reset_context( const string &jobid, const string &sequence, int flag )
{
#ifdef GLITE_WMS_HAVE_LOGGING
  int             res;
  edg_wlc_JobId   id;

  if( this->el_context ) {
    edg_wlc_JobIdParse( jobid.c_str(), &id );
    res = edg_wll_SetLoggingJob( *this->el_context, id, sequence.c_str(), flag );
    edg_wlc_JobIdFree( id );	
    if( res != 0 ) throw LoggerException( this->getLoggingError("Cannot reset logging context:") );
  }
#endif

  return *this;
}

EventLogger &EventLogger::reset_user_proxy( const string &proxyfile )
{
#ifdef GLITE_WMS_HAVE_LOGGING
  bool    erase = false;
  int     res;

  if( proxyfile.size() && (proxyfile != this->el_proxy) ) {
    fs::path    pf(fs::normalize_path(proxyfile), fs::native);

    if( fs::exists(pf) ) {
      this->el_proxy.assign( proxyfile );

      res = edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_X509_PROXY, proxyfile.c_str() );

      if( res ) throw LoggerException( this->getLoggingError("Cannot set proxyfile path inside context:") );
    }
    else erase = true;
  }
  else if( proxyfile.size() == 0 ) erase = true;

  if( erase ) {
    this->el_proxy.erase();

    res = edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_X509_PROXY, NULL );

    if( res ) throw LoggerException( this->getLoggingError("Cannot reset proxyfile path inside context:") );
  }
#endif

  return *this;
}

EventLogger &EventLogger::reset_context( const string &jobid, const string &sequence )
{
#ifdef GLITE_WMS_HAVE_LOGGING
  int             res;
  edg_wlc_JobId   id;

  if( this->el_context ) {
    edg_wlc_JobIdParse( jobid.c_str(), &id );
    res = edg_wll_SetLoggingJob( *this->el_context, id, sequence.c_str(), this->el_flag );
    edg_wlc_JobIdFree( id );
    if( res != 0 ) throw LoggerException( this->getLoggingError("Cannot reset logging context:") );
  }
#endif

  return *this;
}

#ifdef GLITE_WMS_HAVE_LBPROXY

EventLogger &EventLogger::set_LBProxy_context( const string &jobid, const string &sequence, const string &proxyfile)
{ 
#ifdef GLITE_WMS_HAVE_LOGGING
  bool              erase = false;
  int               res;
  edg_wlc_JobId     id;
   
  if( proxyfile.size() && (proxyfile != this->el_proxy) ) {
    fs::path    pf( fs::normalize_path(proxyfile), fs::native );
    if( fs::exists(pf) ) {
      this->el_proxy.assign( proxyfile );
      
      res = edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_X509_PROXY, proxyfile.c_str() );

      if( res ) throw LoggerException( this->getLoggingError("Cannot set proxyfile path inside context:") );
    }
    else erase = true;
  }
  else if( proxyfile.size() == 0 ) erase = true;
                                                                                
  if( erase ) {
    this->el_proxy.erase();
                                                                                
    res = edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_X509_PROXY, NULL );  
    if( res ) throw LoggerException( this->getLoggingError("Cannot reset proxyfile path inside context:") );
  }
                                                                                
  if( this->el_context ) {
    std::string const user_dn = get_proxy_subject( proxyfile );
    edg_wlc_JobIdParse( jobid.c_str(), &id );
    res = edg_wll_SetLoggingJobProxy( *this->el_context, id, sequence.c_str(), user_dn.c_str(), this->el_flag );
    edg_wlc_JobIdFree( id );
    if( res != 0 ) throw LoggerException( this->getLoggingError("Cannot set LBProxy context:") );
  }
#endif
                                                                                
  return *this;
}

#endif

void EventLogger::unhandled_event( const char *descr )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::unhandled_event(...)" );

  elog::cedglog << logger::setlevel( logger::ugly )
		<< "Unhandled event, what to do ?" << '\n'
		<< "Logging nothing..." << '\n';

  return;
}

void EventLogger::condor_submit_event( const string &condorId, const string &rsl )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::condor_submit_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int          res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogAcceptedProxy( *this->el_context, EDG_WLL_SOURCE_JOB_SUBMISSION, "localhost", el_s_unavailable, condorId.c_str() );
#else
      res = edg_wll_LogAccepted( *this->el_context, EDG_WLL_SOURCE_JOB_SUBMISSION, "localhost", el_s_unavailable, condorId.c_str() );
#endif
      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got condor submit event, condor id = " << condorId << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::ugly )
		<< "Unlogged event condor submit, condor id = " << condorId << '\n';
#endif

  return;
}

void EventLogger::globus_submit_event( const string &ce, const string &rsl, const string &logfile )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::globus_submit_event(...)" );
  
#ifdef GLITE_WMS_HAVE_LOGGING
  int res;

  if (this->el_context) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogTransferOKProxy(
        *this->el_context,
        EDG_WLL_SOURCE_LRMS,
        ce.c_str(),
        logfile.c_str(),
        rsl.c_str(),
        "Job successfully submitted to Globus",
        el_s_unavailable
      );
#else
      res = edg_wll_LogTransferOK(
        *this->el_context,
        EDG_WLL_SOURCE_LRMS,
        ce.c_str(), 
        logfile.c_str(),
        rsl.c_str(),
        "Job successfully submitted to Globus",
        el_s_unavailable
      );
#endif
      
      this->testCode(res);
    } while (res != 0);
  } else {
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got globus submit event, ce = " << ce << '\n'
		  << el_s_notLogged << '\n';
  }
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event globus submit, ce = " << ce << '\n';
#endif

  return;
}

void EventLogger::grid_submit_event( const string &ce, const string &logfile )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::grid_submit_event(...)" );
  
#ifdef GLITE_WMS_HAVE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogTransferOKProxy( *this->el_context, EDG_WLL_SOURCE_LRMS, ce.c_str(),
                                   logfile.c_str(), "Grid job - no RSL", "Job successfully submitted over the Grid",
                                   el_s_unavailable );
#else
      res = edg_wll_LogTransferOK( *this->el_context, EDG_WLL_SOURCE_LRMS, ce.c_str(), 
				   logfile.c_str(), "Grid job - no RSL", "Job successfully submitted over the Grid",
				   el_s_unavailable );
#endif
      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got grid submit event, ce = " << ce << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event grid submit, ce = " << ce << '\n';
#endif

  return;
}

void EventLogger::execute_event( const char *host )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::execute_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogRunningProxy( *this->el_context, host );
#else
      res = edg_wll_LogRunning( *this->el_context, host );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got job execute event, host = " << host << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event execute, host = " << host << '\n';
#endif

  return;
}

void EventLogger::terminated_event( int retcode )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::terminated_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int          res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogDoneOKProxy( *this->el_context, (retcode ? "Warning: job exit code != 0" : "Job terminated successfully"), retcode );
#else
      res = edg_wll_LogDoneOK( *this->el_context, (retcode ? "Warning: job exit code != 0" : "Job terminated successfully"), retcode );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got job terminated event, return code = " << retcode << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event terminated, return code = " << retcode << '\n';
#endif

  return;
}

void EventLogger::failed_on_error_event( const string &cause )
{
  logger::StatePusher      pusher( elog::cedglog, "EventLogger::failed_on_error_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogDoneFAILEDProxy( *this->el_context, cause.c_str(), 1 );
#else
      res = edg_wll_LogDoneFAILED( *this->el_context, cause.c_str(), 1 );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got a job failed event." << '\n'
		  << "Reason = \"" << cause << "\"." << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job failed." << '\n'
		<< "Reason = \"" << cause << "\"." << '\n';
#endif

  return;
}

void EventLogger::abort_on_error_event( const string &cause )
{
  logger::StatePusher      pusher( elog::cedglog, "EventLogger::abort_on_error_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogAbortProxy( *this->el_context, cause.c_str() );
#else
      res = edg_wll_LogAbort( *this->el_context, cause.c_str() );
#endif
      
      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got a job aborted event." << '\n'
		  << "Reason = \"" << cause << "\"." << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job failed." << '\n'
		<< "Reason = \"" << cause << "\"." << '\n';
#endif

  return;
}

void EventLogger::aborted_by_system_event( const string &cause )
{
  logger::StatePusher        pusher( elog::cedglog, "EventLogger::abort_by_system_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogDoneFAILEDProxy( *this->el_context, cause.c_str(), 1 );
#else
      res = edg_wll_LogDoneFAILED( *this->el_context, cause.c_str(), 1 );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got aborted by system event." << '\n'
		  << "Cause = \"" << cause << "\"" << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event aborted by system." << '\n'
		<< "Cause = \"" << cause << "\"" << '\n';
#endif

  return;
}

void EventLogger::aborted_by_user_event( void )
{
  logger::StatePusher       pusher( elog::cedglog, "EventLogger::aborted_by_user_event()" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogCancelDONEProxy( *this->el_context, "Aborted by user." );
#else
      res = edg_wll_LogCancelDONE( *this->el_context, "Aborted by user." );
#endif

      this->testCode( res );
    } while( res != 0 );

    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogDoneCANCELLEDProxy( *this->el_context, "Aborted by user", 0 );
#else
      res = edg_wll_LogDoneCANCELLED( *this->el_context, "Aborted by user", 0 );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got aborted by user event." << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event aborted by user." << '\n';
#endif

  return;
}

void EventLogger::globus_submit_failed_event( const string &rsl, const char *reason, const string &logfile )
{
  logger::StatePusher      pusher( elog::cedglog, "EventLogger::globus_submit_failed_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogTransferFAILProxy( *this->el_context, EDG_WLL_SOURCE_LRMS, el_s_unavailable, logfile.c_str(),
					  rsl.c_str(), reason, el_s_unavailable );
#else
      res = edg_wll_LogTransferFAIL( *this->el_context, EDG_WLL_SOURCE_LRMS, el_s_unavailable, logfile.c_str(),
				     rsl.c_str(), reason, el_s_unavailable );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got globus submission failed event." << '\n'
		  << "Reason = \"" << reason << "\"" << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event globus submission failed." << '\n'
		<< "Reason = \"" << reason << "\"" << '\n';
#endif

  return;
}
/*
void EventLogger::globus_resource_down_event( void )
{
  logger::StatePusher      pusher( elog::cedglog, "EventLogger::globus_resource_down_event()" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogDoneFAILEDProxy( *this->el_context, "Globus resource down", 1 );
#else
      res = edg_wll_LogDoneFAILED( *this->el_context, "Globus resource down", 1 );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got Globus resource down event." << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event globus resource down." << '\n';
#endif

  return;
}
*/
void EventLogger::job_held_event( const string &reason )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::job_held_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int            res;
  string         what( "Got a job held event, reason: " );

  what.append( reason );

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogDoneFAILEDProxy( *this->el_context, what.c_str(), 1 );
#else
      res = edg_wll_LogDoneFAILED( *this->el_context, what.c_str(), 1 );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got job held event." << '\n'
		  << "Reason = \"" << reason << "\"" << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job held." << '\n'
		<< "Reason = \"" << reason << "\"" << '\n';
#endif
}

void EventLogger::job_enqueued_start_event( const string &filename, const classad::ClassAd *ad )
{
  logger::StatePusher      pusher( ts::edglog, "EventLogger::job_enqueued_start_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int                         res;
  string                      job;
  classad::ClassAdUnParser    unparser;

  if( ad != NULL ) unparser.Unparse( job, (classad::ClassAd *) ad );

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogEnQueuedSTARTProxy( *this->el_context, filename.c_str(), job.c_str(), el_s_unavailable );
#else
      res = edg_wll_LogEnQueuedSTART( *this->el_context, filename.c_str(), job.c_str(), el_s_unavailable );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else 
    ts::edglog << logger::setlevel( logger::null )
	       << "Job enqueued start event." << '\n'
	       << el_s_notLogged << '\n';

#else
  ts::edglog << logger::setlevel( logger::null )
	     << "Unlogged event job enqueued start." << '\n';
#endif

  return;
}

void EventLogger::job_enqueued_ok_event( const string &filename, const classad::ClassAd *ad )
{
  logger::StatePusher      pusher( ts::edglog, "EventLogger::job_enqueued_ok_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int                         res;
  string                      job;
  classad::ClassAdUnParser    unparser;

  unparser.Unparse( job, (classad::ClassAd *) ad );

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogEnQueuedOKProxy( *this->el_context, filename.c_str(), job.c_str(), el_s_unavailable );
#else
      res = edg_wll_LogEnQueuedOK( *this->el_context, filename.c_str(), job.c_str(), el_s_unavailable );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    ts::edglog << logger::setlevel( logger::null )
	       << "Job enqueued ok event." << '\n'
	       << el_s_notLogged << '\n';

#else
  ts::edglog << logger::setlevel( logger::null )
	     << "Unlogged event job enqueued ok." << '\n';
#endif

  return;
}

void EventLogger::job_enqueued_failed_event( const string &filename, const string &error, const classad::ClassAd *ad )
{
  logger::StatePusher       pusher( ts::edglog, "EventLogger::job_enqueued_failed_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int                        res;
  string                     job;
  classad::ClassAdUnParser   unparser;

  unparser.Unparse( job, (classad::ClassAd *) ad );

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogEnQueuedFAILProxy( *this->el_context, filename.c_str(), job.c_str(), error.c_str() );
#else
      res = edg_wll_LogEnQueuedFAIL( *this->el_context, filename.c_str(), job.c_str(), error.c_str() );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    ts::edglog << logger::setlevel( logger::null )
	       << "Job enqueued failed." << '\n'
	       << "Reason = \"" << error << "\"" << '\n'
	       << el_s_notLogged << '\n';

#else
  ts::edglog << logger::setlevel( logger::null )
	     << "Unlogged event job enqueud failed." << '\n'
	     << "Reason = \"" << error << "\"" << '\n';
#endif

  return;
}

void EventLogger::job_dequeued_event( const string &filename )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::job_dequeued_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int            res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogDeQueuedProxy( *this->el_context, filename.c_str(), el_s_unavailable );
#else
      res = edg_wll_LogDeQueued( *this->el_context, filename.c_str(), el_s_unavailable );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Job dequeued from file " << filename << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job dequeueud." << '\n';
#endif

  return;
}

void EventLogger::job_cancel_requested_event( const string &source )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::job_cancel_requested_event()" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int            res;
  string         reason( "Cancel requested by " );

  reason.append( source );

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogCancelREQProxy( *this->el_context, reason.c_str() );
#else
      res = edg_wll_LogCancelREQ( *this->el_context, reason.c_str() );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got cancel from " << source << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event cancel requested." << '\n';
#endif

  return;
}

void EventLogger::condor_submit_start_event( const string &logfile )
{
  logger::StatePusher      pusher( elog::cedglog, "EventLogger::condor_submit_start_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int            res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogTransferSTARTProxy( *this->el_context, EDG_WLL_SOURCE_LOG_MONITOR, "localhost", logfile.c_str(),
					   el_s_unavailable, el_s_unavailable, el_s_unavailable );
#else
      res = edg_wll_LogTransferSTART( *this->el_context, EDG_WLL_SOURCE_LOG_MONITOR, "localhost", logfile.c_str(),
				      el_s_unavailable, el_s_unavailable, el_s_unavailable );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Condor submit start event." << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event condor submit start." << '\n';
#endif

  return;
}

void EventLogger::condor_submit_ok_event( const string &rsl, const string &condorid, const string &logfile )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::condor_submit_ok_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int            res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogTransferOKProxy( *this->el_context, EDG_WLL_SOURCE_LOG_MONITOR, "localhost", logfile.c_str(),
					rsl.c_str(), el_s_unavailable, condorid.c_str() );
#else
      res = edg_wll_LogTransferOK( *this->el_context, EDG_WLL_SOURCE_LOG_MONITOR, "localhost", logfile.c_str(),
				   rsl.c_str(), el_s_unavailable, condorid.c_str() );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Condor submit ok event." << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event condor submit ok." << '\n';
#endif

  return;
}

void EventLogger::condor_submit_failed_event( const string &rsl, const string &reason, const string &logfile )
{
  logger::StatePusher      pusher( elog::cedglog, "EventLogger::condor_submit_failed_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int            res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogTransferFAILProxy( *this->el_context, EDG_WLL_SOURCE_LOG_MONITOR, "localhost", logfile.c_str(),
					  rsl.c_str(), reason.c_str(), el_s_unavailable );
#else
      res = edg_wll_LogTransferFAIL( *this->el_context, EDG_WLL_SOURCE_LOG_MONITOR, "localhost", logfile.c_str(),
				     rsl.c_str(), reason.c_str(), el_s_unavailable );
#endif

      this->testCode( res );
    } while( res != 0 );

    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogAbortProxy( *this->el_context, "Submission to condor failed." );
#else
      res = edg_wll_LogAbort( *this->el_context, "Submission to condor failed." );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else {
    elog::cedglog << logger::setlevel( logger::null )
		  << "Condor submit failed event." << '\n'
		  << logger::setmultiline( true, "CE-> " ) << "Reason\n" << reason << '\n'
		  << el_s_notLogged << '\n';
    elog::cedglog << logger::setmultiline( false );
  }
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event condor submit failed." << '\n'
		<< logger::setmultiline( true, "CE-> " ) << "Reason\n" << reason << '\n';
  elog::cedglog << logger::setmultiline( false );
#endif

  return;
}

void EventLogger::job_cancel_refused_event( const string &info )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::job_cancel_refused_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int            res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogCancelREFUSEProxy( *this->el_context, info.c_str() );
#else
      res = edg_wll_LogCancelREFUSE( *this->el_context, info.c_str() );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Cancel refused failed event." << '\n'
		  << "Reason \"" << info << "\"" << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event condor cancel failed." << '\n'
		<< "Reason: \"" << info << "\"" << '\n';
#endif

  return;
}

void EventLogger::job_abort_classad_invalid_event( const string &logfile, const string &reason )
{
  logger::StatePusher      pusher( elog::cedglog, "EventLogger::job_abort_classad_invalid_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int            res;
  string         info( "Invalid classad syntax: " );

  info.append( reason );

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogTransferFAILProxy( *this->el_context, EDG_WLL_SOURCE_LOG_MONITOR, "localhost", logfile.c_str(),
					  el_s_unavailable, info.c_str(), el_s_unavailable );
#else
      res = edg_wll_LogTransferFAIL( *this->el_context, EDG_WLL_SOURCE_LOG_MONITOR, "localhost", logfile.c_str(),
				     el_s_unavailable, info.c_str(), el_s_unavailable );
#endif

      this->testCode( res );
    } while( res != 0 );

    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogAbortProxy( *this->el_context, info.c_str() );
#else
      res = edg_wll_LogAbort( *this->el_context, info.c_str() );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Job aborted for invalid classad." << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job abort for invalid classad." << '\n';
#endif

  return;
}

void EventLogger::job_abort_cannot_write_submit_file_event( const string &logfile, const string &filename, const string &moreinfo )
{
  logger::StatePusher    pusher( elog::cedglog, "EventLogger::job_abort_cannot_write_submit_file_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int            res;
  string         info( "Cannot create condor submit file \"" );

  info.append( filename ); info.append( "\": " );
  info.append( moreinfo );

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogTransferFAILProxy( *this->el_context, EDG_WLL_SOURCE_LOG_MONITOR, "localhose", logfile.c_str(),
					  el_s_unavailable, info.c_str(), el_s_unavailable );
#else
      res = edg_wll_LogTransferFAIL( *this->el_context, EDG_WLL_SOURCE_LOG_MONITOR, "localhose", logfile.c_str(),
				     el_s_unavailable, info.c_str(), el_s_unavailable );
#endif

      this->testCode( res );
    } while( res != 0 );

    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogAbortProxy( *this->el_context, info.c_str() );
#else
      res = edg_wll_LogAbort( *this->el_context, info.c_str() );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Job aborted for condor submit error." << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job abort for condor submit error." << '\n';
#endif

  return;
}

void EventLogger::job_resubmitting_event( void )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::job_resubmitted_event()" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int            res;

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogResubmissionWILLRESUBProxy( *this->el_context, el_s_unavailable, el_s_unavailable );
#else
      res = edg_wll_LogResubmissionWILLRESUB( *this->el_context, el_s_unavailable, el_s_unavailable );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Job resubmitting event." << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job resubmitting to WM." << '\n';
#endif

  return;
}

void EventLogger::job_wm_enqueued_start_event( const string &filename, const classad::ClassAd &ad )
{
  logger::StatePusher        pusher(elog::cedglog, "EventLogger::job_wm_enqueued_start_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int                       res;
  string                    job;
  classad::ClassAdUnParser  unparser;

  unparser.Unparse( job, (classad::ClassAd *) &ad );

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogEnQueuedSTARTProxy( *this->el_context, filename.c_str(), job.c_str(), el_s_unavailable );
#else
      res = edg_wll_LogEnQueuedSTART( *this->el_context, filename.c_str(), job.c_str(), el_s_unavailable );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Job enqeueued to WM start event." << '\n'
		  << el_s_notLogged << '\n';

#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job enqueued to WM start." << '\n';
#endif

  return;
}

void EventLogger::job_wm_enqueued_ok_event( const string &filename, const classad::ClassAd &ad )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::job_wm_enqueued_ok_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int                       res;
  string                    job;
  classad::ClassAdUnParser  unparser;

  unparser.Unparse( job, (classad::ClassAd *) &ad );

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogEnQueuedOKProxy( *this->el_context, filename.c_str(), job.c_str(), el_s_unavailable );
#else
      res = edg_wll_LogEnQueuedOK( *this->el_context, filename.c_str(), job.c_str(), el_s_unavailable );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Job enqueued to WM ok event." << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job enqueued to WM ok." << '\n';
#endif

  return;
}

void EventLogger::job_wm_enqueued_failed_event( const string &filename, const classad::ClassAd &ad, const string &error )
{
  logger::StatePusher    pusher( elog::cedglog, "EventLogger::job_wm_enqueued_failed_event(...)" );

#ifdef GLITE_WMS_HAVE_LOGGING
  int                       res;
  string                    job;
  classad::ClassAdUnParser  unparser;

  unparser.Unparse( job, (classad::ClassAd *) &ad );

  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogEnQueuedFAILProxy( *this->el_context, filename.c_str(), job.c_str(), error.c_str() );
#else
      res = edg_wll_LogEnQueuedFAIL( *this->el_context, filename.c_str(), job.c_str(), error.c_str() );
#endif

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Job enqueued to WM failed." << '\n'
		  << "Reason = \"" << error << "\"" << '\n'
		  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job enqueued to wm failed." << '\n'
		<< "Reason = \"" << error << "\"" << '\n';
#endif

  return;
}

// we need the sequence code from the WN computed by the JW.
void EventLogger::job_really_run_event( const string &sc )
{
  logger::StatePusher    pusher( elog::cedglog, "EventLogger::job_really_run_event(...)" );
                                                                                   
#ifdef GLITE_WMS_HAVE_LOGGING
  int                       res;
                                                                                   
  if( this->el_context ) {
    this->startLogging();
    do {
#ifdef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogReallyRunningProxy( *this->el_context, sc.c_str() );
#else
      res = edg_wll_LogReallyRunning( *this->el_context, sc.c_str() );
#endif
      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
                  << "Really running event." << '\n'
                  << el_s_notLogged << '\n';
#else
  elog::cedglog << logger::setlevel( logger::null )
                << "Unlogged really running event." << '\n';
#endif
  
  return;
}

string EventLogger::seq_code_lbproxy( const string &jobid )
{
  string res("undefined");
  edg_wlc_JobId id;
  edg_wlc_JobIdParse(jobid.c_str(), &id);
  
#ifdef GLITE_WMS_HAVE_LBPROXY
  if( this->el_context ) {
    edg_wll_QuerySequenceCodeProxy(*this->el_context, id, &seqcode);

    res.assign( seqcode );
    free( seqcode );
  }	
#else
  res.assign( "UI=000000:NS=000000:WM=000000:BH=000000:JSS=000000:LM=000000:LRMS=000000:APP=000000" );
#endif

   edg_wlc_JobIdFree( id );
  return res;

} 

string EventLogger::sequence_code()
{
  char          *seqcode;
  string         res( "undefined" );

#ifdef GLITE_WMS_HAVE_LOGGING
  if (this->el_context) {
    seqcode = edg_wll_GetSequenceCode(*this->el_context);

    res = seqcode;
    free(seqcode);
  }
#else
  res.assign( "UI=000000:NS=000000:WM=000000:BH=000000:JSS=000000:LM=000000:LRMS=000000:APP=000000" );
#endif

  return res;
}

// Queries to LB

// TO BE CHECKED
string EventLogger::query_condorid( const string &jobid )
{
  string              condor_id;
  edg_wlc_JobId       id;
  edg_wlc_JobIdParse( jobid.c_str(), &id );
  edg_wll_Event       *events = NULL;
  edg_wll_QueryRec    jc[2];
  edg_wll_QueryRec    ec[3];

  memset(jc,0,sizeof jc);
  memset(ec,0,sizeof ec);

  // job condition: JOBID = jobid
  jc[0].attr    = EDG_WLL_QUERY_ATTR_JOBID;
  jc[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  jc[0].value.j = id;
  jc[1].attr    = EDG_WLL_QUERY_ATTR_UNDEF;   

  // event condition: Event type = ACCEPTED AND source = LM
  ec[0].attr    = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  ec[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  ec[0].value.i = EDG_WLL_EVENT_ACCEPTED;
  ec[1].attr    = EDG_WLL_QUERY_ATTR_SOURCE;
  ec[1].op      = EDG_WLL_QUERY_OP_EQUAL;
  ec[1].value.i = EDG_WLL_SOURCE_LOG_MONITOR;
  ec[2].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

#ifdef GLITE_WMS_HAVE_LBPROXY 
  edg_wll_QueryEventsProxy( *this->el_context, jc, ec, &events);
#else
  edg_wll_QueryEvents( *this->el_context, jc, ec, &events);
#endif

  if (events) {
    for (int i = 0; events[i].type; ++i) {
      // if the job has been resubmitted we need the last one
      condor_id = events[i].accepted.local_jobid;
      edg_wll_FreeEvent(&events[i]);
    }
    free(events);
  }
	
  edg_wlc_JobIdFree( id );

  return condor_id;
}


}; // Namespace common

} JOBCONTROL_NAMESPACE_END;
