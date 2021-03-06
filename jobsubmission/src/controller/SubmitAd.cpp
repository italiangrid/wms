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

#include <cctype>

#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>

#include <classad_distribution.h>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

namespace fs = boost::filesystem;

#include "jobcontrol_namespace.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "common/src/configuration/LMConfiguration.h"
#include "common/src/configuration/JCConfiguration.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"

#include "glite/jobid/JobId.h"

#include "common/src/utilities/boost_fs_add.h"
#include "common/files.h"

#include "SubmitAd.h"
#include "SubmitAdExceptions.h"

USING_COMMON_NAMESPACE;
using namespace std;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

void SubmitAd::loadStatus( void )
try {
  const configuration::LMConfiguration   *config = configuration::Configuration::instance()->lm();
  string                      err;
  ifstream                    ifs;
  fs::path     status( config->monitor_internal_dir(), fs::native );

  status /= "controller.status";

  if( fs::exists(status) ) {
    ifs.open( status.native_file_string().c_str() );

    if( ifs.good() ) {
      ifs >> this->sa_lastEpoch >> this->sa_jobperlog;

      ifs.close();
    }
    else
      throw CannotOpenStatusFile( status.native_file_string() );
  }
  else {
    this->sa_lastEpoch = time( NULL );
    this->saveStatus();
  }

  return;
}
catch( fs::filesystem_error &err ) {
  throw FileSystemError( err.what() );
}

void SubmitAd::saveStatus( void )
{
  const configuration::LMConfiguration   *config = configuration::Configuration::instance()->lm();
  string                      err;
  ofstream                    ofs;
  fs::path     status( config->monitor_internal_dir(), fs::native );

  status /= "controller.status";

  ofs.open( status.native_file_string().c_str() );
  if( ofs.good() ) {
    ofs << this->sa_lastEpoch << ' ' << this->sa_jobperlog << endl;

    ofs.close();
  }
  else
    throw CannotOpenStatusFile( status.native_file_string(), 1 );

  return;
}

void SubmitAd::createFromAd( const classad::ClassAd *pad )
{
  const configuration::LMConfiguration    *lmconfig = configuration::Configuration::instance()->lm();
  int                                      maxjobs = lmconfig->jobs_per_condor_log();
  time_t                                   epoch = 0;
  char                                    *dirType;
  glite::jobid::JobId                      edgId;
  string                                   buildPath;
  auto_ptr<jccommon::Files>                files;
  logger::StatePusher                      pusher( elog::cedglog, "SubmitAd::createFromAd(...)" );

  if( this->sa_ad.get() == NULL ) {
    this->sa_ad.reset(static_cast<classad::ClassAd*>(pad->Copy()));
  }

  this->sa_jobtype.assign( glite::jdl::get_type(*this->sa_ad, this->sa_good) );

  if( this->sa_good ) {
    transform( this->sa_jobtype.begin(), this->sa_jobtype.end(), this->sa_jobtype.begin(), ::tolower );

  this->sa_jobid.assign( glite::jdl::get_edg_jobid(*this->sa_ad, this->sa_good) );

  if( !this->sa_good ) this->sa_reason.assign( "Cannot extract \"edg_jobid\" from given classad." );
  else {
     try {
       edgId = glite::jobid::JobId( this->sa_jobid );
     } catch (const glite::jobid::JobIdError &e) {
  elog::cedglog << logger::setlevel( logger::warning )
                  << "Could not create JobId from string: " << e.what() << endl;
      } 
      files.reset(new jccommon::Files(edgId));

      try {
	dirType = "job directory";
	buildPath.assign( files->output_directory().native_file_string() );
	elog::cedglog << logger::setlevel( logger::info ) << "Creating " << dirType << " path." << endl
		      << logger::setlevel( logger::debug ) << "Path = \"" << buildPath << "\"." << endl;

	utilities::create_parents( files->output_directory() );

	if( !fs::exists(files->submit_file().branch_path()) ) {
	  dirType = "submit file";
	  buildPath.assign( files->submit_file().branch_path().native_file_string() );
	  elog::cedglog << logger::setlevel( logger::info ) << "Path for " << dirType << " doesn't exist, creating..." << endl
			<< logger::setlevel( logger::debug ) << "Path = \"" << buildPath << "\"." << endl;

	  utilities::create_parents( files->submit_file().branch_path() );
	}

	if( !fs::exists(files->classad_file().branch_path()) ) {
	  dirType = "classad file";
	  buildPath.assign( files->classad_file().branch_path().native_file_string() );
	  elog::cedglog << logger::setlevel( logger::info ) << "Path for " << dirType << " doesn't exist, creating..." << endl
			<< logger::setlevel( logger::debug ) << "Path = \"" << buildPath << "\"." << endl;

	  utilities::create_parents( files->classad_file().branch_path() );
	}
      }
      catch( utilities::CannotCreateParents const& err ) {
	elog::cedglog << logger::setlevel( logger::fatal )
		      << "Failed to create " << dirType << " path \"" << buildPath << "\"." << endl
		      << "Reason: " << err.what() << endl;

	throw CannotCreateDirectory( dirType, buildPath, err.what() );
      }

      this->sa_submitfile.assign( files->submit_file().native_file_string() );
      this->sa_classadfile.assign( files->classad_file().native_file_string() );

   if( this->sa_jobtype == "job" ) {
	    // Create CondorG log file name
       epoch = this->sa_lastEpoch;
	    if( this->sa_jobperlog >= maxjobs ) {
	      this->sa_last = true;

	      this->sa_lastEpoch = time( NULL );
	      this->sa_jobperlog = 1;

	      elog::cedglog << logger::setlevel( logger::warning )
			  << "Maximum number of jobs per log file reached." << endl
			  << logger::setlevel( logger::info )
			  << "Next job will be submitted to the file: CondorG." << this->sa_lastEpoch << ".log." << endl;
	    } else {
         this->sa_jobperlog += 1;
       }

	    this->saveStatus();

     this->sa_logfile.assign( files->log_file(epoch).native_file_string() );
   }

   try {
	glite::jdl::set_log( *this->sa_ad, this->sa_logfile );
	glite::jdl::set_condor_submit_file( *this->sa_ad, this->sa_submitfile );
      } catch( glite::jdl::CannotGetAttribute &par ) {
	this->sa_reason.assign( "Cannot extract parameter \"" );
	this->sa_reason.append( par.parameter() ); this->sa_reason.append( "\" from given classad." );

	this->sa_good = false;
      } catch( glite::jdl::CannotSetAttribute &par ) {
	this->sa_reason.assign( "Cannot set parameter \"" );
	this->sa_reason.append( par.parameter() ); this->sa_reason.append( "\" into given classad." );

	this->sa_good = false;
      } catch( glite::jdl::CannotRemoveAttribute &par ) {
	this->sa_reason.assign( "Cannot remove parameter \"" );
	this->sa_reason.append( par.parameter() ); this->sa_reason.append( "\" from classad." );

	this->sa_good = false;
      }
    }

  }

  return;
}

SubmitAd::SubmitAd( const classad::ClassAd *pad )
  : sa_good( true ), sa_last( false ),
    sa_jobperlog( 1 ), sa_lastEpoch( 0 ),
    sa_ad( pad ? static_cast<classad::ClassAd*>(pad->Copy()) : NULL ),
    sa_jobid(), sa_jobtype(),
    sa_submitfile(), sa_submitad(), sa_reason(), sa_seqcode(),
    sa_classadfile(), sa_logfile()
{
  if( pad ) this->createFromAd( pad );
}

SubmitAd::~SubmitAd( void ) {}

SubmitAd &SubmitAd::set_sequence_code( const string &code )
{
  string     seqcode, notes;

  this->sa_seqcode.assign( code );

  if( this->sa_good ) {
    if( this->sa_seqcode.size() == 0 )
      seqcode.assign( glite::jdl::get_lb_sequence_code(*this->sa_ad) );
    else
      seqcode.assign( this->sa_seqcode );

    if( this->sa_jobtype == "job" ) {
      notes.assign( "(" ); notes.append( this->sa_jobid ); notes.append( ") (" );
      notes.append( seqcode );notes.append( ") (" );
      notes.append( boost::lexical_cast<string>(this->sa_last) ); notes.append( 1, ')' );

      glite::jdl::set_lb_sequence_code( *this->sa_ad, seqcode );

      seqcode.insert( seqcode.begin(), '\'' ); seqcode.append( 1, '\'' );
      glite::jdl::set_arguments( *this->sa_ad, seqcode ); // Must pass the seqcode as first argument of the JobWrapper...

	glite::jdl::set_submit_event_notes( *this->sa_ad, notes );
    }
  }

  return *this;
}

} // Namespace controller

} JOBCONTROL_NAMESPACE_END
