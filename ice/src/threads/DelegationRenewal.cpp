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
#include "main/Main.h"
#include "DelegationRenewal.h"
#include "utils/IceConfManager.h"
#include "commands/IceCommandDelegationRenewal.h"


/**
 *
 * CE and WMS Headers
 *
 */
#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "common/src/configuration/ICEConfiguration.h"

// boost includes
#include <boost/thread/thread.hpp>

// std includes
#include <vector>

/**
 *
 * System OS Headers
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace glite::wms::ice;
using namespace std;

//______________________________________________________________________________
threads::DelegationRenewal::DelegationRenewal() :
    IceThread( "ICE Proxy Renewer" ),
    m_log_dev( glite::ce::cream_client_api::util::creamApiLogger::instance()->getLogger() )
{
  m_delay = IceConfManager::instance()->getConfiguration()->ice()->proxy_renewal_frequency();
}


//______________________________________________________________________________
void threads::DelegationRenewal::body( void )
{
    while( !isStopped() ) {        
        CREAM_SAFE_LOG(m_log_dev->infoStream()
                       << "DelegationRenewal::body() - new iteration"
                       );
        
	IceCommandDelegationRenewal().execute( "" );

	if(m_delay<=10) 
	  sleep( m_delay );
	else {
	  
	  for(int i=0; i<=m_delay; i++) {
	    if( isStopped() ) return;
	    sleep(1);
	  }
	  
	}
	
        //sleep( m_delay );
    }
}

