#include <fstream>

#include <classad_distribution.h>

#include <boost/lexical_cast.hpp>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"

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
const string  Files::f_s_classadPrefix( "ClassAd." ), Files::f_s_dagPrefix( "dag." );
const string  Files::f_s_stdout( "StandardOutput" ), Files::f_s_stderr( "StandardError" );
const string  Files::f_s_maradona( "Maradona.output" );
const string  Files::f_s_logPrefix( "CondorG." ), Files::f_s_dagLogPrefix( "dag." ), Files::f_s_logSuffix( ".log" );
const string  Files::f_s_Output( "output" ), Files::f_s_Input( "input" );

Files::path *Files::createDagLogFileName( const string &jobid )
{
  const configuration::LMConfiguration    *lmconfig = configuration::Configuration::instance()->lm();
  string               logdir( fs::normalize_path(lmconfig->condor_log_dir()) ), logname( f_s_dagLogPrefix );
  std::auto_ptr<path>  logfile;

  logname.append( jobid ); logname.append( f_s_logSuffix );
  logfile.reset( new path(logdir, fs::native) );
  *logfile /= logname;

  return logfile.release();
}

Files::Files( const glite::wmsutils::jobid::JobId &id ) : f_epoch( 0 ), f_submit(), f_classad(), f_outdir(), f_logfile(), f_maradona(),
					 f_sandbox(), f_insbx(), f_outsbx(), f_dagsubdir(),
					 f_jobid( glite::wmsutils::jobid::to_filename(id) ), f_dagid(),
					 f_jobReduced( glite::wmsutils::jobid::get_reduced_part(id), fs::native ),
					 f_dagReduced()

{}
				
Files::Files( const glite::wmsutils::jobid::JobId &dagid, const glite::wmsutils::jobid::JobId &id ) : f_epoch( 0 ), f_submit(), f_classad(), f_outdir(), f_logfile(),
								    f_maradona(), f_insbx(), f_outsbx(), f_dagsubdir(),
								    f_jobid( glite::wmsutils::jobid::to_filename(id) ),
								    f_dagid( glite::wmsutils::jobid::to_filename(dagid) ),
								    f_jobReduced( glite::wmsutils::jobid::get_reduced_part(id), fs::native ),
								    f_dagReduced( glite::wmsutils::jobid::get_reduced_part(dagid), fs::native )
{}

Files::~Files( void ) {}

const fs::path &Files::dag_submit_directory( void )
{
  const configuration::JCConfiguration    *jcconfig = configuration::Configuration::instance()->jc();

  if( this->f_dagsubdir.get() == NULL ) {
    string   subdir( fs::normalize_path(jcconfig->submit_file_dir()) );
    string   dagname( f_s_dagPrefix );

    if( this->f_dagid.size() == 0 ) dagname.append( this->f_jobid );
    else dagname.append( this->f_dagid );

    this->f_dagsubdir.reset( new path(subdir, fs::native) );

	if( this->f_dagid.size() == 0 )  *this->f_dagsubdir /= this->f_jobReduced / dagname;
    else *this->f_dagsubdir /= this->f_dagReduced / dagname;  
}

  return *this->f_dagsubdir;
}

const fs::path &Files::submit_file( void )
{
  const configuration::JCConfiguration    *jcconfig = configuration::Configuration::instance()->jc();
 
  if( this->f_submit.get() == NULL ) {
    string    filename( f_s_submitPrefix );

    filename.append( this->f_jobid ); filename.append( f_s_submitSuffix );

    if( this->f_dagid.size() == 0 ) {
      string    subdir( fs::normalize_path(jcconfig->submit_file_dir()) );
      this->f_submit.reset( new path(subdir, fs::native) );

      *this->f_submit /= this->f_jobReduced;
    }
    else this->f_submit.reset( new path(this->dag_submit_directory()) );

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

    if( this->f_dagid.size() == 0 ) {
      string     subdir( fs::normalize_path(jcconfig->submit_file_dir()) );
      this->f_wrapper.reset( new path(subdir, fs::native) );

      *this->f_wrapper /= this->f_jobReduced;
    }
    else this->f_wrapper.reset( new path(this->dag_submit_directory()) );

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

    if( this->f_dagid.size() == 0 ) {
      string     subdir( fs::normalize_path(jcconfig->submit_file_dir()) );
      this->f_classad.reset( new path(subdir, fs::native) );

      *this->f_classad /= this->f_jobReduced;
    }
    else this->f_classad.reset( new path(this->dag_submit_directory()) );

    *this->f_classad /= cname;
  }

  return *this->f_classad;
}

const fs::path &Files::output_directory( void )
{
  const configuration::JCConfiguration    *jcconfig = configuration::Configuration::instance()->jc();
 
  if( this->f_classad.get() == NULL ) {
    string   dirname( fs::normalize_path(jcconfig->output_file_dir()) );

    this->f_outdir.reset( new path(dirname, fs::native) );

    if( this->f_dagid.size() != 0 )
      *this->f_outdir /= this->f_dagReduced / fs::path(this->f_dagid, fs::native);
    else
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

const fs::path &Files::dag_log_file( void )
{
  if( this->f_logfile.get() == NULL )
    this->f_logfile.reset( this->createDagLogFileName(this->f_jobid) );

  return *this->f_logfile;
}

const fs::path &Files::log_file( time_t epoch )
{
  const configuration::LMConfiguration    *lmconfig = configuration::Configuration::instance()->lm();

  if( (epoch != this->f_epoch) || (this->f_logfile.get() == NULL) ) {
    if( this->f_dagid.size() == 0 ) {
      string    logdir( fs::normalize_path(lmconfig->condor_log_dir()) ), logname( f_s_logPrefix );

      logname.append( boost::lexical_cast<string>(epoch) );

      logname.append( f_s_logSuffix );

      this->f_logfile.reset( new path(logdir, fs::native) );
      *this->f_logfile /= logname; this->f_epoch = epoch;
    }
    else this->f_logfile.reset( this->createDagLogFileName(this->f_dagid) );
  }

  return *this->f_logfile;
}

const fs::path &Files::log_file( void )
{
  if( this->f_logfile.get() == NULL ) {
    if( this->f_dagid.size() == 0 ) {
      classad::ClassAd        *ad;
      const path              &adfile = this->classad_file();
      ifstream                 ifs( adfile.native_file_string().c_str() );
      classad::ClassAdParser   parser;

      ad = parser.ParseClassAd( ifs );

      if( ad != NULL ) {
	bool     good = false;
	string   logfile( glite::jdl::get_log(*ad, good) );

	if( good )
	  this->f_logfile.reset( new path(fs::normalize_path(logfile), fs::native) );
	else
	  this->f_logfile.reset( new path );
      }
      else this->f_logfile.reset( new path );
    }
    else this->dag_log_file();
  }

  return *this->f_logfile;
}

const fs::path &Files::sandbox_root( void )
{
  const configuration::NSConfiguration    *nsconfig = configuration::Configuration::instance()->ns();

  if( this->f_sandbox.get() == NULL ) {
    string    sbx( fs::normalize_path(nsconfig->sandbox_staging_path()) );

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

}; // Namespace jccommon

} JOBCONTROL_NAMESPACE_END;
