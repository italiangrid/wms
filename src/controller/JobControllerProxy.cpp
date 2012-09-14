// File: JobControllerProxy.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>,
//         Rosario Peluso <Rosario.Peluso@na.infn.it>
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

#include <string>
#include <fstream>
#include <memory>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <condor/user_log.c++.h>

#include "glite/lb/producer.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/configuration/Configuration.h"

#include "glite/jobid/JobId.h"
#include "glite/wmsutils/classads/classad_utils.h"

#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"
#include "jobcontrol_namespace.h"
#include "common/files.h"

#include "JobControllerProxy.h"
#include "JobControllerExceptions.h"
#include "CondorG.h"
#include "Request.h"
#include "RequestExceptions.h"

using namespace std;
namespace ca = glite::wmsutils::classads;

USING_COMMON_NAMESPACE;
RenameLogStreamNS_ts( ts );

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

/*
  Public methods
*/
JobControllerProxy::JobControllerProxy(
  boost::shared_ptr<utils::JobDir> jcp_jd,
  edg_wll_Context *cont
) : 
  jcp_source(static_cast<int>(configuration::Configuration::instance()->get_module())),
  jcp_jobdir(jcp_jd),
  jcp_logger(cont)
{}

int
JobControllerProxy::submit(const classad::ClassAd *ad) try
{
  string                  jobid( glite::jdl::get_edg_jobid(*ad) );
  SubmitRequest           request(*ad, this->jcp_source);
  logger::StatePusher     pusher( ts::edglog, "JobControllerProxy::submit(...)" );

  request.set_sequence_code( this->jcp_logger.sequence_code() );

  this->jcp_logger.job_enqueued_start_event(
    this->jcp_jobdir->base_dir().native_file_string(), 0
  );
  std::string const ad_str(ca::unparse_classad(*ad));
  try {
    this->jcp_jobdir->deliver(ad_str);
  } catch(utilities::JobDirError &error) {
    this->jcp_logger.job_enqueued_failed_event(
      this->jcp_jobdir->base_dir().native_file_string(),
      error.what(),
      &request.get_request()
    );
    throw CannotExecute(error.what());
  }
  this->jcp_logger.job_enqueued_ok_event(
    this->jcp_jobdir->base_dir().native_file_string(),
    &request.get_request()
  );

  return 0;
}
catch( glite::jdl::ManipulationException &par ) {
  string   error( "Cannot extract " );
  
  error.append( par.parameter() );
  error.append( " from passed classad." );
  
  throw CannotExecute( error );
}
catch( RequestException &err ) {
  string    error( err.what() );

  throw CannotExecute( error );
}

bool JobControllerProxy::cancel( const glite::jobid::JobId &id, const char *logfile )
{
  bool                          good;
  string                        proxyfile, lf;
  ifstream                      ifs;
  RemoveRequest                 request( id.toString(), this->jcp_source );
  jccommon::Files               files( id );
  boost::filesystem::path       cadfile( files.classad_file() );
  auto_ptr<classad::ClassAd>    jobad;
  classad::ClassAdParser        parser;

  if( boost::filesystem::exists(cadfile) ) {
    ifs.open( cadfile.native_file_string().c_str() );
    jobad.reset( parser.ParseClassAd(ifs) );

    if( jobad.get() != NULL ) {
      proxyfile.assign( glite::jdl::get_x509_user_proxy(*jobad, good) );

      if( good ) request.set_proxyfile( proxyfile );
      
      if ( !logfile ) { // See lcg2 bug 3883
        lf.assign( glite::jdl::get_log(*jobad, good) );
        if( good ) request.set_logfile( lf );
        }
      }

    ifs.close();
  }

  request.set_sequence_code(this->jcp_logger.sequence_code());
  if(logfile) {
    request.set_logfile(string(logfile));
  }

  std::string const ad_str(ca::unparse_classad(classad::ClassAd(request)));
  try {
    this->jcp_jobdir->deliver(ad_str);
  } catch(utilities::JobDirError &error) {
  }

  return true;
}

bool JobControllerProxy::cancel( int condorid, const char *logfile )
{
  CondorRemoveRequest   request( condorid, this->jcp_source );

  if (logfile) {
    request.set_logfile( string(logfile));
  }

  std::string const ad_str(ca::unparse_classad(classad::ClassAd(request)));
  try {
    this->jcp_jobdir->deliver(ad_str);
  } catch(utilities::JobDirError &error) {
  }

  return true;
}

bool JobControllerProxy::release(int condorid, const char *logfile)
{   
  CondorReleaseRequest request(condorid, this->jcp_source);
  if (logfile) {
    request.set_logfile(string(logfile));
  } 
    
  std::string const ad_str(ca::unparse_classad(classad::ClassAd(request)));
  try {
    this->jcp_jobdir->deliver(ad_str);
  } catch(utilities::JobDirError &error) {
  }

  return true;
} 

size_t JobControllerProxy::queue_size( void )
{
  return 0;
}

} // namespace jobcontrol

} JOBCONTROL_NAMESPACE_END
