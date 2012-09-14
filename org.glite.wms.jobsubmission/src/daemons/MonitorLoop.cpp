#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include <ctime>

#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>
#include <boost/timer.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <signal.h>

#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/filecontainer.h"
#include "glite/wms/common/utilities/boost_fs_add.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/process/process.h"
#include "glite/wms/common/process/user.h"
#include "common/EventLogger.h"
#include "common/IdContainer.h"
#include "common/SignalChecker.h"
#include "logmonitor/CondorMonitor.h"
#include "logmonitor/AbortedContainer.h"
#include "logmonitor/exceptions.h"
#include "logmonitor/processer/JobResubmitter.h"
#include "logmonitor/processer/MonitorData.h"

#include "MonitorLoop.h"
#include "exceptions.h"

using namespace std;
namespace fs = boost::filesystem;

USING_COMMON_NAMESPACE;
RenameLogStreamNS_ts( ts );
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace daemons {

const char   *MonitorLoop::ml_s_abortedFileName = "AbortedJobs.ids";
#ifdef HAVE_CONFIG_H
const char   *MonitorLoop::ml_s_version = VERSION;
const char   *MonitorLoop::ml_s_buildUser = BUILD_USERNAME;
const char   *MonitorLoop::ml_s_buildHost = BUILD_HOSTNAME;
#else
const char   *MonitorLoop::ml_s_version = "2.0";
const char   *MonitorLoop::ml_s_buildUser = NULL, *MonitorLoop::ml_s_buildHost = NULL;
#endif
const char   *MonitorLoop::ml_s_time = __TIME__, *MonitorLoop::ml_s_date = __DATE__;
const int     MonitorLoop::ml_s_signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGTERM };
MonitorLoop  *MonitorLoop::ml_s_instance = NULL;

void MonitorLoop::createDirectories( void )
{
  const configuration::LMConfiguration        *lmconfig = configuration::Configuration::instance()->lm();
  const configuration::WMConfiguration        *wmconfig = configuration::Configuration::instance()->wm();
  vector<fs::path>              paths;
  vector<fs::path>::iterator    pathIt;

  if( this->ml_verbose ) clog << "Checking for directories..." << '\n';

  try {
    paths.push_back(fs::path(lmconfig->monitor_internal_dir() + "/tmp", fs::native));
    paths.push_back(fs::path(lmconfig->condor_log_dir() + "/tmp", fs::native));
    paths.push_back(fs::path(lmconfig->condor_log_recycle_dir() + "/tmp", fs::native));
    paths.push_back(fs::path(lmconfig->log_file(), fs::native));
    paths.push_back(fs::path(lmconfig->external_log_file(), fs::native));
    paths.push_back(fs::path(lmconfig->log_rotation_base_file(), fs::native));
    paths.push_back(fs::path(lmconfig->lock_file(), fs::native));
    paths.push_back(fs::path(wmconfig->input(), fs::native));

    for( pathIt = paths.begin(); pathIt != paths.end(); ++pathIt ) {
      if( (!pathIt->native_file_string().empty()) && !fs::exists(pathIt->branch_path()) ) {
	if( this->ml_verbose ) clog << "\tCreating directory: " << pathIt->branch_path().native_file_string() << "..." << '\n';

	fs::create_parents( pathIt->branch_path() );
      }
      else if( this->ml_verbose ) clog << "\tDirectory: " << pathIt->branch_path().native_file_string() << " exists..." << '\n';
    }
  }
  catch( fs::filesystem_error &err ) {
    clog << "Filesystem error: \"" << err.what() << "\"." << '\n';

    throw CannotStart( err.what() );
  }

  return;
}

void MonitorLoop::activateSignalHandling( void )
{
  vector<bool>::iterator     bIt;
  vector<int>::iterator      iIt;
  vector<bool>               results;
  vector<int>                signals( ml_s_signals, ml_s_signals + sizeof(ml_s_signals) / sizeof(int) );
  logger::StatePusher        pusher( this->ml_stream, "MonitorLoop::activateSignalHandling()" );

  this->ml_stream << logger::setlevel( logger::info ) << "Activating signal handlers." << '\n';
  results = jccommon::SignalChecker::instance()->add_signals( signals );

  this->ml_stream << logger::setlevel( logger::debug ) << "Signals trapped:" << '\n';
  for( bIt = results.begin(), iIt = signals.begin(); bIt != results.end(); ++bIt, ++iIt ) {
    if( *bIt )
      this->ml_stream << logger::setlevel( logger::debug )
#ifdef HAVE_STRSIGNAL
		      << strsignal( *iIt )
#else
		      << *iIt
#endif
		      << " handler successfully set." << '\n';
    else
      this->ml_stream << logger::setlevel( logger::severe ) << "Cannot set handler for signal: "
#ifdef HAVE_STRSIGNAL
		      << strsignal( *iIt )
#else
		      << '(' << *iIt << ')'
#endif
		      << ", proceeding without it !!!" << '\n';
  }

  return;
}

bool MonitorLoop::checkSignal( run_code_t &return_code )
{
  bool                  loop = true;
  int                   signal = jccommon::SignalChecker::instance()->check_signal();
  logger::StatePusher   pusher( this->ml_stream, "MonitorLoop::checkSignal(...)" );

  if( signal != 0 ) {
    this->ml_stream << logger::setlevel( logger::warning ) << "Got a "
#ifdef HAVE_STRSIGNAL
		    << '\"' << strsignal(signal) << '\"'
#endif
		    << '(' << signal << ") signal, checking what to do..." << '\n';

    switch( signal ) {
    case SIGHUP:
      this->ml_stream << logger::setlevel( logger::warning ) << "Try to restart the daemon." << '\n';

      loop = false;
      return_code = reload;

      break;
    case SIGINT:
    case SIGQUIT:
    case SIGTERM:
      this->ml_stream << logger::setlevel( logger::warning ) << "Ending signal, gracefully shutting down." << '\n';

      loop = false;
      return_code = shutdown;

      break;
    default:
      this->ml_stream << logger::setlevel( logger::error )
		      << "Unhandled signal received, continuing work..." << '\n';

      loop = true;
      return_code = do_nothing;

      break;
    }
  }

  return loop;
}

MonitorLoop::MonitorLoop( const utilities::LineParser &options ) : ml_verbose( options.is_present('v') ), ml_stream( elog::cedglog ),
								   ml_logger(), ml_idContainer(), ml_abContainer(),
								   ml_resubmitter(), ml_options( options )
{
  const configuration::LMConfiguration       *config = configuration::Configuration::instance()->lm();
  const configuration::CommonConfiguration   *common = configuration::Configuration::instance()->common();
  const char                    *previous = NULL;
  fs::path        logname( config->log_file(), fs::native );
  fs::path        internal( config->monitor_internal_dir(), fs::native ), idrep, abrep;
  process::User                  currentUser;

  if( ml_s_instance ) delete ml_s_instance;

  if( currentUser.uid() == 0 ) {
    if( this->ml_verbose )
      clog << "Running as root, trying to lower permissions..." << '\n';

    if( process::Process::self()->drop_privileges_forever(common->dguser().c_str()) ) {
      if( this->ml_verbose )
	clog << "Failed: reason \"" << strerror(errno) << "\"" << '\n';

      if( !options.is_present('r') )
	throw CannotStart( "Cannot drop privileges, avoiding running as root." );
      else {
	previous = "Cannot drop privileges, running as root can be dangerous !";
	if( this->ml_verbose ) clog << previous << '\n';
      }
    }
    else if( this->ml_verbose )
      clog << "Running as user " << common->dguser() << "..." << '\n';
  }
  else if( this->ml_verbose )
    clog << "Running in an unprivileged account..." << '\n';

  this->createDirectories();

  this->ml_stream.open( logname.native_file_string(), (logger::level_t) config->log_level() );
  this->ml_stream.activate_log_rotation( config->log_file_max_size(), config->log_rotation_base_file(),
					 config->log_rotation_max_file_number() );
  if( this->ml_stream.good() ) {
    if( this->ml_verbose ) clog << "Opened log file: " << logname.native_file_string() << '\n';
  }
  else {
    string    error( "Cannot open log file \"" );
    error.append( logname.native_file_string() ); error.append( "\"" );

    throw CannotStart( error );
  }
  logger::StatePusher     pusher( this->ml_stream, "MonitorLoop::MonitorLoop(...)" );

  try {
    this->ml_logger.reset( this->ml_options.is_present('l') ? new jccommon::EventLogger(NULL) : new jccommon::EventLogger );
    this->ml_logger->initialize_logmonitor_context();
  }
  catch( jccommon::LoggerException &error ) {
    this->ml_stream << logger::setlevel( logger::fatal )
		    << "Cannot create LB logger object." << '\n'
		    << "Reason: \"" << error.what() << "\"." << '\n';

    throw CannotStart( error.what() );
  }

  this->ml_stream << logger::setlevel( logger::null )
		  << "<------------>" << '\n' << "<<   MARK   >>" << '\n' << "<------------>" << '\n';

  this->ml_stream << logger::setlevel( logger::null )
		  << "**********************************************************************" << '\n'
		  << logger::setlevel( logger::debug )
		  << logger::setlevel( logger::null )
		  << "* gLite LogMonitor Version. " << ml_s_version << "                                        *" << '\n'
		  << "* Compiled at " << ml_s_date << ", " << ml_s_time << "                                  *" << '\n'
		  << "**********************************************************************" << '\n';

  if( previous )
    this->ml_stream << logger::setlevel( logger::null ) << previous << '\n';

  try {
    idrep = internal / config->id_repository_name();
    abrep = internal / ml_s_abortedFileName;

    this->ml_idContainer.reset( new jccommon::IdContainer(idrep.native_file_string()) );
    this->ml_stream << "Successfully created the id repository file." << '\n';

    this->ml_abContainer.reset( new logmonitor::AbortedContainer(abrep.native_file_string()) );
    this->ml_stream << "Successfully created the aborted job repository file." << '\n';
  }
  catch( fs::filesystem_error &err ) {
    this->ml_stream << logger::setlevel( logger::fatal )
		    << "Filesystem error: \"" << err.what() << "\"." << '\n';

    throw CannotStart( err.what() );
  }
  catch( utilities::FileContainerError &err ) {
    string     error( "Cannot create container: " );
    error.append( err.string_error() );

    this->ml_stream << logger::setlevel( logger::fatal ) << error << '\n';

    throw CannotStart( error );
  }

  this->ml_resubmitter.reset( new logmonitor::processer::JobResubmitter(this->ml_logger));
  this->ml_stream << logger::setlevel( logger::info ) << "Successfully created the resubmitter to WM." << '\n';

  logmonitor::CondorMonitor::recycle_directory(config->condor_log_recycle_dir());

  ml_s_instance = this;

  ts::edglog.unsafe_attach( this->ml_stream ); // Attach edglog to the right stream
}

MonitorLoop::~MonitorLoop()
{
  ml_s_instance = NULL;
}

typedef  boost::shared_ptr<logmonitor::CondorMonitor>   MonitorPtr;
typedef  map<string, MonitorPtr>::iterator              MapIterator;

MonitorLoop::run_code_t MonitorLoop::run()
try {
  const configuration::LMConfiguration   *config = configuration::Configuration::instance()->lm();
  bool                                    loop = true, found_event;
  int                                     remaining, empty_loops = 0, total = config->main_loop_duration();
  unsigned int                            seconds;
  size_t                                  comthr = config->containers_compact_threshold();
  run_code_t                              ret = do_nothing;
  logmonitor::CondorMonitor::status_t     status = logmonitor::CondorMonitor::no_events;
  logmonitor::CondorMonitor              *tmp;
  double                                  elapsed;
  string                                  name, lastfile;
  map<string, MonitorPtr>                 filemap;
  map<string, MonitorPtr>::iterator       filemapIt;
  vector<MapIterator>                     removables;
  vector<MapIterator>::iterator           remIt;
  boost::timer                            elapser;
  fs::path                 logdir( config->condor_log_dir(), fs::native );
  fs::directory_iterator   logIt, logBegin, logEnd;
  logmonitor::MonitorData data(
    this->ml_abContainer,
    this->ml_idContainer,
    this->ml_resubmitter,
    this->ml_logger
  );
  logger::StatePusher                     pusher( this->ml_stream, "MonitorLoop::run()" );

  static boost::regex                     expr( "^\\..*$" );

  this->activateSignalHandling();

  do {
    loop = this->checkSignal( ret );

    if (loop) {
      elapser.restart();
      found_event = false;

      logBegin = fs::directory_iterator( logdir );

      for( logIt = logBegin; logIt != logEnd; ++logIt ) {
        name.assign( logIt->leaf() );
        if (!fs::exists(*logIt)) { // see lcg2 bug 3807
          ; // ignore
        } else if (!fs::is_directory(*logIt) && !boost::regex_match(name, expr) && (filemap.count(name) == 0)) {
          this->ml_stream << logger::setlevel( logger::low )
            << "Adding new condor log file: " << logIt->native_file_string() << '\n';
          
          try {
            tmp = new logmonitor::CondorMonitor(logIt->native_file_string(), data);
            filemap.insert(
              map<string, MonitorPtr>::value_type(logIt->leaf(), MonitorPtr(tmp))
            );

          } catch( logmonitor::CannotOpenFile &err ) {
            this->ml_stream << logger::setlevel( logger::error )
                            << "Detected an error while reading condor log file: " << '\n'
                            << err.reason() << '\n' 
                            << "Probably it is not a CondorLog file: REMOVE it please!!! " << '\n'; 
          }
        }
      }
      
      for (filemapIt = filemap.begin(); filemapIt != filemap.end(); ++filemapIt ) {
        if (lastfile != filemapIt->first) {
          this->ml_stream << logger::setlevel(logger::veryugly)
			      << "Examining file: " << filemapIt->first << '\n';

        lastfile.assign( filemapIt->first );
      }

        do {
          loop = this->checkSignal( ret );

          try {
            if (loop) {
              status = filemapIt->second->process_next_event();

	        if (status == logmonitor::CondorMonitor::event_read) {
            found_event = true;
          }
	      }
	    } catch (jccommon::SignalChecker::Exception &signal) {
	      if(signal.signal()) {
	        loop = this->checkSignal(ret);
        }
	    }
	  } while (loop && (status == logmonitor::CondorMonitor::event_read));

	  if (
      (status == logmonitor::CondorMonitor::no_events)
      && filemapIt->second->file_completed()
    ) {
	    this->ml_stream << logger::setlevel( logger::info ) 
			  << "No more jobs in condor log file." << '\n'
			  << "Scheduling for removal." << '\n';

	    removables.push_back( filemapIt );
    } else if (status == logmonitor::CondorMonitor::event_error) {
      this->ml_stream << logger::setlevel(logger::error)
        << "Detected an error while reading condor log file." << '\n'
        << "Removing it." << '\n';

	    removables.push_back( filemapIt );
    }
  }

  if (loop) {
    if (!removables.empty()) {
	  for (
      remIt = removables.begin();
      remIt != removables.end();
      ++remIt) {
	      filemap.erase( *remIt ); // Removing the CondorMonitor will also trigger its recycling when necessary
    }

	  removables.clear();
	}

	loop = this->checkSignal(ret);

	if (loop) {
	  elapsed = elapser.elapsed();
	  remaining = total - static_cast<int>(elapsed) + 1;

	  if( !found_event ) empty_loops += 1;
	  else empty_loops = 0;

	  if( empty_loops == 1 )
	    this->ml_stream << logger::setlevel( logger::info )
			    << "No new event found, going to sleep." << '\n'
			    << "Checking each " << total << " seconds for new events." << '\n';

	  if( remaining > 0 ) {
	    if( found_event ) {
	      this->ml_stream << logger::setlevel( logger::info )
			      << "Spent " << elapsed << " seconds in the last file loop." << '\n'
			      << "Must wait for other " << remaining << " seconds." << '\n';

	      lastfile.erase();
	    }

	    seconds = sleep(remaining);
	  }

	  loop = this->checkSignal(ret);

	  if( loop ) {
	    try {
	      if( this->ml_idContainer->inserted() >= comthr ) {
		this->ml_stream << logger::setlevel( logger::medium )
				<< "IdContainer size exceeded, compacting." << '\n';

		this->ml_idContainer->compact();
	      }

	      if( this->ml_abContainer->inserted() >= comthr ) {
		this->ml_stream << logger::setlevel( logger::medium )
				<< "AbortedContainer size exceeded, compacting." << '\n';

		this->ml_abContainer->compact();
	      }
	    }
	    catch( utilities::FileContainerError &error ) {
	      this->ml_stream << logger::setlevel( logger::null )
			      << "Cannot compact container." << '\n'
			      << "Reason: " << error.string_error() << '\n';

	      loop = false;
	      ret = shutdown;
	    }
	  }
	}
      }
    }
  } while( loop );

  return ret;
}
catch( logmonitor::MonitorException &error ) {
  logger::StatePusher      pusher( this->ml_stream, "MonitorLoop::run()" );

  this->ml_stream << logger::setlevel( logger::null )
		  << "Cannot run the CondorMonitor, fatal error." << '\n'
		  << error.what() << '\n';

  throw CannotExecute( error.what() );
}
catch( exception &err ) {
  logger::StatePusher      pusher( this->ml_stream, "MonitorLoop::run()" );

  this->ml_stream << logger::setlevel( logger::null )
		  << "Got an unhandled standard exception:" << '\n'
		  << err.what() << '\n';

  throw;
}
catch( ... ) {
  logger::StatePusher      pusher( this->ml_stream, "MonitorLoop::run()" );

  this->ml_stream << logger::setlevel( logger::null )
		  << "unknown exception" << '\n';

  throw;
}

}; // Namespace daemons

} JOBCONTROL_NAMESPACE_END;