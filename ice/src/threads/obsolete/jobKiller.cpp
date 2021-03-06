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
#include "jobKiller.h"
#include "iceCommandJobKill.h"

// GLITE stuff
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"

using namespace std;
using namespace glite::wms::ice::util;

//____________________________________________________________________________
jobKiller::jobKiller() : 
    iceThread( "Job Killer" ),
    m_valid( true ),
    m_threshold_time( iceConfManager::getInstance()->getConfiguration()->ice()->job_cancellation_threshold_time())
{
    if ( m_threshold_time < 60 ) 
        m_threshold_time = 60;
    m_delay = m_threshold_time/2;
}

//____________________________________________________________________________
jobKiller::~jobKiller()
{

}

//____________________________________________________________________________
void jobKiller::body()
{
    log4cpp::Category* log_dev( glite::ce::cream_client_api::util::creamApiLogger::instance()->getLogger() );
    static const char* method_name = "jobKiller::body() - ";
    
    while ( !isStopped() ) {

      if(m_delay<=10) 
        sleep( m_delay );
      else {
	
        for(int i=0; i<=m_delay; i++) {
          if( isStopped() ) return;
          sleep(1);
        }
	
      }
      
      
      //sleep( m_delay );
        CREAM_SAFE_LOG( log_dev->infoStream() << method_name
                        << "New iteration"
                         );
        iceCommandJobKill( ).execute();
    }
}
