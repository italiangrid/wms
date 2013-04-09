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

#include "iceCommandLeaseUpdater.h"
#include "CreamProxyMethod.h"
#include "DNProxyManager.h"
#include "Lease_manager.h"
#include "iceDb/GetAllJobs.h"
#include "iceDb/Transaction.h"
#include "iceDb/GetJobByGid.h"
#include "iceDb/GetLeaseByID.h"
#include "iceDb/RemoveJobByGid.h"
#include "iceDb/UpdateJobByGid.h"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/JobFilterWrapper.h"
#include "glite/ce/cream-client-api-c/ResultWrapper.h"
#include "glite/ce/cream-client-api-c/JobIdWrapper.h"
#include "glite/ce/cream-client-api-c/VOMSWrapper.h"

#include "iceLBLogger.h"
#include "iceLBEvent.h"
#include "iceConfManager.h"
#include "iceUtils.h"
#include "creamJob.h"
#include "ice-core.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"

#include <set>
#include <vector>
#include "boost/functional.hpp"
#include "boost/scoped_ptr.hpp"

namespace api_util  = glite::ce::cream_client_api::util;
namespace cream_api = glite::ce::cream_client_api;
namespace cream_exceptions = glite::ce::cream_client_api::cream_exceptions;
namespace soap_proxy = glite::ce::cream_client_api::soap_proxy;

using namespace glite::wms::ice::util;
using namespace std;

//________________________________________________________________________
bool iceCommandLeaseUpdater::job_can_be_removed( const CreamJob& job ) 
  const throw()
{
  if ( !job.get_lease_id().empty() ) {
    
    bool found = false;
    Lease_manager::Lease_t leaseInfo( "", "", time(0), "" );
    {
      db::GetLeaseByID getter( job.get_lease_id(), "iceCommandLeaseUpdater::job_can_be_removed" );
      db::Transaction tnx(false, false);
      tnx.execute( &getter );
      found = getter.found();
      if(found)
	leaseInfo = getter.get_lease();
    }
    
    //Lease_manager::const_iterator it = m_lease_manager->find( job.get_lease_id() );
    
    //
    // A job which refers to a non-existing lease ID can be removed
    //
    if ( !found /*it == m_lease_manager->end()*/ )
      return true;
    
    //
    // A job which refers to an existing lease ID, and such that
    // the lease ID refers to an expired lease, can be removed
    // from the cache.
    //
    //        if ( it != m_lease_manager->end() && it->m_expiration_time < time(0) )
    if( found && leaseInfo.m_expiration_time < time(0) )
      return true;
  }
  
  // 
  // A job which is active, can be purged or has no lease cannot be
  // removed from the cache by the lease updater thread.
  //
  // if ( job.is_active() || job.can_be_purged() || job.get_lease_id().empty() )
  // return false; // this job cannot be removed here
  
  return false;
}

//________________________________________________________________________
bool iceCommandLeaseUpdater::lease_can_be_renewed( const CreamJob& job ) const throw() 
{
    //
    // We do not update leases for the following jobs:
    // 
    // 1. Jobs whose CREAM job id is empty. Most likely, these are jobs
    // being concurrently submitted by another ICE thread, so they should
    // not be touched until submission completes.
    //
    // 2. Jobs which are not active (that is, jobs which reached a
    // terminal state).
    //
    // 3. Jobs which can be purged (similar to case 2.) FIXME: is this
    // really necessary?
    //
    if ( job.get_complete_cream_jobid().empty() || !job.is_active() || job.can_be_purged() )
        return false;
    
    bool found = false;
    Lease_manager::Lease_t leaseInfo( "", "", time(0), "" );
    {
      db::GetLeaseByID getter( job.get_lease_id(), "iceCommandLeaseUpdater::lease_can_be_renewed" );
      db::Transaction tnx(false, false);
      tnx.execute( &getter );
      found = getter.found();
      if(found)
	leaseInfo = getter.get_lease();
    }

    //Lease_manager::const_iterator it = m_lease_manager->find( job.get_lease_id() );
    
//     if ( it != m_lease_manager->end() && it->m_expiration_time > time(0)
//          && it->m_expiration_time - time(0) < m_frequency*2 ) 

//         return true; 

    if( found && (leaseInfo.m_expiration_time > time(0)) && (leaseInfo.m_expiration_time - time(0) < m_frequency*2) )
      return true;

    return false;    
}

//____________________________________________________________________________
iceCommandLeaseUpdater::iceCommandLeaseUpdater( bool only_update ) throw() : 
    iceAbsCommand( "iceCommandLeaseUpdater", "" ),
    m_log_dev( api_util::creamApiLogger::instance()->getLogger() ),
    m_lb_logger( iceLBLogger::instance() ),
    m_frequency( iceConfManager::getInstance()->getConfiguration()->ice()->lease_update_frequency() ),
    m_only_update( only_update ),
    m_lease_manager( Lease_manager::instance() )
{
    
}

//____________________________________________________________________________
void iceCommandLeaseUpdater::execute( const std::string& tid ) throw()
{
  m_thread_id = tid;

    static const char* method_name = "iceCommandLeaseUpdater::execute() - ";

    set< string > lease_to_renew; //< Set of lease IDs to renew
    list< string > jobs_to_remove; //< List of Grid job IDs which have the lease expired, and thus must be removed from the job cache

    { // acquire lock on the job cache
      //        boost::recursive_mutex::scoped_lock M( jobCache::mutex );
      //      boost::recursive_mutex::scoped_lock M( CreamJob::globalICEMutex );

      list< CreamJob > allJobs;
      {
	db::GetAllJobs getter(&allJobs, 0,0, "iceCommandLeaseUpdater::execute");
	db::Transaction tnx(false, false);
	//tnx.begin( );
	tnx.execute( &getter );
      }

      for ( list< CreamJob >::const_iterator it = allJobs.begin();
	    it != allJobs.end(); ++it ) {

            // WATNING: There exists an interval in which both
            // lease_can_be_renewed( x ) and job_can_be_removed( x )
            // both hold for the same iterator x.
            if ( lease_can_be_renewed( *it ) ) {                
                lease_to_renew.insert( it->get_lease_id() );                
            } else {
                if ( !m_only_update && job_can_be_removed( *it ) ) {
                    jobs_to_remove.push_back( it->get_grid_jobid() );
                }
            }
        }
    } // releases lock on the job cache  

    //
    // Renew leases which are going to expire
    //
    for ( set< string >::const_iterator it = lease_to_renew.begin();
          it != lease_to_renew.end(); ++it ) 
      {
	
        CREAM_SAFE_LOG(m_log_dev->infoStream() << method_name
                       << "Will renew lease ID " << *it
                       );
        
        // Get the Lease_t object from the lease manager cache
        try {
	  time_t new_lease = m_lease_manager->renew_lease( *it );
        } catch( const std::exception& ex ) {
	  CREAM_SAFE_LOG( m_log_dev->infoStream() << method_name
			  << "Lease update for lease ID " << *it << " failed. " 
			  << "Exception is " << ex.what()
			  << ". This lease will be removed after expiration."
			  );
	  // Nothing to do. The next iteration will take care of
	  // non existing leases, or expired leases
        } catch( ... ) {
	  CREAM_SAFE_LOG( m_log_dev->infoStream() << method_name
			  << "Lease update for lease ID " << *it << " failed " 
			  << "due to an unknown exception. "
			  << "This lease will be removed after expiration."
			  );
        }        
      }
    

    for( list<string>::const_iterator it = jobs_to_remove.begin();
          jobs_to_remove.end() != it; ++it ) {

      //        jobCache::iterator job_it( m_cache->lookupByGridJobID( *it ) );
      CreamJob the_job;
      {
	db::GetJobByGid getter( *it, "iceCommandLeaseUpdater::execute" );
	db::Transaction tnx(false, false);
	//tnx.begin( );
	tnx.execute( &getter );
	if( !getter.found() )
	  continue;
	the_job = getter.get_job();
      }

//         if ( m_cache->end() == job_it ) 
//              continue; // something wrong happened, skip this job

        //CreamJob the_job( *job_it );

        // 
        // Remove lease-expired jobs from the job cache
        //
        CREAM_SAFE_LOG(m_log_dev->warnStream() << method_name
                       << "Removing job " << the_job.describe() 
                       << " with lease id " << the_job.get_lease_id() 
                       << " because the lease id does not exist or is expired"
                       );
    
        string proxy = DNProxyManager::getInstance()->getAnyBetterProxyByDN( the_job.get_user_dn() ).get<0>() ;
        
        try {
            soap_proxy::VOMSWrapper V( the_job.get_user_proxyfile() );
            if( !V.IsValid( ) ) {
                throw soap_proxy::auth_ex( V.getErrorMessage() );
            }
            
            vector< soap_proxy::JobIdWrapper > job_vec;
            job_vec.push_back( soap_proxy::JobIdWrapper(the_job.get_cream_jobid(), 
                                                        the_job.get_cream_address(), 
                                                        vector<soap_proxy::JobPropertyWrapper>())
                               );
            
            
            soap_proxy::JobFilterWrapper req( job_vec, vector<string>(), -1, -1, "", "");
            soap_proxy::ResultWrapper res;
            
            CreamProxy_Cancel( the_job.get_cream_address(), proxy, &req, &res ).execute( 3 );

        } catch(...) {            
            // We ignore any cancellation error here            
        }
        
        the_job.set_failure_reason( "Lease expired" );
	{
	  list<pair<string, string> > params;
	  params.push_back( make_pair("failure_reson", "Lease expired" ) );
	  db::UpdateJobByGid updater( the_job.get_grid_jobid(), params, "iceCommandLeaseUpdater::execute" );
	  db::Transaction tnx(false, false);
	  tnx.execute( &updater );
	}
        iceLBLogger::instance()->logEvent( new job_done_failed_event( the_job ) );
        glite::wms::ice::Ice::instance()->resubmit_job( &the_job, "Lease expired" );
        
	{
	  db::RemoveJobByGid remover( the_job.get_grid_jobid(), "iceCommandLeaseUpdater::execute" );
	  db::Transaction tnx(false, false);
	  tnx.execute( &remover );
	}
    }
}
