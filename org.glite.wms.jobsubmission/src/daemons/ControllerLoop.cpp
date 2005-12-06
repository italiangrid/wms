#include <cstring>
#include <cerrno>

#include <memory>
#include <iostream>
#include <vector>

#include <classad_distribution.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <signal.h>

#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/boost_fs_add.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/jdl/JobAdManipulation.h"
#include "glite/wms/jdl/PrivateAdManipulation.h"
#include "glite/wms/jdl/ManipulationExceptions.h"
#include "glite/wms/common/process/process.h"
#include "glite/wms/common/process/user.h"
#include "../common/EventLogger.h"
#include "../common/SignalChecker.h"
#include "../controller/CondorG.h"
#include "../controller/JobController.h"
#include "../controller/JobControllerClient.h"
#include "../controller/JobControllerExceptions.h"
#include "../controller/Request.h"
#include "../controller/RequestExceptions.h"
#include "../common/EventLogger.h"

#include "ControllerLoop.h"
#include "exceptions.h"

using namespace std;
namespace fs = boost::filesystem;

USING_COMMON_NAMESPACE;
RenameLogStreamNS_ts( ts );
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace daemons {

#ifdef HAVE_CONFIG_H
const char         *ControllerLoop::cl_s_version = VERSION;
const char         *ControllerLoop::cl_s_buildUser = BUILD_USERNAME;
const char         *ControllerLoop::cl_s_buildHost = BUILD_HOSTNAME;
#else
const char         *ControllerLoop::cl_s_version = "2.0";
const char         *ControllerLoop::cl_s_buildUser = NULL, *ControllerLoop::cl_s_buildHost = NULL;
#endif
const char         *ControllerLoop::cl_s_time = __TIME__, *ControllerLoop::cl_s_date = __DATE__;
const int           ControllerLoop::cl_s_signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGTERM };
ControllerLoop     *ControllerLoop::cl_s_instance = NULL;

void ControllerLoop::createDirectories( void )
{
  const configuration::JCConfiguration        *jcconfig = configuration::Configuration::instance()->jc();
  const configuration::LMConfiguration        *lmconfig = configuration::Configuration::instance()->lm();
  fs::path                      partial;
  vector<fs::path>              paths;
  vector<fs::path>::iterator    pathIt;

  if( this->cl_verbose ) clog << "Checking for directories..." << endl;

  try {
    paths.push_back(fs::path(jcconfig->input(), fs::native));
    paths.push_back(fs::path(jcconfig->log_file(), fs::native));
    paths.push_back(fs::path(jcconfig->log_rotation_base_file(), fs::native));
    paths.push_back(fs::path(jcconfig->submit_file_dir() + "/boh", fs::native));
    paths.push_back(fs::path(jcconfig->lock_file(), fs::native));
    paths.push_back(fs::path(lmconfig->monitor_internal_dir() + "/boh", fs::native));
    paths.push_back(fs::path(lmconfig->condor_log_dir() + "/boh", fs::native));

    for( pathIt = paths.begin(); pathIt != paths.end(); ++pathIt ) {
      if( (!pathIt->native_file_string().empty()) && !fs::exists(pathIt->branch_path()) ) {
	if( this->cl_verbose ) clog << "\tCreating directory: " << pathIt->branch_path().native_file_string() << "..." << endl;

	fs::create_parents( pathIt->branch_path() );
      }
      else if( this->cl_verbose ) clog << "\tDirectory: " << pathIt->branch_path().native_file_string() << " exists..." << endl;
    }
  }
  catch( fs::filesystem_error &err ) {
    clog << "Filesystem error: \"" << err.what() << "\"." << endl;

    throw CannotStart( err.what() );
  }

  return;
}

void ControllerLoop::activateSignalHandling( void )
{
  vector<bool>::iterator     bIt;
  vector<int>::iterator      iIt;
  vector<bool>               results;
  vector<int>                signals( cl_s_signals, cl_s_signals + sizeof(cl_s_signals) / sizeof(int) );
  logger::StatePusher        pusher( this->cl_stream, "ControllerLoop::activateSignalHandling()" );

  this->cl_stream << logger::setlevel( logger::info ) << "Activating signal handlers." << endl;
  results = jccommon::SignalChecker::instance()->add_signals( signals );

  this->cl_stream << logger::setlevel( logger::debug ) << "Signals trapped:" << endl;
  for( bIt = results.begin(), iIt = signals.begin(); bIt != results.end(); ++bIt, ++iIt ) {
    if( *bIt )
      this->cl_stream << logger::setlevel( logger::debug )
#ifdef HAVE_STRSIGNAL
		      << strsignal( *iIt )
#else
		      << *iIt
#endif
		      << " handler successfully set." << endl;
    else
      this->cl_stream << logger::setlevel( logger::severe ) << "Cannot set handler for signal: "
#ifdef HAVE_STRSIGNAL
		      << strsignal( *iIt )
#else
		      << '(' << *iIt << ')'
#endif
		      << ", proceeding without it !!!" << endl;
  }

  return;
}

bool ControllerLoop::checkSignal( run_code_t &return_code )
{
  bool                  loop = true;
  int                   signal = jccommon::SignalChecker::instance()->check_signal();
  logger::StatePusher   pusher( this->cl_stream, "ControllerLoop::checkSignal(...)" );

  if( signal != 0 ) {
    this->cl_stream << logger::setlevel( logger::warning ) << "Got a "
#ifdef HAVE_STRSIGNAL
		    << '\"' << strsignal(signal) << '\"'
#endif
		    << '(' << signal << ") signal, checking what to do..." << endl;

    switch( signal ) {
    case SIGHUP:
      this->cl_stream << logger::setlevel( logger::warning ) << "Try to restart the daemon." << endl;

      loop = false;
      return_code = reload;

      break;
    case SIGINT:
    case SIGQUIT:
    case SIGTERM:
      this->cl_stream << logger::setlevel( logger::warning ) << "Ending signal, gracefully shutting down." << endl;

      loop = false;
      return_code = shutdown;

      break;
    default:
      this->cl_stream << logger::setlevel( logger::error )
		      << "Unhandled signal received, continuing work..." << endl;

      loop = true;
      return_code = do_nothing;

      break;
    }
  }

  return loop;
}

ControllerLoop::ControllerLoop( const utilities::LineParser &options ) : cl_verbose( options.is_present('v') ),
									 cl_stream( elog::cedglog ),
									 cl_logger(), cl_queuefilename(), cl_executer(),
									 cl_client(),
									 cl_options( options )
{
  const configuration::JCConfiguration       *config = configuration::Configuration::instance()->jc();
  const configuration::CommonConfiguration   *common = configuration::Configuration::instance()->common();
  const char                    *previous = NULL;
  fs::path        listname( config->input(), fs::native );
  fs::path        logname( config->log_file(), fs::native );
  process::User                  currentUser;

  /*
    This will crash the daemon if two instances of this objects have been
    allocated and the first one was on the stack...
    Singltonitude by dangerousity
  */
  if( cl_s_instance ) delete cl_s_instance;

  if( currentUser.uid() == 0 ) {
    if( this->cl_verbose )
      clog << "Running as root, trying to lower permissions..." << endl;

    if( process::Process::self()->drop_privileges_forever(common->dguser().c_str()) ) {
      if( this->cl_verbose )
	clog << "Failed: reason \"" << strerror(errno) << "\"" << endl;

      if( !options.is_present('r') )
	throw CannotStart( "Cannot drop privileges, avoiding running as root." );
      else {
	previous = "Cannot drop privileges, running as root can be dangerous !";
	if( this->cl_verbose ) clog << previous << endl;
      }
    }
    else if( this->cl_verbose )
      clog << "Running as user " << common->dguser() << "..." << endl;
  }
  else if( this->cl_verbose )
    clog << "Running in an unprivileged account..." << endl;

  this->createDirectories();

  this->cl_stream.open( logname.native_file_string(), (logger::level_t) config->log_level() );
  this->cl_stream.activate_log_rotation( config->log_file_max_size(), config->log_rotation_base_file(),
					 config->log_rotation_max_file_number() );
  if( this->cl_stream.good() ) {
    if( this->cl_verbose ) clog << "Opened log file: " << logname.native_file_string() << endl;
  }
  else {
    string    error( "Cannot open log file \"" );
    error.append( logname.native_file_string() ); error.append( "\"" );

    throw CannotStart( error );
  }
  logger::StatePusher      pusher( this->cl_stream, "ControllerLoop::ControllerLoop(...)" );

  try {
    this->cl_logger.reset( this->cl_options.is_present('l') ? new jccommon::EventLogger(NULL) : new jccommon::EventLogger );
    this->cl_logger->initialize_jobcontroller_context();
  }
  catch( jccommon::LoggerException &error ) {
    this->cl_stream << logger::setlevel( logger::fatal )
		    << "Cannot create LB logger object." << endl
		    << "Reason: \"" << error.what() << "\"." << endl;

    throw CannotStart( error.what() );
  }

  this->cl_stream << logger::setlevel( logger::null )
		  << "<------------>" << endl << "<<   MARK   >>" << endl << "<------------>" << endl;

  this->cl_stream << logger::setlevel( logger::high )
		  << "**********************************************************************" << endl
		  << logger::setlevel( logger::veryugly )
		  << "* Vax Headroom Enterprises Inc. and INFN are proud to present you... *" << endl
		  << "* ...the very shiny and groovy brand new JobController !!!           *" << endl
		  << logger::setlevel( logger::high )
		  << "* GLITE JobController Version. " << cl_s_version << "                                     *" << endl
		  << "* Compiled at " << cl_s_date << ", " << cl_s_time << "                                  *" << endl
		  << "**********************************************************************" << endl;

  this->cl_executer.reset( new controller::CondorG(config) );
  this->cl_stream << logger::setlevel( logger::high ) << "Created condor object." << endl;

  if( previous )
    this->cl_stream << logger::setlevel( logger::null ) << previous << endl;

  try {
    this->cl_queuefilename.assign( listname.native_file_string() );
    this->cl_client.reset( new controller::JobControllerClient() );
  }
  catch( controller::CannotCreate &error ) {
    throw CannotStart( error.reason() );
  }

  cl_s_instance = this;

  ts::edglog.unsafe_attach( this->cl_stream ); // Attach edglog to the right stream
}

ControllerLoop::~ControllerLoop( void )
{
  cl_s_instance = NULL;
}

ControllerLoop::run_code_t ControllerLoop::run( void )
try {
  bool                                    loop = true;
  run_code_t                              ret = do_nothing;
  configuration::ModuleType::module_type  source;
  controller::Request::request_code_t     command = controller::Request::unknown;
  const controller::Request              *request;
  controller::JobController               controller( *this->cl_logger );
  logger::StatePusher                     pusher( this->cl_stream, "ControllerLoop::run()" );

  this->activateSignalHandling();

  do {
    loop = this->checkSignal( ret );

    if( loop ) {
      try {
	this->cl_client->extract_next_request();

	this->cl_stream << logger::setlevel( logger::debug ) << "Got new request..." << endl;
	request = this->cl_client->get_current_request();

	command = request->get_command();
	source = static_cast<configuration::ModuleType::module_type>( request->get_source() );

	switch( command ) {
	case controller::Request::submit: {
	  const controller::SubmitRequest     *subreq = static_cast<const controller::SubmitRequest *>( request );
	  const classad::ClassAd              *jobad = subreq->get_jobad();

	  this->cl_stream << logger::setlevel( logger::info ) << "Got new submit request..." << endl; 

#ifdef GLITE_WMS_HAVE_LBPROXY
          this->cl_logger->set_LBProxy_context(glite::wms::jdl::get_edg_jobid(*jobad),
                                               glite::wms::jdl::get_lb_sequence_code(*jobad),
                                               glite::wms::jdl::get_x509_user_proxy(*jobad) );
#else
	  this->cl_logger->reset_user_proxy( glite::wms::jdl::get_x509_user_proxy(*jobad) );
	  this->cl_logger->reset_context( glite::wms::jdl::get_edg_jobid(*jobad), glite::wms::jdl::get_lb_sequence_code(*jobad) );
#endif

	  this->cl_logger->job_dequeued_event( this->cl_queuefilename );

	  this->cl_stream << logger::setlevel( logger::debug ) << logger::setmultiline( true, "--> " )
			  << "Classad received:\n" << request->get_request() << endl;
	  this->cl_stream << logger::setmultiline( false );

	  this->cl_stream << "Executing submit request..." << endl;

	  controller.submit( jobad );

	  break;
	}
	case controller::Request::remove: {
	  const controller::RemoveRequest     *remreq = static_cast<const controller::RemoveRequest *>( request );
	  bool                                 force( remreq->get_force() );
	  string                               jobid( remreq->get_jobid() );

	  this->cl_stream << logger::setlevel( logger::info ) << "Got new remove request (JOB ID = " << jobid << ")..." << endl
			  << "Must " << ( force ? "" : "not " ) << "force job removal !" << endl;

#ifdef GLITE_WMS_HAVE_LBPROXY
          this->cl_logger->set_LBProxy_context( jobid, remreq->get_sequence_code(), remreq->get_proxyfile() );
#else 
	  this->cl_logger->reset_user_proxy( remreq->get_proxyfile() );
	  this->cl_logger->reset_context( jobid, remreq->get_sequence_code() );
#endif

	  if( source == configuration::ModuleType::workload_manager )
	    this->cl_logger->job_cancel_requested_event( configuration::ModuleType::module_name(source) );
	  
	  // See lcg2 bug: 3883
	  fs::path        logfile( remreq->get_logfile(), fs::native );
	  
	  this->cl_stream << logger::setlevel( logger::debug ) << "Executing remove request..." << endl;

	  if( !logfile.empty() )
	    controller.cancel( glite::wmsutils::jobid::JobId(jobid), logfile.native_file_string().c_str(), force );
	  else
	    controller.cancel( glite::wmsutils::jobid::JobId(jobid), NULL, force );
	  
	  break;
	}
	case controller::Request::condorremove: {
	  const controller::CondorRemoveRequest   *cremreq = static_cast<const controller::CondorRemoveRequest *>( request );
	  bool                                     force( cremreq->get_force() );
	  int                                      condorid( cremreq->get_condorid() );

	  this->cl_stream << logger::setlevel( logger::info ) << "Got new remove request (condor ID = " << condorid << ")..." << endl
			  << "Must " << ( force ? "" : "not " ) << "force job removal !" << endl
			  << logger::setlevel( logger::debug ) << "Executing remove request..." << endl;

	  if( source == configuration::ModuleType::log_monitor ) {
	    fs::path    logfile( cremreq->get_logfile(), fs::native );
	    controller.cancel( condorid, logfile.native_file_string().c_str(), force );
	  }
	  else
	    controller.cancel( condorid, NULL, force );

	  break;
	}
	default:
	  this->cl_stream << logger::setlevel( logger::null ) << "__NOT__ Executing unimplemented command \"" 
			  << controller::Request::string_command( command ) << "\"." << endl;

	  break;
	}
      }
      catch( jccommon::SignalChecker::Exception &signal ) {
	if( signal.signal() != 0 )
	  loop = this->checkSignal( ret );
      }
      catch( controller::MalformedRequest &err ) {
	this->cl_stream << logger::setlevel( logger::severe )
			<< "Got malformed request." << endl
			<< logger::setlevel( logger::debug ) << logger::setmultiline( true, "--> " )
			<< "Broken classad = " << err.classad() << endl;
	this->cl_stream << logger::setmultiline( false ) << logger::setlevel( logger::info )
			<< "Ignoring request..." << endl;
      }
      catch( controller::MismatchedProtocol &err ) {
	this->cl_stream << logger::setlevel( logger::severe )
			<< "Cannot execute command \"" << controller::Request::string_command( command ) << "\"." << endl
			<< "Got request with mismatching protocol." << endl
			<< "Must be: \"" << err.default_protocol() << "\", was \"" << err.current_protocol() << "\"." << endl
			<< logger::setlevel( logger::info )
			<< "Ignoring request..." << endl;
      }
      catch( glite::wms::jdl::ManipulationException &par ) {
	this->cl_stream << logger::setlevel( logger::severe )
			<< "Cannot execute command \"" << controller::Request::string_command( command ) << "\"." << endl
			<< "Reason: parameter \"" << par.parameter() << "\" not found in the classad." << endl
			<< logger::setlevel( logger::info )
			<< "Ignoring request..." << endl;
      }
      catch( controller::ControllerError &error ) {
	this->cl_stream << logger::setlevel( logger::severe )
			<< "Cannot execute command \"" << controller::Request::string_command( command ) << "\"." << endl
			<< "Reason: " << error.reason() << endl
			<< logger::setlevel( logger::info )
			<< "Ignoring request..." << endl;
      }
      catch( glite::wmsutils::jobid::JobIdException &error ) {
	this->cl_stream << logger::setlevel( logger::severe )
			<< "Cannot execute command \"" << controller::Request::string_command( command ) << "\"." << endl
			<< "Error creating job id: \"" << error.what() << "\"" << endl
			<< logger::setlevel( logger::info )
			<< "Ignoring request..." << endl;
      }
      catch( jccommon::LoggerException &error ) {
	this->cl_stream << logger::setlevel( logger::fatal )
			<< "Cannot log event to L&B service." << endl
			<< "Reason: " << error.what() << endl
			<< "Giving up." << endl;

	loop = false;
	ret = shutdown;
      }
      catch( std::exception &error ) {
	this->cl_stream << logger::setlevel( logger::null ) << "Got a standard exception..." << endl
			<< "What = \"" << error.what() << "\"" << endl;
	loop = false;
	ret = shutdown;
      }
      catch( ... ) {
	this->cl_stream << logger::setlevel( logger::null ) << "Got an unknown exception..." << endl;

	loop = false;
	ret = shutdown;
      }

      if( loop ) this->cl_client->release_request();
    }
  } while( loop );

  return ret;
}
catch( exception &err ) {
  logger::StatePusher     pusher( this->cl_stream, "ControllerLoop::run()" );

  this->cl_stream << logger::setlevel( logger::null )
		  << "Got an unhandled standard exception !!!" << endl
		  << "Namely: \"" << err.what() << "\"" << endl
		  << "Aborting daemon..." << endl;

  throw;
}
catch( ... ) {
  logger::StatePusher     pusher( this->cl_stream, "ControllerLoop::run()" );

  this->cl_stream << logger::setlevel( logger::null )
		  << "OUCH !!! Got an unknown exception !!!" << endl
		  << "Aborting daemon..." << endl;

  throw;
}

}; // Namespace daemons

} JOBCONTROL_NAMESPACE_END;
