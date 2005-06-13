#include <cstdio>
#include <ctime>

#include <string>
#include <memory>

#include <user_log.c++.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/regex.hpp>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/utilities/boost_fs_add.h"
#include "glite/wms/common/utilities/streamdescriptor.h"
#include "../jobcontrol_namespace.h"
#include "../logmonitor/processer/EventFactory.h"
#include "../logmonitor/processer/EventInterface.h"
#include "../logmonitor/processer/MonitorData.h"

#include "CondorMonitor.h"
#include "Timer.h"
#include "SizeFile.h"
#include "exceptions.h"

using namespace std;
namespace fs = boost::filesystem;

USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor {

string  CondorMonitor::cm_s_recycleDirectory;

struct internal_data_s {
  internal_data_s( const string &filename );

  auto_ptr<processer::EventFactory>  id_event_factory;
  fs::path            id_logfile_path;
  ReadUserLog                        id_logfile_parser;
};

internal_data_s::internal_data_s( const string &filename ) : id_event_factory(),
							     id_logfile_path( filename, fs::native ),
							     id_logfile_parser()
{}

void CondorMonitor::doRecycle( void )
{
  fs::path    recycleDir( cm_s_recycleDirectory, fs::native );
  fs::path   &oldfile = this->cm_internal_data->id_logfile_path;
  fs::path    dotfile( this->cm_shared_data->md_sizefile->filename(), fs::native ), newfile;
  logger::StatePusher        pusher( elog::cedglog, "CondorMonitor::doRecycle()" );

  newfile = recycleDir / oldfile.native_file_string();

  fs::copy_file( oldfile, newfile );
  elog::cedglog << logger::setlevel( logger::info ) << "Old log file (" << this->cm_shared_data->md_logfile_name
		<< ") copied to recycle file \"" << newfile.native_file_string() << "\"" << endl;

  fs::remove( oldfile );
  elog::cedglog << logger::setlevel( logger::debug ) << "Successfully removed old file." << endl;

  fs::remove( dotfile );
  elog::cedglog << logger::setlevel( logger::debug ) << "Successfully removed size file." << endl;

  return;
}

void CondorMonitor::writeCurrentPosition( void )
{
  long int               currpos;
  FILE                  *fp;
  string                 err;
  logger::StatePusher    pusher( elog::cedglog, "CondorMonitor::writeCurrentPosition()" );

  if( this->cm_shared_data->md_sizefile->good() ) {
    fp = this->cm_internal_data->id_logfile_parser.getfp();
    currpos = ftell( fp );

    if( currpos == -1 ) {
      elog::cedglog << logger::setlevel( logger::null ) << "Error during ftell operation on log file \""
		    << this->cm_shared_data->md_logfile_name << "\"." << endl;

      err.assign( "Cannot ftell log file \"" ); err.append( this->cm_shared_data->md_logfile_name ); err.append( "\"." );
      throw FileSystemError( err );
    }
    else {
      this->cm_shared_data->md_sizefile->update_position( currpos );

      if( !this->cm_shared_data->md_sizefile->good() ) {
        err.assign( "Error while writing on position file \"" );
	err.append( this->cm_shared_data->md_sizefile->filename() ); err.append( "\"." );

        elog::cedglog << logger::setlevel( logger::fatal ) << err << endl;

	throw FileSystemError( err );
      }
    }
  }
  else {
    err.assign( "Size file object contained a previous error. Giving up." );
    elog::cedglog << logger::setlevel( logger::fatal ) << err << endl;

    throw CannotExecute( err );
  }

  return;
}

CondorMonitor::status_t CondorMonitor::checkAndProcessTimers( void )
{
  status_t                              stat = no_events;
  auto_ptr<processer::EventInterface>   processor;
  Timer::EventIterator                  timerIt;
  list<Timer::EventIterator>            removedTimers;
  list<Timer::EventIterator>::iterator  remIt;
  logger::StatePusher                   pusher( elog::cedglog, "CondorMonitor::checkAndProcessTimers()" );

  for( timerIt = this->cm_shared_data->md_timer->begin();
       (timerIt != this->cm_shared_data->md_timer->end()) && timerIt->second->expired();
       ++timerIt ) {
    elog::cedglog << logger::setlevel( logger::verylow ) << "Expired timeout for cluster: " 
		  << timerIt->second->to_event()->cluster << endl;

    processor.reset( this->cm_internal_data->id_event_factory->create_processor(timerIt->second->to_event(), true) );
    processor->process_event();
    processor.reset();

    stat = timer_expired;

    removedTimers.push_back( timerIt );
  }

  if( !removedTimers.empty() ) {
    for( remIt = removedTimers.begin(); remIt != removedTimers.end(); ++remIt )
      this->cm_shared_data->md_timer->remove_timeout( *remIt );

    removedTimers.clear();
  }

  return stat;
}

CondorMonitor::CondorMonitor( const string &filename, MonitorData &data ) : 
  cm_shared_data( new processer::MonitorData(filename, data) ),
  cm_internal_data( new internal_data_s(this->cm_shared_data->md_logfile_name) )
{
  const configuration::LMConfiguration *conf = configuration::Configuration::instance()->lm();

  FILE                                           *fp;
  string                                         &logfile_name = this->cm_shared_data->md_logfile_name;
  fs::path                        &logfile_path = this->cm_internal_data->id_logfile_path;
  string                                          dagId, error, logfile_nopath( logfile_path.native_file_string() );
  boost::match_results<string::const_iterator>    match_pieces;
  fs::path                         timer_path( conf->monitor_internal_dir(), fs::native );
  logger::StatePusher                             pusher( elog::cedglog, "CondorMonitor::CondorMonitor()" );

  static boost::regex   dagid_expression( ".*DagId = ([^\\s]+).*" );

  if( fs::exists(logfile_path) ) { 
    this->cm_shared_data->md_sizefile.reset( new SizeFile(logfile_name.c_str()) );

    elog::cedglog << logger::setlevel( logger::info ) << "Opened old log position file." << endl;
  }
  else {
    utilities::create_file( logfile_name.c_str() );
    this->cm_shared_data->md_sizefile.reset( new SizeFile(logfile_name.c_str(), true) );

    elog::cedglog << logger::setlevel( logger::info ) << "Created new log position file." << endl;
  }

  this->cm_internal_data->id_logfile_parser.initialize( logfile_name.c_str() );
  if( this->cm_internal_data->id_logfile_parser.getfd() == -1 ) {
    elog::cedglog << logger::setlevel( logger::severe )
		  << "Cannot open Condor log file \"" << logfile_name << "\"." << endl;

    throw CannotOpenFile( logfile_name );
  }

  if( this->cm_shared_data->md_sizefile->size_field().position() != 0 ) {
    elog::cedglog << logger::setlevel( logger::info ) << "Old (known) status for this file:" << endl
		  << "Log position = " << this->cm_shared_data->md_sizefile->size_field().position() << ", "
		  << this->cm_shared_data->md_sizefile->size_field().pending() << " job(s) running." << endl
		  << "Last job " << ( this->cm_shared_data->md_sizefile->size_field().last() ? "yet" : "not" ) << " submitted." << endl;

    fp = this->cm_internal_data->id_logfile_parser.getfp();

    if( fseek(fp, this->cm_shared_data->md_sizefile->size_field().position(), SEEK_SET) == -1 ) {
      error.assign( "Cannot seek file \"" ); error.append( this->cm_shared_data->md_logfile_name );
      error.append( "\" to old position " );
      error.append( boost::lexical_cast<string>(this->cm_shared_data->md_sizefile->size_field().position()) );
      error.append( ", reason: " );
      error.append( strerror(errno) ); error.append( "." );

      elog::cedglog << logger::setlevel( logger::fatal ) << "Cannot seek file \"" << logfile_name
		    << "\" to old position " << this->cm_shared_data->md_sizefile->size_field().position() << endl
		    << "Due to: \"" << strerror(errno) << "\"" << endl;

      throw FileSystemError( error );
    }
  }

  elog::cedglog << logger::setlevel( logger::info ) << "Condor log file parser initialized." << endl;

  if( boost::regex_match(this->cm_shared_data->md_sizefile->header().header(), match_pieces, dagid_expression) ) {
    this->cm_shared_data->md_dagId.assign( match_pieces[1].first, match_pieces[2].second );

    try {
      glite::wmsutils::jobid::JobId     dId( dagId );

      this->cm_shared_data->md_isDagLog = true;
    }
    catch( const glite::wmsutils::jobid::JobIdException &err ) {
      this->cm_shared_data->md_dagId.clear();
      this->cm_shared_data->md_isDagLog = false;
    }
  }
  else {
    this->cm_shared_data->md_dagId.clear();
    this->cm_shared_data->md_isDagLog = false;
  }

  if( this->cm_shared_data->md_isDagLog )
    elog::cedglog << logger::setlevel( logger::info ) << "Log file is attached to DAG id: " << this->cm_shared_data->md_dagId << endl
		  << "Entering DAG mode..." << endl;

  logfile_nopath.append( ".timer" );
  timer_path /= logfile_nopath;

  this->cm_shared_data->md_timer.reset( new Timer(timer_path.native_file_string()) );

  this->cm_internal_data->id_event_factory.reset( new processer::EventFactory(this->cm_shared_data) );
}

CondorMonitor::~CondorMonitor( void )
{
  logger::StatePusher     pusher( elog::cedglog, "CondorMonitor::~CondorMonitor()" );

  if( this->cm_internal_data.unique() && this->cm_shared_data->md_sizefile->completed() ) {
    try {
      fs::path   timerfile( this->cm_shared_data->md_timer->filename(), fs::native );
      this->cm_shared_data->md_timer.reset(); // Remove the timer and the filelist it was containing before deleting the file...

      elog::cedglog << logger::setlevel( logger::info )
		    << "Removing timer file " << timerfile.native_file_string() << " from staging directory." << endl;
      fs::remove( timerfile );
      elog::cedglog << logger::setlevel( logger::debug ) << "Successfully removed." << endl;
    }
    catch( fs::filesystem_error &err ) {
      elog::cedglog << logger::setlevel( logger::severe )
		    << "Removing of the timer file failed." << endl
		    << "Filesystem error catched: \"" << err.what() << "\"." << endl;
    }

    try { this->doRecycle(); }
    catch( fs::filesystem_error &err ) {
      elog::cedglog << logger::setlevel( logger::severe )
		    << "Condor log recycling operation failed." << endl
		    << "Filesystem error catched: \"" << err.what() << "\"." << endl;
    }
    catch( ... ) {
      elog::cedglog << logger::setlevel( logger::severe )
		    << "Condor log recycling operation failed." << endl
		    << "Unknown error happened." << endl;
    }
  }
}

CondorMonitor::status_t CondorMonitor::process_next_event( void )
{
  status_t                              stat = event_read;
  streampos                             size;
  ULogEventOutcome                      outcome;
  ULogEvent                            *event = NULL;
  string                               &logfile_name = this->cm_shared_data->md_logfile_name;
  fs::path              &logfile_path = this->cm_internal_data->id_logfile_path;
  auto_ptr<processer::EventInterface>   processor;
  auto_ptr<ULogEvent>                   scoped_event;
  logger::StatePusher                   pusher( elog::cedglog, "CondorMonitor::process_next_event()" );

  try {
    size = fs::file_size( logfile_path );
  }
  catch( fs::filesystem_error &err ) {
    throw FileSystemError( err.what() );
  }

  if( size > this->cm_shared_data->md_sizefile->size_field().position() ) {
    outcome = this->cm_internal_data->id_logfile_parser.readEvent( event );
    scoped_event.reset( event );

    switch( outcome ) {
    case ULOG_NO_EVENT:
      elog::cedglog << logger::setlevel( logger::verylow ) << "Reached the end." << endl;

      this->writeCurrentPosition();

      stat = this->checkAndProcessTimers();
      break;
    case ULOG_RD_ERROR:
    case ULOG_UNK_ERROR:
      elog::cedglog << logger::setlevel( logger::null ) << "Error while reading log file \""
		    << logfile_name << "\"." << endl;

      stat = event_error;
      break;
    case ULOG_OK:
      processor.reset( this->cm_internal_data->id_event_factory->create_processor(event, true) );
      processor->process_event();
      processor.reset();

      this->writeCurrentPosition();

      stat = event_read;
      break;
    default:
      elog::cedglog << logger::setlevel( logger::severe ) << "Reached unreachable code." << endl;

      stat = event_error;
      break;
    }
  }
  else stat = this->checkAndProcessTimers();

  return stat;
}

bool CondorMonitor::file_completed( void ) const
{
  return this->cm_shared_data->md_sizefile->completed();
}

bool CondorMonitor::got_last( void ) const
{
  return this->cm_shared_data->md_sizefile->size_field().last();
}

unsigned int CondorMonitor::pending_jobs( void ) const
{
  return this->cm_shared_data->md_sizefile->size_field().pending();
}

const string &CondorMonitor::logfile_name( void ) const
{
  return this->cm_shared_data->md_logfile_name;
}

}; // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END;
