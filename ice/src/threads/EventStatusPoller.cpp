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
// ICE includes
#include "main/Main.h"
#include "EventStatusPoller.h"
#include "commands/IceCommandStatusPoller.h"
#include "commands/IceCommandEventQuery.h"
#include "utils/logging.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#include "db/GetAllDN.h"
#include "db/GetCEUrl.h"
#include "db/Transaction.h"
#include "db/DNHasJobs.h"

// other glite includes
//#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "common/src/configuration/ICEConfiguration.h"

// boost includes
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/functional.hpp>

#include <algorithm>
#include <cstdlib>

#include <csignal>

namespace cream_api     = glite::ce::cream_client_api;
namespace configuration = glite::wms::common::configuration;
using namespace glite::wms::ice;
using namespace std;

//boost::recursive_mutex EventStatusPoller::s_proxymutex;

//____________________________________________________________________________
threads::EventStatusPoller::EventStatusPoller( glite::wms::ice::Main* manager, int d )
    : IceThread( "event status poller" ),
      m_delay( d ),
      m_iceManager( manager ),
      //m_log_dev( cream_api::util::creamApiLogger::instance()->getLogger() ),
      m_threadPool( manager->get_ice_commands_pool() )
{
  edglog_fn("EventStatusPoller::EventStatusPoller");
  sigset_t set;
  ::sigemptyset(&set);
  ::sigaddset(&set, SIGCHLD);
  if(::pthread_sigmask( SIG_BLOCK, &set, 0 ) < 0 ) 
    //CREAM_SAFE_LOG( m_log_dev->fatalStream() << "EventStatusPoller::CTOR"
  	                               edglog(fatal)      << "pthread_sigmask failed. This could compromise correct working"
                	                     << " of ICE's threads..."<<endl;// );
             
}

//____________________________________________________________________________
threads::EventStatusPoller::~EventStatusPoller()
{

}

//____________________________________________________________________________
void threads::EventStatusPoller::body( void )
{
 edglog_fn("EventStatusPoller::body");
  while( !isStopped() ) {
    
    /**
     * We don't use boost::thread::sleep because right now
     * (18/11/2005) the documentation says it will be replaced by
     * a more robust mechanism in the future.
     */
    if(m_delay<=10) 
      sleep( m_delay );
    else {
      
      for(int i=0; i<=m_delay; i++) {
	if( isStopped() ) return;
	sleep(1);
      }
      
    }
    
    // Thread wakes up
    
    //CREAM_SAFE_LOG( m_log_dev->infoStream()
		   edglog(info) << "EventStatusPoller::body - New iteration" << endl;
		   // );
    
    set< string > dns;
    {
      db::GetAllDN getter( dns, "EventStatusPoller::body" );
	
      db::Transaction tnx(false, false);
      tnx.execute( &getter ); 
    }
    
    set< string > ces;
    {
      db::GetCEUrl getter( ces, "EventStatusPoller::body" );
	
      db::Transaction tnx(false, false);
      tnx.execute( &getter ); 
    }

    set<string>::const_iterator ceit;// = ces.begin();
    
    //for( ceit = ces.begin(); ceit != ces.end(); ++ceit ) {
    
    set<string>::const_iterator dnit;// = dns.begin();
    
    for( dnit = dns.begin(); dnit != dns.end(); ++dnit ) {
    
      if(  dnit->empty() ) {
//	CREAM_SAFE_LOG(m_log_dev->debugStream() << "EventStatusPoller::body - "
		     edglog(debug)  << "Empty DN string! Skipping..." << endl;
		       //);
	continue;// next CE
      }
      
      for( ceit = ces.begin(); ceit != ces.end(); ++ceit ) {
      //for( dnit = dns.begin(); dnit != dns.end(); ++dnit ) {
	
        if( ceit->empty() ) {
       //   CREAM_SAFE_LOG(m_log_dev->debugStream() << "EventStatusPoller::body - "
			edglog(debug) << "Empty CE string! Skipping... " << endl;
			// );
	  continue; // next DN
        }
	
        {
          db::DNHasJobs hasjob( *dnit, *ceit, "EventStatusPoller::body" );
          db::Transaction tnx(false, false);
          tnx.execute( &hasjob );
          if( !hasjob.found( ) ) {
           // CREAM_SAFE_LOG(m_log_dev->debugStream() << "EventStatusPoller::body - "
		    edglog(debug)       << "DN [" 
		           << *dnit << "] has not job on the CE ["
			   << *ceit << "] in the ICE's database at the moment. Skipping query..."<<endl;
		        //   );
	    continue;
          }
        }
      
        while( m_threadPool->get_command_count( ) >= 10 ) {
	  //CREAM_SAFE_LOG( m_log_dev->debugStream()
	  		 edglog(debug) << "EventStatusPoller::body - "
			  << "Too many commands in the queue. Waiting 10 seconds..." << endl;
			  //);
	  sleep( 10 );
	}
      
        //CREAM_SAFE_LOG( m_log_dev->debugStream()
 			 edglog(debug)   << "EventStatusPoller::body - "
 			    << "Adding EventQuery command for couple (" 
 			    << *dnit << ", "
 			    << *ceit << ") to the thread pool..." << endl;
 			   // );
      
        m_threadPool->add_request( new IceCommandEventQuery( m_iceManager, *dnit , *ceit ) );
      
        while( m_threadPool->get_command_count( ) > 0 )
	  sleep(5);
      }
    }

  }
}
