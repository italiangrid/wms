// File: JobControllerProxy.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>,
//         Rosario Peluso <Rosario.Peluso@na.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <string>
#include <fstream>
#include <memory>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "glite/lb/producer.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/configuration/Configuration.h"

#include "glite/wmsutils/jobid/JobId.h"

#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"
#include "../jobcontrol_namespace.h"
#include "../common/files.h"

#include "JobControllerProxy.h"
#include "JobControllerExceptions.h"
#include "CondorG.h"
#include "Request.h"
#include "RequestExceptions.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS_ts( ts );

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

/*
  Public methods
*/
JobControllerProxy::JobControllerProxy( queue_type &q, mutex_type &m, edg_wll_Context *cont ) : 
  jcp_source( static_cast<int>(configuration::Configuration::instance()->get_module()) ),
  jcp_mutex( m ), jcp_queue( q ),
  jcp_logger( cont )
{}

JobControllerProxy::~JobControllerProxy()
{}

int JobControllerProxy::submit( const classad::ClassAd *ad )
try {
  string                  jobid( glite::jdl::get_edg_jobid(*ad) );
  SubmitRequest           request( *ad, this->jcp_source );
  logger::StatePusher     pusher( ts::edglog, "JobControllerProxy::submit(...)" );

  this->jcp_logger.job_enqueued_start_event( this->jcp_queue.filename(), 0 );			
  
  request.set_sequence_code( this->jcp_logger.sequence_code() );

  try {
    utilities::FileListLock     lock( this->jcp_mutex );
    this->jcp_queue.push_back( request );

    this->jcp_logger.job_enqueued_ok_event( this->jcp_queue.filename(), &request.get_request() );
  }
  catch( utilities::FileContainerError &error ) {
    this->jcp_logger.job_enqueued_failed_event( this->jcp_queue.filename(), error.string_error(), &request.get_request() );

    throw CannotExecute( error.string_error() );
  }

  return 0;
}
catch( glite::jdl::ManipulationException &par ) {
  string   error( "Cannot extract " );
  
  error.append( par.parameter() ); error.append( " from passed classad." );
  
  throw CannotExecute( error );
}
catch( RequestException &err ) {
  string    error( err.what() );

  throw CannotExecute( error );
}

bool JobControllerProxy::cancel( const glite::wmsutils::jobid::JobId &id, const char *logfile, bool force )
{
  bool                          good;
  string                        proxyfile, lf;
  ifstream                      ifs;
  RemoveRequest                 request( id.toString(), this->jcp_source, force );
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

  request.set_sequence_code( this->jcp_logger.sequence_code() );
  if( logfile ) request.set_logfile( string(logfile) );

  try {
    utilities::FileListLock     lock( this->jcp_mutex );
    this->jcp_queue.push_back( request );
  }
  catch( utilities::FileContainerError &error ) {
    throw CannotExecute( error.string_error() );
  }

  return true;
}

bool JobControllerProxy::cancel( int condorid, const char *logfile, bool force )
{
  CondorRemoveRequest   request( condorid, this->jcp_source, force );

  if( logfile ) request.set_logfile( string(logfile) );

  try {
    utilities::FileListLock   lock( this->jcp_mutex );
    this->jcp_queue.push_back( request );
  }
  catch( utilities::FileContainerError &error ) {
    throw CannotExecute( error.string_error() );
  }

  return true;
}

size_t JobControllerProxy::queue_size( void )
{
  size_t                      size;
  utilities::FileListLock     lock( this->jcp_mutex );

  try { size = this->jcp_queue.size(); }
  catch( utilities::FileContainerError &error ) {
    throw CannotExecute( error.string_error() );
  }

  return size;
}

}; // namespace jobcontrol

} JOBCONTROL_NAMESPACE_END;
