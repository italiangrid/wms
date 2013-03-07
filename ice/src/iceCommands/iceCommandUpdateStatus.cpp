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
 * ICE Headers
 *
 */
#include "iceCommandUpdateStatus.h"
#include "emptyStatusNotification.h"
#include "normalStatusNotification.h"
#include "iceLBLogger.h"
#include "iceLBEventFactory.h"
#include "iceUtils.h"
#include "ice-core.h"
//#include "subscriptionManager.h"
#include "DNProxyManager.h"
#include "iceDb/GetJobByCid.h"
#include "iceDb/Transaction.h"
#include "iceDb/GetCreamURLUserDN.h"
#include "iceDb/GetFields.h"

// CREAM stuff
#include "glite/ce/cream-client-api-c/creamApiLogger.h"

// other gLite stuff
#include "classad_distribution.h"

// boost includes
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/string.hpp>

// System includes
#include <iostream>

// STL stuff
#include <set>
#include <list>

namespace api = glite::ce::cream_client_api;
using namespace glite::wms::ice::util;
using namespace std;

iceCommandUpdateStatus::iceCommandUpdateStatus( const vector<monitortypes__Event>& ev, const string& cemondn ) :
    iceAbsCommand( "iceCommandUpdateStatus", "" ),
    m_log_dev( api::util::creamApiLogger::instance()->getLogger() ),
    m_ev( ev ),
    m_cemondn( cemondn )
{
    static const char* method_name = "iceCommandUpdateStatus::iceCommandUpdateStatus() - ";
    if ( m_log_dev->isInfoEnabled() ) {
        // Dumps the content of the notification
        vector< monitortypes__Event >::const_iterator it;
        for ( it = m_ev.begin(); m_ev.end() != it; ++it ) {
            CREAM_SAFE_LOG(m_log_dev->debugStream()
                           << method_name 
                           << "Message dump follows"
                           );

            vector< string >::const_iterator msg_it;
            for ( msg_it = it->Message.begin(); 
                  msg_it != it->Message.end(); msg_it++ ) {
                CREAM_SAFE_LOG(m_log_dev->debugStream()
                               << method_name 
                               << "Got notification: "
                               << boost::replace_all_copy( *msg_it, "\n", " " )
                               );
            }
        }
    }
}

//____________________________________________________________________________
void iceCommandUpdateStatus::execute( const std::string& tid) throw( )
{   
  //    jobCache *cache( jobCache::getInstance() );

  m_thread_id = tid;

    static const char* method_name = "iceCommandUpdateStatus::execute() - ";

    // We define two "sets" of operations which must be performed by 
    // the status change notifications.
 
    set< string > subscription_set; // The set of subscription IDs for
                                    // which an empty status change
                                    // notification has been received

    list< absStatusNotification* > commands; // The set of commands
                                             // which will be executed
                                             // to update the jobs
                                             // according to the
                                             // content of each
                                             // notification received.

    //
    // This method operates in two passes.
    // 
    // During pass 1. we scan the vector of monitortypes__Event. For
    // each event: 
    //
    // 1a: if it is a "normal" (i.e., non-empty status notification),
    // we create an appropriate normalStatusNotification object which
    // will update the status of that specific job, and push that
    // object in the commands list.
    //
    // 1b: if it is an "empty" status notification, then we insert in
    // the subscription_set set the pair (subscription_id, cemondn),
    // so that each pair will appear only once in the set.
    //

      
      vector< monitortypes__Event >::const_iterator it;
      for ( it = m_ev.begin(); m_ev.end() != it; ++it ) {
	
        if( it->Message.empty() ) {        
	  CREAM_SAFE_LOG( m_log_dev->warnStream()
			  << method_name 
			  << "got a CEMon notification with no messages. Skipping."
			  );        
            continue; // Skip to the next notification
        }

        CREAM_SAFE_LOG( m_log_dev->debugStream()
                        << method_name
                        << "processing notification, with "
                        << it->Message.size() << " events"
                        );
        
        string first_event( *(it->Message.begin()) );
        string printable_first_event( boost::replace_all_copy( first_event, "\n", " " ) );

	bool has_keep_alive;
	bool has_subid;
	bool keep_alive;
	string subs_id;
	{ // Classad-mutex protected region

	  boost::recursive_mutex::scoped_lock M_classad( util::CreamJob::s_classad_mutex );

	  classad::ClassAdParser parser;
	  classad::ClassAd *ad = parser.ParseClassAd( first_event );
	  
	  if ( !ad ) {
	    CREAM_SAFE_LOG(m_log_dev->errorStream()
			   << method_name
			   << "Cannot parse notification classad "
			   << printable_first_event
			   );
	    continue; // Skip to the next notification
	  }
	  
	  // Classad gets destroyed when exiting the scope        
	  boost::scoped_ptr< classad::ClassAd > classad_safe_ptr( ad );
	  
	  keep_alive = false;
	  //string subs_id;
        

	  // Check whether the current notification is an empty notification
	  has_keep_alive = classad_safe_ptr->EvaluateAttrBool( "KEEP_ALIVE", keep_alive );
	  has_subid      = classad_safe_ptr->EvaluateAttrString( "SUBSCRIPTION_ID", subs_id );
	} // end of Classad-mutex protected region

        if ( has_keep_alive && keep_alive && has_subid ) {
            // Push the subs_id into the set, which will be considered
            // after all the notifications have been examined.
            subscription_set.insert( subs_id );
            
        } else {
            // Push back a normalStatusNotification, which will update
            // the status of the job referenced in the monitor event
            normalStatusNotification* notif = 0;
            try {
                notif = new normalStatusNotification( *it, m_cemondn );
            } catch( ... ) {
                CREAM_SAFE_LOG( m_log_dev->errorStream()
                                << method_name
                                << "Unable to make normal status notification for notification "
                                << printable_first_event
                                << " Skipping to the next notification"
                                );
                continue;
            }

            commands.push_front( notif );

            // Gets the job which is mentioned in the notification
	    //            boost::recursive_mutex::scoped_lock L( jobCache::mutex );    
	    //boost::recursive_mutex::scoped_lock L( CreamJob::globalICEMutex );
           
            //jobCache::iterator job_it( cache->lookupByCompleteCreamJobID( notif->get_complete_cream_job_id() ) );
	    CreamJob theJob;
	    {
	      db::GetJobByCid getter( notif->get_complete_cream_job_id(), "iceCommandUpdateStatus::execute" );
	      db::Transaction tnx(false, false);
	      //tnx.begin( );
	      tnx.execute( &getter );
	      if( !getter.found() ) 
		{
		  CREAM_SAFE_LOG( m_log_dev->warnStream()
				  << method_name
				  << "Job with CREAM job id ["
				  << notif->get_complete_cream_job_id()
				  << "] was not found in the cache. Cannot update "
				  << "the set of subscriptions the jobs belongs to"
				  );
		} else {
		theJob = getter.get_job();
		// Gets the subscription ID which is used to get
                // notifications associated with that job.
		
		iceSubscription subscription;
		string cemon_url;
		string proxy = DNProxyManager::getInstance()->getAnyBetterProxyByDN( theJob.get_user_dn() ).get<0>();
		//subscriptionManager::getInstance()->getCEMonURL(proxy, theJob.get_cream_address(), cemon_url);
		//subscriptionManager::getInstance()->getSubscriptionByDNCEMon( theJob.get_user_dn(), cemon_url, subscription );
                string subs_id( subscription.getSubscriptionID()/*job_it->getSubscriptionID()*/ );
		
                // Then, push the pair (subs_id, cemondn) into the
                // set.  This is necessary, as a normal status
                // notification should also update all the jobs of
                // that subs_id, in the exact same way as empty status
                // notifications do.
                CREAM_SAFE_LOG( m_log_dev->debugStream()
                                << method_name
                                << "Normal status notification for job "
                                << theJob.describe()
                                << " requires adding subscription id "
                                << subs_id
                                << " to the set of subscriptions whose jobs "
                                << "will be updated"
                                );
		
                subscription_set.insert( subs_id );
	      }
	    }
            
//             if ( cache->end() != job_it ) {

//                 // Gets the subscription ID which is used to get
//                 // notifications associated with that job.
		
// 		iceSubscription subscription;
// 		string cemon_url;
// 		string proxy = DNProxyManager::getInstance()->getAnyBetterProxyByDN( job_it->getUserDN() ).get<0>();
// 		subscriptionManager::getInstance()->getCEMonURL(proxy, job_it->getCreamURL(), cemon_url);
// 		subscriptionManager::getInstance()->getSubscriptionByDNCEMon( job_it->getUserDN(), cemon_url, subscription );
//                 string subs_id( subscription.getSubscriptionID()/*job_it->getSubscriptionID()*/ );

//                 // Then, push the pair (subs_id, cemondn) into the
//                 // set.  This is necessary, as a normal status
//                 // notification should also update all the jobs of
//                 // that subs_id, in the exact same way as empty status
//                 // notifications do.
//                 CREAM_SAFE_LOG( m_log_dev->debugStream()
//                                 << method_name
//                                 << "Normal status notification for job "
//                                 << job_it->describe()
//                                 << " requires adding subscription id "
//                                 << subs_id
//                                 << " to the set of subscriptions whose jobs "
//                                 << "will be updated"
//                                 );

//                 subscription_set.insert( subs_id );
//             } else {
//                 CREAM_SAFE_LOG( m_log_dev->warnStream()
//                                 << method_name
//                                 << "Job with CREAM job id ["
//                                 << notif->get_complete_cream_job_id()
//                                 << "] was not found in the cache. Cannot update the set of subscriptions the jobs belongs to"
//                                 );
//             }
        }
    }

    // At this point, we complete the list of commands to add also the
    // processing of empty status notifications. We do this by
    // checking al the jobs in the cache.
    {
      //boost::recursive_mutex::scoped_lock L( jobCache::mutex );    
      //      boost::recursive_mutex::scoped_lock L( CreamJob::globalICEMutex );
      

      /**
	 SELECT DISTINCT creamurl,userdn FROM jobs;
      */

      list< pair<string, string> > list_creamurl_userdn, save;
      
      {
 	list<string> params;
	params.push_back( "creamurl" );
	params.push_back( "userdn" );
	list< vector<string> > result;
	db::GetFields getter( params, list<pair<string, string> >(), result, "iceCommandUpdateStatus::execute",true/* use DISTINCT = true */ );
        {
	  db::Transaction tnx(false, false);
	  tnx.execute( &getter );
	}

        for( list< vector<string> >::const_iterator it=result.begin();
             it != result.end();
	     ++it )
	{
	  string creamurl = it->at(0);
	  string userdn   = it->at(1);

	  list_creamurl_userdn.push_back( make_pair( creamurl, userdn ) );
	}	

      }

      for( list< pair<string, string> >::const_iterator it=list_creamurl_userdn.begin();
	   it != list_creamurl_userdn.end();
	   ++it)
	{
	  string proxy = DNProxyManager::getInstance()->getAnyBetterProxyByDN( it->second ).get<0>();
	  iceSubscription subscription;
	  string cemon_url;
	  subscriptionManager::getInstance()->getCEMonURL(proxy, it->first, cemon_url);
	  subscriptionManager::getInstance()->getSubscriptionByDNCEMon( it->second, cemon_url, subscription );
	  string subs_id( subscription.getSubscriptionID() );
	  
	  if ( subscription_set.end() !=  subscription_set.find( subs_id ) ) {
	    save.push_back( *it );
	  }
	}


      list<string> cids;
      for( list< pair<string, string> >::const_iterator it=save.begin();
	   it != save.end();
	   ++it) 
	{
	  list<string> fields_to_retrieve;
	  fields_to_retrieve.push_back( "complete_cream_jobid" );
	  list<pair<string, string> > clause;
	  clause.push_back( make_pair("creamurl", it->first ) );
	  clause.push_back( make_pair("userdn", it->second ) );
	  
	  //list<list<string> > result;
	  //db::GetCidByCreamURLUserDN getter( *it );
	  list< vector< string> > tmp;
	  db::GetFields getter( fields_to_retrieve, clause, tmp, "iceCommandUpdateStatus::execute" );
	  db::Transaction tnx(false, false);
	  tnx.execute( &getter );
	  
	  //	  list< vector< string> > tmp = getter.get_values();
	  list< string > _tmp_cids;
	  for(list< vector< string> >::const_iterator it=tmp.begin();
	      it!=tmp.end();
	      ++it)
	      {
	        string thisCompleteCreamJobID = it->at(0);
	        _tmp_cids.push_back( thisCompleteCreamJobID );
	      }
	  
	  cids.merge( _tmp_cids );
	}
      
      for( list<string>::const_iterator it=cids.begin();
	   it != cids.end();
	   ++it)
	{
	  commands.push_front( new emptyStatusNotification( *it ) );
	}
      
    } // end block, unlock global mutex

    // Now executes all the commands in the list
      while( ! commands.empty() ) {
        boost::scoped_ptr< absStatusNotification > elem( commands.front() );
        commands.pop_front();
        try {
	  elem->apply();
        } catch( ... ) {
	  // FIXME
        }
      }
    }

