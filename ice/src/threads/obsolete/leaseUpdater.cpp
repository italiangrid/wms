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

#include "leaseUpdater.h"
#include "iceConfManager.h"
#include "iceCommandLeaseUpdater.h"

#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"

using namespace glite::wms::ice::util;
using namespace std;

//____________________________________________________________________________
leaseUpdater::leaseUpdater( ) :
    iceThread( "ICE Lease Updater" ),
    m_delay( iceConfManager::getInstance()->getConfiguration()->ice()->lease_update_frequency() ),
    m_log_dev( glite::ce::cream_client_api::util::creamApiLogger::instance()->getLogger() )
{
   
}

//____________________________________________________________________________
leaseUpdater::~leaseUpdater( )
{

}

//______________________________________________________________________________
void leaseUpdater::body( void )
{
    while ( !isStopped() ) {
        CREAM_SAFE_LOG(m_log_dev->infoStream()
                       << "leaseUpdater::body() - new iteration"
                       );
	iceCommandLeaseUpdater().execute( "" );

	if(m_delay<=10) {
	  if( isStopped() ) return;
	  sleep( m_delay );
	}
	else {
	  
	  for(int i=0; i<=m_delay; i++) {
	    if( isStopped() ) return;
	    sleep(1);
	  }
	  
	}


	//        sleep( m_delay );
    }
}
