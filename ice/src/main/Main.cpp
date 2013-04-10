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



#include "Main.h"
#include "utils/IceConfManager.h"
#include "utils/IceLBLogger.h"
#include "utils/IceLBEvent.h"
#include "threads/EventStatusPoller.h"
#include "threads/DelegationRenewal.h"
#include "utils/IceLBEvent.h"
#include "utils/IceLBEventFactory.h"
#include "utils/IceLBLogger.h"
#include "utils/CreamProxyMethod.h"
#include "commands/IceCommandReschedule.h"
#include "commands/IceCommandSubmit.h"
#include "commands/IceCommandCancel.h"
#include "utils/Request_source_jobdir.h"
#include "utils/Request.h"
#include "utils/RequestParser.h"
#include "utils/DNProxyManager.h"
#include "utils/CreamJob.h"
#include "glite/ce/cream-client-api-c/VOMSWrapper.h"
#include "db/GetJobsToPoll.h"
#include "db/CheckGridJobID.h"
#include "db/GetTerminatedJobs.h"
#include "db/Transaction.h"
#include "db/RemoveJobByGid.h"
#include "db/RemoveJobByUserDN.h"
#include "utils/IceConfManager.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "utils/logging.h"

#include "glite/ce/cream-client-api-c/job_statuses.h"
#include "glite/ce/cream-client-api-c/ResultWrapper.h"
#include "glite/ce/cream-client-api-c/JobIdWrapper.h"
#include "glite/ce/cream-client-api-c/JobFilterWrapper.h"
//#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/AbsCreamProxy.h"
//#include "glite/ce/cream-client-api-c/creamApiLogger.h"

#include "glite/ce/cream-client-api-c/certUtil.h"

#include "purger/src/purger.h"
#ifdef HAVE_GLITE_JOBID
#include "glite/jobid/JobId.h"
#else
#include "glite/wmsutils/jobid/JobId.h"
#endif
#include "glite/security/proxyrenewal/renewal.h"
#include "common/src/configuration/ICEConfiguration.h"
#include "common/src/configuration/WMConfiguration.h"
#include "common/src/configuration/CommonConfiguration.h"
//#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include "classad_distribution.h"

#include <sys/time.h>
#include <exception>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
using namespace std;
using namespace glite::wms::ice;

//namespace ice_util = glite::wms::ice::util;
namespace cream_api = glite::ce::cream_client_api;
namespace soap_proxy = glite::ce::cream_client_api::soap_proxy;
namespace config_ns = glite::wms::common::configuration;

Main                   *Main::s_instance = 0;
boost::recursive_mutex  Main::s_mutex;


//______________________________________________________________________________
long long Main::check_my_mem( const pid_t pid )
{
  edglog_fn("Main::check_my_mem");

  char cmd[128];
  char used_rss_mem[64];
  memset((void*) cmd, 0, 64);
  
  sprintf( cmd, "/bin/ps --cols 200 -orss -p %d |/bin/grep -v RSS", pid);


  FILE * in = popen( cmd, "r");
  if(!in) return (long long)0;
  
  while (fgets(used_rss_mem, 64, in) != NULL)
    edglog(debug) 
//    CREAM_SAFE_LOG( m_log_dev->debugStream()
		    << "Used RSS Memory: "
		    << used_rss_mem << endl;
//		    );
  pclose(in);

  return atoll(used_rss_mem);
}

class Resubmit_Or_Purge {
  glite::wms::ice::Main *m_theIce;

public:
  Resubmit_Or_Purge( glite::wms::ice::Main *theIce ) : m_theIce( theIce ) {}
  void operator()( CreamJob& job ) { m_theIce->resubmit_or_purge_job( &job ); }
};

//____________________________________________________________________________
Main::IceThreadHelper::IceThreadHelper( const std::string& name ) :
    m_name( name ),
    m_thread( 0 )
    //m_log_dev( cream_api::util::creamApiLogger::instance()->getLogger() )
{
}

//____________________________________________________________________________
Main::IceThreadHelper::~IceThreadHelper( ) 
{
  stop( );
  delete m_thread;
}

//____________________________________________________________________________
void Main::IceThreadHelper::start( threads::IceThread* obj ) throw( IceInitException& )
{
  m_ptr_thread = boost::shared_ptr< threads::IceThread >( obj );
  try {
    m_thread = new boost::thread(boost::bind(&threads::IceThread::operator(), m_ptr_thread) );
  } catch( boost::thread_resource_error& ex ) {
    throw IceInitException( ex.what() );
  }
}

//____________________________________________________________________________
bool Main::IceThreadHelper::is_started( void ) const 
{
    return ( 0 != m_thread );
}

//____________________________________________________________________________
void Main::IceThreadHelper::stop( void )
{
  edglog_fn("Main::IceThreadHelper::stop");
  if( m_thread && m_ptr_thread->isRunning() ) {
//    CREAM_SAFE_LOG( m_log_dev->debugStream()
	edglog(debug)
		   << "Main::IceThreadHelper::stop() - Waiting for thread " 
		   << m_name 
		   << " termination..."<<endl;
                   
//		   );
    m_ptr_thread->stop();
    m_thread->join();
//    CREAM_SAFE_LOG( m_log_dev->debugStream()
	edglog (debug)	   
		   << "Main::IceThreadHelper::stop() - Thread " 
		   << m_name << " finished"<<endl;
//		   );
  }
}

//____________________________________________________________________________
Main* Main::instance( void )
{
  edglog_fn("Main::instance");

    //log4cpp::Category* m_log_dev = cream_api::util::creamApiLogger::instance()->getLogger();

    if ( 0 == s_instance ) {
        try {
            s_instance = new Main( ); // may throw IceInitException

        } catch(IceInitException& ex) {
//            CREAM_SAFE_LOG( m_log_dev->fatalStream() 
                          edglog (fatal)
                           << "Main::instance() - " 
                           << ex.what() << endl;
                           
//                           );
            abort();
        } catch(...) {
//            CREAM_SAFE_LOG( m_log_dev->fatalStream() 
                 edglog  (fatal)         << "Main::instance() - " 
                           << "Catched unknown exception" << endl;
                           
//                           );
            abort();
        }
        s_instance->init( );    
    }
    return s_instance;
}

//____________________________________________________________________________
Main::Main( ) throw(IceInitException&) : 
    m_poller_thread( "Event Status Poller" ),
    m_proxy_renewer_thread( "Proxy Renewer" ),
    m_wms_input_queue( new Request_source_jobdir( IceConfManager::instance()->getConfiguration()->wm()->input(), true ) /*util::Request_source_factory::make_source_input_wm()*/ ),
    m_ice_input_queue( new Request_source_jobdir( IceConfManager::instance()->getConfiguration()->ice()->input(), true ) /*util::Request_source_factory::make_source_input_ice()*/ ),
    m_reqnum(util::IceConfManager::instance()->getConfiguration()->ice()->max_ice_threads()),
    //m_log_dev( cream_api::util::creamApiLogger::instance()->getLogger() ),
    m_lb_logger( util::IceLBLogger::instance() ),
    m_configuration( util::IceConfManager::instance()->getConfiguration() ),
    m_times_too_queue_full( 0 )
{
  /**
     Must check if ICE is starting from scratch. In that case all Event
     Query calls must be done with fromDate = 'now', because ICE is not
     intersted in old jobs/events that has been removed from its database
  */
  
  edglog_fn("Main::Main");
  
  m_start_time = time(0)-600;

  if(m_reqnum < 5) m_reqnum = 5;
   int thread_num = m_configuration->ice()->max_ice_threads();
   if(thread_num<1) thread_num=1;

   int poll_tnum = 0;
   if(thread_num >=2)
     poll_tnum = thread_num/2;
   else
     poll_tnum = 2;

   m_requests_pool     = new threads::IceThreadPool("ICE Submission Pool", thread_num );
   m_ice_commands_pool = new threads::IceThreadPool( "ICE Poller Pool", poll_tnum);
   m_ice_lblog_pool    = new threads::IceThreadPool( "ICE LB Logging Pool", 2);

    try {

      m_hostdn = cream_api::certUtil::getDN( m_configuration->ice()->ice_host_cert() );

    } catch( glite::ce::cream_client_api::soap_proxy::auth_ex& ex ) {
      string hostcert = m_configuration->ice()->ice_host_cert();
//        CREAM_SAFE_LOG( m_log_dev->fatalStream()
                       edglog(fatal) << "Main::CTOR() - Unable to extract user DN from ["
                        <<  hostcert << "]"
                        << ". Cannot perform JobRegister and cannot start Listener. Stop!"<<endl;
                     //   );
        exit(1);
    }
}

//____________________________________________________________________________
Main::~Main( )
{

}

//____________________________________________________________________________
void Main::stopAllThreads( void ) 
{
  /**
   * The following call to stop() method of IteThreadHelper
   * causes the call of IceThread::stop() and boost::thread::join()
   */
  if(m_poller_thread.is_started())
    m_poller_thread.stop();

  if(m_proxy_renewer_thread.is_started())
    m_proxy_renewer_thread.stop();
}

//____________________________________________________________________________
void Main::init( void )
{    
  // Handle resubmitted/purged jobs
  list< glite::wms::ice::util::CreamJob > allJobs;
  {
    db::GetTerminatedJobs getter( &allJobs, "Main::init" );
    db::Transaction tnx(false, false);
    tnx.execute( &getter );
  }

  Resubmit_Or_Purge checker( this ); 
  for_each( allJobs.begin(), allJobs.end(),  checker );
	
}

//____________________________________________________________________________
void Main::startPoller( void )
{
  edglog_fn("Main::startPoller");

    if ( ! m_configuration->ice()->start_poller() ) {
//        CREAM_SAFE_LOG( m_log_dev->warnStream()
                       edglog(warning) << "Main::startPoller() - "
                        << "Poller disabled in configuration file. "
                        << "Not started"<<endl;
                        
//                        );
        return;
    }
    
    threads::EventStatusPoller* poller;
    
    // I removed the try/catch because in the new schema using the
    // iceCommandStatusPoller there is not anymore that exception
    
    poller = new threads::EventStatusPoller( this, m_configuration->ice()->poller_delay() );
    m_poller_thread.start( poller );

}

//----------------------------------------------------------------------------
/*void Ice::startLeaseUpdater( void ) 
{
    if ( !m_configuration->ice()->start_lease_updater() ) {
        CREAM_SAFE_LOG( m_log_dev->warnStream()
                        << "Ice::startLeaseUpdater() - "
                        << "Lease Updater disabled in configuration file. "
                        << "Not started"
                        
                        );
        return;
    }
    util::leaseUpdater* lease_updater = new util::leaseUpdater( );
    m_lease_updater_thread.start( lease_updater );
}
*/
//-----------------------------------------------------------------------------
void Main::startProxyRenewer( void ) 
{
  edglog_fn("Main::startProxyRenewer");
  if ( !m_configuration->ice()->start_proxy_renewer() ) {
//    CREAM_SAFE_LOG( m_log_dev->warnStream()
		   edglog(warning) << "Main::startProxyRenewer() - "
		    << "Delegation Renewal disabled in configuration file. "
		    << "Not started"<<endl;
		    
		    //);
    return;
  }
  threads::DelegationRenewal* proxy_renewer = new threads::DelegationRenewal( );
  m_proxy_renewer_thread.start( proxy_renewer );
}

//-----------------------------------------------------------------------------
// void Main::startJobKiller( void )
// {
//     // FIXME: uncomment this method to activate the jobKiller    
// /*    if ( !m_configuration->ice()->start_job_killer() ) {
//         CREAM_SAFE_LOG( m_log_dev->warnStream()
//                         << "Ice::startJobKiller() - "
//                         << "Job Killer disabled in configuration file. "
//                         << "Not started"
//                         
//                         );
//         return;
//     }
//     util::jobKiller* jobkiller = new util::jobKiller( );
//     m_job_killer_thread.start( jobkiller ); */
// }

//____________________________________________________________________________
void Main::getNextRequests( std::list< util::Request* >& ops) 
{
  //  int reqnum = ice_util::IceConfManager::getInstance()->getConfiguration()->ice()->max_ice_threads();
  //  if(reqnum < 5) reqnum = 5;
  ops = m_ice_input_queue->get_requests( m_reqnum );
}

//____________________________________________________________________________
void Main::removeRequest( util::Request* req )
{
    m_ice_input_queue->remove_request( req );
}

//----------------------------------------------------------------------------
size_t Main::get_input_queue_size( void )
{
    return m_ice_input_queue->get_size();
}

//____________________________________________________________________________
// bool Main::is_listener_started( void ) const
// {
//     return m_listener_thread.is_started( );
// }

//____________________________________________________________________________
bool Main::is_poller_started( void ) const
{
    return m_poller_thread.is_started( );
}

//____________________________________________________________________________
// bool Main::is_lease_updater_started( void ) const
// {
//     return m_updater_thread.is_started( );
// }

//____________________________________________________________________________
bool Main::is_proxy_renewer_started( void ) const
{
    return m_proxy_renewer_thread.is_started( );
}

//____________________________________________________________________________
// bool Main::is_job_killer_started( void ) const
// {
//     return m_job_killer_thread.is_started( );
// }

//____________________________________________________________________________
void Main::resubmit_job( util::CreamJob* the_job, const string& reason ) throw()
{

  edglog_fn("Main::resubmit_job");
  if( ::getenv( "GLITE_WMS_ICE_NORESUBMIT" ) ) {

//     CREAM_SAFE_LOG( m_log_dev->warnStream() 
                   edglog(warning) << "Main::resubmit_job() - RESUBMISSION DISABLED."<<endl;
                  //  );
     return;
  }

  cream_api::soap_proxy::VOMSWrapper V( the_job->user_proxyfile(),  !::getenv("GLITE_WMS_ICE_DISABLE_ACVER") );
  
  if ( V.getProxyTimeEnd( ) <= time(0)+300) {
//    CREAM_SAFE_LOG( m_log_dev->errorStream() 
		edglog(error)    << "Main::resubmit_job() - Will NOT resubmit job ["
		    << the_job->describe() << "] " 
		    << "because it's Input Sandbox proxy file is expired: "
		    << V.getErrorMessage()<<endl;
//		    );
    IceLBEvent* ev = new job_aborted_event( *the_job );
    if ( ev ) {
      
      bool log_with_cancel_seqcode = (the_job->status( ) == glite::ce::cream_client_api::job_statuses::CANCELLED) && (!the_job->cancel_sequence_code( ).empty( ));
      
      m_lb_logger->logEvent( ev, log_with_cancel_seqcode, true );
      
    }
    
    return;
  }
  
    util::CreamJob _the_job(*the_job);
    try {
      boost::recursive_mutex::scoped_lock M( s_mutex );
        
	
        _the_job = m_lb_logger->logEvent( new util::ice_resubmission_event( _the_job, reason ), false, true );
        
        _the_job = m_lb_logger->logEvent( new util::ns_enqueued_start_event( _the_job, m_wms_input_queue->get_name() ), false, true );
        
	string resub_request;


	{ /**
	   * ClassAd-mutex protected region
	   */
	  boost::recursive_mutex::scoped_lock M_classad( util::CreamJob::s_classad_mutex );
	  
	  classad::ClassAd command;
	  classad::ClassAd arguments;
	  
	  command.InsertAttr( "version", string("1.0.0") );
	  command.InsertAttr( "command", string("jobresubmit") );
	  arguments.InsertAttr( "id", _the_job.grid_jobid() );
	  arguments.InsertAttr( "lb_sequence_code", _the_job.sequence_code() );
	  command.Insert( "arguments", arguments.Copy() );
	  
	  classad::ClassAdUnParser unparser;
	  unparser.Unparse( resub_request, &command );        
	} // releasing classad mutex

//        CREAM_SAFE_LOG(
                   //    m_log_dev->infoStream()
                   edglog(info)    << "Main::resubmit_job() - Putting ["
                       << resub_request << "] to WM's Input file"<<endl;
                       
                    //   );

        m_wms_input_queue->put_request( resub_request );
        _the_job = m_lb_logger->logEvent( new util::ns_enqueued_ok_event( _the_job, m_wms_input_queue->get_name() ), false, true );
	
    } catch(std::exception& ex) {
//        CREAM_SAFE_LOG( m_log_dev->errorStream() 
			edglog(error)<< "Main::resubmit_job() - "
                        << ex.what() << endl;
//                         );

        m_lb_logger->logEvent( new util::ns_enqueued_fail_event( _the_job, m_wms_input_queue->get_name(), ex.what() ), false, true );
	_the_job.set_failure_reason( string("resubmission failed: ") + ex.what() );
	m_lb_logger->logEvent( new util::job_aborted_event( _the_job ), false, true );
    }
}

//----------------------------------------------------------------------------
//ice_util::jobCache::iterator
void Main::purge_job( const util::CreamJob* theJob , 
		     const string& reason )
  throw() 
{
    //static const char* method_name = "Main::purge_job() - ";
    edglog_fn("Main::purge_job");
    string jobdesc( theJob->describe() );
    string _gid(  theJob->grid_jobid() );

    {
      db::CheckGridJobID checker( _gid, "Main::purge_job" );
      db::Transaction tnx(false, false);
      tnx.execute( &checker );
      if( !checker.found() )
	return;
    }
    if ( !m_configuration->ice()->purge_jobs() ) {
      
      /**
	 PURGE on the CE is DISABLED, let's:
	 - decrement job counter
	 - remove job from ICE's database
	 - return
      */
//      CREAM_SAFE_LOG(m_log_dev->infoStream() << method_name
		    edglog(info) << "JobPurge is DISABLED, will not Purge job ["
		     << jobdesc << "]." << endl;
		    // );
      return;
    }
      
    vector<cream_api::soap_proxy::JobIdWrapper> target;
    cream_api::soap_proxy::ResultWrapper result;

    target.push_back(cream_api::soap_proxy::JobIdWrapper (theJob->cream_jobid(), 
							  theJob->cream_address( ), 
							  std::vector<cream_api::soap_proxy::JobPropertyWrapper>()
							  ));
    
    cream_api::soap_proxy::JobFilterWrapper filter(target, vector<string>(), -1, -1, "", "");
    
    // Gets the proxy to use for authentication
    string better_proxy = util::DNProxyManager::getInstance()->getAnyBetterProxyByDN( theJob->user_dn() ).get<0>();

    if( better_proxy.empty() ) {
//      CREAM_SAFE_LOG( m_log_dev->warnStream() << method_name
		     edglog(warning) << "DNProxyManager returned an empty string for BetterProxy of user DN ["
		      << "] for job ["
		      << jobdesc
		      << "]. Using the Job's proxy." << endl;
		     // );

      better_proxy = theJob->user_proxyfile();
    }
    
    cream_api::soap_proxy::VOMSWrapper V( better_proxy,  !::getenv("GLITE_WMS_ICE_DISABLE_ACVER") );
    if( !V.IsValid( ) ) {

      /**
	 Cannot PURGE job on the CE;
	 let's leave it on the ICE's database
	 and DO NOT decrement the job counter
	 of the 'super' better proxy.
      */
 //     CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
		    edglog(error)  << "Unable to purge job ["
		      << jobdesc
		      << "] due to authentication error: " 
		      << V.getErrorMessage() << endl;
		     // );
      return;// jit;
    }

    try {
        
//      CREAM_SAFE_LOG(m_log_dev->infoStream() << method_name
		   edglog(info)  << "Calling JobPurge for job ["
		     << jobdesc << "]" << endl;
		     //);
      // We cannot accumulate more jobs to purge in a
      // vector because we must authenticate different
      // jobs with different user certificates.
      
      glite::wms::ice::util::CreamProxy_Purge( theJob->cream_address(), 
					       better_proxy,
					       &filter, 
					       &result ).execute( 3 );
      
      
      
      // the following code accumulate all the results in tmp list
      // If an error occurred tmp should contain 1 (and only 1) element
      // (because we sent purge for only 1 job)
      list<pair<cream_api::soap_proxy::JobIdWrapper, string> > tmp;
      result.getNotExistingJobs( tmp );
      result.getNotMatchingStatusJobs( tmp );
      result.getNotMatchingDateJobs( tmp );
      result.getNotMatchingProxyDelegationIdJobs( tmp );
      result.getNotMatchingLeaseIdJobs( tmp );
      
      // It is sufficient look for "empty-ness" because
      // we've started only one job
      if( !tmp.empty() ) {
	pair<cream_api::soap_proxy::JobIdWrapper, string> wrong = *( tmp.begin() ); // we trust there's only one element because we've purged ONLY ONE job
	string errMex = wrong.second;
	
//	CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name
		     edglog(error)  << "Cannot purge job [" << jobdesc 
		       << "] - Reason is: " << errMex << endl;
		    //   );
	
	/**
	   Another poll will be tried again
	   later
	*/
	return;// jit;
      }
      
    } catch ( util::ClassadSyntax_ex& ex ) {
      /**
       * this exception should not be raised because
       * the CreamJob is created from another valid one
       */
//      CREAM_SAFE_LOG(m_log_dev->fatalStream() << method_name
		    edglog(fatal) << "Fatal error: CreamJob creation failed "
		     << "copying from a valid one!!!" << endl;
		    // );
      abort();
    } catch(cream_api::soap_proxy::auth_ex& ex) {
//      CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name
		   edglog(error)  << "Cannot purge job [" << jobdesc
		     << "]. Reason is: " << ex.what() << endl;
		    // );
    } catch(cream_api::cream_exceptions::BaseException& ex) {
//      CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name
		    edglog(error) << "Cannot purge job [" << jobdesc
		     << "]. Reason is BaseException: " << ex.what()<<endl;
		    // );
    } catch(cream_api::cream_exceptions::InternalException& ex) {
//      CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name
		  edglog(error)   << "Cannot purge job [" << jobdesc
		     << "]. Reason is InternalException: " << ex.what()<<endl;
		   //  );
    } catch( std::exception& ex ) {
//      CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name
		   edglog(error)  << "Cannot purge job [" << jobdesc
		     << "]. Reason is an exception: " << ex.what() << endl;
		     
		   //  );
    } catch( ... ) {
//      CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name
		  edglog(error)   << "Cannot purge job [" << jobdesc
		     << "]. Reason is an unknown exception" << endl;
		    // );
    }
}


//____________________________________________________________________________
void Main::deregister_proxy_renewal( const util::CreamJob* job ) throw()
{
  edglog_fn("Main::deregister_proxy_renewal");
  string jobdesc( job->describe() );
    if ( !::getenv( "ICE_DISABLE_DEREGISTER") ) {
        // must deregister proxy renewal
        int      err = 0;
        
//        CREAM_SAFE_LOG(
                      // m_log_dev->infoStream()
                     edglog(info)  << "Main::deregister_proxy_renewal() - "
                       << "Unregistering Proxy for job ["
                       << jobdesc << "]" << endl;
                     //  );
        
        err = glite_renewal_UnregisterProxy( job->grid_jobid().c_str(), NULL );
        
        if ( err && (err != EDG_WLPR_PROXY_NOT_REGISTERED) ) {
	    const char* errmex = edg_wlpr_GetErrorText(err);
   //         CREAM_SAFE_LOG(
                        //   m_log_dev->errorStream()
                        edglog(error)   << "Main::deregister_proxy_renewal() - "
                           << "ICE cannot unregister the proxy " 
                           << "for job [" << jobdesc
                           << "]. Reason: \"" << errmex
                           << "\"." << endl;
                      //     );
        } else {
            if ( err == EDG_WLPR_PROXY_NOT_REGISTERED ) {
            //    CREAM_SAFE_LOG(
                            //   m_log_dev->warnStream()
                           edglog(warning)    << "Main::deregister_proxy_renewal() - "
                               << "Job proxy not registered for job ["
                               << jobdesc 
                               << "]. Going ahead." << endl;
                            //   );
            }
        }
    } else {
   //     CREAM_SAFE_LOG(
                   //    m_log_dev->warnStream()
                    edglog(warning)   << "Main::deregister_proxy_renewal() - "
                       << "Proxy unregistration disable. To reenable, " 
                       << "unset the environment variable ICE_DISABLE_DEREGISTER"<<endl;
                    //   );
    }
}


//____________________________________________________________________________
void Main::purge_wms_storage( const util::CreamJob* job ) throw()
{
  edglog_fn("Main::purge_wms_storage");
  string jobdesc( job->describe() );
    if ( !::getenv( "ICE_DISABLE_PURGER" ) ) {
        try {
       //     CREAM_SAFE_LOG(
                        //   m_log_dev->infoStream()
                         edglog(info)  << "Main::purge_wms_storage() - "
                           << "Purging storage for job ["
                           << jobdesc << "]" << endl;
                           
                         //  );
#ifdef HAVE_GLITE_JOBID
            glite::jobid::JobId j_id( job->grid_jobid() );
#else        
            glite::wmsutils::jobid::JobId j_id( job->get_grid_jobid() );
#endif
	    bool lbp = m_configuration->common()->lbproxy();
            wms::purger::Purger the_purger( lbp /*util::IceConfManager::getInstance()->getConfiguration()->common()->lbproxy()*/ );
            the_purger(j_id);

        } catch( std::exception& ex ) {
           // CREAM_SAFE_LOG(
                           //m_log_dev->errorStream()
                           edglog(error) << "Main::purge_wms_storage() - "
                           << "Cannot purge storage for job ["
                           << jobdesc
                           << "]. Reason is: " << ex.what() << endl;
                           
                           //);
            
        }
    } else {
        //CREAM_SAFE_LOG(
                   //    m_log_dev->warnStream()
                   edglog(warning)    << "Main::purge_wms_storage() - "
                       << "WMS job purger disabled in ICE. To reenable "
                       << "unset the environment variable ICE_DISABLE_PURGER"<< endl;
                       
                    //   );
    }
}

//____________________________________________________________________________
//ice_util::jobCache::iterator 
bool Main::resubmit_or_purge_job( util::CreamJob* tmp_job )
throw() 
{
  cream_api::job_statuses::job_status st = (cream_api::job_statuses::job_status)tmp_job->status();
  
  edglog_fn("Main::resubmit_or_purge_job");
  
  bool ok = false;
  
  if ( cream_api::job_statuses::CANCELLED == st ||
       cream_api::job_statuses::DONE_OK == st ) {
    
    deregister_proxy_renewal( tmp_job );
    
  }
  
  /**
    Remove job from DB. If the job is the last one of current
    DN, the JobPurge may fail because the super better proxy
    is removed (if the were not another better proxy non 'super'.
  */
  if( cream_api::job_statuses::DONE_OK == st ||
       cream_api::job_statuses::CANCELLED == st ||
       cream_api::job_statuses::DONE_FAILED == st ||
       cream_api::job_statuses::ABORTED == st ) 
  {
    //CREAM_SAFE_LOG(m_log_dev->debugStream() << "Main::resubmit_or_purge_job() - "
 	  	  edglog(debug) << "Removing purged job [" << tmp_job->describe()
 		   << "] from ICE's database"<<endl;
 		   //);

    if(tmp_job->proxy_renewable())
      util::DNProxyManager::getInstance()->decrementUserProxyCounter( tmp_job->user_dn(), tmp_job->myproxy_address() );
    {
      db::RemoveJobByGid remover( tmp_job->grid_jobid(), "Main::resubmit_or_purge_job" );
      db::Transaction tnx(false, false);
      tnx.execute( &remover );
    }
    ok = true;// notify to the caller (EventQuery that the job has been removed from ICE's DB
  }
  
  if ( ( cream_api::job_statuses::DONE_FAILED == st ||
	 cream_api::job_statuses::ABORTED == st ) &&
       !tmp_job->killed_byice() ) {
    
    resubmit_job( tmp_job, "Job resubmitted by ICE" );
    
  }        
  

  
  if ( cream_api::job_statuses::DONE_OK == st ||
       cream_api::job_statuses::CANCELLED == st ||
       cream_api::job_statuses::DONE_FAILED == st ||
       cream_api::job_statuses::ABORTED == st ) {
    
    purge_job( /*it*/tmp_job, "Job purged by ICE" );// this method also decrement user's proxy
    
  }
  
  
  if ( cream_api::job_statuses::CANCELLED == st ) {
    
    purge_wms_storage( tmp_job );
    
  }
  return ok;
}

//____________________________________________________________________________
int Main::main_loop( void ) {

//  static const char* method_name = "Main::main_loop";
edglog_fn("Main::main_loop");
  /**
   *
   * Prepares a list that will contains requests fetched from 
   * input filelist/jobdir
   *
   */
  list< glite::wms::ice::util::Request* > requests;

  /**
   *
   * Sets parameters needed to max memory check
   *
   */
  int mem_threshold_counter = 0;
   
  long long max_ice_mem = m_configuration->ice()->max_ice_mem();
   
 // CREAM_SAFE_LOG(m_log_dev->debugStream()
		edglog(debug) 
		 << " - Max ICE memory threshold set to "
		 << max_ice_mem << " kB" <<endl;  
		 //);
   
  pid_t myPid = ::getpid();
   
  /**
   *
   * Enter the main loop: fetches requests, process them, insert iceCommand into threadPool's queue
   *
   */
  while( true ) {
    //
    // BEWARE!! the get_command_count() method locks the
    // threadPool object. Hence, it is *extremely* dangerous to
    // call the get_command_count() method inside a CREAM_SAFE_LOG()
    // block, because it would result in a hold-and-wait potential
    // race condition.
    //
    unsigned int command_count( this->get_requests_pool()->get_command_count() );

    if ( command_count > m_configuration->ice()->max_ice_threads() ) {
    //  CREAM_SAFE_LOG(m_log_dev->debugStream()
		    edglog(debug) 
		     << " - There are currently too many requests ("
		     << command_count
		     << ") in the internal command queue. "
		     << "Will check again in 1 second." << endl;
		    // );
		     
      m_times_too_queue_full++;
      // if after 3600 secs (1 h) the queue is still full
      // let's perform a self-restart
      if(m_times_too_queue_full > 3600 ) {
        this->stopAllThreads(); // this return only when all threads have finished
        this->get_requests_pool()->stopAllThreads();
        this->get_ice_commands_pool()->stopAllThreads();
        this->get_ice_lblog_pool( )->stopAllThreads();

    //    CREAM_SAFE_LOG( m_log_dev->fatalStream()
                     edglog(fatal)  
                        << " - The thread pool is full since too much time (1 hour). Need self-restarting..."<<endl;
                        //);
                
        return 2; // return special code '2' to main. 
		  // It means max mem reached, 
		  // but we use it also in this case to tell the
	          // glite-wms-ice-safe process to restart ICE.

      }
    } else {	  
      
      m_times_too_queue_full = 0;
      
      requests.clear();
      this->getNextRequests( requests );
	  
      if( !requests.empty() )
	//CREAM_SAFE_LOG(
		     //  m_log_dev->infoStream()
		     edglog(info)  
		       << " - *** Found " << requests.size() << " new request(s)" << endl;
                           
		    //   );
	  
      for( list< glite::wms::ice::util::Request* >::iterator it = requests.begin();
	   it != requests.end(); ++it ) 
	{
	  string reqstr = (*it)->to_string();

	      

	//  CREAM_SAFE_LOG(
			// m_log_dev->debugStream()
			edglog(debug)
			 << " - *** Unparsing request <"
			 << reqstr
			 << ">"  <<endl;
			// );

	  util::RequestParser parser( *it );
	  glite::wms::ice::util::CreamJob theJob;
	  
	  try {
	    
	    string cmdtype;

	    parser.unparse_request( );

	    glite::wms::ice::IceAbstractCommand* cmd = 0;	
	    if( boost::algorithm::iequals( parser.get_command( ), "cancel" ) ) {
	      cmd = new glite::wms::ice::IceCommandCancel( *it ); 
	      this->get_requests_pool()->add_request( cmd );
	      continue;
	    }
	
	    theJob = parser.get_job( );
	
	    glite::ce::cream_client_api::soap_proxy::VOMSWrapper V( theJob.user_proxyfile(),  !::getenv("GLITE_WMS_ICE_DISABLE_ACVER") );
	    if( !V.IsValid( ) ) {
	     // CREAM_SAFE_LOG( m_log_dev->errorStream()
			     edglog(error)
			      << " - For job ["
			      << theJob.grid_jobid() << "]"
			      << " the proxyfile ["
			      << theJob.user_proxyfile() 
			      << "] is not valid: "
			      << V.getErrorMessage()
			      << ". Skipping processing of this job. "
			      << "Logging an abort and removing request from filelist/jobdir" << endl;
			  //    );
			
	      theJob.set_failure_reason( V.getErrorMessage() );
	      theJob.set_status( glite::ce::cream_client_api::job_statuses::ABORTED ); 
	      /**
		 The job will not be put in the ICE's database
		 in fact logEvent just does an SQL UPDATE ... WHERE grid_jobid = ''.
		 So if the gridjobid is not there the UPDATE does not take place.
	      */
	      glite::wms::ice::util::IceLBEvent* ev = glite::wms::ice::util::IceLBEventFactory::mkEvent( theJob );
	      if ( ev ) {
	        bool log_with_cancel_seqcode = (theJob.status( ) == glite::ce::cream_client_api::job_statuses::CANCELLED) && (!theJob.cancel_sequence_code( ).empty( ));
		theJob = glite::wms::ice::util::IceLBLogger::instance()->logEvent( ev, log_with_cancel_seqcode, false );
	      } else {
		//CREAM_SAFE_LOG( m_log_dev->errorStream()
				edglog(error)
				<< " - For job ["
				<< theJob.grid_jobid() 
				<< "Couldn't log abort event."<<endl;
				//);
	      }
	      this->removeRequest( *it );
	      delete( *it );
	      continue;
	    }

	    theJob.set_user_dn( V.getDNFQAN() );
	    theJob.set_isbproxy_time_end( V.getProxyTimeEnd() );

	    if( boost::algorithm::iequals( parser.get_command( ), "submit" ) ) {
	      cmd = new glite::wms::ice::IceCommandSubmit( *it, theJob ); 
	    }
		
	    if( boost::algorithm::iequals( parser.get_command( ), "reschedule" ) ) {
	      cmd = new glite::wms::ice::IceCommandReschedule( *it, theJob );
	    }
		
	    this->get_requests_pool()->add_request( cmd );

	  } catch( std::exception& ex ) {
	 //   CREAM_SAFE_LOG( m_log_dev->errorStream()
			  edglog(error) 
			    << " - Got exception \"" << ex.what()
			    << "\". Removing BAD request..." << endl;
			  //  );

	    this->removeRequest( *it );
	    delete( *it );
	  }
	}
    }

    sleep(1);
	
    /**
     *
     * Every 2 minutes ICE checks its mem usage
     *
     */
    ++mem_threshold_counter;

    if(mem_threshold_counter >= 120) { // every 120 seconds check the memory
      mem_threshold_counter = 0;
      long long mem_now = check_my_mem(myPid);
      if(mem_now > max_ice_mem) {
	    
	this->stopAllThreads(); // this return only when all threads have finished
	this->get_requests_pool()->stopAllThreads();
	this->get_ice_commands_pool()->stopAllThreads();
	this->get_ice_lblog_pool( )->stopAllThreads();

	//CREAM_SAFE_LOG( m_log_dev->fatalStream()
		edglog(fatal)
			<< " - Max memory reached ["
			<< mem_now << " kB] ! EXIT!" << endl;
		//	);
		
	return 2; // return special code '2' to main. It means max mem reached.
      }
    }
	
  }
}
