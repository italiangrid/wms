
/* 
 * Copyright (c) Members of the EGEE Collaboration. 2004. 
 * See http://www.eu-egee.org/partners/ for details on the copyright
 * holders.  
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at 
 *
 *    http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and 
 * limitations under the License.
 *
 * ICE proxy renewal command
 *
 * Authors: Alvise Dorigo <alvise.dorigo@pd.infn.it>
 *          Moreno Marzolla <moreno.marzolla@pd.infn.it>
 */

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
#include "jobCache.h"
#include "iceUtils.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"
/**
 *
 * Cream Client API C++ Headers
 *
 */
#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/CEUrl.h"

/**
 *
 * OS Headers
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>

namespace cream_api = glite::ce::cream_client_api;

using namespace std;
using namespace glite::wms::ice::util;

#define DELEGATION_EXPIRATION_THRESHOLD_TIME 1200

//______________________________________________________________________________
iceCommandProxyRenewal::iceCommandProxyRenewal( ) :
    iceAbsCommand( "iceCommandProxyRenewal" ),
  m_log_dev( cream_api::util::creamApiLogger::instance()->getLogger() ),
  m_cache( jobCache::getInstance() )
{
  
}

//______________________________________________________________________________
//bool iceCommandProxyRenewal::renewProxy( const pair<string, string>& deleg, 
//					 const string& proxy) throw()
							      //{
  
//   string cream_deleg_url = deleg.second;
//   string delegation_ID   = deleg.first;

//   try {

//     CreamProxy_ProxyRenew( cream_deleg_url,
// 			   proxy,
// 			   delegation_ID).execute( 3 );

//   } catch( exception& ex ) {
//     // FIXME: what to do? for now let's continue with an error message
//     CREAM_SAFE_LOG( m_log_dev->errorStream() 
// 		    << "iceCommandProxyRenewal::execute() - "
// 		    << "Proxy renew for delegation ID ["
// 		    << delegation_ID
// 		    << "] failed: "
// 		    << ex.what() 
// 		    );
    
//     return false;
//     // Let's ignore; probably another proxy renewal will be done
//   }
  
//  return true;
//}

//______________________________________________________________________________
void iceCommandProxyRenewal::execute( void ) throw()
{

//   map< pair<string, string>, list<CreamJob>, ltstring > jobMap;
//   map< string, time_t > timeMap;
  
//   {
//     boost::recursive_mutex::scoped_lock M( jobCache::mutex );
//     for(jobCache::iterator jobIt = m_cache->begin(); jobIt != m_cache->end(); ++jobIt) {
//       if( !jobIt->is_active() ) 
// 	continue; // skip terminated jobs
//       if( !jobIt->is_proxy_renewable() )
// 	continue;
      
//       struct stat buf;

//       if( ::stat( jobIt->getUserProxyCertificate().c_str(), &buf) == 1 ) {
// 	CREAM_SAFE_LOG(m_log_dev->errorStream() 
// 		       << "iceCommandProxyRenewal::execute() - "
// 		       << "Cannot stat proxy file ["
// 		       << jobIt->getUserProxyCertificate() << "] for job "
// 		       << jobIt->describe()
// 		       << ". Skipping it..."
// 		       );
// 	continue; // skip to next job
// 	// FIXME: what to do?
//       }

//       if( buf.st_mtime > jobIt->getProxyCertLastMTime() ) {
// 	CREAM_SAFE_LOG(m_log_dev->infoStream() 
// 		       << "iceCommandProxyRenewal::execute() - "
// 		       << "Proxy file ["
// 		       << jobIt->getUserProxyCertificate() << "] for job "
// 		       << jobIt->describe()
// 		       << " was modified on "
// 		       << time_t_to_string( buf.st_mtime )
// 		       << ", the last proxy file modification time "
// 		       << "recorded by ICE is "
// 		       << time_t_to_string( jobIt->getProxyCertLastMTime() )
// 		       << ". Renewing proxy."
// 		       );
	
// 	glite::wms::ice::util::DNProxyManager::getInstance()->setUserProxyIfLonger( jobIt->getUserDN(), jobIt->getUserProxyCertificate() );
// 	jobMap[ make_pair(jobIt->getDelegationId(), jobIt->getCreamDelegURL()) ].push_back( *jobIt );
// 	timeMap[ jobIt->getCompleteCreamJobID() ] = buf.st_mtime;
//       }
//     }
//   } // unlock the jobCache
  
  
  // now must loop over the keys of jobMap
  // and renew each proxy.
//   map< pair<string, string>, list<CreamJob>, ltstring >::iterator jobMap_it = jobMap.begin();
//   map< pair<string, string>, list<CreamJob>, ltstring >::const_iterator end = jobMap.end();

//   while( jobMap_it != end ) {

//     // jobMap::iterator.first is the couple (delegationID,CREAM_Deleg_URL)
//     // jobMap::iterator.second is the list of job

//     CREAM_SAFE_LOG(m_log_dev->infoStream() 
// 		   << "iceCommandProxyRenewal::execute() - "
// 		   << "Renewing proxy for delegation ID ["
// 		   << jobMap_it->first.first << "] to CREAMDelegation service ["
// 		   << jobMap_it->first.second << "] using proxy certificate file ["
// 		   << jobMap_it->second.begin()->getUserProxyCertificate()
// 		   << "]"
// 		   );

//     if( renewProxy( jobMap_it->first, jobMap_it->second.begin()->getUserProxyCertificate() ) ) {
//         boost::recursive_mutex::scoped_lock M( jobCache::mutex );
// 	// update the cache for all jobs that have this delegationID
// 	list<CreamJob>::iterator jobit     = jobMap_it->second.begin();
// 	list<CreamJob>::iterator jobit_end = jobMap_it->second.end();
// 	while( jobit != jobit_end ) {
//             jobit->setProxyCertMTime( timeMap[jobit->getCompleteCreamJobID()] );
//             m_cache->put( *jobit );
//             ++jobit;
// 	}
//     }
//     ++jobMap_it;
//   }

  
  set<string> allPhysicalProxyFiles;
  {
    

    /** 
	lock the cache
    */
    boost::recursive_mutex::scoped_lock M( jobCache::mutex );

    /**
       First: must check all on-disk different proxyfiles for the recently modified
       in order to update the better proxy
    */

    CREAM_SAFE_LOG(m_log_dev->infoStream() 
		   << "iceCommandProxyRenewal::getAllPhysicalNewProxies() - "
		   << "Going to collect al physical different proxies that has been renewed..."
		   );

    this->getAllPhysicalNewProxies( allPhysicalProxyFiles );// this is matter of cycle 
                                                            // over the cache and get and readlink the certs
                                                            // it should be quite fast.

    CREAM_SAFE_LOG(m_log_dev->infoStream() 
		   << "iceCommandProxyRenewal::getAllPhysicalNewProxies() - "
		   << "Found " << allPhysicalProxyFiles.size() << " physical proxies modified:"
		   );

    for(set<string>::const_iterator it=allPhysicalProxyFiles.begin();
	it != allPhysicalProxyFiles.end();
	++it)
      {
	CREAM_SAFE_LOG(m_log_dev->debugStream() 
		       << "iceCommandProxyRenewal::getAllPhysicalNewProxies() - "
		       << "Renewed physical proxy: [" << *it << "]"
		       );
      }

  } // unlock the jobCache
  
  /**
     Second: in allPhysicalProxyFiles set there are all different physical proxy files
     that have been "touched" (i.e. renewd by myproxy).
     Now let's check all of them to update the better proxies... also we hope that
     in the meanwhile the proxy files do not disappear from disk.
  */
  for(set<string>::const_iterator it = allPhysicalProxyFiles.begin();
      it != allPhysicalProxyFiles.end();
      ++it)
    {
      DNProxyManager::getInstance()->setUserProxyIfLonger( *it );
    }

  //
  // NOW all better proxies related to job in cache of ICE are updated.
  //
  
  this->renewAllDelegations();

}

//______________________________________________________________________________
void iceCommandProxyRenewal::getAllPhysicalNewProxies( set<string>& allPhysicalProxyFiles ) throw()
{
  string thisProxy;
  /**
     loop over all jobs in the cache. Must loop over all the cache because
     for each job must also update the last modification time of the certificate
  */
  for(jobCache::iterator jobIt = m_cache->begin(); jobIt != m_cache->end(); ++jobIt) {
    struct stat buf;
    
    thisProxy = jobIt->getUserProxyCertificate();

    CREAM_SAFE_LOG(m_log_dev->infoStream() 
		   << "iceCommandProxyRenewal::getAllPhysicalNewProxies() - "
		   << "Checking proxyfile ["
		   << thisProxy << "] ...."
		   );

    if( ::stat( thisProxy.c_str(), &buf) != 0 ) {
      int saveerr = errno;
      CREAM_SAFE_LOG(m_log_dev->errorStream() 
		     << "iceCommandProxyRenewal::getAllPhysicalNewProxies() - "
		     << "Cannot stat proxy file ["
		     << thisProxy << "] for job "
		     << jobIt->describe()
		     << ": " 
		     << strerror( saveerr )
		     << ". Skipping it..."
		     );
      continue; // skip to next job
      // FIXME: what to do?
    }
    
    

    /**
       If file has been touched, we assume the proxy renewal (myproxy) renewed it
    */
    if( buf.st_mtime > jobIt->getProxyCertLastMTime() ) {
      CREAM_SAFE_LOG(m_log_dev->infoStream() 
		     << "iceCommandProxyRenewal::getAllPhysicalNewProxies() - "
		     << "Proxy file ["
		     << thisProxy << "] for job "
		     << jobIt->describe()
		     << " was modified on "
		     << time_t_to_string( buf.st_mtime )
		     << ", the last proxy file modification time "
		     << "recorded by ICE is "
		     << time_t_to_string( jobIt->getProxyCertLastMTime() )
		     << ". Will use it to check the better proxy of the same user."
		     );
      
      /**
	 If the proxy file is a regular (physical) file then insert it into the set;
	 otherwise it is a symlink that must be resolved
      */
      if(buf.st_mode && S_IFLNK) { // the proxy is actually a symlink to a physical proxy file
	char pathbuf[1024];
	memset( (void*)pathbuf, 0, 1024 );
	if( ::readlink( thisProxy.c_str(), pathbuf, 1024) < 0 ) {
	  int saveerr = errno;
	  CREAM_SAFE_LOG(m_log_dev->errorStream() 
			 << "iceCommandProxyRenewal::getAllPhysicalNewProxies() - "
			 << "Error reading symlink ["
			 << thisProxy << "]: "
			 << strerror( saveerr );
			 );
	  continue;
	}
	CREAM_SAFE_LOG(m_log_dev->infoStream() 
		       << "iceCommandProxyRenewal::getAllPhysicalNewProxies() - "
		       << "The proxyfile ["
		       << thisProxy << "] is a symlink to ["
		       << pathbuf << "]."
		       );
	allPhysicalProxyFiles.insert(pathbuf);
	
      } else { // if it is not a symlink then it is for sure a physical file.
	CREAM_SAFE_LOG(m_log_dev->infoStream() 
		       << "iceCommandProxyRenewal::getAllPhysicalNewProxies() - "
		       << "The proxyfile ["
		       << thisProxy << "] is NOT a symlink."
		       );
	allPhysicalProxyFiles.insert(thisProxy);
      }
      
      jobIt->setProxyCertMTime( buf.st_mtime );
      m_cache->put( *jobIt );
      
    }
  }
}

//______________________________________________________________________________
void iceCommandProxyRenewal::renewAllDelegations( void ) throw() 
{
  /**
     Now, let's check all delegations for expiration
     and renew them
  */

  vector< boost::tuple< string, string, string, time_t> > allDelegations;
  map< string, CreamJob > delegID_CreamJob;

  Delegation_manager::instance()->getDelegationEntries( allDelegations );

  /**
     Create a MAP
     delegationID -> array<Job> (jobs that have that delegationID)
     This maps is needed below to update all jobs for the deleg expiration times.
   */
  map<string, list<CreamJob> > mapDelegJob;
  {
    boost::recursive_mutex::scoped_lock M( jobCache::mutex );
    for( jobCache::iterator jobit = m_cache->begin(); jobit != m_cache->end(); ++jobit )
      {
	mapDelegJob[ jobit->getDelegationId() ].push_back( *jobit );
      }
  }

  vector<boost::tuple<string, string, string, time_t> >::const_iterator it = allDelegations.begin();

  map<string, time_t> mapDelegTime;
  
  CREAM_SAFE_LOG( m_log_dev->debugStream() 
		  << "iceCommandProxyRenewal::renewAllDelegations() - "
		  << "There are [" << allDelegations.size() << "] Delegation(s) to check..."
		  );

  /**
     Loop over all different delegations
  */
  //  while( it != allDelegations.end() )
  for( vector<boost::tuple<string, string, string, time_t> >::const_iterator it = allDelegations.begin();
       it != allDelegations.end();
       ++it)
    {

      time_t thisExpTime   = it->get<3>();
      string thisDelegID   = it->get<0>();

      mapDelegTime[ thisDelegID ] = thisExpTime;

      /**
	 if the current delegation ID is not expiring then skip it 
	 but update the map mapDelegTime anyway, in order to update the job cache
	 later
       */
      if( thisExpTime - time(0) > DELEGATION_EXPIRATION_THRESHOLD_TIME ) {

	CREAM_SAFE_LOG( m_log_dev->debugStream() 
			<< "iceCommandProxyRenewal::renewAllDelegations() - "
			<< "Delegation ID ["
			<< thisDelegID << "] will expire on ["
			<< time_t_to_string(thisExpTime) << "]. Threshold time is ["
			<< DELEGATION_EXPIRATION_THRESHOLD_TIME << "] seconds. Will NOT renew it now..."
			);

	continue;
      } 
      
      string thisUserDN    = it->get<2>();
      string thisCEUrl     = it->get<1>();

      /**
	 Obtain the better proxy for the DN-FQAN related to current delegation ID
      */
      pair<string, time_t> thisBetterPrx = DNProxyManager::getInstance()->getBetterProxyByDN( thisUserDN ); // all better proxies have been updated (see above)
      
      /**
	 If the better proxy for this delegation ID is expired, it means that
	 there're no new jobs for this DN-FQAN since long... then we can
	 remove the delegatiob ID from memory.
      */
      if( thisBetterPrx.second <= (time(0)-5) ) {
	CREAM_SAFE_LOG( m_log_dev->errorStream() 
			<< "iceCommandProxyRenewal::renewAllDelegations() - "
			<< "For current Delegation ID ["
			<< thisDelegID <<"] DNProxyManager returned the Better Proxy ["
			<< thisBetterPrx.first << "] that is EXPIRED. Removing this delegation ID from "
			<< "Delegation_manager ..."
		      );
	
	Delegation_manager::instance()->removeDelegation( thisDelegID );
	mapDelegTime.erase( thisDelegID );
	mapDelegJob.erase( thisDelegID );
	continue;
      }

      CREAM_SAFE_LOG( m_log_dev->infoStream() 
		      << "iceCommandProxyRenewal::renewAllDelegations() - "
		      << "Will Renew Delegation ID ["
		      << thisDelegID << "] with BetterProxy ["
		      << thisBetterPrx.first
		      << "] that will expire on ["
		      << time_t_to_string(thisBetterPrx.second) << "]"
		      );
      try {

	string thisDelegUrl = thisCEUrl;

	boost::replace_all( thisDelegUrl, 
			    iceConfManager::getInstance()->getConfiguration()->ice()->cream_url_postfix(), 
			    iceConfManager::getInstance()->getConfiguration()->ice()->creamdelegation_url_postfix() 
			    );
	
	CreamProxy_ProxyRenew( thisDelegUrl,
			       thisBetterPrx.first,
			       thisDelegID).execute( 3 );

	mapDelegTime[ thisDelegID ] = thisBetterPrx.second;
	
      } catch( exception& ex ) {
	CREAM_SAFE_LOG( m_log_dev->errorStream() 
			<< "iceCommandProxyRenewal::renewAllDelegations() - "
			<< "Proxy renew for delegation ID ["
			<< thisDelegID
			<< "] failed: "
			<< ex.what() 
			);
      }

    }
    
  /**
     Now that all delegations have been renewed; lets update the jobCache and
     the Delegation_manager's cache too
  */
  {
    boost::recursive_mutex::scoped_lock M( jobCache::mutex );
    //map<string, list<CreamJob> >::iterator delegationIT = mapDelegJob.begin();

    for( map<string, list<CreamJob> >::iterator delegationIT = mapDelegJob.begin();
	 delegationIT != mapDelegJob.end(); 
	 ++delegationIT )
      {
	for(list<CreamJob>::iterator jobIT = delegationIT->second.begin();
	    jobIT != delegationIT->second.end();
	    ++jobIT) 
	  {
	    jobIT->setDelegationExpirationTime( mapDelegTime[ delegationIT->first ] );
	    m_cache->put( *jobIT );
	  }
	
      }
  }

  for(map<string, time_t>::iterator it = mapDelegTime.begin();
      it != mapDelegTime.end();
      ++it)
    {
      Delegation_manager::instance()->updateDelegation( *it );
    }
  
}
