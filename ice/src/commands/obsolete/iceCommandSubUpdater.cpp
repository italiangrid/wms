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

#include "iceCommandSubUpdater.h"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "subscriptionManager.h"
#include "DNProxyManager.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"

#include <string>
#include <vector>

#include <boost/format.hpp>

using namespace std;

namespace api_util  = glite::ce::cream_client_api::util;
namespace cream_api = glite::ce::cream_client_api;
namespace ice_util = glite::wms::ice::util;

//______________________________________________________________________________
namespace {

  class singleSubUpdater {
    //ice_util::subscriptionManager *m_subManager;
    string m_dn;

  public:
    singleSubUpdater( string dn ) : m_dn( dn ) { }

    void operator()( string endpoint ) {

        ice_util::iceSubscription sub;
      if(!ice_util::subscriptionManager::getInstance()->getSubscriptionByDNCEMon(m_dn, endpoint, sub)) {
	// this should almost never happen because before to update
	// the caller also invokes the subManager->checkSubscription()
	CREAM_SAFE_LOG(api_util::creamApiLogger::instance()->getLogger()->errorStream() 
		       << "iceCommandSubUpdater::execute() - "
		       << "Not found any subscription for DN ["
		       << m_dn << "] and CEMon ["
		       << endpoint << "]. Skipping..."
		       );
	return;
      }
      
      CREAM_SAFE_LOG(api_util::creamApiLogger::instance()->getLogger()->debugStream() 
		     << "iceCommandSubUpdater::execute() - "
		     << "Checking Subscription validity for DN ["
		     << m_dn << "] to CEMon ["
		     << endpoint << "]"
		     );
      
      time_t timeleft = sub.getExpirationTime() - time(NULL);

      if(timeleft < ice_util::iceConfManager::getInstance()->getConfiguration()->ice()->subscription_update_threshold_time()) {
	string betterProxy;
	//{
	//  boost::recursive_mutex::scoped_lock M( ice_util::DNProxyManager::mutex );
	betterProxy = ice_util::DNProxyManager::getInstance()->getAnyBetterProxyByDN( m_dn ).get<0>();//first;
	  //}
	
	if(betterProxy == "") {
	  CREAM_SAFE_LOG(api_util::creamApiLogger::instance()->getLogger()->errorStream() 
			 << "iceCommandSubUpdater::execute() - "
			 << "Cannot get better proxy file for DN ["
			 << m_dn << "]. Skipping..."
			 );
	  return;
	}
	
	CREAM_SAFE_LOG(api_util::creamApiLogger::instance()->getLogger()->infoStream() 
		       << "iceCommandSubUpdater::execute() - "
		       << "Updating subscription ["<<sub.getSubscriptionID() 
		       << "] to CEMon [" << endpoint <<"] for user DN ["
		       << m_dn << "] using proxy ["
		       << betterProxy << "]."
		       );
	
	ice_util::subscriptionManager::getInstance()->renewSubscription( betterProxy , endpoint );
      }
    }
  }; // end class
} // end anonymous namespace

//______________________________________________________________________________
ice_util::iceCommandSubUpdater::iceCommandSubUpdater( ) throw() : 
    ice_util::iceCommandSubUpdater::iceAbsCommand( "iceCommandSubUpdater", "" ),
  m_log_dev( api_util::creamApiLogger::instance()->getLogger() ),
  m_conf( ice_util::iceConfManager::getInstance() )		     
{
}

//______________________________________________________________________________
void ice_util::iceCommandSubUpdater::execute( const std::string& tid) throw()
{
  map<string, set<string> > UserCEMons;
  ice_util::subscriptionManager *subManager = ice_util::subscriptionManager::getInstance();
  
  //{
  //boost::recursive_mutex::scoped_lock M( ice_util::subscriptionManager::mutex ); 
  // no mutex needed, because the followin method already acquire the cache's mutex
  // then 2 different threads cannot execute this method contemporary
  subManager->getUserCEMonMapping( UserCEMons, true );// this is a map userproxy -> set_of_cemons retrieved from the jobCache
  //}
  

  // first of all let's check all subs

  for(map<string, set<string> >::const_iterator it=UserCEMons.begin();
      it != UserCEMons.end();
      ++it)
  {
    CREAM_SAFE_LOG(m_log_dev->infoStream() 
		   << "iceCommandSubUpdater::execute() - "
		   << "Checking Subscriptions for owner of the DN ["
		   << it->first<< "]."
		   );

    //subManager->purgeOldSubscription( it );

    {
      //boost::recursive_mutex::scoped_lock M( ice_util::subscriptionManager::mutex ); 
      subManager->checkSubscription( *it );// acquire a mutex internally
      
      
      singleSubUpdater updater( it->first );
      
      for_each(it->second.begin(), it->second.end(), updater);
    } // unlock subscriptionManager::mutex

  }
}
