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

#include "subscriptionUpdater.h"
#include "iceCommandSubUpdater.h"
#include "iceUtils.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"

namespace iceUtil = glite::wms::ice::util;
namespace api = glite::ce::cream_client_api;
using namespace std;

//______________________________________________________________________________
iceUtil::subscriptionUpdater::subscriptionUpdater( ) :
  iceThread( "subscription Updater" )
{
  m_iteration_delay = (int)(iceUtil::iceConfManager::getInstance()->getConfiguration()->ice()->subscription_update_threshold_time()/4);

  if(m_iteration_delay < 60) m_iteration_delay=60;

}

//______________________________________________________________________________
void iceUtil::subscriptionUpdater::body( void )
{

  while( !isStopped() ) {
  
    iceCommandSubUpdater cmd;
    cmd.execute( "" );
    
    if(m_iteration_delay<=10) 
      sleep( m_iteration_delay );
    else {
      
      for(int i=0; i<=m_iteration_delay; i++) {
	if( isStopped() ) return;
	sleep(1);
      }
      
    }

    //sleep( m_iteration_delay );
  }
}
