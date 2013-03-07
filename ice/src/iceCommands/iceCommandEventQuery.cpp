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

#include "glite/ce/cream-client-api-c/EventWrapper.h"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/job_statuses.h"
#include "glite/ce/cream-client-api-c/scoped_timer.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "common/src/configuration/ICEConfiguration.h"

#include "iceCommandStatusPoller.h"
#include "iceCommandEventQuery.h"
#include "iceCommandLBLogging.h"
#include "iceUtils/iceLBEventFactory.h"
#include "iceUtils/CreamProxyMethod.h"
#include "iceUtils/DNProxyManager.h"
#include "iceUtils/IceConfManager.h"
#include "iceUtils/iceLBLogger.h"
#include "iceUtils/IceLBEvent.h"
#include "ice/IceCore.h"
#include "iceUtils/IceUtils.h"

#include "iceDb/GetDbID.h"
#include "iceDb/SetDbID.h"
#include "iceDb/GetJobsByDN.h"
#include "iceDb/DNHasJobs.h"
#include "iceDb/InsertStat.h"
#include "iceDb/GetEventID.h"
#include "iceDb/SetEventID.h"
#include "iceDb/SetEventIDForCE.h"
#include "iceDb/GetJobByGid.h"
#include "iceDb/GetJobByCid.h"
#include "iceDb/Transaction.h"
#include "iceDb/GetJobsByDbID.h"
#include "iceDb/RemoveJobByGid.h"
#include "iceDb/RemoveJobByCid.h"
#include "iceDb/UpdateJob.h"
#include "iceDb/RemoveJobsByDbID.h"
#include "iceDb/RemoveJobByUserDN.h"

#include <boost/lexical_cast.hpp>
#include <boost/functional.hpp>
#include "boost/algorithm/string.hpp"
#include "boost/regex.hpp"
#include "boost/format.hpp"

using namespace std;

using namespace glite::wms;


namespace cream_api  = glite::ce::cream_client_api;
namespace soap_proxy = glite::ce::cream_client_api::soap_proxy;
namespace iceUtil    = glite::wms::ice::util;

namespace {
  class cleanup {
    list<soap_proxy::EventWrapper*>* m_toclean;
    
  public:
    cleanup( list<soap_proxy::EventWrapper*>* toClean ) : m_toclean( toClean ) {}
    ~cleanup() {
      if(m_toclean) {
        list<soap_proxy::EventWrapper*>::iterator it;// = m_toclean->begin();

	for( it = m_toclean->begin(); it != m_toclean->end(); ++it )
	  delete( *it );
      }
    }
  };
}



//______________________________________________________________________________
ice::util::iceCommandEventQuery::~iceCommandEventQuery( ) throw()
{
}

//______________________________________________________________________________
ice::util::iceCommandEventQuery::iceCommandEventQuery( ice::IceCore* theIce,
						       const std::string& dn,
						       const std::string& ce)
  : iceAbsCommand( "iceCommandEventQuery", "" ),
    m_log_dev( cream_api::util::creamApiLogger::instance()->getLogger() ),
    m_lb_logger( ice::util::iceLBLogger::instance() ),
    m_iceManager( theIce ),
    m_conf( ice::util::IceConfManager::instance() ),
    m_stopped( false ),
    m_dn( dn ),
    m_ce( ce )
{

}

//______________________________________________________________________________
std::string ice::util::iceCommandEventQuery::get_grid_job_id() const
{
  ostringstream randid( "" );
  struct timeval T;
  gettimeofday( &T, 0 );
  randid << T.tv_sec << "." << T.tv_usec;
  return randid.str();
}

//______________________________________________________________________________
void ice::util::iceCommandEventQuery::execute( const std::string& tid) throw()
{
 m_thread_id = tid;
  static const char* method_name = "iceCommandEventQuery::execute - ";

    list<soap_proxy::EventWrapper*> events;
    cleanup cleaner( &events );

    CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		   << "Retrieving last Event ID for user dn ["
		   << m_dn << "] and ce url ["
		   << m_ce << "]..."
		   );

    long long thisEventID = this->getEventID( m_dn, m_ce );
    
    if( thisEventID == -1 ) {
      CREAM_SAFE_LOG(m_log_dev->warnStream() << method_name << " TID=[" << getThreadID() << "] "
		     << "Couldn't find any last Event ID for current couple "
		     << "userdn [" 
		     << m_dn << "] and ce url ["
		     << m_ce << "]. Inserting 0."
		     );
      
      this->set_event_id( m_dn, m_ce, 0 );
      
      thisEventID = 0;

    } else {

      CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		     << "Last Event ID for current couple "
		     << "userdn [" 
		     << m_dn << "] and ce url ["
		     << m_ce << "] is ["
		     << thisEventID << "]"
		     );
    }
    
    boost::tuple<string, time_t, long long int> proxyinfo = DNProxyManager::getInstance()->getAnyBetterProxyByDN( m_dn );
    
    if ( proxyinfo.get<0>().empty() ) {
    
      // see BUG https://savannah.cern.ch/bugs/index.php?59453 
    
      CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name << " TID=[" << getThreadID() << "] "
		      << "A valid proxy file for DN [" << m_dn
		      << "] ce url ["
		      << m_ce << "] is not available. Skipping EventQuery. Going to remove all jobs of this user..."
		      );
      list< CreamJob > toRemove;
      {
    	db::GetJobsByDN getter( toRemove, m_dn, method_name );
    	db::Transaction tnx( false, false );
    	tnx.execute( &getter );
      }
      list< CreamJob >::iterator jit;
      for( jit = toRemove.begin(); jit != toRemove.end(); ++jit ) {
    	jit->set_failure_reason( "Job Aborted because proxy expired" );
    	jit->set_status( cream_api::job_statuses::ABORTED ); 
    	jit->set_exit_code( 0 );
      }
      while( IceCore::instance()->get_ice_lblog_pool()->get_command_count() > 2 )
        sleep(2);
      
      IceCore::instance()->get_ice_lblog_pool()->add_request( new iceCommandLBLogging( toRemove ) );
      
      return;
      
    }
    
    if ( proxyinfo.get<1>() < time(0) ) {
      CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name << " TID=[" << getThreadID() << "] "
		      << "The returned proxy ["
		      << proxyinfo.get<0>() << "] for DN [" << m_dn
		      << "] ce url ["
		      << m_ce << "] is not valid anymore. Skipping EventQuery. Going to remove all jobs of this user..."
		      );
      list< CreamJob > toRemove;
      {
    	list<pair<string, string> > clause;
    	clause.push_back( make_pair( util::CreamJob::user_dn_field(), m_dn ) );
    
    	//db::GetJobs getter( clause, toRemove, method_name );
	db::GetJobsByDN getter( toRemove, m_dn, method_name );
    	db::Transaction tnx( false, false );
    	tnx.execute( &getter );
      }
      list< CreamJob >::iterator jit;
      for( jit = toRemove.begin(); jit != toRemove.end(); ++jit ) {
    	jit->set_failure_reason( "Job Aborted because proxy expired" );
    	jit->set_status( cream_api::job_statuses::ABORTED ); 
    	jit->set_exit_code( 0 );
      }
      while( IceCore::instance()->get_ice_lblog_pool()->get_command_count() > 2 )
        sleep(2);
      
      IceCore::instance()->get_ice_lblog_pool()->add_request( new iceCommandLBLogging( toRemove ) );
      //Ice::instance()->delete_jobs_by_dn( m_dn );
      
      return;
      
    }
    
    string from( boost::lexical_cast<string>( (long long int) thisEventID)/*util::IceUtils::to_string( (long long int)thisEventID )*/ );
    
    string sdbid;
    time_t exec_time;
    
    string iceid = m_iceManager->getHostDN();
    boost::trim_if(iceid, boost::is_any_of("/"));
    boost::replace_all( iceid, "/", "_" );
    boost::replace_all( iceid, "=", "_" );
   
    CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
                      << "Going to execute EventQuery for user DN ["
		      << m_dn << "] to CE ["
		      << m_ce << "] using proxy file ["
		      << proxyinfo.get<0>() << "]" 
                      ); 
 
    try {
      api_util::scoped_timer Tot( string("iceCommandEventQuery::execute() - SOAP Connection for QueryEvent - ") + "TID=[" + getThreadID() + "]" );
      vector<pair<string, string> > states;

      if( !thisEventID ) // EventID ZERO means ICE has been scratched
        CreamProxy_QueryEvent( m_ce, 
			       proxyinfo.get<0>(), 
			       from,
			       "-1",
			       m_iceManager->getStartTime(),
			       "JOB_STATUS",
			       500,
			       states,
			       sdbid,
			       exec_time,
			       events,
			       iceid,
			       false /* ignore blacklisted CE */).execute( 3 );
       else
         CreamProxy_QueryEvent( m_ce, 
			       proxyinfo.get<0>(), 
			       from,
			       "-1",
			       0,
			       "JOB_STATUS",
			       500,
			       states,
			       sdbid,
			       exec_time,
			       events,
			       iceid,
			       false /* ignore blacklisted CE */).execute( 3 );
      
    } catch(soap_proxy::auth_ex& ex) {
      
      CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name << " TID=[" << getThreadID() << "] "
		     << "Cannot query events for UserDN ["
		     << m_dn << "] CEUrl ["
		     << m_ce << "]. Exception auth_ex is [" << ex.what() << "]"
		     );

      
      return;
      
    } catch(soap_proxy::soap_ex& ex) {
      
      CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name << " TID=[" << getThreadID() << "] "
		     << "Cannot query events for UserDN ["
		     << m_dn << "] CEUrl ["
		     << m_ce << "]. Exception soap_ex is [" << ex.what() << "]"
		     );
      return;

    } catch(cream_api::cream_exceptions::InternalException& ex) {
      
      CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name << " TID=[" << getThreadID() << "] "
		     << "Cannot query events for UserDN ["
		     << m_dn << "] CEUrl ["
		     << m_ce << "]. Exception Internal ex is [" << ex.what() << "]"
		     );
      
      boost::regex pattern;
      boost::cmatch what;
      pattern = ".*No such operation 'QueryEventRequest'.*";
      if( boost::regex_match(ex.what(), what, pattern) ) {
	CREAM_SAFE_LOG(m_log_dev->warnStream() << method_name << " TID=[" << getThreadID() << "] "
		     << "Not present QueryEvent on CE ["
		     << m_ce << "]. Falling back to old-style StatusPoller."
		     );

	iceCommandStatusPoller( m_iceManager, make_pair( m_dn, m_ce), false ).execute( m_thread_id );
      }

      return ;

    } catch(exception& ex) {
      
      CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name << " TID=[" << getThreadID() << "] "
		     << "Cannot query events for UserDN ["
		     << m_dn << "] CEUrl ["
		     << m_ce << "]. Exception ex is [" << ex.what() << "]"
		     );
      return;

    } catch(...) {
      
      CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name << " TID=[" << getThreadID() << "] "
		     << "Cannot query status job for UserDN ["
		     << m_dn << "] CEUrl ["
		     << m_ce << "]. Unknown Exception"
		     );
      return;
    }
    
    long long dbid = atoll(sdbid.c_str());
    long long olddbid;
    
    if( !this->checkDatabaseID( m_ce, dbid, olddbid ) ) {

      CREAM_SAFE_LOG(m_log_dev->warnStream() << method_name << " TID=[" << getThreadID() << "] "
		     << "*** CREAM [" << m_ce << "] HAS PROBABLY BEEN SCRATCHED. GOING TO ERASE"
		     << " ALL JOBS RELATED TO OLD DB_ID ["
		     << olddbid << "] ***"
		     );
      
      {
	db::SetEventIDForCE setter( m_ce, 0, "iceCommandEventQuery::execute" );
	db::Transaction tnx(false, false);
	tnx.execute( &setter );
      }
      
      list<CreamJob> toRemove;
      {
	list<pair<string, string> > clause;
	db::GetJobsByDbID getter( toRemove, olddbid, 
				  "iceCommandDelegationRenewal::renewAllDelegations" );
	db::Transaction tnx(false, false);
	tnx.execute( &getter );
      }
      
      list<CreamJob>::iterator jobit = toRemove.begin();
      for(  jobit = toRemove.begin(); jobit != toRemove.end(); ++jobit ) {
	jobit->set_status( cream_api::job_statuses::ABORTED );
	jobit->set_failure_reason( "CREAM'S database has been scratched and all its jobs have been lost" );

      }
      
      while( IceCore::instance()->get_ice_lblog_pool()->get_command_count() > 2 )
	sleep(2);
      
      IceCore::instance()->get_ice_lblog_pool()->add_request( new iceCommandLBLogging( toRemove ) );
      
      return;
    } // if( !this->checkDatabaseID..... )
    
    CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		   << "There're [" << events.size() << "] event(s) "
		   << "for the couple DN ["
		   << m_dn <<"] CEUrl [" 
		   << m_ce <<"]"
		   );

    CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		   << "Database ID=[" << sdbid << "]"
		   );
    
    CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		   << "Exec time ID=[" << exec_time << "]"
		   );
    
    if( events.size() ) {

      Url url( m_ce );
      string endpoint( url.get_schema() + "://" + url.get_endpoint() );

      long long last_event_id = this->processEvents( endpoint, events, make_pair( m_dn, m_ce ) );
      
      CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		     << "Setting new Event ID=[" << (last_event_id+1) << "] for user dn ["
		     << m_dn << "] ce url ["
		     << m_ce << "]"
		   );

      this->set_event_id( m_dn, m_ce, last_event_id+1 );
    }
}

//______________________________________________________________________________
long long 
ice::util::iceCommandEventQuery::getEventID( const string& dn, const string& ce)
{
  db::GetEventID getter( dn, ce, "iceCommandEventQuery::getEventID" );
  db::Transaction tnx(false, false);
  tnx.execute( &getter );
  if( getter.found() )
    return getter.getEventID();
  else
    return -1;
}

//______________________________________________________________________________
void 
ice::util::iceCommandEventQuery::set_event_id( const std::string& dn, 
					       const std::string& ce, 
					       const long long id)
{
  CREAM_SAFE_LOG(m_log_dev->debugStream() << "iceCommandEventQuery::set_event_id - " 
		 << " TID=[" << getThreadID() << "] "
		 << "Setting EventID for UserDN ["
		 << dn <<"] CEUrl ["
		 << ce << "] to ["
		 << id << "]"
		 );

  db::SetEventID setter( dn, ce, id, "iceCommandEventQuery::setEventID" );
  db::Transaction tnx(false, false);
  tnx.execute( &setter );
}

//______________________________________________________________________________
void 
ice::util::iceCommandEventQuery::getJobsByDbID( std::list<ice::util::CreamJob>& jobs, 
						const long long db_id )
{
  db::GetJobsByDbID getter( jobs, db_id, "iceCommandEventQuery::getJobsByDbID" );
  db::Transaction tnx(false, false);
  tnx.execute( &getter );
}

//______________________________________________________________________________
bool
ice::util::iceCommandEventQuery::checkDatabaseID( const string& ceurl,
						  const long long dbid,
						  long long& olddbid )
{
  db::GetDbID getter( ceurl, "iceCommandEventQuery::checkDatabaseID" );
  {
    db::Transaction tnx(false, false);
    tnx.execute( &getter);
  }
  if( !getter.found() ) {
    {
      /**
	 There's not yet association CEUrl -> DB_ID
	 Must be created ex-novo.
      */
      db::SetDbID setter( ceurl, dbid, "iceCommandEventQuery::checkDatabaseID" );
      db::Transaction tnx(false, false);
      tnx.execute( &setter );
    }
    return true;

  } else {
    /** 
	The DB_ID for CE does exist.
    */
    if( getter.getDbID() != dbid )
      {
        olddbid = getter.getDbID();
	/**
	   CREAM has been scratched because the 
	   previously saved DB_ID differs
	   from this one
	*/
	db::SetDbID setter( ceurl, dbid, "iceCommandEventQuery::checkDatabaseID" );
	db::Transaction tnx(false, false);
	tnx.execute( &setter );
	return false;
      }

    return true;
  }

  return true;
}

//______________________________________________________________________________
long long
ice::util::iceCommandEventQuery::processEvents( const std::string& endpoint, 
						list<soap_proxy::EventWrapper*>& events,
						const pair<string, string>& dnce )
{

  api_util::scoped_timer procTimeEvents( string("iceCommandEventQuery::processEvents() - TID=[") + getThreadID() + "] All Events Proc Time" );
  /**
     Group the events per GridJobID
  */
  map< string, list<soap_proxy::EventWrapper*> > mapped_events;

  long long last_id = 0;
  
  list<soap_proxy::EventWrapper*>::const_iterator it;// = events.begin();
  for(  it = events.begin();  it != events.end(); ++it ) {

    CREAM_SAFE_LOG( m_log_dev->debugStream() << "iceCommandEventQuery::processEvents - "  << " TID=[" << getThreadID() << "] "
		    << "Received Event ID=[" << (*it)->id.c_str() << "] for DN ["
		    << dnce.first << "] and CE URL ["
		    << dnce.second << "]"
		    );

    long long tmpid = atoll((*it)->id.c_str() ) ;
    
    if( tmpid > last_id )
      last_id = tmpid;
    
    mapped_events[ endpoint + "/" + (*it)->getPropertyValue("jobId") ].push_back( *it );

  }

  map< string, list<soap_proxy::EventWrapper*> >::const_iterator eit;// = mapped_events.begin();
  for( eit = mapped_events.begin(); eit != mapped_events.end();  ++eit ) {
    this->processEventsForJob( eit->first, eit->second );
  }

  return last_id;
}

//______________________________________________________________________________
void
ice::util::iceCommandEventQuery::processEventsForJob( const string& CID, 
						      const list<soap_proxy::EventWrapper*>& ev )
{
  

  static const char* method_name = "iceCommandEventQuery::processEventsForJob() - ";

  //if(GID=="N/A") return;

  {  //MUTEX FOR RESCHEDULE
    boost::recursive_mutex::scoped_lock M_reschedule( glite::wms::ice::util::CreamJob::s_reschedule_mutex );
    string ignore_reason;  
    CreamJob tmp_job;
    if( glite::wms::ice::util::IceUtils::ignore_job( CID, tmp_job, ignore_reason ) ) {
      CREAM_SAFE_LOG( m_log_dev->warnStream() << method_name  << " TID=[" << getThreadID() << "] "
      		      << "IGNORING EVENTS for CreamJobID ["
		      << CID << "] for reason: " << ignore_reason
		      );
		            
      return;
    }
  
   
  
  int num_events = ev.size();

  if( !num_events ) // this should not happen
    return;

  CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		 << "Processing [" << num_events << "] event(s) for Job ["
		 << tmp_job.describe() << "] userdn ["
		 << tmp_job.user_dn() << "] and ce url ["
		 << tmp_job.cream_address() << "]."
		 );
  
  list<soap_proxy::EventWrapper*>::const_iterator evt_it;// = ev.begin();
  int evt_counter = 0;
  for( evt_it = ev.begin(); evt_it != ev.end(); ++evt_it ) {
    
    CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		   << "EventID ["
		   << (*evt_it)->id << "] timestsamp ["
		   << (*evt_it)->timestamp << "]"
		 );
    
    bool removed = false;
    /**
       WRITE ON ICE's DATABASE ONLY IF THIS IS THE LAST EVENT
       FOR THE CURRENT JOB, i.e. if evt_counter == (num_events-1)
    */
    this->processSingleEvent( tmp_job, *evt_it, evt_counter == (num_events-1), removed );

    if(removed)
      return;
    
    ++evt_counter;
  }
  } // MUTEX FOR RESCHEDULE
}

//______________________________________________________________________________
void 
ice::util::iceCommandEventQuery::processSingleEvent( CreamJob& theJob, 
						     soap_proxy::EventWrapper* event,
						     const bool is_last_event,
						     bool& removed )
{
  api_util::scoped_timer procTimeEvents( string("iceCommandEventQuery::processSingleEvent - TID=[") + getThreadID() + "] Entire method" );
  static const char* method_name = "iceCommandEventQuery::processSingleEvent() - ";

  string jobdesc( theJob.describe() );

  cream_api::job_statuses::job_status status = (cream_api::job_statuses::job_status)atoi(event->getPropertyValue("type").c_str());

  string exit_code   = event->getPropertyValue("exitCode");
  string fail_reason = event->getPropertyValue("failureReason");
  string description = event->getPropertyValue("description");
  string worker_node = event->getPropertyValue("workerNode");

  theJob.set_worker_node( worker_node );

#ifdef GLITE_WMS_ICE_ENABLE_STATS
  {
    db::InsertStat inserter( time(0), event->timestamp,(short)status, "iceCommandEventQuery::processSingleEvent" );
    db::Transaction tnx(false, false);
    tnx.execute( &inserter );
  }
#endif

  if ( status == cream_api::job_statuses::PURGED ) {
    CREAM_SAFE_LOG(m_log_dev->warnStream() << method_name << " TID=[" << getThreadID() << "] "
		   << "Job [" << jobdesc
		   << "] is reported as PURGED. Removing from database"
		   ); 
    {
      if( theJob.proxy_renewable() )
	DNProxyManager::getInstance()->decrementUserProxyCounter( theJob.user_dn(), theJob.myproxy_address() );
      db::RemoveJobByGid remover( theJob.grid_jobid( ), "iceCommandEventQuery::processSingleEvent" );
      db::Transaction tnx(false, false);
      tnx.execute( &remover );
    }
    removed = true;
    return;
  }

  

  theJob.set_status( status );
  
  try {
    theJob.set_exit_code( boost::lexical_cast< int >( exit_code ) );
  } catch( boost::bad_lexical_cast & ) {
    theJob.set_exit_code( 0 );
  }
  //
  // See comment in normalStatusNotification.cpp
  //
  if ( status == cream_api::job_statuses::CANCELLED ) {
    theJob.set_failure_reason( description );
  } else {
    theJob.set_failure_reason( fail_reason );
  }

  /**
     WRITE ON ICE's DATABASE ONLY IF THIS IS THE LAST EVENT
     FOR THE CURRENT JOB
  */
  if(is_last_event) {
    CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		   << "Updating ICE's database for Job [" << jobdesc
		   << "] status = [" << cream_api::job_statuses::job_status_str[ status ] << "]"
		   << " exit_code = [" << exit_code << "]"
		   << " failure_reason = [" << fail_reason << "]"
		   << " description = [" << description << "]"
		   );

    //api_util::scoped_timer updatetimer( string("iceCommandEventQuery::processSingleEvent - TID=[") + getThreadID() + "] ICE DB Update" );
    
    db::UpdateJob updater( theJob, "iceCommandEventQuery::processSingleEvent");
    db::Transaction tnx(false, false);
    tnx.execute( &updater );
  }
  theJob.reset_change_flags( );
  
  /**
    In both cases ABORTED or DONE_FAILED must log to LB just "DONE_FAILED"
    the second parameter 'true' forces to log a DONE_FAILED event even when an ABORTED status has been detected
  */
  
  IceLBEvent* ev = iceLBEventFactory::mkEvent( theJob, true );
  if ( ev ) {
    
    bool log_with_cancel_seqcode = (theJob.status( ) == glite::ce::cream_client_api::job_statuses::CANCELLED) && (!theJob.cancel_sequence_code( ).empty( ));
	
    theJob = m_lb_logger->logEvent( ev, log_with_cancel_seqcode, true );

  }
  
  /**
     Let's check if the job must be purged or resubmitted
     only if a new status has been received
  */
  if(is_last_event) {
    //api_util::scoped_timer resubtimer( string("iceCommandEventQuery::processSingleEvent - TID=[") + getThreadID() + "] RESUBMIT_OR_PURGE_JOB" );
    removed = m_iceManager->resubmit_or_purge_job( &theJob );
  }
  else
    removed = false;
}

//______________________________________________________________________________
// void 
// ice::util::iceCommandEventQuery::deleteJobsByDN( void ) throw( )
// {

//   list< CreamJob > results;
//   {
//     list<pair<string, string> > clause;
//     clause.push_back( make_pair( util::CreamJob::user_dn_field(), m_dn ) );
    
//     db::GetJobs getter( clause, results, "iceCommandEventQuery::deleteJobsForDN" );
//     db::Transaction tnx( false, false );
//     tnx.execute( &getter );
//   }

//   list< CreamJob >::iterator jit;// = results.begin();
//   for( jit = results.begin(); jit != results.end(); ++jit ) {
// //  while( jit != results.end() ) {
//     jit->set_failure_reason( "Job Aborted because proxy expired." );
//     jit->set_status( cream_api::job_statuses::ABORTED ); 
//     jit->set_exit_code( 0 );
//     iceLBEvent* ev = iceLBEventFactory::mkEvent( *jit );
//     if ( ev ) {
//       m_lb_logger->logEvent( ev );
//     }
    
//     if( jit->proxy_renewable() )
//       DNProxyManager::getInstance()->decrementUserProxyCounter( jit->user_dn(), jit->myproxy_address() );
    
// //    ++jit;
//   }

//   {
//     db::RemoveJobByUserDN remover( m_dn, "iceCommandEventQuery::deleteJobsForDN" );
//     db::Transaction tnx( false, false );
//     tnx.execute( &remover );
//   }

// }

//______________________________________________________________________________
// bool 
// ice::util::iceCommandEventQuery::ignore_job( const string& CID, 
// 					     CreamJob& tmp_job, string& reason ) 
// {
//    
//   
//     glite::wms::ice::db::GetJobByCid getter( CID, "iceCommandEventQuery::processEventsForJob" );
//     glite::wms::ice::db::Transaction tnx(false, false);
//     tnx.execute( &getter );
//     if( !getter.found() )
//       {
// 	reason = "JobID disappeared from ICE database !"		       
// 	return true;
//       }
//     
//     tmp_job = getter.get_job();
//     
//     /**
//      * The following if is needed by the Feedback mechanism
//      */
//     if( tmp_job.cream_jobid( ).empty( ) ) {
//       reason = "CreamJobID is EMPTY";
//       return true;
//     }
//   
//   
//   string new_token;
//   if( !boost::filesystem::exists( boost::filesystem::path( tmp_job.token_file() ) )
//       && glite::wms::ice::util::IceUtils::exists_subsequent_token( tmp_job.token_file(), new_token ) ) 
//   {
//     reason = "Token file [";
//     reason += tmp_job.token_file();
//     reason += "] DOES NOT EXISTS but subsequent token [";
//     reason += new_token;
//     reason += "] does exist; the job could have been just reschedule by WM.";
//     return true;
//   }
// }
