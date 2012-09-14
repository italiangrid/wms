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
// ICE stuff
#include "eventStatusListener.h"
#include "iceUtils/iceUtils.h"
#include "ice/IceCore.h"
#include "iceCommand/iceCommandUpdateStatus.h"
#include "iceThreadPool.h"

// CREAM stuff
#include "glite/ce/cream-client-api-c/creamApiLogger.h"
//#include "glite/ce/cream-client-api-c/scoped_timer.h"

// other GLITE stuff
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"

// System includes
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include <sys/select.h>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
//#include <errno.h>

//extern int errno;
//extern int *__errno_location(void);
//#define errno (*__errno_location())

namespace api = glite::ce::cream_client_api;
using namespace glite::wms::ice::util;
using namespace std;

#define ACCEPT_TIMEOUT 30

void eventStatusListener::createObject( void )
{
    try {
        m_myname = utilities::getHostName( );
    } catch( runtime_error& ex ) {
        m_isOK = false;
        return;
    }
    

    CREAM_SAFE_LOG(m_log_dev->info( "eventStatusListener::createObject() - Listener created!" ));
    
}

//______________________________________________________________________________
eventStatusListener::eventStatusListener(const int& i)
    : CEConsumer(i),
      iceThread( "event status listener" ),
      m_conf(IceConfManager::instance()->getConfiguration()->ice()),
      m_log_dev( api::util::creamApiLogger::instance()->getLogger() ),
      m_isOK( true ),
      m_ice_manager( glite::wms::ice::Ice::instance() )
{
    createObject();
}

//______________________________________________________________________________
eventStatusListener::eventStatusListener(const int& i,
						  const string& cert,
						  const string& key)
  : CEConsumer(i, cert.c_str(), key.c_str()),
    iceThread( "event status listener" ),
    m_conf(IceConfManager::instance()->getConfiguration()->ice()),
    m_log_dev( api::util::creamApiLogger::instance()->getLogger() ),
    m_isOK( true ),
    m_ice_manager( glite::wms::ice::Ice::instance() )
{
    createObject();
}

//______________________________________________________________________________
void eventStatusListener::body( void )
{
    while( !isStopped() ) {
        if(!getenv("NO_LISTENER_MESS"))
            CREAM_SAFE_LOG(m_log_dev->infoStream() 
                           << "eventStatusListener::body() - "
                           << "Waiting for job status notification"
                           );
        acceptJobStatus();
        sleep(1);
    }
}

//______________________________________________________________________________
void eventStatusListener::acceptJobStatus(void)
{
  if( isStopped() ) {
    CREAM_SAFE_LOG(m_log_dev->infoStream()
		   << "eventStatusListener::acceptJobStatus() - "
		   << "eventStatusListener is ending."
		   );
    return;
  }
  /**
   * Waits for an incoming connection
   */
  int localSocket = this->getLocalSocket();
  fd_set fds;
  struct timeval timeout;
  //sigset_t sig;
  int ret;
  FD_ZERO(&fds);
  FD_SET(localSocket, &fds);
  timeout.tv_sec = ACCEPT_TIMEOUT;
  timeout.tv_usec = 0;
  ret = select(localSocket+1, &fds, NULL, NULL, &timeout);

  if (ret == -1) {
    CREAM_SAFE_LOG(m_log_dev->errorStream()
		   << "eventStatusListener::acceptJobStatus() - "
		   << "select() error: "
		   << strerror(errno)
		   );
    //perror("select()");
    return;
  }
  else if (ret) { /* will do actualy the this->accept() call */ 
    CREAM_SAFE_LOG(m_log_dev->debugStream()
		   << "eventStatusListener::acceptJobStatus() - "
		   << "Listening socket received something to read: ready to accept a SOAP connection"
		   );
  }
    //printf("Data is available now.\n");
    /* FD_ISSET(0, &rfds) will be true. */
  else {
    //printf("No data within five seconds.\n");
    CREAM_SAFE_LOG(m_log_dev->debugStream()
		   << "eventStatusListener::acceptJobStatus() - "
		   << "No connection within " << ACCEPT_TIMEOUT << " seconds. Listening again."
		   );
    return;
  }

    if( !this->accept() ) {
      //if(!getenv("NO_LISTENER_MESS"))
	CREAM_SAFE_LOG(m_log_dev->errorStream()
		       << "eventStatusListener::acceptJobStatus()"
		       << " - CEConsumer::Accept() returned false."
		       );
      return;
    }

    if(!getenv("NO_LISTENER_MESS"))
      CREAM_SAFE_LOG(m_log_dev->infoStream()
		     << "eventStatusListener::acceptJobStatus() - "
		     << "Connection accepted from ["
		     << this->getClientName() << "] (" << this->getClientIP() << ")"
		     );

  /**
   * acquires the event from the client
   * and deserializes the data structures
   */
  if( !this->serve() ) {
    CREAM_SAFE_LOG(m_log_dev->errorStream() 
		   << "eventStatusListener::acceptJobStatus() - "
		   << "ErrorCode=["
		   << this->getErrorCode() << "]"
		   << " ErrorMessage=["
		   << this->getErrorMessage() << "]"
		   );
    return;
  }

  { // start TIMING notification handling

    //api::util::scoped_timer T("*** eventStatusListener::acceptJobStatus - Notification Receiving Time ***");

    if( m_conf->listener_enable_authz() && m_conf->listener_enable_authn() ) {
      string dn = this->getClientDN();
      if(!getenv("NO_LISTENER_MESS"))
	CREAM_SAFE_LOG(m_log_dev->infoStream()
		       << "eventStatusListener::acceptJobStatus() - "
		       << "Got DN ["
		       << dn << "] for host ["
		       << this->getClientName() << "]"
		       );
      
      /*if( !subscriptionManager::getInstance()->isAuthorized( dn ) ) {
	if(!getenv("NO_LISTENER_MESS"))
	  CREAM_SAFE_LOG(m_log_dev->warnStream() 
			 << "eventStatusListener::acceptJobStatus() - "
			 << "Remote notifying client DN has not been found is the"
			 << " subscriptionManager. Cannot authorize this notification. Ignoring it..." 
			 );
	return;
      }*/
    }
    
    /**
     * Checks if the SOAP message was correct (i.e.
     * containing a non-null topic)
     */
    if(!this->getEventTopic()) {
      CREAM_SAFE_LOG(m_log_dev->warnStream() 
		     << "eventStatusListener::acceptJobStatus() - "
		     << "NULL Topic received. Ignoring this notification...." 
		     );
      return;
    }
    
    /**
     * Ignores notifications generated by non-ICE topics
     */
    if(this->getEventTopic()->Name != m_conf->ice_topic()) {
      CREAM_SAFE_LOG(m_log_dev->warnStream() 
		     << "eventStatusListener::acceptJobStatus() - "
		     << "Received a notification with TopicName="
		     << this->getEventTopic()->Name
		     << " that differs from the ICE's topic "
		     << "official name. Ignoring this notification."
		     );
      return;
    }
    
    iceThreadPool* threadPool( m_ice_manager->get_ice_commands_pool() );

    //    int command_count = threadPool->get_command_count();

//     if( command_count  > (10*m_conf->max_ice_threads()) ) {
//       CREAM_SAFE_LOG(m_log_dev->debugStream()
//     		     << "eventStatusListener::acceptJobStatus() - Currently threadPool contains ["
// 		     << command_count
// 		     << "] requests still to be processed. Dropping notification..."
// 		     );
//     } else {

      const vector< monitortypes__Event >& events( getEvents() );    
      iceAbsCommand* cmd = new iceCommandUpdateStatus( events, getClientDN() );
      //delete cmd;
      threadPool->add_request( cmd ); // ownership of the cmd pointer is passed to the threadPool object
      //    }
  } // stop TIMING notification handling
}

