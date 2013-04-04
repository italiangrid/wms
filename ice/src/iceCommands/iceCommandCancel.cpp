/* LICENSE:
Copyright (c) Members of the EGEE Collaboration. 2010. 
See http://www.eu-egee.org/partners/ for details on the copyright
holders.  

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

   http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. 
See the License for the specific language governing permissions and 
limitations under the License.

END LICENSE */
/**
 *
 * ICE  Headers
 *
 */
#include "iceCommandCancel.h"
#include "ice/IceCore.h"
#include "iceUtils/iceLBLogger.h"
#include "iceUtils/IceLBEvent.h"
#include "iceUtils/CreamProxyMethod.h"
#include "Request_source_purger.h"
#include "iceUtils/Request.h"
#include "iceUtils/DNProxyManager.h"
#include "iceDb/Transaction.h"
#include "iceDb/CreateJob.h"
#include "iceDb/GetJobByGid.h"
#include "iceDb/UpdateJob.h"
/**
 *
 * Cream Client API Headers
 *
 */
#include "glite/ce/cream-client-api-c/JobFilterWrapper.h"
#include "glite/ce/cream-client-api-c/ResultWrapper.h"
#include "glite/ce/cream-client-api-c/JobIdWrapper.h"
#include "glite/ce/cream-client-api-c/VOMSWrapper.h"
#include "glite/wms/common/utilities/scope_guard.h"

#include <boost/algorithm/string.hpp>

namespace cream_api = glite::ce::cream_client_api::soap_proxy;
namespace cream_ex  = glite::ce::cream_client_api::cream_exceptions;
namespace wms_utils = glite::wms::common::utilities;
using namespace std;
using namespace glite::wms::ice;

//
//
//______________________________________________________________________________
iceCommandCancel::iceCommandCancel( util::Request* request ) 
  throw(util::ClassadSyntax_ex&, util::JobRequest_ex&) :
  IceAbstractCommand( "iceCommandCancel", "" ),
  m_log_dev(glite::ce::cream_client_api::util::creamApiLogger::instance()->getLogger()),
  m_lb_logger( util::iceLBLogger::instance() ),
  m_request( request )
{

  
  string commandStr;
  string protocolStr;

  {//  ClassAd-mutex protected region
    boost::recursive_mutex::scoped_lock M_classad( glite::wms::ice::util::CreamJob::s_classad_mutex );
    
    classad::ClassAdParser parser;
    classad::ClassAd *rootAD = parser.ParseClassAd( request->to_string() );
    
    if (!rootAD)
      throw util::ClassadSyntax_ex("ClassAd parser returned a NULL pointer parsing entire request");
    
    boost::scoped_ptr< classad::ClassAd > classad_safe_ptr( rootAD );
    
    // Parse the "version" attribute
    if ( !classad_safe_ptr->EvaluateAttrString( "protocol", protocolStr ) ) {
      throw util::JobRequest_ex("attribute \"protocol\" not found or is not a string");
    }
    // Check if the version is exactly 1.0.0
    if ( protocolStr.compare("1.0.0") ) {
      throw util::JobRequest_ex("Wrong \"Protocol\" for jobCancel: expected 1.0.0, got " + protocolStr );
    }
    
    classad::ClassAd *argumentsAD = 0; // no need to free this
    // Parse the "arguments" attribute
    if ( !classad_safe_ptr->EvaluateAttrClassAd( "arguments", argumentsAD ) ) {
      throw util::JobRequest_ex("attribute \"arguments\" not found or is not a classad");
    }
    
    // Look for "id" attribute inside "Arguments"
    if ( !argumentsAD->EvaluateAttrString( "jobid", m_gridJobId ) ) {
      throw util::JobRequest_ex( "attribute \"jobid\" inside \"arguments\" not found, or is not a string" );
    }
    
    if ( !argumentsAD->EvaluateAttrString( "SequenceCode", m_seq_code ) ) {
      throw util::JobRequest_ex( "attribute \"SequenceCode\" inside \"arguments\" not found, or is not a string" );
    }
    
    // Look for "lb_sequence_code" attribute inside "Arguments"
    if ( !argumentsAD->EvaluateAttrString( "sequencecode", m_sequence_code ) ) {
      // FIXME: This should be an error to throw. For now, we try anyway...
      CREAM_SAFE_LOG( m_log_dev->warnStream()
		      << "iceCommandCancel::execute() - Cancel request does not have a "
		      << "\"sequencecode\" attribute. "
		      << "Fine for now, should not happen in the future"
		      
		      );
    } else {
      boost::trim_if(m_sequence_code, boost::is_any_of("\""));        
    }
  }// end Classad-mutex protected region
 
}

//
//
//______________________________________________________________________________
void iceCommandCancel::execute( const std::string& tid ) throw ( iceCommandFatal_ex&, iceCommandTransient_ex& )
{
#ifdef ICE_PROFILE
  util::ice_timer timer("iceCommandCancel::execute");
#endif

    m_thread_id = tid;

    CREAM_SAFE_LOG( 
                   m_log_dev->infoStream()
                   << "iceCommandCancel::execute() - This request is a Cancel..."
                   
                   );

    Request_source_purger r( m_request );
    wms_utils::scope_guard remove_request_guard( r );
    db::GetJobByGid get( m_gridJobId, "iceCommandCancel::execute" );
    {
      db::Transaction tnx(false, false);
      tnx.execute( &get );
      
      if( !get.found() ) {
	CREAM_SAFE_LOG( 
		       m_log_dev->errorStream()
		       << "iceCommandCancel::execute() - Cancel operation cannot locate jobid=["
		       << m_gridJobId 
		       << "] in the database. Giving up"
		       );
	
	
	throw iceCommandFatal_ex( string("ICE cannot cancel job with grid job id=[") 
				  + m_gridJobId 
				  + string("], as the job does not appear to exist") );
	
      }
    }

    // According to the following mail, the sequence from a cancel
    // request should NOT be replaced. Hence, this code has been
    // commented out and will be removed
    //
    // Date: Wed, 6 Dec 2006 15:18:05 +0100 From: Zdenek Salvet <salvet@ics.muni.cz> To: Local EGEE JRA1 group <egee-jra1@lindir.ics.muni.cz> Cc: Milos Mulac <mulac@civ.zcu.cz>, Alessio Gianelle <gianelle@pd.infn.it>, Massimo Sgaravatto - INFN Padova <massimo.sgaravatto@pd.infn.it> Subject: Re: [Egee-jra1] Problem with LB & ICE On Wed, Dec 06, 2006 at 03:01:54PM +0100, Moreno Marzolla wrote:

    // I don't think the problem is caused by this race, LB and its
    // event sequence codes has been designed with this in mind.  It
    // appears to me that ICE(LogMonitor) replaces its stored LB
    // sequence code for the running job with the one coming in cancel
    // request. Then, ReallyRunning appears to be logically following
    // cancellation. It should not do that.

    util::CreamJob theJob( get.get_job( ) );

    theJob.set_cancel_sequence_code( m_seq_code );
    {
      glite::wms::ice::db::UpdateJob updater( theJob, "iceCommandCancel::execute" );
      glite::wms::ice::db::Transaction tnx(false, false);
      tnx.execute( &updater );
    }
    
    // Log cancel request event
    theJob = m_lb_logger->logEvent( new util::cream_cancel_request_event( theJob, string("Cancel request issued by user") ), true, true );
    
    string jobdesc( theJob.describe()  );

    CREAM_SAFE_LOG(    
                   m_log_dev->infoStream()
                   << "iceCommandCancel::execute() - Sending cancellation request to ["
                   << theJob.cream_address() << "] for GridJobID ["
	 	   << theJob.grid_jobid( ) << "]"
                   );

    /**
     * Getting betterproxy for current job. Note that this betterproxy should be there
     * becase this procedure already checked that this job is in the cache and then
     * it has been already submitted (that implies that its proxy has been put in the 
     * DNProxyManager's cache of betterproxies).
     */
    string betterproxy;

    betterproxy = util::DNProxyManager::getInstance()->getAnyBetterProxyByDN( theJob.user_dn() ).get<0>();

    if( betterproxy.empty() ) {
      CREAM_SAFE_LOG( m_log_dev->warnStream()
		      << "iceCommandCancel::execute() - DNProxyManager returned an empty string for BetterProxy of user DN ["
		      << "] for job ["
		      << jobdesc
		      << "]. Using the Job's proxy." 
		      );

      betterproxy = theJob.user_proxyfile();
    }

    try {
      cream_api::VOMSWrapper V( betterproxy,  !::getenv("GLITE_WMS_ICE_DISABLE_ACVER") );
      if( !V.IsValid( ) ) {
        throw cream_api::auth_ex( V.getErrorMessage() );
      }
      
      theJob.set_failure_reason( "Aborted by user" );
      //list< pair<string, string> > params;
      //params.push_back( make_pair( util::CreamJob::failure_reason_field(), "Aborted by user" ));
      {
	db::Transaction tnx( false, false );
	db::UpdateJob updater(theJob, "iceCommandCancel::cancel" );
	tnx.execute( &updater );
      }
      theJob.reset_change_flags( );
      vector<cream_api::JobIdWrapper> toCancel;
      toCancel.push_back( cream_api::JobIdWrapper(theJob.cream_jobid(), 
						  theJob.cream_address(), 
						  std::vector<cream_api::JobPropertyWrapper>())
			  );
      
      cream_api::JobFilterWrapper req( toCancel, vector<string>(), -1, -1, "", "");
      cream_api::ResultWrapper res;
      
      util::CreamProxy_Cancel( theJob.cream_address(), betterproxy, &req, &res ).execute( 3 );
      
      list< pair<cream_api::JobIdWrapper, string> > tmp;
      
      res.getNotExistingJobs( tmp );
      res.getNotMatchingStatusJobs( tmp );
      res.getNotMatchingDateJobs( tmp );
      res.getNotMatchingProxyDelegationIdJobs( tmp );
      res.getNotMatchingLeaseIdJobs( tmp );
      
      // We tried to cancel only one job.
      // Then if the operation went wrong
      // tmp contains only one element, the first one!
      if( !tmp.empty() )
	{
	  
	  // let's get only the first element of the array
	  // because we Cancel one job by one
	  // it is safe to dereference the .begin() because 
	  // the list is not empty
	  pair<cream_api::JobIdWrapper, string> errorJob = *(tmp.begin());
	  
	  string errMex = "Cancellation of the Job [";
	  errMex += jobdesc + "] failed: [";
	  errMex += errorJob.second + "]";
	  
	  CREAM_SAFE_LOG(    
			 m_log_dev->errorStream() << "iceCommandCancel::execute - "
			 << errMex
			 );
	  
	  m_lb_logger->logEvent( new util::cream_cancel_refuse_event( theJob, string("Error: ") + errMex ), true, true );
	  throw iceCommandFatal_ex( errMex );
	}
    } catch(cream_api::auth_ex& ex) {
      m_lb_logger->logEvent( new util::cream_cancel_refuse_event( theJob, string("Authentication Exception: ") + ex.what() ), true, true );
      throw iceCommandFatal_ex( string("auth_ex: ") + ex.what() );
    } catch(cream_api::soap_ex& ex) {
      m_lb_logger->logEvent( new util::cream_cancel_refuse_event( theJob, string("SOAP Exception: ") + ex.what() ), true, true );
      throw iceCommandTransient_ex( string("soap_ex: ") + ex.what() );
    } catch(cream_ex::BaseException& base) {
      m_lb_logger->logEvent( new util::cream_cancel_refuse_event( theJob, string("BaseException: ") + base.what() ), true, true );
      throw iceCommandFatal_ex( string("BaseException: ") + base.what() );
    } catch(cream_ex::InternalException& intern) {
      m_lb_logger->logEvent( new util::cream_cancel_refuse_event( theJob, string("InternalException: ") + intern.what() ), true, true );
      throw iceCommandFatal_ex( string("InternalException: ") + intern.what() );
    } catch( ConnectionTimeoutException& ex) {
      throw iceCommandTransient_ex( string("CREAM Cancel raised a ConnectionTimeoutException ") + ex.what() ) ;
    }
}
