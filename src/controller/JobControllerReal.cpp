// File: JobControllerReal.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//         Rosario Peluso <Rosario.Peluso@pd.infn.it>
// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

// $Id$

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cctype>

#include <memory>
#include <iostream>
#include <fstream>
#include <algorithm>
#ifdef HAVE_STRINGSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

#include <classad_distribution.h>
#include <condor/user_log.c++.h>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"

#include "glite/jobid/JobId.h"

#include "glite/jdl/convert.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"

#include "common/id_container.h"
#include "common/ram_container.h"
#include "common/JobFilePurger.h"
#include "common/ProxyUnregistrar.h"
#include "common/constants.h"

#include "JobControllerReal.h"
#include "JobControllerExceptions.h"
#include "CondorG.h"
#include "glite/wms/jobsubmission/SubmitAdapter.h"
#include "SubmitAd.h"
#include "SubmitAdExceptions.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

namespace {

const int jcr_s_threshold = 10;

GenericEvent *createGenericEvent( int evn )
{
  GenericEvent    *event = (GenericEvent *) instantiateEvent( ULOG_GENERIC );
  string           info( "JC: " );

  info.append( boost::lexical_cast<string>(evn) );
  info.append( " - " );
  info.append( jccommon::generic_events_string[evn] );

  strncpy( event->info, info.c_str(), (sizeof(event->info) - 1) );

  return event;
}

void logGenericEvent( jccommon::generic_event_t ev, int condorid, const char *logfile )
{
  int                      evn = static_cast<int>( ev );
  auto_ptr<GenericEvent>   event( createGenericEvent(evn) );
  WriteUserLog                  logger( "owner", logfile, boost::lexical_cast<int>(condorid), 0, 0 );

  logger.writeEvent( event.get() );

  return;
}

bool cancelJob( const string &condorid, string &info )
{
  int                      result;
  string                   parameters;
  logger::StatePusher      pusher( elog::cedglog, "cancelJob(...)" );
                                                                                                                                                            
  elog::cedglog << logger::setlevel( logger::debug ) << "Condor id of job was: " << condorid << endl;
                                                                                                                                                            
  parameters.assign( "-constraint 'ClusterId==" );
  string::size_type pos = condorid.find('.');
                                                                                                                                                            
  if( pos != std::string::npos ) { // procID is defined
    parameters.append( condorid.substr(0, pos) );
    parameters.append( " && ProcId==" );
    parameters.append( condorid.substr( pos+1 ) );
  } else { // procID is not define so put 0
    parameters.append( condorid );
    parameters.append( " && ProcId==0" );
  }
  // remove job only if is not in "removed" status
  parameters.append( " && JobStatus!=3'");
                                                                                                                                                            
  result = CondorG::instance()->set_command( CondorG::remove, parameters )->execute( info );
                                                                                                                                                            
  if( result ) { // normal cancellation has been refused, try to force it
    elog::cedglog << logger::setlevel( logger::severe ) << "Job cancellation refused." << endl
         << "Condor ID = " << condorid << endl
         << "Reason: \"" << info << "\"." << endl;
                                                                                                                                                            
    elog::cedglog << logger::setlevel( logger::info ) << "Try to force job removal  (only for _globus_ job)." << endl;
                                                                                                                                                            
    parameters.assign( "-f -constraint 'ClusterId==" );
                                                                                                                                                            
    string::size_type pos = condorid.find('.');
                                                                                                                                                            
    if( pos != std::string::npos ) { // procID is defined
      parameters.append( condorid.substr(0, pos) );
      parameters.append( " && ProcId==" );
      parameters.append( condorid.substr( pos+1 ) );
    } else { // procID is not define so put 0
      parameters.append( condorid );
      parameters.append( " && ProcId==0" );
    }
    // force removal must be used only for globus jobs
    parameters.append( " && JobUniverse==9 && JobGridType==\"globus\"'" );
                                                                                                                                                            
    result = CondorG::instance()->set_command( CondorG::remove, parameters )->execute( info );
  }

  if( !result )
    elog::cedglog << logger::setlevel( logger::info ) << "Job has been succesfully removed." << endl;
                                                                                                                                                          
  return !result;
}

} // Anonymous namespace

JobControllerReal::JobControllerReal( edg_wll_Context *cont ) : jcr_logger(cont), jcr_threshold(0)
{
  logger::StatePusher                     pusher(elog::cedglog, "JobControllerReal::JobControllerReal()");
  const configuration::LMConfiguration   *lmconfig = configuration::Configuration::instance()->lm();
  const configuration::JCConfiguration   *jcconfig = configuration::Configuration::instance()->jc();
  string                                  repname( lmconfig->id_repository_name() );
  fs::path                                repfile(lmconfig->monitor_internal_dir(), fs::native);

  repfile /= repname;

  try {
    auto_ptr<jccommon::IdContainer> repository(
      new jccommon::IdContainer(repfile.native_file_string().c_str())
    );
    this->jcr_repository.reset( new jccommon::RamContainer(*repository) );
  } catch(jccommon::FileContainerError const& err) {
    elog::cedglog << logger::setlevel( logger::null )
      << "File container error: " << err.string_error() << endl;

    throw CannotCreate( err.string_error() );
  }

  jcr_threshold = jcconfig->container_refresh_threshold(jcr_s_threshold);
  if (jcr_threshold < jcr_s_threshold) {
    jcr_threshold = jcr_s_threshold;
  }

  elog::cedglog << logger::setlevel( logger::ugly ) << "Controller created..." << endl;
}

JobControllerReal::~JobControllerReal( void )
{}

void JobControllerReal::readRepository()
{
  const configuration::LMConfiguration   *lmconfig = configuration::Configuration::instance()->lm();
  string                                  repname(lmconfig->id_repository_name());
  fs::path                                repfile(lmconfig->monitor_internal_dir(), fs::native);
  logger::StatePusher                     pusher(elog::cedglog, "JobControllerReal::readRepository()");

  repfile /= repname;

  try {
    elog::cedglog << logger::setlevel(logger::medium)
      << "Reading repository from LogMonitor file: " << repfile.native_file_string() << endl;

    auto_ptr<jccommon::IdContainer> repository(
      new jccommon::IdContainer(repfile.native_file_string().c_str())
    );
    jcr_repository->copy(*repository);
  } catch(jccommon::FileContainerError const& err) {
    elog::cedglog << logger::setlevel(logger::null)
       << "File container error: " << err.string_error() << endl;

    throw CannotCreate(err.string_error());
  }

  return;
}

int JobControllerReal::submit( const classad::ClassAd *pad )
try {
  int                                result = 1;
  int  										 numberId = -1;
  string                             rsl, parameters, info, condorid, seqcode;
  SubmitAdapter                      sad( *pad );
  logger::StatePusher                pusher( elog::cedglog, "JobControllerReal::submit(...)" );
  auto_ptr<classad::ClassAd>         ad;
  ofstream                           ofs;
#ifdef HAVE_STRINGSTREAM
  ostringstream                      oss;
#else
  ostrstream                         oss;
#endif

  bool const have_lbproxy = configuration::Configuration::instance()->common()->lbproxy();
  boost::match_results<string::const_iterator>    pieces;
  static boost::regex                             expr( "^.*[0-9]+ job\\(s\\) submitted to cluster ([0-9]+)\\.[0-9]*.*$" );

  this->jcr_logger.condor_submit_start_event( sad->log_file() );

  seqcode.assign( this->jcr_logger.sequence_code() );
  ad.reset( sad.adapt_for_submission(seqcode) );

  elog::cedglog << logger::setlevel( logger::verylow ) << "Submitting job \"" << sad->job_id() << "\"" << endl;

  if (ad.get()) {
    glite::jdl::to_submit_stream( oss, *ad );

#ifndef HAVE_STRINGSTREAM
    if( oss.str() == NULL ) {
      elog::cedglog << logger::setlevel( logger::verylow ) << "Submit file is empty, aborting job..." << endl;
      this->jcr_logger.job_abort_cannot_write_submit_file_event( sad->log_file(), sad->submit_file(), "Submit file empty" );

      jccommon::ProxyUnregistrar( sad->job_id() ).unregister();
      jccommon::JobFilePurger( sad->job_id(), have_lbproxy).do_purge( true );

      return 0;
    }
#endif

    rsl.assign( oss.str() );

#ifndef HAVE_STRINGSTREAM
    oss.freeze( false );
#else
    if( rsl.length() == 0 ) {
      elog::cedglog << logger::setlevel( logger::verylow ) << "Submit file is empty, aborting job..." << endl;
      this->jcr_logger.job_abort_cannot_write_submit_file_event( sad->log_file(), sad->submit_file(), "Submit file empty" );

      jccommon::ProxyUnregistrar( sad->job_id() ).unregister();
      jccommon::JobFilePurger(sad->job_id(), have_lbproxy).do_purge( true );

      return 0;
    }
#endif

    ofs.open( sad->submit_file().c_str() );

    if( ofs.good() ) {
      ofs << rsl << endl;
      ofs.close();
      elog::cedglog << logger::setlevel( logger::medium ) << "Submit file created..." << endl;

      parameters.assign( "-d " ); parameters.append( sad->submit_file() ); parameters.append( " 2>&1" );

      int count = 3;	      

      while ( ( count > 0 ) && ( result ) ) { // fix for bug #23401
      	result = CondorG::instance()->set_command( CondorG::submit, parameters )->execute( info );
				count--;
        if ( result ) sleep( 2 * count );
      }	

      if( result || !boost::regex_match(info, pieces, expr) ) {
  // The condor command has failed... Do the right thing
  elog::cedglog << logger::setlevel( logger::null )
          << "Error submitting job." << endl
          << "condor_submit return code = " << result << endl
          << logger::setmultiline( true, "CE-> " ) << "Given reason\n" << info << endl;
  elog::cedglog << logger::setmultiline( false );
    
  this->jcr_logger.condor_submit_failed_event( rsl, info, sad->log_file() );

	jccommon::ProxyUnregistrar( sad->job_id() ).unregister();
	jccommon::JobFilePurger(sad->job_id(), have_lbproxy).do_purge(true);
      } else {
	// The condor command worked fine... Do the right thing
	condorid.assign( pieces[1].first, pieces[1].second );
	numberId = boost::lexical_cast<int>( condorid );

	elog::cedglog << logger::setlevel( logger::verylow )
		      << "Job submitted to Condor cluster: " << condorid << endl;

   if (jcr_repository->inserted() >= jcr_threshold) {
     this->readRepository();
   }
   jcr_repository->insert(sad->job_id(), condorid);

	this->jcr_logger.condor_submit_ok_event( "(unavailable)", condorid, sad->log_file() );
      }
    } else {
      elog::cedglog << logger::setlevel( logger::null ) << "Cannot open condor submit file for writing." << endl
		    << "File: \"" << sad->submit_file() << "\"" << endl;

      this->jcr_logger.job_abort_cannot_write_submit_file_event( sad->log_file(), sad->submit_file(), "Cannot open file" );

      jccommon::ProxyUnregistrar( sad->job_id() ).unregister();
      jccommon::JobFilePurger(sad->job_id(), have_lbproxy).do_purge(true);

      throw CannotExecute( "Cannot open condor submit file." );
    }
  } else { // Submit classad not good...
    bool      good;
    string    id( glite::jdl::get_edg_jobid(*pad, good) ), type( glite::jdl::get_type(*pad, good) );

    elog::cedglog << logger::setlevel( logger::null ) << "Received classad is invalid." << endl
		  << "Reason: \"" << sad->reason() << "\"" << endl;

    this->jcr_logger.job_abort_classad_invalid_event( sad->log_file(), sad->reason() );

    transform( type.begin(), type.end(), type.begin(), ::tolower );
    jccommon::ProxyUnregistrar( id ).unregister();
    jccommon::JobFilePurger(id, have_lbproxy).do_purge(true);
  }

  return numberId;
} catch( SubmitAdException &error ) {
  throw CannotExecute( error.error() );
}

bool JobControllerReal::cancel( const glite::jobid::JobId &id, const char *logfile )
{
  logger::StatePusher   pusher(elog::cedglog, "JobControllerReal::cancel(...)");
  bool                  good = true;
  int                   icid = 0;

  elog::cedglog << logger::setlevel(logger::info)
		<< "Asked to remove job: " << id.toString() << endl;

  string sid(id.toString());
  string condorid(this->jcr_repository->condor_id(sid));

  if (condorid.empty()) { // syncronize the "ram" repository with the LM's one
     readRepository();
     condorid = jcr_repository->condor_id(sid);
  } else {
    // Comunicate to LM that this request comes from the user
    if(logfile) {
      icid = boost::lexical_cast<int>(condorid);
    }
    if (logfile) {
      logGenericEvent(jccommon::user_cancelled_event, icid, logfile);
    }

    string info;
    if ((good = cancelJob(condorid, info))) { // The condor command worked fine
     if(logfile) {
       logGenericEvent(jccommon::cancelled_event, icid, logfile);
     }

     elog::cedglog << logger::setlevel( logger::verylow ) << "Job " << sid << " successfully marked for removal." << endl;
    }
  }
  return good;
}

bool JobControllerReal::cancel( int condorid, const char *logfile )
{
  logger::StatePusher   pusher( clog, "JobControllerReal::cancel(...)" );
  string                sid(boost::lexical_cast<string>(condorid));

  clog << logger::setlevel(logger::info)
       << "Asked to remove job: " << sid << " (by condor ID)." << endl;

  bool good = false;
  string info;
  if ((good = cancelJob(sid, info))) {
    clog << logger::setlevel( logger::info ) << "Job " << sid << " successfully marked for removal." << endl;

    if( logfile ) logGenericEvent( jccommon::cancelled_event, condorid, logfile );
  } else if( logfile ) {
    logGenericEvent(jccommon::cannot_cancel_event, condorid, logfile);
  }

  return good;
}

bool JobControllerReal::release(int condorid, const char *logfile )
{
  logger::StatePusher pusher( clog, "JobControllerReal::release(...)" );
  clog << logger::setlevel( logger::info )
     << "Asked to release job: " << condorid << " (by condor ID)." << endl;
  elog::cedglog << logger::setlevel( logger::debug ) << "Condor id of job was: " << condorid << endl;
  int result;
  string parameters("-constraint 'ClusterId==" + boost::lexical_cast<std::string>(condorid) + "'");
  string info;
  result = CondorG::instance()->set_command( CondorG::release, parameters)->execute(info);
  if (result) { // normal cancellation has been refused, try to force it
    elog::cedglog << logger::setlevel( logger::severe ) << "Job release refused." << endl << "Condor ID = " << condorid << endl << "Reason: \"" << info << "\"." << endl;
  }

  return result ;
}

size_t JobControllerReal::queue_size( void )
{
  return 0;
}

} // namespace controller

} JOBCONTROL_NAMESPACE_END
