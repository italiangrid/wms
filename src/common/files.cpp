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

#include <fstream>

#include <classad_distribution.h>

#include <boost/lexical_cast.hpp>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"

#include "glite/jobid/JobId.h"
#include "glite/wms/common/utilities/manipulation.h"

#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/wms/common/utilities/boost_fs_add.h"
#include "jobcontrol_namespace.h"

#include "constants.h"
#include "files.h"

USING_COMMON_NAMESPACE;
using namespace std;
namespace fs = boost::filesystem;

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

const string  Files::f_s_submitPrefix( "Condor." ), Files::f_s_submitSuffix( ".submit" );
const string  Files::f_s_wrapperPrefix( "JobWrapper." ), Files::f_s_scriptSuffix( ".sh" );
const string  Files::f_s_classadPrefix( "ClassAd." );
const string  Files::f_s_stdout( "StandardOutput" ), Files::f_s_stderr( "StandardError" );
const string  Files::f_s_maradona( "Maradona.output" );
const string  Files::f_s_logPrefix( "CondorG." ), Files::f_s_logSuffix( ".log" );
const string  Files::f_s_Output( "output" ), Files::f_s_Input( "input" );

Files::Files( const glite::jobid::JobId &id ) : f_epoch( 0 ), f_submit(), f_classad(), f_outdir(), f_logfile(), f_maradona(),
					 f_sandbox(), f_insbx(), f_outsbx(),
					 f_jobid( utilities::to_filename(id) ),
					 f_jobReduced( utilities::get_reduced_part(id), fs::native )

{}
				
Files::~Files( void ) {}

const fs::path &Files::submit_file( void )
{
  const configuration::JCConfiguration    *jcconfig = configuration::Configuration::instance()->jc();
 
  if( this->f_submit.get() == NULL ) {
    string    filename( f_s_submitPrefix );

    filename.append( this->f_jobid ); filename.append( f_s_submitSuffix );

      string    subdir( utilities::normalize_path(jcconfig->submit_file_dir()) );
      this->f_submit.reset( new path(subdir, fs::native) );

      *this->f_submit /= this->f_jobReduced;

    *this->f_submit /= filename;
  }

  return *this->f_submit;
}

const fs::path &Files::jobwrapper_file( void )
{
  const configuration::JCConfiguration    *jcconfig = configuration::Configuration::instance()->jc();

  if( this->f_wrapper.get() == NULL ) {
    string name( f_s_wrapperPrefix );

    name.append( this->f_jobid ); name.append( f_s_scriptSuffix );

      string     subdir( utilities::normalize_path(jcconfig->submit_file_dir()) );
      this->f_wrapper.reset( new path(subdir, fs::native) );

      *this->f_wrapper /= this->f_jobReduced;

    *this->f_wrapper /= name;
  }

  return *this->f_wrapper;
}

const fs::path &Files::classad_file( void )
{
  const configuration::JCConfiguration    *jcconfig = configuration::Configuration::instance()->jc();
 
  if( this->f_classad.get() == NULL ) {
    string     cname( f_s_classadPrefix );

    cname.append( this->f_jobid );

      string     subdir( utilities::normalize_path(jcconfig->submit_file_dir()) );
      this->f_classad.reset( new path(subdir, fs::native) );

      *this->f_classad /= this->f_jobReduced;

    *this->f_classad /= cname;
  }

  return *this->f_classad;
}

const fs::path &Files::output_directory( void )
{
  const configuration::JCConfiguration    *jcconfig = configuration::Configuration::instance()->jc();
 
  if( this->f_classad.get() == NULL ) {
    string   dirname( utilities::normalize_path(jcconfig->output_file_dir()) );

    this->f_outdir.reset( new path(dirname, fs::native) );

    *this->f_outdir /= this->f_jobReduced;

    *this->f_outdir /= fs::path(this->f_jobid, fs::native);
  }

  return *this->f_outdir;
}

const fs::path &Files::standard_output( void )
{
  if( this->f_stdout.get() == NULL ) {
    const path    &outdir = this->output_directory();

    this->f_stdout.reset( new path(outdir / f_s_stdout) );
  }

  return *this->f_stdout;
}

const fs::path &Files::standard_error( void )
{
  if( this->f_stderr.get() == NULL ) {
    const path    &outdir = this->output_directory();

    this->f_stderr.reset( new path(outdir / f_s_stderr) );
  }

  return *this->f_stderr;
}

const fs::path &Files::maradona_file( void )
{
  if( this->f_maradona.get() == NULL ) {
    this->f_maradona.reset( new path(this->sandbox_root()) );

    *this->f_maradona /= f_s_maradona;
  }

  return *this->f_maradona;
}

const fs::path &Files::log_file( time_t epoch )
{
  const configuration::LMConfiguration    *lmconfig = configuration::Configuration::instance()->lm();

  if( (epoch != this->f_epoch) || (this->f_logfile.get() == NULL) ) {
      string    logdir( utilities::normalize_path(lmconfig->condor_log_dir()) ), logname( f_s_logPrefix );

      logname.append( boost::lexical_cast<string>(epoch) );

      logname.append( f_s_logSuffix );

      this->f_logfile.reset( new path(logdir, fs::native) );
      *this->f_logfile /= logname; this->f_epoch = epoch;
  }

  return *this->f_logfile;
}

const fs::path &Files::log_file( void )
{
  if( this->f_logfile.get() == NULL ) {
      classad::ClassAd        *ad;
      const path              &adfile = this->classad_file();
      ifstream                 ifs( adfile.native_file_string().c_str() );
      classad::ClassAdParser   parser;

      ad = parser.ParseClassAd( ifs );

      if( ad != NULL ) {
	bool     good = false;
	string   logfile( glite::jdl::get_log(*ad, good) );

	if( good )
	  this->f_logfile.reset( new path(utilities::normalize_path(logfile), fs::native) );
	else
	  this->f_logfile.reset( new path );
      }
      else this->f_logfile.reset( new path );
  }

  return *this->f_logfile;
}

const fs::path &Files::sandbox_root( void )
{
  const configuration::NSConfiguration    *nsconfig = configuration::Configuration::instance()->ns();

  if( this->f_sandbox.get() == NULL ) {
    string    sbx( utilities::normalize_path(nsconfig->sandbox_staging_path()) );

    this->f_sandbox.reset( new path(sbx, fs::native) );
    *this->f_sandbox /= this->f_jobReduced / fs::path(this->f_jobid, fs::native);
  }

  return *this->f_sandbox;
}

const fs::path &Files::input_sandbox( void )
{
  if( this->f_insbx.get() == NULL ) {
    this->f_insbx.reset( new path(this->sandbox_root()) );

    *this->f_insbx /= f_s_Input;
  }

  return *this->f_insbx;
}

const fs::path &Files::output_sandbox( void )
{
  if( this->f_outsbx.get() == NULL ) {
    this->f_outsbx.reset( new path(this->sandbox_root()) );

    *this->f_outsbx /= f_s_Output;
  }

  return *this->f_outsbx;
}

} // Namespace jccommon

} JOBCONTROL_NAMESPACE_END
