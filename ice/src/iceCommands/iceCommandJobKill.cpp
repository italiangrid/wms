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

// ICE Headers
#include "iceCommandJobKill.h"
#include "CreamProxyMethod.h"
#include "DNProxyManager.h"
#include "iceConfManager.h"
#include "iceLBLogger.h"
#include "iceLBEvent.h"
#include "jobCache.h"
#include "iceUtils.h"


// CE and WMS Headers
#include "glite/ce/cream-client-api-c/certUtil.h"
#include "glite/ce/cream-client-api-c/VOMSWrapper.h"
#include "glite/ce/cream-client-api-c/ResultWrapper.h"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/JobFilterWrapper.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"

// STL C++ Headers
#include <string>
#include <vector>
#include <algorithm>

// Boost Headers
#include <boost/format.hpp>
#include <boost/functional.hpp>

namespace api_util  = glite::ce::cream_client_api::util;
namespace cream_api = glite::ce::cream_client_api;

using namespace glite::wms::ice::util;
using namespace std;

//____________________________________________________________________________
namespace {
    
    bool job_can_be_killed(const CreamJob& J) {

        static const char* method_name = "iceCommandJobKill::job_can_be_killed() - ";
        log4cpp::Category* log_dev = api_util::creamApiLogger::instance()->getLogger();
        int threshold( iceConfManager::getInstance()->getConfiguration()->ice()->job_cancellation_threshold_time() );
        jobCache* cache( jobCache::getInstance() );

        if( J.getCompleteCreamJobID().empty() ) 
            return false;
        
        if( J.is_killed_by_ice() ) {
            CREAM_SAFE_LOG( log_dev->debugStream() << method_name
                            << J.describe()
                            << " is reported as already been killed by ICE. "
                            << "Skipping..."
                            );     
            return false; 
        }

	cream_api::soap_proxy::VOMSWrapper V( J.getUserProxyCertificate() );
 	if ( !V.IsValid( ) ) {
            CREAM_SAFE_LOG( log_dev->errorStream() << method_name
                            << "The user proxy " << J.getUserProxyCertificate()
                            << " for job " << J.describe()
                            << " is not valid. Reason is \""
                            << V.getErrorMessage() << "\". "
                            << "This job will be killed."
                            );
            CreamJob tmp( J ); // trick to discard const
            tmp.set_failure_reason( "User proxy is not valid: " + V.getErrorMessage() );
            cache->put( tmp );
            return true;
 	}

        if ( V.getProxyTimeEnd() < time(0) + threshold ) {
            CREAM_SAFE_LOG( log_dev->warnStream() << method_name
                            << "Proxy [" << J.getUserProxyCertificate()
                            << "] of user [" << J.getUserDN()
                            << "] is expiring. Will kill job " << J.describe()
                            );            
            CreamJob tmp( J ); // trick to discard const
            tmp.set_failure_reason( "User proxy is expiring");
            cache->put( tmp );
            return true;
        }

        return false;
    }

    /**
     * This class is used to log a CANCEL_REQUEST event. It is defined
     * so that it can be used inside a for_each() costruct.  NOTE:
     * this class also sets the "is_killed_by_ice" flag for job j to
     * true.
     */
    class cancel_request_logger {
    public:
        cancel_request_logger( void ) {};

        void operator()( const CreamJob& j ) throw() 
        {
            iceLBLogger* logger( iceLBLogger::instance() );
            jobCache* cache( jobCache::getInstance() );
            boost::recursive_mutex::scoped_lock L( jobCache::mutex );
            jobCache::iterator it=cache->lookupByGridJobID( j.getGridJobID() );
            if ( it != cache->end() ) {
                CreamJob tmp( *it );
                tmp = logger->logEvent( new cream_cancel_request_event( tmp, tmp.get_failure_reason() ) );
                tmp.set_killed_by_ice();
                // tmp.set_failure_reason( m_reason );
                cache->put( tmp );                
            }
        }
    };

    /**
     * This class is used to log a CANCEL_REFUSE event. It is defined
     * so that it can be used inside a for_each() costruct.
     */
    class cancel_refuse_logger {
    protected:
        const string m_reason;
    public:
        cancel_refuse_logger( const string& reason ) : m_reason ( reason ) {};

        void operator()( const CreamJob& j ) throw() 
        {
            iceLBLogger* logger( iceLBLogger::instance() );
            jobCache* cache( jobCache::getInstance() );
            boost::recursive_mutex::scoped_lock L( jobCache::mutex );
            jobCache::iterator it=cache->lookupByGridJobID( j.getGridJobID() );
            if ( it != cache->end() ) {
                CreamJob tmp( *it );
                logger->logEvent( new cream_cancel_refuse_event( tmp, m_reason ) );
            }
        }

        void operator()( const pair<cream_api::soap_proxy::JobIdWrapper, string>& info ) 
            throw() 
        {
            string cream_job_id = info.first.getCreamURL();
            boost::replace_all( cream_job_id, iceConfManager::getInstance()->getConfiguration()->ice()->cream_url_postfix(), "" );        
            cream_job_id += "/" + info.first.getCreamJobID();
            string reason( info.second );
            iceLBLogger* logger( iceLBLogger::instance() );
            jobCache* cache( jobCache::getInstance() );
            boost::recursive_mutex::scoped_lock L( jobCache::mutex );
            jobCache::iterator job_it = cache->lookupByCompleteCreamJobID( cream_job_id );
            if ( job_it != cache->end() ) {            
                CreamJob tmp( *job_it );
                logger->logEvent( new cream_cancel_refuse_event( tmp, reason ) );
            }
        }
    };


    /**
     * This class is used to log an ABORTED event. It is defined so
     * that it can be used inside a for_each() costruct. This class
     * also removes the job from the job cache, after setting the
     * "is_killed_by_ice" flag to trye.
     */
    class aborted_logger {
    protected:
        const string m_reason;
    public:
        aborted_logger( const string& reason ) : m_reason ( reason ) {};

        void operator()( const CreamJob& j ) throw() 
        {
            iceLBLogger* logger( iceLBLogger::instance() );
            jobCache* cache( jobCache::getInstance() );
            boost::recursive_mutex::scoped_lock L( jobCache::mutex );
            jobCache::iterator it=cache->lookupByGridJobID( j.getGridJobID() );
            if ( it != cache->end() ) {
                CreamJob tmp(*it);
                tmp.set_killed_by_ice();
                tmp.set_failure_reason( m_reason );
                logger->logEvent( new job_aborted_event( tmp ) );
                cache->erase( it );
            }
        }
    };


}; // end anonymous namespace

//____________________________________________________________________________
iceCommandJobKill::iceCommandJobKill( ) throw() : 
    iceAbsCommand( "iceCommandJobKill" ),
    m_log_dev( api_util::creamApiLogger::instance()->getLogger()),
    m_threshold_time( iceConfManager::getInstance()->getConfiguration()->ice()->job_cancellation_threshold_time() ),
    m_lb_logger( iceLBLogger::instance() ),
    m_cache( jobCache::getInstance() )
{

}

//____________________________________________________________________________
void iceCommandJobKill::execute() throw()
{
    static const char* method_name = "iceCommandJobKill::execute() - ";

    CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
                    << "Executing new iteration"
                    );            
    boost::recursive_mutex::scoped_lock M( jobCache::mutex );
    map< pair<string, string>, list< CreamJob >, ltstring> jobMap;    

    jobMap_appender appender( jobMap, &job_can_be_killed );
    { 
        boost::recursive_mutex::scoped_lock L( jobCache::mutex );
        for_each( m_cache->begin(), m_cache->end(), appender);
    }
    
    // execute killJob DN-CEMon by DN-CEMon (then, on multiple jobs)
    // on all elements of jobMap (that contains all jobs with expired
    // proxy)
    for_each( jobMap.begin(), jobMap.end(), 
              boost::bind1st( boost::mem_fn( &iceCommandJobKill::killJob ), this ));
    
}

//____________________________________________________________________________
void iceCommandJobKill::killJob( const pair< pair<string, string>, list< CreamJob > >& aList ) throw()
{
    // static const char* method_name = "iceCommandJobKill::killJob() - ";
  string better_proxy( DNProxyManager::_getInstance()->_getAnyBetterProxyByDN( aList.first.first ).get<0>() );
    int query_size( iceConfManager::getInstance()->getConfiguration()->ice()->bulk_query_size() );

    cream_api::soap_proxy::VOMSWrapper V( better_proxy );
    list< CreamJob >::const_iterator it = aList.second.begin();
    list< CreamJob >::const_iterator list_end = aList.second.end();
    if( V.IsValid( ) ) {
        // The proxy is valid. Go on and send a cancel request to all
        // the jobs
        while ( it != list_end ) {        
            list< CreamJob > jobs_to_cancel;
            it = copy_n_elements( it, list_end, query_size, 
                                  back_inserter( jobs_to_cancel ) );
            
            cancel_jobs(better_proxy, aList.first.second, jobs_to_cancel); 
        } 
    } else {
        // The proxy is not valid. We mark this bunch of jobs as
        // aborted, and remove it from the job cache.
        for_each( it, list_end, aborted_logger( "The job has been killed because its proxy is no longer valid" ) );
    }    
}

//____________________________________________________________________________
void iceCommandJobKill::cancel_jobs(const string& better_proxy, 
                                    const string& endpoint, 
				    const list<CreamJob>& jobs) throw()
{
    static const char* method_name = "iceCommandJobKill::cancel_jobs() - ";

    //
    // Logs "cancel requests"
    //
    for_each( jobs.begin(), jobs.end(), cancel_request_logger( ) );

    //
    // Actualy do the cancel operations
    //
    cream_api::soap_proxy::ResultWrapper res;
    try {        
        vector<cream_api::soap_proxy::JobIdWrapper> toCancel;        
        for( list<CreamJob>::const_iterator it=jobs.begin(); it != jobs.end(); ++it ) {            
            string thisJob = endpoint;
            boost::replace_all( thisJob, iceConfManager::getInstance()->getConfiguration()->ice()->cream_url_postfix(), "" );
            thisJob += it->getCreamJobID();
            
            CREAM_SAFE_LOG( m_log_dev->infoStream() << method_name
                            << "Will cancel JobID [" << thisJob 
                            << "] because its original proxy"
                            << " is going to expire..."
                            
                            );
            
            toCancel.push_back( cream_api::soap_proxy::JobIdWrapper( it->getCompleteCreamJobID(), endpoint, vector<cream_api::soap_proxy::JobPropertyWrapper>()) );
        }
        
        cream_api::soap_proxy::JobFilterWrapper req( toCancel, vector<string>(), -1, -1, "", "");
        
        CreamProxy_Cancel( endpoint, better_proxy, &req, &res ).execute( 3 );
    } catch(exception& ex) {
        // We do not remove the job from the cache, as the
        // statuspoller (or the event listener) will eventually get
        // rid of it.
        for_each( jobs.begin(), jobs.end(), cancel_refuse_logger( ex.what() ) );
        return;
    } catch(...) {
        // idem
        for_each( jobs.begin(), jobs.end(), cancel_refuse_logger( "Unknown exception caught by ICE during CANCEL operation" ) );
        return;        
    }         
    // 
    // Log to LB the failure reasons
    //
    list< pair<cream_api::soap_proxy::JobIdWrapper, string> > tmp;

    res.getNotExistingJobs( tmp );
    for_each( tmp.begin(), tmp.end(), cancel_refuse_logger( "Job not existing" ) );

    tmp.clear();
    res.getNotMatchingStatusJobs( tmp );
    for_each( tmp.begin(), tmp.end(), cancel_refuse_logger( "Job status does not match" ) );

    tmp.clear();
    res.getNotMatchingDateJobs( tmp );
    for_each( tmp.begin(), tmp.end(), cancel_refuse_logger( "Job date does not match" ) );

    tmp.clear();
    res.getNotMatchingProxyDelegationIdJobs( tmp );
    for_each( tmp.begin(), tmp.end(), cancel_refuse_logger( "Job proxy delegation id does not match" ) );

    tmp.clear();
    res.getNotMatchingLeaseIdJobs( tmp );
    for_each( tmp.begin(), tmp.end(), cancel_refuse_logger( "Job lease id does not match" ) );

}
