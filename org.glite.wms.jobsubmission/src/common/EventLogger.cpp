#include <cstdlib>
#include <cstdio>
#include <cerrno>

#include <iostream>
#include <string>

#include <classad_distribution.h>

#ifdef ENABLE_LOGGING
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "glite/wms/common/utilities/boost_fs_add.h"
#include "glite/lb/producer.h"
#include "glite/lb/context.h"
#endif
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/logger/edglog.h"

#include "../jobcontrol_namespace.h"

#include "SignalChecker.h"
#include "EventLogger.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS_ts( ts );
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

unsigned int  EventLogger::el_s_retries = 3, EventLogger::el_s_sleep = 60;
const char   *EventLogger::el_s_notLogged = "Event not logged, context unset.", *EventLogger::el_s_unavailable = "unavailable";
const char   *EventLogger::el_s_OK = "OK", *EventLogger::el_s_failed = "Failed";

LoggerException::LoggerException( const char *reason ) : le_reason( reason ? reason : "" )
{}

LoggerException::LoggerException( const string &reason ) : le_reason( reason )
{}

LoggerException::~LoggerException( void ) throw() {}

const char *LoggerException::what( void ) const throw()
{
  return this->le_reason.c_str();
}

string EventLogger::getLoggingError( const char *preamble )
{
  string       cause( preamble ? preamble : "" );

  if( preamble ) cause.append( 1, ' ' ); 

#ifdef ENABLE_LOGGING
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

  if( code ) {
    cause = this->getLoggingError( NULL );

    switch( code ) {
    case EINVAL:
      ts::edglog << logger::setlevel( logger::critical )
		 << "Critical error in L&B calls: EINVAL." << endl
		 << "Cause = \"" << cause << "\"." << endl;

      code = 0; // Don't retry...
      break;
#ifdef ENABLE_LOGGING
    case EDG_WLL_ERROR_GSS:
      ts::edglog << logger::setlevel( logger::severe )
		 << "Severe error in GSS layer while communicating with L&B daemons." << endl
		 << "Cause = \"" << cause << "\"." << endl;

      if( this->el_hostProxy ) {
	ts::edglog << "The log with the host certificate has just been done. Giving up." << endl;

	code = 0; // Don't retry...
      }
      else {
	ts::edglog << logger::setlevel( logger::info )
		   << "Retrying using host proxy certificate..." << endl;

	host_proxy = conf->host_proxy_file();

	if( host_proxy.length() == 0 ) {
	  ts::edglog << logger::setlevel( logger::warning )
		     << "Host proxy file not set inside configuration file." << endl
		     << "Trying with a default NULL and hoping for the best." << endl;

	  ret = edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_X509_PROXY, NULL );
	}
	else {
	  ts::edglog << logger::setlevel( logger::info )
		     << "Host proxy file found = \"" << host_proxy << "\"." << endl;

	  ret = edg_wll_SetParam( *this->el_context, EDG_WLL_PARAM_X509_PROXY, host_proxy.c_str() );
	}

	if( ret ) {
	  ts::edglog << logger::setlevel( logger::severe )
		     << "Cannot set the host proxy inside the context. Giving up." << endl;

	  code = 0; // Don't retry.
	}
	else this->el_hostProxy = true; // Set and retry (code is still != 0)
      }

      break;
#endif
    default:
      if( ++this->el_count > el_s_retries ) {
	ts::edglog << logger::setlevel( logger::error )
		   << "L&B call retried " << this->el_count << " times always failed." << endl
		   << "Ignoring." << endl;

	code = 0; // Don't retry anymore
      }
      else {
	ts::edglog << logger::setlevel( logger::warning )
		   << "L&B call got a transient error. Waiting " << el_s_sleep << " seconds and trying again." << endl
		   << logger::setlevel( logger::info )
		   << "Try n. " << this->el_count << "/" << el_s_retries << endl;

	sleep( el_s_sleep );
      }
      break;
    }
  }
  else // The logging call worked fine, do nothing
    ts::edglog << logger::setlevel( logger::debug ) << "L&B call succeeded." << endl;

  SignalChecker::instance()->throw_on_signal();

  return;
}

EventLogger::EventLogger( void ) : el_remove( true ),
#ifdef ENABLE_LOGGING
				   el_flag( EDG_WLL_SEQ_NORMAL ),
#else
				   el_flag( 0 ),
#endif
				   el_count( 0 ), el_context( NULL ), el_proxy()
{
  this->el_context = new edg_wll_Context;

#ifdef ENABLE_LOGGING
  if( edg_wll_InitContext(this->el_context) )
    throw LoggerException( "Cannot initialize logging context" );
#endif
}

EventLogger::EventLogger( edg_wll_Context *cont, int flag ) : el_remove( false ), el_hostProxy( false ),
							      el_flag( flag ), el_count( 0 ), el_context( cont ),
							      el_proxy()
{}

EventLogger::~EventLogger( void )
{
  if( this->el_context && this->el_remove ) {
#ifdef ENABLE_LOGGING
    edg_wll_FreeContext( *this->el_context );
#endif

    delete this->el_context;
  }
}

EventLogger &EventLogger::initialize_jobcontroller_context( ProxySet *ps )
{
#ifdef ENABLE_LOGGING
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
#ifdef ENABLE_LOGGING
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
#ifdef ENABLE_LOGGING
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
#ifdef ENABLE_LOGGING
  bool    erase = false;
  int     res;

  if( proxyfile.size() && (proxyfile != this->el_proxy) ) {
    boost::filesystem::path    pf( boost::filesystem::normalize_path(proxyfile), boost::filesystem::system_specific );

    if( boost::filesystem::exists(pf) ) {
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
#ifdef ENABLE_LOGGING
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

void EventLogger::unhandled_event( const char *descr )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::unhandled_event(...)" );

  elog::cedglog << logger::setlevel( logger::ugly )
		<< "Unhandled event, what to do ?" << endl
		<< "Logging nothing..." << endl;

  return;
}

void EventLogger::condor_submit_event( const string &condorId, const string &rsl )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::condor_submit_event(...)" );

#ifdef ENABLE_LOGGING
  int          res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogAccepted( *this->el_context, EDG_WLL_SOURCE_JOB_SUBMISSION, "localhost", el_s_unavailable, condorId.c_str() );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got condor submit event, condor id = " << condorId << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::ugly )
		<< "Unlogged event condor submit, condor id = " << condorId << endl;
#endif

  return;
}

void EventLogger::globus_submit_event( const string &ce, const string &rsl, const string &logfile )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::globus_submit_event(...)" );
  
#ifdef ENABLE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogTransferOK( *this->el_context, EDG_WLL_SOURCE_LRMS, ce.c_str(), 
				   logfile.c_str(), rsl.c_str(), "Job successfully submitted to Globus",
				   el_s_unavailable );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got globus submit event, ce = " << ce << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event globus submit, ce = " << ce << endl;
#endif

  return;
}

void EventLogger::execute_event( const char *host )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::execute_event(...)" );

#ifdef ENABLE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogRunning( *this->el_context, host );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got job execute event, host = " << host << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event execute, host = " << host << endl;
#endif

  return;
}

void EventLogger::terminated_event( int retcode )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::terminated_event(...)" );

#ifdef ENABLE_LOGGING
  int          res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogDoneOK( *this->el_context, (retcode ? "Warning: job exit code != 0" : "Job terminated successfully"), retcode );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got job terminated event, return code = " << retcode << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event terminated, return code = " << retcode << endl;
#endif

  return;
}

void EventLogger::failed_on_error_event( const string &cause )
{
  logger::StatePusher      pusher( elog::cedglog, "EventLogger::failed_on_error_event(...)" );

#ifdef ENABLE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogDoneFAILED( *this->el_context, cause.c_str(), 1 );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got a job failed event." << endl
		  << "Reason = \"" << cause << "\"." << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job failed." << endl
		<< "Reason = \"" << cause << "\"." << endl;
#endif

  return;
}

void EventLogger::abort_on_error_event( const string &cause )
{
  logger::StatePusher      pusher( elog::cedglog, "EventLogger::abort_on_error_event(...)" );

#ifdef ENABLE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogAbort( *this->el_context, cause.c_str() );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got a job aborted event." << endl
		  << "Reason = \"" << cause << "\"." << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job failed." << endl
		<< "Reason = \"" << cause << "\"." << endl;
#endif

  return;
}

void EventLogger::aborted_by_system_event( const string &cause )
{
  logger::StatePusher        pusher( elog::cedglog, "EventLogger::abort_by_system_event(...)" );

#ifdef ENABLE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogDoneFAILED( *this->el_context, cause.c_str(), 1 );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got aborted by system event." << endl
		  << "Cause = \"" << cause << "\"" << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event aborted by system." << endl
		<< "Cause = \"" << cause << "\"" << endl;
#endif

  return;
}

void EventLogger::aborted_by_user_event( void )
{
  logger::StatePusher       pusher( elog::cedglog, "EventLogger::aborted_by_user_event()" );

#ifdef ENABLE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogCancelDONE( *this->el_context, "Aborted by user." );

      this->testCode( res );
    } while( res != 0 );

    this->startLogging();
    do {
      res = edg_wll_LogDoneCANCELLED( *this->el_context, "Aborted by user", 0 );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got aborted by user event." << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event aborted by user." << endl;
#endif

  return;
}

void EventLogger::globus_submit_failed_event( const string &rsl, const char *reason, const string &logfile )
{
  logger::StatePusher      pusher( elog::cedglog, "EventLogger::globus_submit_failed_event(...)" );

#ifdef ENABLE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogTransferFAIL( *this->el_context, EDG_WLL_SOURCE_LRMS, el_s_unavailable, logfile.c_str(),
				     rsl.c_str(), reason, el_s_unavailable );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got globus submission failed event." << endl
		  << "Reason = \"" << reason << "\"" << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event globus submission failed." << endl
		<< "Reason = \"" << reason << "\"" << endl;
#endif

  return;
}

void EventLogger::globus_resource_down_event( void )
{
  logger::StatePusher      pusher( elog::cedglog, "EventLogger::globus_resource_down_event()" );

#ifdef ENABLE_LOGGING
  int           res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogDoneFAILED( *this->el_context, "Globus resource down", 1 );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got Globus resource down event." << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event globus resource down." << endl;
#endif

  return;
}

void EventLogger::job_held_event( const string &reason )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::job_held_event(...)" );

#ifdef ENABLE_LOGGING
  int            res;
  string         what( "Got a job held event, reason: " );

  what.append( reason );

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogDoneFAILED( *this->el_context, what.c_str(), 1 );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got job held event." << endl
		  << "Reason = \"" << reason << "\"" << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job held." << endl
		<< "Reason = \"" << reason << "\"" << endl;
#endif
}

void EventLogger::job_enqueued_start_event( const string &filename, const classad::ClassAd *ad )
{
  logger::StatePusher      pusher( ts::edglog, "EventLogger::job_enqueued_start_event(...)" );

#ifdef ENABLE_LOGGING
  int                         res;
  string                      job;
  classad::ClassAdUnParser    unparser;

  if( ad != NULL ) unparser.Unparse( job, (classad::ClassAd *) ad );

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogEnQueuedSTART( *this->el_context, filename.c_str(), job.c_str(), el_s_unavailable );

      this->testCode( res );
    } while( res != 0 );
  }
  else 
    ts::edglog << logger::setlevel( logger::null )
	       << "Job enqueued start event." << endl
	       << el_s_notLogged << endl;

#else
  ts::edglog << logger::setlevel( logger::null )
	     << "Unlogged event job enqueued start." << endl;
#endif

  return;
}

void EventLogger::job_enqueued_ok_event( const string &filename, const classad::ClassAd *ad )
{
  logger::StatePusher      pusher( ts::edglog, "EventLogger::job_enqueued_ok_event(...)" );

#ifdef ENABLE_LOGGING
  int                         res;
  string                      job;
  classad::ClassAdUnParser    unparser;

  unparser.Unparse( job, (classad::ClassAd *) ad );

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogEnQueuedOK( *this->el_context, filename.c_str(), job.c_str(), el_s_unavailable );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    ts::edglog << logger::setlevel( logger::null )
	       << "Job enqueued ok event." << endl
	       << el_s_notLogged << endl;

#else
  ts::edglog << logger::setlevel( logger::null )
	     << "Unlogged event job enqueued ok." << endl;
#endif

  return;
}

void EventLogger::job_enqueued_failed_event( const string &filename, const string &error, const classad::ClassAd *ad )
{
  logger::StatePusher       pusher( ts::edglog, "EventLogger::job_enqueued_failed_event(...)" );

#ifdef ENABLE_LOGGING
  int                        res;
  string                     job;
  classad::ClassAdUnParser   unparser;

  unparser.Unparse( job, (classad::ClassAd *) ad );

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogEnQueuedFAIL( *this->el_context, filename.c_str(), job.c_str(), error.c_str() );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    ts::edglog << logger::setlevel( logger::null )
	       << "Job enqueued failed." << endl
	       << "Reason = \"" << error << "\"" << endl
	       << el_s_notLogged << endl;

#else
  ts::edglog << logger::setlevel( logger::null )
	     << "Unlogged event job enqueud failed." << endl
	     << "Reason = \"" << error << "\"" << endl;
#endif

  return;
}

void EventLogger::job_dequeued_event( const string &filename )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::job_dequeued_event(...)" );

#ifdef ENABLE_LOGGING
  int            res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogDeQueued( *this->el_context, filename.c_str(), el_s_unavailable );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Job dequeued from file " << filename << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job dequeueud." << endl;
#endif

  return;
}

void EventLogger::job_cancel_requested_event( const string &source )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::job_cancel_requested_event()" );

#ifdef ENABLE_LOGGING
  int            res;
  string         reason( "Cancel requested by " );

  reason.append( source );

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogCancelREQ( *this->el_context, reason.c_str() );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Got cancel from " << source << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event cancel requested." << endl;
#endif

  return;
}

void EventLogger::condor_submit_start_event( const string &logfile )
{
  logger::StatePusher      pusher( elog::cedglog, "EventLogger::condor_submit_start_event(...)" );

#ifdef ENABLE_LOGGING
  int            res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogTransferSTART( *this->el_context, EDG_WLL_SOURCE_LOG_MONITOR, "localhost", logfile.c_str(),
				      el_s_unavailable, el_s_unavailable, el_s_unavailable );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Condor submit start event." << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event condor submit start." << endl;
#endif

  return;
}

void EventLogger::condor_submit_ok_event( const string &rsl, const string &condorid, const string &logfile )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::condor_submit_ok_event(...)" );

#ifdef ENABLE_LOGGING
  int            res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogTransferOK( *this->el_context, EDG_WLL_SOURCE_LOG_MONITOR, "localhost", logfile.c_str(),
				   rsl.c_str(), el_s_unavailable, condorid.c_str() );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Condor submit ok event." << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event condor submit ok." << endl;
#endif

  return;
}

void EventLogger::condor_submit_failed_event( const string &rsl, const string &reason, const string &logfile )
{
  logger::StatePusher      pusher( elog::cedglog, "EventLogger::condor_submit_failed_event(...)" );

#ifdef ENABLE_LOGGING
  int            res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogTransferFAIL( *this->el_context, EDG_WLL_SOURCE_LOG_MONITOR, "localhost", logfile.c_str(),
				     rsl.c_str(), reason.c_str(), el_s_unavailable );

      this->testCode( res );
    } while( res != 0 );

    this->startLogging();
    do {
      res = edg_wll_LogAbort( *this->el_context, "Submission to condor failed." );

      this->testCode( res );
    } while( res != 0 );
  }
  else {
    elog::cedglog << logger::setlevel( logger::null )
		  << "Condor submit failed event." << endl
		  << logger::setmultiline( true, "CE-> " ) << "Reason\n" << reason << endl
		  << el_s_notLogged << endl;
    elog::cedglog << logger::setmultiline( false );
  }
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event condor submit failed." << endl
		<< logger::setmultiline( true, "CE-> " ) << "Reason\n" << reason << endl;
  elog::cedglog << logger::setmultiline( false );
#endif

  return;
}

void EventLogger::job_cancel_refused_event( const string &info )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::job_cancel_refused_event(...)" );

#ifdef ENABLE_LOGGING
  int            res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogCancelREFUSE( *this->el_context, info.c_str() );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Cancel refused failed event." << endl
		  << "Reason \"" << info << "\"" << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event condor cancel failed." << endl
		<< "Reason: \"" << info << "\"" << endl;
#endif

  return;
}

void EventLogger::job_abort_classad_invalid_event( const string &logfile, const string &reason )
{
  logger::StatePusher      pusher( elog::cedglog, "EventLogger::job_abort_classad_invalid_event(...)" );

#ifdef ENABLE_LOGGING
  int            res;
  string         info( "Invalid classad syntax: " );

  info.append( reason );

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogTransferFAIL( *this->el_context, EDG_WLL_SOURCE_LOG_MONITOR, "localhost", logfile.c_str(),
				     el_s_unavailable, info.c_str(), el_s_unavailable );

      this->testCode( res );
    } while( res != 0 );

    this->startLogging();
    do {
      res = edg_wll_LogAbort( *this->el_context, info.c_str() );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Job aborted for invalid classad." << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job abort for invalid classad." << endl;
#endif

  return;
}

void EventLogger::job_abort_cannot_write_submit_file_event( const string &logfile, const string &filename, const string &moreinfo )
{
  logger::StatePusher    pusher( elog::cedglog, "EventLogger::job_abort_cannot_write_submit_file_event(...)" );

#ifdef ENABLE_LOGGING
  int            res;
  string         info( "Cannot create condor submit file \"" );

  info.append( filename ); info.append( "\": " );
  info.append( moreinfo );

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogTransferFAIL( *this->el_context, EDG_WLL_SOURCE_LOG_MONITOR, "localhose", logfile.c_str(),
				     el_s_unavailable, info.c_str(), el_s_unavailable );

      this->testCode( res );
    } while( res != 0 );

    this->startLogging();
    do {
      res = edg_wll_LogAbort( *this->el_context, info.c_str() );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Job aborted for condor submit error." << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job abort for condor submit error." << endl;
#endif

  return;
}

void EventLogger::job_resubmitting_event( void )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::job_resubmitted_event()" );

#ifdef ENABLE_LOGGING
  int            res;

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogResubmissionWILLRESUB( *this->el_context, el_s_unavailable, el_s_unavailable );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Job resubmitting event." << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job resubmitting to WM." << endl;
#endif

  return;
}

void EventLogger::job_wm_enqueued_start_event( const string &filename, const classad::ClassAd &ad )
{
  logger::StatePusher        pusher(elog::cedglog, "EventLogger::job_wm_enqueued_start_event(...)" );

#ifdef ENABLE_LOGGING
  int                       res;
  string                    job;
  classad::ClassAdUnParser  unparser;

  unparser.Unparse( job, (classad::ClassAd *) &ad );

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogEnQueuedSTART( *this->el_context, filename.c_str(), job.c_str(), el_s_unavailable );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Job enqeueued to WM start event." << endl
		  << el_s_notLogged << endl;

#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job enqueued to WM start." << endl;
#endif

  return;
}

void EventLogger::job_wm_enqueued_ok_event( const string &filename, const classad::ClassAd &ad )
{
  logger::StatePusher     pusher( elog::cedglog, "EventLogger::job_wm_enqueued_ok_event(...)" );

#ifdef ENABLE_LOGGING
  int                       res;
  string                    job;
  classad::ClassAdUnParser  unparser;

  unparser.Unparse( job, (classad::ClassAd *) &ad );

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogEnQueuedOK( *this->el_context, filename.c_str(), job.c_str(), el_s_unavailable );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Job enqueued to WM ok event." << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job enqueued to WM ok." << endl;
#endif

  return;
}

void EventLogger::job_wm_enqueued_failed_event( const string &filename, const classad::ClassAd &ad, const string &error )
{
  logger::StatePusher    pusher( elog::cedglog, "EventLogger::job_wm_enqueued_failed_event(...)" );

#ifdef ENABLE_LOGGING
  int                       res;
  string                    job;
  classad::ClassAdUnParser  unparser;

  unparser.Unparse( job, (classad::ClassAd *) &ad );

  if( this->el_context ) {
    this->startLogging();
    do {
      res = edg_wll_LogEnQueuedFAIL( *this->el_context, filename.c_str(), job.c_str(), error.c_str() );

      this->testCode( res );
    } while( res != 0 );
  }
  else
    elog::cedglog << logger::setlevel( logger::null )
		  << "Job enqueued to WM failed." << endl
		  << "Reason = \"" << error << "\"" << endl
		  << el_s_notLogged << endl;
#else
  elog::cedglog << logger::setlevel( logger::null )
		<< "Unlogged event job enqueued to wm failed." << endl
		<< "Reason = \"" << error << "\"" << endl;
#endif

  return;
}

string EventLogger::sequence_code( void )
{
  char          *seqcode;
  string         res( "undefined" );

#ifdef ENABLE_LOGGING
  if( this->el_context ) {
    seqcode = edg_wll_GetSequenceCode( *this->el_context );

    res.assign( seqcode );
    free( seqcode );
  }
#else
  res.assign( "UI=000000:NS=000000:WM=000000:BH=000000:JSS=000000:LM=000000:LRMS=000000:APP=000000" );
#endif

  return res;
}

}; // Namespace common

} JOBCONTROL_NAMESPACE_END;
