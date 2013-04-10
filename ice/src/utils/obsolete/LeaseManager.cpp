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


#include "Lease_manager.h"
#include "CreamProxyMethod.h"
#include "IceConfManager.h"
#include "iceUtils.h"
#include "DNProxyManager.h"
#include "db/GetAllJobs.h"
#include "db/Transaction.h"
#include "db/CreateLease.h"
#include "db/GetLease.h"
#include "db/GetLeaseByID.h"
#include "db/RemoveLease.h"

#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/CreamProxyFactory.h"
#include "glite/ce/cream-client-api-c/certUtil.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"

#include <ctime>
#include <utility> // defines std::pair<>
#include <set>

namespace cream_api = glite::ce::cream_client_api::soap_proxy;
namespace api_util = glite::ce::cream_client_api::util;
namespace cert_util = glite::ce::cream_client_api::certUtil;
using namespace glite::wms::ice::util;
using namespace std;

Lease_manager* Lease_manager::s_instance = 0;
boost::recursive_mutex Lease_manager::m_mutex;

//////////////////////////////////////////////////////////////////////////////

lease_exception::lease_exception( const char* reason ) :
    m_reason( reason ? reason : "" )
{

}

lease_exception::lease_exception( const string& reason ) :
    m_reason( reason )
{

}

const char* lease_exception::what( void ) const throw()
{
    return m_reason.c_str();
}

//////////////////////////////////////////////////////////////////////////////
Lease_manager::Lease_manager( ) :
    m_log_dev( api_util::creamApiLogger::instance()->getLogger()),
    m_operation_count( 0 ),
    m_operation_count_max( 20 ), // FIXME: hardcoded default
    m_host_dn( "UNKNOWN_ICE_DN" ),
    m_lease_delta_time( 60*60 ),
    m_lease_update_frequency( 30*60 )
{
    glite::wms::common::configuration::Configuration* conf = 0;
    static char* method_name = "Lease_manager::Lease_manager() - ";

    try {
        conf = IceConfManager::instance()->getConfiguration(); // can raise ConfigurationManager_ex
        m_lease_delta_time = conf->ice()->lease_delta_time();
        m_lease_update_frequency = conf->ice()->lease_update_frequency();
        m_host_dn = cert_util::getDN( conf->ice()->ice_host_cert() ); // can raise auth_ex 
    } catch( ConfigurationManager_ex& ex ) {
        CREAM_SAFE_LOG( m_log_dev->errorStream()
                        << method_name
                        << "Error while accessing ICE configuration file. "
                        << "Error is: \"" << ex.what() << "\". "
                        << "Giving up, but this may cause troubles."
                         );
    } catch( const glite::ce::cream_client_api::soap_proxy::auth_ex& ex ) {
        CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
                        << "Could not get DN for the local host. " 
                        << "Error is \"" << ex.what() << "\". "
                        << "Using hardcoded default of \"UNKNOWN_ICE_DN\""
                         );
        m_host_dn = "UNKNOWN_ICE_DN";
    }    
    
    try {
      //init();
    } catch( ... ) {
        CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
                        << "Error during initialization. "
                        << "This error will be ignored."
                         );
    }
}

//______________________________________________________________________________
Lease_manager* Lease_manager::instance( ) 
{
    boost::recursive_mutex::scoped_lock L( m_mutex );
    if ( 0 == s_instance ) 
        s_instance = new Lease_manager( );
    return s_instance;
}

//______________________________________________________________________________
// void Lease_manager::init( void )
// {
//     static const char* method_name = "Lease_manager::init() - ";
//     set<string> failed_lease_ids; // Set of lease IDs which do not
//                                   // exist (CREAM reported an error
//                                   // checking for them). This set is
//                                   // used to avoid asking the same
//                                   // non-existant lease ID over and
//                                   // over for each job it is contained
//                                   // into.

//     // Scan the job cache 
//     //    jobCache* the_cache( jobCache::getInstance() );
//     //    boost::recursive_mutex::scoped_lock L( jobCache::mutex );
//     boost::recursive_mutex::scoped_lock L( CreamJob::globalICEMutex );

//     list< CreamJob > allJobs;

//     //    for ( jobCache::iterator it = the_cache->begin(); the_cache->end() != it; ++it ) {

//     {
//       db::GetAllJobs getter;
//       db::Transaction tnx;
//       tnx.execute( &getter );
//       allJobs = getter.get_jobs();
//     }

//     for ( list< CreamJob >::const_iterator it = allJobs.begin(); allJobs.end() != it; ++it ) {

//         string lease_id( it->get_lease_id() );
//         // If a lease id: 1) is empty, OR 2) has already been inserted
//         // into the lease cache, OR 3) has already been inserted into
//         // the set of non existend lease IDs, then we skip this entry
//         if ( lease_id.empty() || end() != find( lease_id ) || failed_lease_ids.end() != failed_lease_ids.find( lease_id ) )
//             continue; 

//         // Utility variables
//         const string cert_file( it->getUserProxyCertificate() );
//         const string cream_url( it->getCreamURL() );
//         const string user_DN( it->getUserDN() );

//         CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
//                         << "Checking lease with lease ID \""
//                         << lease_id << "\" CREAM URL " << cream_url
//                         << " user DN " << user_DN << " user cert file "
//                         << cert_file  );
        
//         pair< string, time_t > result;
//         try {
//             CreamProxy_LeaseInfo( cream_url, cert_file, lease_id, &result ).execute( 3 );        
//         } catch( const exception& ex ) {
//             CREAM_SAFE_LOG( m_log_dev->warnStream() << method_name
//                             << "Got exception \"" << ex.what() << "\" while "
//                             << "checking lease with lease ID \""
//                             << lease_id << "\" CREAM URL " << cream_url
//                             << " user DN " << user_DN << " user cert file "
//                             << cert_file << ". Will ignore this lease ID."
//                              );
//             failed_lease_ids.insert( lease_id );
//             continue;
//         } catch( ... ) {
//             CREAM_SAFE_LOG( m_log_dev->warnStream() << method_name
//                             << "Got unknown exception while "
//                             << "checking lease with lease ID \""
//                             << lease_id << "\" CREAM URL " << cream_url
//                             << " user DN " << user_DN << " user cert file "
//                             << cert_file << ". Will ignore this lease ID."
//                              );
//             failed_lease_ids.insert( lease_id );
//             continue;
//         }
//         time_t expiration_time = result.second;
//         CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
//                         << "Lease ID \"" << lease_id 
//                         << " has expiration time "
//                         << time_t_to_string( expiration_time )
//                          );
        
//         // Insert the lease ID into the lease set
//         m_lease_set.insert( Lease_t( user_DN, cream_url, expiration_time, lease_id ) );        
//     }
// }

//______________________________________________________________________________
string Lease_manager::make_lease( const CreamJob& job, bool force ) 
  throw( lease_exception& )
{
    boost::recursive_mutex::scoped_lock L( m_mutex );

    static char* method_name = "Lease_manager::make_lease() - ";
    string lease_id; // lease ID to return as a result

    // Utility variables
    const string cert_file( job.get_user_proxy_certificate() );
    const string cream_url( job.get_creamurl() );
    const string user_DN( job.get_user_dn() );

    // Check whether the cache should be cleaned up
    if ( ++m_operation_count > m_operation_count_max ) { // FIXME: Hardcoded default
        purge_old_lease_ids( );
        m_operation_count = 0;
    }

    // Lookup the (user_DN,cream_url) pair into the set
    //typedef t_lease_set::nth_index<0>::type t_lease_by_key;
    //t_lease_by_key& lease_by_key_view( m_lease_set.get<0>() );

    //    t_lease_by_key::iterator it = lease_by_key_view.find( boost::make_tuple(user_DN, cream_url));

    bool found = false;
    Lease_t leaseInfo("", "", time(0), "");
    {
      db::GetLease getter(user_DN, cream_url, "Lease_manager::make_lease");
      db::Transaction tnx(false, false);
      tnx.execute( &getter );
      found = getter.found();
      if( found )
	leaseInfo = getter.get_lease();
    }

//     if ( lease_by_key_view.end() != it &&
//          ( force || it->m_expiration_time < time(0) ) ) {
//         // Remove the lease if it is expired, or if the user requested
//         // the creation of a new lease
//         lease_by_key_view.erase( it );
//         it = lease_by_key_view.end();
//     }

    if( found && ( force || leaseInfo.m_expiration_time < time(0) ) ) {
      {
	db::RemoveLease remover( user_DN, cream_url,"Lease_manager::make_lease" );
	db::Transaction tnx(false, false);
	tnx.execute( &remover );
      }
      found = false;
    }

    //    if ( lease_by_key_view.end() == it ) {
    if (!found) {
        // lease id not found (or force). Creates a new lease ID
        time_t expiration_time = time(0) + m_lease_delta_time;

        lease_id = 
            canonizeString( m_host_dn ).
            append("--").
            append(canonizeString( user_DN )).
            append("--").
            append(canonizeString( cream_url ));

        CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
                        << "Creating new lease with lease ID \""
                        << lease_id << "\" CREAM URL " << cream_url
                        << " user DN " << user_DN << " user cert file "
                        << cert_file << " expiration time "
                        << time_t_to_string( expiration_time )
                         );
        
        pair< string, time_t > 
            lease_in = make_pair( lease_id, expiration_time ),
            lease_out;

        try {
	    CreamProxy_Lease( cream_url, cert_file, lease_in, &lease_out ).execute( 3 );
        } catch( const exception& ex ) {
            // Lease operation failed
            string error_msg( boost::str( boost::format( "Lease operation FAILED for lease ID %1% CREAM URL %2% user DN %3% expiration date %4%. Error is %5%") % lease_id % cream_url % user_DN % time_t_to_string( expiration_time ) % ex.what() ) );

            CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
                            << error_msg 
                            );
            // Returns an empty string
            throw lease_exception( error_msg );
        }     
        // lease operation succesful. Prints the output
        lease_id = lease_out.first; // FIXME: is that correct???
        expiration_time = lease_out.second;
        CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
                        << "Lease operation SUCCESFUL. Returned "
                        << "lease ID \"" << lease_id << "\" CREAM URL "
                        << cream_url << " user DN " << user_DN 
                        << " new expiration time "
                        << time_t_to_string( expiration_time )
                         );        

        // Inserts the lease ID into the lease set
	//        m_lease_set.insert( Lease_t( user_DN, cream_url, expiration_time, lease_id ) );
	{
	  db::CreateLease creator( user_DN, cream_url, expiration_time, lease_id, "Lease_manager::make_lease" );
	  db::Transaction tnx(false, false);
	  tnx.execute( &creator );
	}

        // Inserts into the front of the list; this is guaranteed to
        // be a new element, as it has not been found in this "if"
        // branch
        //
        // FIXME: not needed at all?
        //
        // lease_by_seq.push_front( Lease_t( user_DN, cream_url, expiration_time, lease_id ) );
    } else {
        // Lease id FOUND. If it is expiring, renew before returning
        // it to the caller.
      //          lease_id = it->m_lease_id;
      lease_id = leaseInfo.m_lease_id;
  
      //        time_t expiration_time = it->m_expiration_time;
      time_t expiration_time = leaseInfo.m_expiration_time;

//         if ( it->m_expiration_time < time(0) + m_lease_update_frequency ) {
//             // renew the lease
//             expiration_time = renew_lease( lease_id );
//         }

      if( leaseInfo.m_expiration_time < time(0) + m_lease_update_frequency ) {
	// renew the lease
	expiration_time = renew_lease( lease_id );
      }
	
        CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
                        << "Using existing lease ID \"" << lease_id
                        << "\" for CREAM URL " << cream_url
                        << " user DN " << user_DN << " expiration time "
                        << time_t_to_string( expiration_time )
                         );
    }
    return lease_id;
}

//______________________________________________________________________________
time_t Lease_manager::renew_lease( const string& lease_id ) 
  throw( lease_exception& )
{
    boost::recursive_mutex::scoped_lock L( m_mutex );

    static char* method_name = "Lease_manager::renew_lease() - ";

//     typedef t_lease_set::index<idx_lease_id>::type t_lease_by_id;
//     t_lease_by_id& lease_by_id_view( m_lease_set.get<idx_lease_id>() );

//     t_lease_by_id::iterator it = lease_by_id_view.find( lease_id );

    bool found = false;
    Lease_t leaseInfo("", "", time(0), "");
    {
      db::GetLeaseByID getter( lease_id,"Lease_manager::renew_lease" );
      db::Transaction tnx(false, false);
      tnx.execute( &getter );
      found = getter.found();
      if(found)
	leaseInfo = getter.get_lease();
    }
    
    //    if ( lease_by_id_view.end() == it ) {
    if( !found ) {
        string error_msg( boost::str( boost::format( "Cannot renew lease with lease ID %1% because it can not be found in the lease cache" ) % lease_id ) );

        CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
                        << error_msg
                        );
        throw lease_exception( error_msg );
    }

    string cert_file = DNProxyManager::getInstance()->getAnyBetterProxyByDN( leaseInfo.m_user_dn ).get<0>();

    if ( cert_file.empty() ) {
        string error_msg( boost::str( boost::format( "Cannot renew lease with lease ID \"%1%\" because ICE cannot retrieve a proxy cert file for user DN \"%2%\"") % lease_id % leaseInfo.m_user_dn ) );

        CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
                        << error_msg
                        );
        throw lease_exception( error_msg );
    }
    
    //struct Lease_t entry = *it; // this is the (old) entry
    time_t expiration_time = time(0) + m_lease_delta_time;

    pair< string, time_t > 
        lease_in = make_pair( leaseInfo.m_lease_id, expiration_time ),
        lease_out;
    
    try {
        CreamProxy_Lease( leaseInfo.m_cream_url, cert_file, lease_in, &lease_out ).execute( 3 );
    } catch( const std::exception& ex ) {
        // Lease operation failed
        string error_msg( boost::str( boost::format( "Lease renew operation FAILED for lease ID \"%1%\" user DN %2% CREAM URL %3% desired expiration date %4%. Exception is %5%") % lease_id % leaseInfo.m_user_dn % leaseInfo.m_cream_url % time_t_to_string( expiration_time ) % ex.what() ) );
        CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
                        << error_msg );
        throw lease_exception( error_msg );                          
    } catch( ... ) {
        // Lease operation failed due to unknown exception
        string error_msg( boost::str( boost::format( "Lease renew operation FAILED for lease ID \"%1%\" user DN %2% CREAM URL %3% desired expiration date %4%. Unknown exception caught") % lease_id % leaseInfo.m_user_dn % leaseInfo.m_cream_url % time_t_to_string( expiration_time ) ) );
        CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
                        << error_msg );
        throw lease_exception( error_msg );                          
    }         
    CREAM_SAFE_LOG( m_log_dev->infoStream() << method_name
                    << "Succesfully renewed lease for "
                    << "lease ID \"" << lease_id << "\" user DN "
                    << leaseInfo.m_user_dn << " CREAM URL "
                    << leaseInfo.m_cream_url << " new expiration time "
                    << time_t_to_string( lease_out.second )
                     );
    leaseInfo.m_expiration_time = lease_out.second;
    //lease_by_id_view.replace( it, leaseInfo );
    {
      db::CreateLease creator( leaseInfo.m_user_dn, leaseInfo.m_cream_url, leaseInfo.m_expiration_time, leaseInfo.m_lease_id, "Lease_manager::renew_lease" );
      db::Transaction tnx(false, false);
      tnx.execute( &creator );
    }
    return leaseInfo.m_expiration_time;
}

//______________________________________________________________________________
void Lease_manager::purge_old_lease_ids( void )
{
  //    static char* method_name = "Lease_manager::purge_old_lease_ids() - ";

    //    typedef t_lease_set::nth_index<1>::type t_lease_by_expiration;
    //    t_lease_by_expiration& lease_time_view( m_lease_set.get<1>() );

    //    t_lease_by_expiration::iterator it_end = lease_time_view.lower_bound( time(0) );
    //    size_t size_before = lease_time_view.size();    
    //    lease_time_view.erase( lease_time_view.begin(), it_end );
    //    size_t size_after = lease_time_view.size();

//     if ( size_before != size_after ) {
//         CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
//                         << "Purged " << size_before - size_after
//                         << " elements from the lease cache"
//                         );
//     }
}

// Lease_manager::const_iterator Lease_manager::begin() const
// {
//     return m_lease_set.get< idx_sequence >().begin();
// }

// Lease_manager::const_iterator Lease_manager::end() const 
// {
//     return m_lease_set.get< idx_sequence >().end();
// }

// Lease_manager::const_iterator Lease_manager::find( const string& lease_id ) const
// {
//     if ( lease_id.empty() ) 
//         return end();

//     //static char* method_name = "Lease_manager::find() - ";

//     typedef t_lease_set::index<idx_lease_id>::type t_lease_by_id;
//     const t_lease_by_id& lease_by_id_view( m_lease_set.get<idx_lease_id>() );

//     t_lease_by_id::const_iterator it = lease_by_id_view.find( lease_id );

//     return m_lease_set.project< idx_sequence >( it );
// }
