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
//#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/job_statuses.h"
#include "glite/ce/cream-client-api-c/scoped_timer.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "common/src/configuration/ICEConfiguration.h"

#include "IceCommandStatusPoller.h"
#include "IceCommandEventQuery.h"
#include "IceCommandLBLogging.h"
#include "utils/IceLBEventFactory.h"
#include "utils/CreamProxyMethod.h"
#include "utils/DNProxyManager.h"
#include "utils/IceConfManager.h"
#include "utils/IceLBLogger.h"
#include "utils/IceLBEvent.h"
#include "main/Main.h"
#include "utils/IceUtils.h"

#include "db/GetDbID.h"
#include "db/SetDbID.h"
#include "db/GetJobsByDN.h"
#include "db/DNHasJobs.h"
#include "db/InsertStat.h"
#include "db/GetEventID.h"
#include "db/SetEventID.h"
#include "db/SetEventIDForCE.h"
#include "db/GetJobByGid.h"
#include "db/GetJobByCid.h"
#include "db/Transaction.h"
#include "db/GetJobsByDbID.h"
#include "db/RemoveJobByGid.h"
#include "db/RemoveJobByCid.h"
#include "db/UpdateJob.h"
#include "db/RemoveJobsByDbID.h"
#include "db/RemoveJobByUserDN.h"

#include <boost/lexical_cast.hpp>
#include <boost/functional.hpp>
#include "boost/algorithm/string.hpp"
#include "boost/regex.hpp"
#include "boost/format.hpp"

#include "utils/logging.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

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
ice::util::IceCommandEventQuery::~IceCommandEventQuery( ) throw()
{
}

//______________________________________________________________________________
ice::util::IceCommandEventQuery::IceCommandEventQuery( ice::Main* theIce,
						       const std::string& dn,
						       const std::string& ce)
  : IceAbstractCommand( "IceCommandEventQuery", "" ),
    //m_log_dev( cream_api::util::creamApiLogger::instance()->getLogger() ),
    m_lb_logger( ice::util::IceLBLogger::instance() ),
    m_iceManager( theIce ),
    m_conf( ice::util::IceConfManager::instance() ),
    m_stopped( false ),
    m_dn( dn ),
    m_ce( ce )
{

}

//______________________________________________________________________________
std::string ice::util::IceCommandEventQuery::get_grid_job_id() const
{
  ostringstream randid( "" );
  struct timeval T;
  gettimeofday( &T, 0 );
  randid << T.tv_sec << "." << T.tv_usec;
  return randid.str();
}

//______________________________________________________________________________
void ice::util::IceCommandEventQuery::execute( const std::string& tid) throw()
{
 m_thread_id = tid;
  //static const char* method_name = "IceCommandEventQuery::execute - ";
  edglog_fn("IceCommandEventQuery::execute");
    list<soap_proxy::EventWrapper*> events;
    cleanup cleaner( &events );

    //CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		 edglog(debug)  << "Retrieving last Event ID for user dn ["
		   << m_dn << "] and ce url ["
		   << m_ce << "]..."<<endl;
		  // );

    long long thisEventID = this->getEventID( m_dn, m_ce );
    
    if( thisEventID == -1 ) {
    //  CREAM_SAFE_LOG(m_log_dev->warnStream() << method_name << " TID=[" << getThreadID() << "] "
		  edglog(warning)   << "Couldn't find any last Event ID for current couple "
		     << "userdn [" 
		     << m_dn << "] and ce url ["
		     << m_ce << "]. Inserting 0."<<endl;
		   //  );
      
      this->set_event_id( m_dn, m_ce, 0 );
      
      thisEventID = 0;

    } else {

     // CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		  edglog(debug)   << "Last Event ID for current couple "
		     << "userdn [" 
		     << m_dn << "] and ce url ["
		     << m_ce << "] is ["
		     << thisEventID << "]"<<endl;
		  //   );
    }
    
    boost::tuple<string, time_t, long long int> proxyinfo = DNProxyManager::getInstance()->getAnyBetterProxyByDN( m_dn );
    
    if ( proxyinfo.get<0>().empty() ) {
    
      // see BUG https://savannah.cern.ch/bugs/index.php?59453 
    
     // CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name << " TID=[" << getThreadID() << "] "
		   edglog(error)   << "A valid proxy file for DN [" << m_dn
		      << "] ce url ["
		      << m_ce << "] is not available. Skipping EventQuery. Going to remove all jobs of this user..."<<endl;
		      //);
      list< CreamJob > toRemove;
      {
    	db::GetJobsByDN getter( toRemove, m_dn, "IceCommandEventQuery::execute" );
    	db::Transaction tnx( false, false );
    	tnx.execute( &getter );
      }
      list< CreamJob >::iterator jit;
      for( jit = toRemove.begin(); jit != toRemove.end(); ++jit ) {
    	jit->set_failure_reason( "Job Aborted because proxy expired" );
    	jit->set_status( cream_api::job_statuses::ABORTED ); 
    	jit->set_exit_code( 0 );
      }
      while( Main::instance()->get_ice_lblog_pool()->get_command_count() > 2 )
        sleep(2);
      
      Main::instance()->get_ice_lblog_pool()->add_request( new IceCommandLBLogging( toRemove ) );
      
      return;
      
    }
    
    if ( proxyinfo.get<1>() < time(0) ) {
     // CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name << " TID=[" << getThreadID() << "] "
		  edglog(error)    << "The returned proxy ["
		      << proxyinfo.get<0>() << "] for DN [" << m_dn
		      << "] ce url ["
		      << m_ce << "] is not valid anymore. Skipping EventQuery. Going to remove all jobs of this user..."<<endl;
		     // );
      list< CreamJob > toRemove;
      {
    	list<pair<string, string> > clause;
    	clause.push_back( make_pair( util::CreamJob::user_dn_field(), m_dn ) );
    
    	//db::GetJobs getter( clause, toRemove, method_name );
	db::GetJobsByDN getter( toRemove, m_dn, "IceCommandEventQuery::execute" );
    	db::Transaction tnx( false, false );
    	tnx.execute( &getter );
      }
      list< CreamJob >::iterator jit;
      for( jit = toRemove.begin(); jit != toRemove.end(); ++jit ) {
    	jit->set_failure_reason( "Job Aborted because proxy expired" );
    	jit->set_status( cream_api::job_statuses::ABORTED ); 
    	jit->set_exit_code( 0 );
      }
      while( Main::instance()->get_ice_lblog_pool()->get_command_count() > 2 )
        sleep(2);
      
      Main::instance()->get_ice_lblog_pool()->add_request( new IceCommandLBLogging( toRemove ) );
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
   
  //  CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
                   edglog(debug)   << "Going to execute EventQuery for user DN ["
		      << m_dn << "] to CE ["
		      << m_ce << "] using proxy file ["
		      << proxyinfo.get<0>() << "]" <<endl;
                     // ); 
 
    try {
      api_util::scoped_timer Tot( string("IceCommandEventQuery::execute() - SOAP Connection for QueryEvent - ") + "TID=[" + getThreadID() + "]" );
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
      
  //    CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name << " TID=[" << getThreadID() << "] "
		   edglog(error)  << "Cannot query events for UserDN ["
		     << m_dn << "] CEUrl ["
		     << m_ce << "]. Exception auth_ex is [" << ex.what() << "]"<<endl;
		   //  );

      
      return;
      
    } catch(soap_proxy::soap_ex& ex) {
      
 //     CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name << " TID=[" << getThreadID() << "] "
		 edglog(error)    << "Cannot query events for UserDN ["
		     << m_dn << "] CEUrl ["
		     << m_ce << "]. Exception soap_ex is [" << ex.what() << "]"<<endl;
		    // );
      return;

    } catch(cream_api::cream_exceptions::InternalException& ex) {
      
     // CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name << " TID=[" << getThreadID() << "] "
		  edglog(error)   << "Cannot query events for UserDN ["
		     << m_dn << "] CEUrl ["
		     << m_ce << "]. Exception Internal ex is [" << ex.what() << "]"<<endl;
		    // );
      
      boost::regex pattern;
      boost::cmatch what;
      pattern = ".*No such operation 'QueryEventRequest'.*";
      if( boost::regex_match(ex.what(), what, pattern) ) {
	//CREAM_SAFE_LOG(m_log_dev->warnStream() << method_name << " TID=[" << getThreadID() << "] "
		  edglog(warning)   << "Not present QueryEvent on CE ["
		     << m_ce << "]. Falling back to old-style StatusPoller."<<endl;
		    // );

	IceCommandStatusPoller( m_iceManager, make_pair( m_dn, m_ce), false ).execute( m_thread_id );
      }

      return ;

    } catch(exception& ex) {
    //  
    //  CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name << " TID=[" << getThreadID() << "] "
		   edglog(error)  << "Cannot query events for UserDN ["
		     << m_dn << "] CEUrl ["
		     << m_ce << "]. Exception ex is [" << ex.what() << "]"<<endl;
		  //   );
      return;

    } catch(...) {
      
    //  CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name << " TID=[" << getThreadID() << "] "
		   edglog(error)  << "Cannot query status job for UserDN ["
		     << m_dn << "] CEUrl ["
		     << m_ce << "]. Unknown Exception"<<endl;
		  //   );
      return;
    }
    
    long long dbid = atoll(sdbid.c_str());
    long long olddbid;
    
    if( !this->checkDatabaseID( m_ce, dbid, olddbid ) ) {

    //  CREAM_SAFE_LOG(m_log_dev->warnStream() << method_name << " TID=[" << getThreadID() << "] "
		 edglog(warning)    << "*** CREAM [" << m_ce << "] HAS PROBABLY BEEN SCRATCHED. GOING TO ERASE"
		     << " ALL JOBS RELATED TO OLD DB_ID ["
		     << olddbid << "] ***"<<endl;
		    // );
      
      {
	db::SetEventIDForCE setter( m_ce, 0, "IceCommandEventQuery::execute" );
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
      
      while( Main::instance()->get_ice_lblog_pool()->get_command_count() > 2 )
	sleep(2);
      
      Main::instance()->get_ice_lblog_pool()->add_request( new IceCommandLBLogging( toRemove ) );
      
      return;
    } // if( !this->checkDatabaseID..... )
    
   // CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		edglog(debug)   << "There're [" << events.size() << "] event(s) "
		   << "for the couple DN ["
		   << m_dn <<"] CEUrl [" 
		   << m_ce <<"]"<<endl;
		 //  );

  //  CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		edglog(debug)   << "Database ID=[" << sdbid << "]"<<endl;
		//   );
    
   /// CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		edglog(debug)   << "Exec time ID=[" << exec_time << "]"<<endl;
		 //  );
    
    if( events.size() ) {

      Url url( m_ce );
      string endpoint( url.get_schema() + "://" + url.get_endpoint() );

      long long last_event_id = this->processEvents( endpoint, events, make_pair( m_dn, m_ce ) );
      
    //  CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		 edglog(debug)    << "Setting new Event ID=[" << (last_event_id+1) << "] for user dn ["
		     << m_dn << "] ce url ["
		     << m_ce << "]"<<endl;
		//   );

      this->set_event_id( m_dn, m_ce, last_event_id+1 );
    }
}

//______________________________________________________________________________
long long 
ice::util::IceCommandEventQuery::getEventID( const string& dn, const string& ce)
{
 // edglog_fn("IceCommandEventQuery::getEventID");
  db::GetEventID getter( dn, ce, "IceCommandEventQuery::getEventID" );
  db::Transaction tnx(false, false);
  tnx.execute( &getter );
  if( getter.found() )
    return getter.getEventID();
  else
    return -1;
}

//______________________________________________________________________________
void 
ice::util::IceCommandEventQuery::set_event_id( const std::string& dn, 
					       const std::string& ce, 
					       const long long id)
{
  edglog_fn("IceCommandEventQuery::set_event_id");
  //CREAM_SAFE_LOG(m_log_dev->debugStream() << "IceCommandEventQuery::set_event_id - " 
		edglog(debug) << " TID=[" << getThreadID() << "] "
		 << "Setting EventID for UserDN ["
		 << dn <<"] CEUrl ["
		 << ce << "] to ["
		 << id << "]"<<endl;
		// );

  db::SetEventID setter( dn, ce, id, "IceCommandEventQuery::setEventID" );
  db::Transaction tnx(false, false);
  tnx.execute( &setter );
}

//______________________________________________________________________________
void 
ice::util::IceCommandEventQuery::getJobsByDbID( std::list<ice::util::CreamJob>& jobs, 
						const long long db_id )
{
  //edglog_fn("IceCommandEventQuery::getJobsByDbID");
  db::GetJobsByDbID getter( jobs, db_id, "IceCommandEventQuery::getJobsByDbID" );
  db::Transaction tnx(false, false);
  tnx.execute( &getter );
}

//______________________________________________________________________________
bool
ice::util::IceCommandEventQuery::checkDatabaseID( const string& ceurl,
						  const long long dbid,
						  long long& olddbid )
{
  //edglog_fn("IceCommandEventQuery::checkDatabaseID");
  db::GetDbID getter( ceurl, "IceCommandEventQuery::checkDatabaseID" );
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
      db::SetDbID setter( ceurl, dbid, "IceCommandEventQuery::checkDatabaseID" );
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
	db::SetDbID setter( ceurl, dbid, "IceCommandEventQuery::checkDatabaseID" );
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
ice::util::IceCommandEventQuery::processEvents( const std::string& endpoint, 
						list<soap_proxy::EventWrapper*>& events,
						const pair<string, string>& dnce )
{
  edglog_fn("IceCommandEventQuery::processEvents");
  api_util::scoped_timer procTimeEvents( string("IceCommandEventQuery::processEvents() - TID=[") + getThreadID() + "] All Events Proc Time" );
  /**
     Group the events per GridJobID
  */
  map< string, list<soap_proxy::EventWrapper*> > mapped_events;

  long long last_id = 0;
  
  list<soap_proxy::EventWrapper*>::const_iterator it;// = events.begin();
  for(  it = events.begin();  it != events.end(); ++it ) {

   // CREAM_SAFE_LOG( m_log_dev->debugStream() << "IceCommandEventQuery::processEvents - "  << " TID=[" << getThreadID() << "] "
		edglog(debug)    << "Received Event ID=[" << (*it)->id.c_str() << "] for DN ["
		    << dnce.first << "] and CE URL ["
		    << dnce.second << "]"<<endl;
		  //  );

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
ice::util::IceCommandEventQuery::processEventsForJob( const string& CID, 
						      const list<soap_proxy::EventWrapper*>& ev )
{
  edglog_fn("IceCommandEventQuery::processEventsForJob");

  //static const char* method_name = "IceCommandEventQuery::processEventsForJob() - ";

  //if(GID=="N/A") return;

  {  //MUTEX FOR RESCHEDULE
    boost::recursive_mutex::scoped_lock M_reschedule( glite::wms::ice::util::CreamJob::s_reschedule_mutex );
    string ignore_reason;  
    CreamJob tmp_job;
    if( glite::wms::ice::util::IceUtils::ignore_job( CID, tmp_job, ignore_reason ) ) {
    //  CREAM_SAFE_LOG( m_log_dev->warnStream() << method_name  << " TID=[" << getThreadID() << "] "
      		edglog(warning)      << "IGNORING EVENTS for CreamJobID ["
		      << CID << "] for reason: " << ignore_reason<<endl;
		//      );
		            
      return;
    }
  
   
  
  int num_events = ev.size();

  if( !num_events ) // this should not happen
    return;

 // CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		edglog(debug) << "Processing [" << num_events << "] event(s) for Job ["
		 << tmp_job.describe() << "] userdn ["
		 << tmp_job.user_dn() << "] and ce url ["
		 << tmp_job.cream_address() << "]."<<endl;
		// );
  
  list<soap_proxy::EventWrapper*>::const_iterator evt_it;// = ev.begin();
  int evt_counter = 0;
  for( evt_it = ev.begin(); evt_it != ev.end(); ++evt_it ) {
    
  //  CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		 edglog(debug)  << "EventID ["
		   << (*evt_it)->id << "] timestsamp ["
		   << (*evt_it)->timestamp << "]"<<endl;
	//	 );
    
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
ice::util::IceCommandEventQuery::processSingleEvent( CreamJob& theJob, 
						     soap_proxy::EventWrapper* event,
						     const bool is_last_event,
						     bool& removed )
{
  edglog_fn("IceCommandEventQuery::processSingleEvent");
  api_util::scoped_timer procTimeEvents( string("IceCommandEventQuery::processSingleEvent - TID=[") + getThreadID() + "] Entire method" );
  //static const char* method_name = "IceCommandEventQuery::processSingleEvent() - ";

  string jobdesc( theJob.describe() );

  cream_api::job_statuses::job_status status = (cream_api::job_statuses::job_status)atoi(event->getPropertyValue("type").c_str());

  string exit_code   = event->getPropertyValue("exitCode");
  string fail_reason = event->getPropertyValue("failureReason");
  string description = event->getPropertyValue("description");
  string worker_node = event->getPropertyValue("workerNode");

  theJob.set_worker_node( worker_node );

#ifdef GLITE_WMS_ICE_ENABLE_STATS
  {
    db::InsertStat inserter( time(0), event->timestamp,(short)status, "IceCommandEventQuery::processSingleEvent" );
    db::Transaction tnx(false, false);
    tnx.execute( &inserter );
  }
#endif

  if ( status == cream_api::job_statuses::PURGED ) {
 //   CREAM_SAFE_LOG(m_log_dev->warnStream() << method_name << " TID=[" << getThreadID() << "] "
		 edglog(warning)  << "Job [" << jobdesc
		   << "] is reported as PURGED. Removing from database"<<endl;
	//	   ); 
    {
      if( theJob.proxy_renewable() )
	DNProxyManager::getInstance()->decrementUserProxyCounter( theJob.user_dn(), theJob.myproxy_address() );
      db::RemoveJobByGid remover( theJob.grid_jobid( ), "IceCommandEventQuery::processSingleEvent" );
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
    //CREAM_SAFE_LOG(m_log_dev->debugStream() << method_name << " TID=[" << getThreadID() << "] "
		edglog(debug)   << "Updating ICE's database for Job [" << jobdesc
		   << "] status = [" << cream_api::job_statuses::job_status_str[ status ] << "]"
		   << " exit_code = [" << exit_code << "]"
		   << " failure_reason = [" << fail_reason << "]"
		   << " description = [" << description << "]"<<endl;
		  // );

    //api_util::scoped_timer updatetimer( string("IceCommandEventQuery::processSingleEvent - TID=[") + getThreadID() + "] ICE DB Update" );
    
    db::UpdateJob updater( theJob, "IceCommandEventQuery::processSingleEvent");
    db::Transaction tnx(false, false);
    tnx.execute( &updater );
  }
  theJob.reset_change_flags( );
  
  /**
    In both cases ABORTED or DONE_FAILED must log to LB just "DONE_FAILED"
    the second parameter 'true' forces to log a DONE_FAILED event even when an ABORTED status has been detected
  */
  
  IceLBEvent* ev = IceLBEventFactory::mkEvent( theJob, true );
  if ( ev ) {
    
    bool log_with_cancel_seqcode = (theJob.status( ) == glite::ce::cream_client_api::job_statuses::CANCELLED) && (!theJob.cancel_sequence_code( ).empty( ));
	
    theJob = m_lb_logger->logEvent( ev, log_with_cancel_seqcode, true );

  }
  
  /**
     Let's check if the job must be purged or resubmitted
     only if a new status has been received
  */
  if(is_last_event) {
    //api_util::scoped_timer resubtimer( string("IceCommandEventQuery::processSingleEvent - TID=[") + getThreadID() + "] RESUBMIT_OR_PURGE_JOB" );
    removed = m_iceManager->resubmit_or_purge_job( &theJob );
  }
  else
    removed = false;
}

//______________________________________________________________________________
// void 
// ice::util::IceCommandEventQuery::deleteJobsByDN( void ) throw( )
// {

//   list< CreamJob > results;
//   {
//     list<pair<string, string> > clause;
//     clause.push_back( make_pair( util::CreamJob::user_dn_field(), m_dn ) );
    
//     db::GetJobs getter( clause, results, "IceCommandEventQuery::deleteJobsForDN" );
//     db::Transaction tnx( false, false );
//     tnx.execute( &getter );
//   }

//   list< CreamJob >::iterator jit;// = results.begin();
//   for( jit = results.begin(); jit != results.end(); ++jit ) {
// //  while( jit != results.end() ) {
//     jit->set_failure_reason( "Job Aborted because proxy expired." );
//     jit->set_status( cream_api::job_statuses::ABORTED ); 
//     jit->set_exit_code( 0 );
//     iceLBEvent* ev = IceLBEventFactory::mkEvent( *jit );
//     if ( ev ) {
//       m_lb_logger->logEvent( ev );
//     }
    
//     if( jit->proxy_renewable() )
//       DNProxyManager::getInstance()->decrementUserProxyCounter( jit->user_dn(), jit->myproxy_address() );
    
// //    ++jit;
//   }

//   {
//     db::RemoveJobByUserDN remover( m_dn, "IceCommandEventQuery::deleteJobsForDN" );
//     db::Transaction tnx( false, false );
//     tnx.execute( &remover );
//   }

// }

//______________________________________________________________________________
// bool 
// ice::util::IceCommandEventQuery::ignore_job( const string& CID, 
// 					     CreamJob& tmp_job, string& reason ) 
// {
//    
//   
//     glite::wms::ice::db::GetJobByCid getter( CID, "IceCommandEventQuery::processEventsForJob" );
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
