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
#include "iceCommandProxyRenewal.h"
#include "Delegation_manager.h"
#include "CreamProxyMethod.h"
#include "DNProxyManager.h"
#include "iceConfManager.h"
#include "iceUtils.h"
#include "iceDb/GetJobByGid.h"
#include "iceDb/Transaction.h"
#include "iceDb/UpdateJobByGid.h"

#include "glite/wms/common/configuration/ICEConfiguration.h"
/**
 *
 * Cream Client API C++ Headers
 *
 */
#include "glite/ce/cream-client-api-c/scoped_timer.h"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"
//#include "glite/ce/cream-client-api-c/VOMSWrapper.h"
#include "glite/ce/cream-client-api-c/CEUrl.h"


#include "glite/security/proxyrenewal/renewal.h"
/**
 *
 * OS Headers
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>
#include <cerrno>

//extern int errno;

namespace cream_api = glite::ce::cream_client_api;

using namespace std;
using namespace glite::wms::ice::util;

#define DELEGATION_EXPIRATION_THRESHOLD_TIME 3600

//______________________________________________________________________________
iceCommandProxyRenewal::iceCommandProxyRenewal( ) :
    iceAbsCommand( "iceCommandProxyRenewal" ),
    m_log_dev( cream_api::util::creamApiLogger::instance()->getLogger() )
{
  
}

//______________________________________________________________________________
void iceCommandProxyRenewal::execute( const std::string& tid) throw()
{  
  
  renewAllDelegations();
    
}

//______________________________________________________________________________
void iceCommandProxyRenewal::renewAllDelegations( void ) throw() 
{
    static const char* method_name = "iceCommandProxyRenewal::renewAllDelegations() - ";
    
    /**
       Now, let's check all delegations for expiration and renew them
    */
    
#ifdef ICE_PROFILE_ENABLE
    api_util::scoped_timer T( "iceCommandProxyRenewal::renewAllDelegations()" );
#endif
    
    vector< boost::tuple< string, string, string, time_t, int, bool, string> > allDelegations;
    
    Delegation_manager::instance()->getDelegationEntries( allDelegations, true );
    
    map<string, pair<time_t, int> > mapDelegTime; // delegationID -> (Expiration time, Absolute Duration)
    
    CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
                    << "There are [" << allDelegations.size() 
                    << "] Delegation(s) to check..."
                    );
    
    if( allDelegations.size() == 0 ) return;
    
    /**
       Loop over all different delegations
    */
    for( vector<boost::tuple<string, string, string, time_t, int, bool, string> >::const_iterator it = allDelegations.begin();
         it != allDelegations.end(); ++it)
      {
	
	time_t thisExpTime   = it->get<3>();
	string thisDelegID   = it->get<0>();        
	int    thisDuration  = it->get<4>();
	
	mapDelegTime[ thisDelegID ] = make_pair(thisExpTime, thisDuration);
	
	/**
	   if the current delegation ID is not expiring then skip it
	   but remove it from the map mapDelegTime anyway, in order to not update
	   the job cache later
	*/
	int remainingTime = thisExpTime - time(0);
	
	if( (remainingTime > 0.2 * thisDuration) && (remainingTime > DELEGATION_EXPIRATION_THRESHOLD_TIME) )
	  {
	    CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
			    << "Delegation ID ["
			    << thisDelegID << "] will expire in ["
			    << remainingTime << "] seconds (on ["
			    << time_t_to_string(thisExpTime) << "]). Duration is ["
			    << thisDuration << "] seconds. Will NOT renew it now..."
			    );
	    mapDelegTime.erase( thisDelegID );
	    continue;
	    
	  }
	
	string thisUserDN    = it->get<2>();
	string thisCEUrl     = it->get<1>();
	string thisMyPR      = it->get<6>();
	
	/**
	   Obtain the better proxy for the DN-FQAN related to current
	   delegation ID.
	   Looking at the "new" code of DNProxyManager one can see that this better
	   proxy can be obtained in two different ways:
	   1. as the proxy that ICE registered to the myproxyserver and that is automatically renewed by it
	   2. as the ICE-calculated better proxy (calculated at each submission)
	*/
	
	CREAM_SAFE_LOG(m_log_dev->debugStream() 
		       << method_name
		       << "Looking for the better proxy for DN ["
		       << thisUserDN << "] MyProxy Server name ["
		       << thisMyPR << "]..."
		       );
	
	boost::tuple<string, time_t, long long int> thisBetterPrx = DNProxyManager::getInstance()->getExactBetterProxyByDN( thisUserDN, thisMyPR );
	
	/**
	   Proxy NOT found for the userdn related to this delegation.
	   Remove the delegation
	*/
	if(thisBetterPrx.get<0>().empty()) {
	  CREAM_SAFE_LOG(m_log_dev->debugStream() 
			 << method_name
			 << "DNProxyManager::getExactBetterProxyByDN didn't return any better proxy for DN ["
			 << thisUserDN << "] and MyProxy Server name ["
			 << thisMyPR << "]... Skipping renew of this delegation ["
			 << thisDelegID << "]."
			 );
	  mapDelegTime.erase( thisDelegID );

	  Delegation_manager::instance()->removeDelegation( thisDelegID );

	  continue;
	}
	
	/**
	   if the returned better proxy is the "new" one must get the actual expiration time
	   (remember that it is renewed by myproxyserver asynchronously and ICE doesn't know
	   when this happen)
	*/
	
	//	cream_api::soap_proxy::VOMSWrapper V( thisBetterPrx.get<0>() );

	pair<bool, time_t> result_validity = isvalid( thisBetterPrx.get<0>() );
	if(!result_validity.first) {
	  CREAM_SAFE_LOG(m_log_dev->errorStream() 
			 << "iceCommandProxyRenewal::renewAllDelegations() - "
			 << "iceUtil::isvalid() function reported an error while" 
			 << " parsing proxy [ " << thisBetterPrx.get<0>() 
			 << "]. Skipping renew of delegation ["
			 << thisDelegID <<"]..."
			 );
	  mapDelegTime.erase( thisDelegID );
	  continue;
	}
	
	time_t proxy_time_end = result_validity.second;
	
	/**
	   Must update the expiration time inside the map of DNProxyManager
	*/
	DNProxyManager::getInstance()->updateBetterProxy( thisUserDN, thisMyPR, boost::make_tuple(thisBetterPrx.get<0>(), proxy_time_end, thisBetterPrx.get<2>()) );
	
	
	
	//proxy_time_end = thisBetterPrx.get<1>();

	/**
	   If the better proxy for this delegation ID is expired, it
	   means that there're no new jobs for this DN-FQAN_MYPROXY since
	   long... then we can remove the delegatiob ID from memory.
	*/
	if( proxy_time_end <= (time(0)-5) ) {
	  CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
			  << "For current Delegation ID [" << thisDelegID 
			  <<"] DNProxyManager returned the Better Proxy ["
			  << thisBetterPrx.get<0>() 
			  << "] that is EXPIRED! Removing this delegation ID from "
			  << "Delegation_manager, removing proxy from DNProxyManager's cache, won't renew delegation ..."
			  );
	  
	  Delegation_manager::instance()->removeDelegation( thisDelegID );
	  
	  DNProxyManager::getInstance()->removeBetterProxy( thisUserDN, thisMyPR );
	  
	  const char* regID = compressed_string( thisUserDN ).c_str();
	  int err = glite_renewal_UnregisterProxy( regID, NULL );
	  
	  if ( err && (err != EDG_WLPR_PROXY_NOT_REGISTERED) ) {
	    CREAM_SAFE_LOG(
			   m_log_dev->errorStream()
			   << method_name
			   << "Couldn't unregister the proxy registered with ID ["
			   << regID << "]. Error is: "
			   << edg_wlpr_GetErrorText(err) << ". Ignoring..."
			   );
	  }
	  
	  /**
	     must also unlink the symlink
	  */
	  if(::unlink( thisBetterPrx.get<0>().c_str() ) < 0)
	    {
	      int saveerr = errno;
	      CREAM_SAFE_LOG(
			     m_log_dev->errorStream()
			     << method_name
			     << "Unlink of file ["
			     << thisBetterPrx.get<0>() << "] is failed. Error is: "
			     << strerror(saveerr);
			     );
	    }
	  
	  
	  mapDelegTime.erase( thisDelegID );
	  continue;
	}
	
	/**
	   If the BetterProxy for this delegation is not expiring after the delegation itself
	   let's continue with next delegation...
	*/
	if( !(proxy_time_end > thisExpTime)) {
	  CREAM_SAFE_LOG(m_log_dev->warnStream() 
			 << "iceCommandProxyRenewal::renewAllDelegations() - "
			 << "The better proxy ["
			 << thisBetterPrx.get<0>() << "] is expiring NOT AFTER the current delegation ["
			 << thisDelegID << "]. Skipping ... "
			 );
	  
	  mapDelegTime.erase( thisDelegID );
	  continue;
	}

	CREAM_SAFE_LOG( m_log_dev->infoStream() << method_name
			<< "Will Renew Delegation ID ["
			<< thisDelegID << "] with BetterProxy ["
			<< thisBetterPrx.get<0>()
			<< "] that will expire on ["
			<< time_t_to_string(thisBetterPrx.get<1>()) << "]"
			);
	try {
	  
	  string thisDelegUrl = thisCEUrl;
	  
	  boost::replace_all( thisDelegUrl, 
			      iceConfManager::getInstance()->getConfiguration()->ice()->cream_url_postfix(), 
			      iceConfManager::getInstance()->getConfiguration()->ice()->creamdelegation_url_postfix() 
			      );
	  
	  CreamProxy_ProxyRenew( thisDelegUrl,
				 thisBetterPrx.get<0>(),
				 thisDelegID).execute( 3 );
	  
	  mapDelegTime[ thisDelegID ] = make_pair(thisBetterPrx.get<1>(), thisBetterPrx.get<1>() - time(0) );
	  
	} catch( exception& ex ) {
	  CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
			  << "Proxy renew for delegation ID ["
			  << thisDelegID << "] failed: " << ex.what() 
			  );
	  
	  mapDelegTime.erase( thisDelegID );
	  continue;
	}
	
      }
    
    for(map<string, pair<time_t, int> >::iterator it = mapDelegTime.begin();
        it != mapDelegTime.end(); ++it) {
      
      Delegation_manager::instance()->updateDelegation( boost::make_tuple((*it).first, (*it).second.first, (*it).second.second ) );
      
    }    
}
