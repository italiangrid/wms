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

#include "iceCommandLBLogging.h"
#include "iceCommandDelegationRenewal.h"
#include "iceUtils/DelegationManager.h"
#include "iceUtils/CreamProxyMethod.h"
#include "iceUtils/DNProxyManager.h"
#include "iceUtils/IceConfManager.h"
#include "ice/IceCore.h"
#include "iceUtils/IceUtils.h"
#include "iceDb/GetJobsByDNMyProxy.h"
#include "iceDb/GetJobByGid.h"
#include "iceDb/Transaction.h"
#include "common/src/configuration/ICEConfiguration.h"
#include "common/src/configuration/CommonConfiguration.h"
/**
 *
 * Cream Client API C++ Headers
 *
 */
#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/CEUrl.h"
#include "glite/ce/cream-client-api-c/VOMSWrapper.h"


/**
 *
 * OS Headers
 *
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>
#include <cerrno>

#include <boost/algorithm/string/predicate.hpp>

namespace cream_api = glite::ce::cream_client_api;

using namespace std;
using namespace glite::wms::ice::util;

#define DELEGATION_EXPIRATION_THRESHOLD_TIME 3600

//______________________________________________________________________________
iceCommandDelegationRenewal::iceCommandDelegationRenewal( ) :
    IceAbstractCommand( "iceCommandDelegationRenewal", "" ),
    m_log_dev( cream_api::util::creamApiLogger::instance()->getLogger() ),
    m_ctx( NULL )
{
}

//______________________________________________________________________________
iceCommandDelegationRenewal::~iceCommandDelegationRenewal( )
{
}

//______________________________________________________________________________
string iceCommandDelegationRenewal::get_grid_job_id( ) const
{
  ostringstream randid( "" );
  struct timeval T;
  gettimeofday( &T, 0 );
  randid << T.tv_sec << "." << T.tv_usec;
  return randid.str();
}

//______________________________________________________________________________
void iceCommandDelegationRenewal::execute( const std::string& tid ) throw()
{  
#ifdef ICE_PROFILE
  ice_timer timer("iceCommandDelegationRenewal::execute");
#endif
  this->renewAllDelegations();
    
}

//______________________________________________________________________________
void iceCommandDelegationRenewal::renewAllDelegations( void ) throw() 
{
    static const char* method_name = "iceCommandDelegationRenewal::renewAllDelegations() - ";
    
    /**
       Now, let's check all delegations for expiration and renew them
    */
    
    vector< Delegation_manager::table_entry > allDelegations;
    
    int alldeleg_size = Delegation_manager::instance()->getDelegationEntries( allDelegations, true );
    
    map<string, pair<time_t, int> > mapDelegTime; // delegationID -> (Expiration time, Absolute Duration)
    
    CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
                    << "There are [" << alldeleg_size
                    << "] Delegation(s) to check..."
                    );
    
    if( alldeleg_size == 0 ) return;
    
    /**
       Loop over all different delegations
    */
    for( vector<Delegation_manager::table_entry >::const_iterator it = allDelegations.begin();
	 it != allDelegations.end(); 
	 ++it)
      {
	
	
	time_t thisExpTime   = it->m_expiration_time;
	string thisDelegID   = it->m_delegation_id;        
	int    thisDuration  = it->m_delegation_duration;
	int    remainingTime = thisExpTime - time(0);
	if( (remainingTime > 0.2 * thisDuration) && 
	    (remainingTime > DELEGATION_EXPIRATION_THRESHOLD_TIME) )
	  {
	    CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
			    << "Delegation ID ["
			    << thisDelegID << "] will expire in ["
			    << remainingTime << "] seconds (on ["
			    << IceUtils::time_t_to_string(thisExpTime) << "]). Duration is ["
			    << thisDuration << "] seconds. Will NOT renew it now..."
			    );
	    mapDelegTime.erase( thisDelegID );
	    continue;
	    
	  }
	

	/**
	   Let's try to download from MyProxy Service
	   a fresh and hopefully most long-living proxy for current user
	*/
	boost::tuple<string, time_t, long long int> SBP( DNProxyManager::getInstance()->getExactBetterProxyByDN(it->m_user_dn, it->m_myproxyserver) );
	string certfile( SBP.get<0>() );
	if( certfile.empty() ) {
	  CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
			  << "There is NOT a better proxy for user [" 
			  << it->m_user_dn << "] related to current delegation id to check [" 
			  << it->m_delegation_id <<"]. Skipping this delegation renewal check..."
			  );
	  mapDelegTime.erase( thisDelegID );
	  continue;
	}
	int timeout = IceConfManager::instance()->getConfiguration()->ice( )->proxy_renewal_timeout( );
	string timeoutStr( boost::lexical_cast<string>(timeout) );
	CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
			<< "Contacting MyProxy server [" 
			<< it->m_myproxyserver << "] for user dn ["
			<< it->m_user_dn << "] with proxy certificate ["
			<< certfile << "] to renew it..."
			);

        string newcert = certfile+".renewed";

	char *renewal_args[] = { "/usr/bin/glite-wms-ice-proxy-renew", 
	                         "--timeout", 
				 (char*)timeoutStr.c_str( ), 
				 "-s", 
				 (char*)it->m_myproxyserver.c_str(), 
				 "-p", 
				 (char*)certfile.c_str(), 
				 "-o", 
				 (char*)newcert.c_str(), 
				 0 };
	
	string env_cert = string("X509_USER_CERT=") + IceConfManager::instance()->getConfiguration()->common()->host_proxy_file();
	string env_key  = string("X509_USER_KEY=") + IceConfManager::instance()->getConfiguration()->common()->host_proxy_file();
/* 
	char *renewal_envs[] = { "X509_USER_CERT", (char*)IceConfManager::instance()->getConfiguration()->common()->host_proxy_file().c_str(), 
				 "X509_USER_KEY", (char*)IceConfManager::instance()->getConfiguration()->common()->host_proxy_file().c_str(), 0 
				};
*/

	char *renewal_envs[] = { (char*)env_cert.c_str(), (char*)env_key.c_str(), 0 };

	pid_t renewal_process_pid = fork();
	if(renewal_process_pid == -1 ) {
	  CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
			  << "fork returned error: [" 		  
			  << strerror(errno) << "]"
			  << ". Cannot get a fresh proxy for user ["
			  << it->m_user_dn << "]."
			  );
	} else {
	  if(renewal_process_pid == 0 ) {
	    // child that has to do execve
	    ::execve( "/usr/bin/glite-wms-ice-proxy-renew", renewal_args, renewal_envs );
	    CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
			  << "execve returned error: [" 		  
			  << strerror(errno) << "]"
			  << ". Cannot get a fresh proxy for user ["
			  << it->m_user_dn << "]."
			  );
	  } else {
	    // parent that has to wait for the child termination
	    int status;
    	    int retwaitpid = ::waitpid(renewal_process_pid, &status, WUNTRACED);
	    if(retwaitpid == renewal_process_pid) {
	      // the child finised. must retrieve command's output from file in tmp and parse it to
	      // check if OK or ERROR
	      ifstream in;
	      in.open((string("/tmp/glite-wms-ice-proxy-renew.output.") + boost::lexical_cast<string>(retwaitpid)).c_str(), ios::in);
	      string renewal_command_output;
	      string line;
   	      while (getline(in, line)) renewal_command_output += line;
   	      in.close();
	      ::unlink( (string("/tmp/glite-wms-ice-proxy-renew.output.") + boost::lexical_cast<string>(retwaitpid)).c_str() );
	      
	      /**
	       *
	       * ERROR 
	       *
	       */
	      if(boost::starts_with(renewal_command_output, "ERROR")) {
	        CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
			        << "Proxy renewal failed: [" 		  
			        << renewal_command_output << "]"
			      );
	      }
	      
	      /**
	       *
	       * OK
	       *
	       */
	      if(boost::starts_with(renewal_command_output, "OK")) {
	        CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
			    << "Proxy renewal successful for DN=["
			    << it->m_user_dn
			    <<"] MyProxyURL=["
			    << it->m_myproxyserver
			    << "]: new proxy is [" 		  
			    << newcert << "]. It will overwrite the better one..."
			    );
	        /**
	           Must substitute old better proxy with new downloaded one, if it's more long-living 
	         */
	        cream_api::soap_proxy::VOMSWrapper V( certfile + ".renewed", !::getenv("GLITE_WMS_ICE_DISABLE_ACVER") );
	        if( !V.IsValid( ) ) {
	          CREAM_SAFE_LOG(m_log_dev->errorStream() 
		   	     << method_name
			     << "Cannot parse downloaded new proxy ["
			     << certfile + ".renewed" << "] from MyProxy Service ["
			     << it->m_myproxyserver << "]. Error is: "
			     << V.getErrorMessage()
			     );
	      
	        } else {
	      
	        /** SUBSTITUTE old better proxy with downloaded one. 
	         */
	        boost::tuple<string, time_t, long long int> newPrx = boost::make_tuple( certfile + ".renewed", V.getProxyTimeEnd(), (long long int)-1 ); // -1 means to not modify the job counter
	        DNProxyManager::getInstance()->updateBetterProxy( it->m_user_dn,
								it->m_myproxyserver,
								newPrx );
	        ::unlink( (certfile + ".renewed").c_str() );
	      
	       }
	      }
	    }
	  }

	}
	
	/**
	 *
	 * CODE to POPEN to glite-wms-ice-proxy-renew
	 *
	 */
// 	string output;
// 	string command = "export X509_USER_CERT=" + IceConfManager::instance()->getConfiguration()->common()->host_proxy_file();
// 	command += "; export X509_USER_KEY=" + IceConfManager::instance()->getConfiguration()->common()->host_proxy_file();
//         timeout = IceConfManager::instance()->getConfiguration()->ice( )->proxy_renewal_timeout( );
// 	command += "; /usr/bin/glite-wms-ice-proxy-renew --timeout " + boost::lexical_cast<string>(timeout);
// 	command += " -s " + it->m_myproxyserver;
// 	command += " -p " + certfile;
// 	command += " -o " + certfile + ".renewed";
// 
// 	CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
// 			<< "Executing command [" 
// 			<< command << "]..."
// 			);
// 
// 	FILE *res = popen(command.c_str(), "r");
// 	if(!res) {
// 	  CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
// 			  << "Couldn't popen command [" 		  
// 			  << command << "]: " << strerror(errno)
// 			  << ". Skipping this delegation renew..."
// 			  );
// 	} else {
// 
// 	  while(!feof(res)) {
// 	    output += fgetc(res);
// 	  }
// 
// 	  int pcloseret = pclose( res );
// 
// // 	  CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
// // 			  << "Command output is ["
// // 			  << output << "]"
// // 			  );
// 
// 	  if( pcloseret == -1 ) // the call to pclose failed for some reason
// 	    {
// 	      CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
// 			  << "pclose failed for error: " 		  
// 			  << strerror(errno)
// 			  << ". Cannot determine if proxy has been correctly renewed..."
// 			  );
// 	    }
// 	  if( pcloseret != 0 ) {
// 	    CREAM_SAFE_LOG( m_log_dev->errorStream() << method_name
// 			  << "Proxy renewal failed: [" 		  
// 			    << output << "]"
// 			  );	    
// 	  }
// 
// 	  if( pcloseret == 0 ) {
// 	    CREAM_SAFE_LOG( m_log_dev->debugStream() << method_name
// 			    << "Proxy renewal successful for DN=["
// 			    << it->m_user_dn
// 			    <<"] MyProxyURL=["
// 			    << it->m_myproxyserver
// 			    << "]: new proxy is [" 		  
// 			    << certfile + ".renewed" << "]. It will overwrite the better one..."
// 			    );
// 	    /**
// 	       Must substitute old better proxy with new downloaded one, if it's more long-living 
// 	    */
// 	    cream_api::soap_proxy::VOMSWrapper V( certfile + ".renewed", !::getenv("GLITE_WMS_ICE_DISABLE_ACVER") );
// 	    if( !V.IsValid( ) ) {
// 	      CREAM_SAFE_LOG(m_log_dev->errorStream() 
// 			     << method_name
// 			     << "Cannot parse downloaded new proxy ["
// 			     << certfile + ".renewed" << "] from MyProxy Service ["
// 			     << it->m_myproxyserver << "]. Error is: "
// 			     << V.getErrorMessage()
// 			     );
// 	      
// 	    } else {
// 	      
// 	      /** SUBSTITUTE old better proxy with downloaded one. 
// 	       */
// 	      boost::tuple<string, time_t, long long int> newPrx = boost::make_tuple( certfile + ".renewed", V.getProxyTimeEnd(), (long long int)-1 ); // -1 means to not modify the job counter
// 	      DNProxyManager::getInstance()->updateBetterProxy( it->m_user_dn,
// 								it->m_myproxyserver,
// 								newPrx );
// 	      ::unlink( (certfile + ".renewed").c_str() );
// 	      
// 	    }
// 	    
// 	    
// 	  }
// 	}
	
	mapDelegTime[ thisDelegID ] = make_pair(thisExpTime, thisDuration);
	
	/**
	   if the current delegation ID is not expiring then skip it
	   but remove it from the map mapDelegTime anyway, in order to not update
	   the job cache later
	*/
	string thisUserDN    = it->m_user_dn;
	string thisCEUrl     = it->m_cream_url;
	string thisMyPR      = it->m_myproxyserver;
	
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

	pair<bool, time_t> result_validity = IceUtils::is_good_proxy( thisBetterPrx.get<0>() );
	if(!result_validity.first) {
	  CREAM_SAFE_LOG(m_log_dev->errorStream() 
			 << "iceCommandProxyRenewal::renewAllDelegations() - "
			 << "iceUtil::isgood() function reported an error while" 
			 << " parsing proxy [" << thisBetterPrx.get<0>() 
			 << "]. Skipping renew of delegation ["
			 << thisDelegID <<"]..."
			 );
	  mapDelegTime.erase( thisDelegID );
	  continue;
	}
	
	time_t proxy_time_end = result_validity.second;
	
	/**
	   If the super better proxy for this delegation ID is expired, it
	   means that there're no new jobs for this DN-FQAN_MYPROXY since
	   long and/or has not been done connections to myProxy or those connections
	   have failed... then we can remove the delegatiob ID from memory.
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
	  
	  list<CreamJob> toRemove;
	  {
//	    list<pair<string, string> > clause;
//	    clause.push_back( make_pair( util::CreamJob::user_dn_field(), thisUserDN) );
//	    clause.push_back( make_pair( util::CreamJob::myproxy_address_field(), thisMyPR) );
	    //db::GetJobs getter( clause, toRemove, "iceCommandDelegationRenewal::renewAllDelegations" );
	    db::GetJobsByDNMyProxy getter( toRemove, thisUserDN, thisMyPR, "iceCommandDelegationRenewal::renewAllDelegations" );
	    db::Transaction tnx(false, false);
	    tnx.execute( &getter );
	  }
	  
	  list<CreamJob>::iterator jobit;// = toRemove.begin();
	  for( jobit = toRemove.begin(); jobit != toRemove.end(); ++jobit ) {
//	  while( jobit != toRemove.end() ) {
  
    	    jobit->set_status( cream_api::job_statuses::ABORTED );
    	    jobit->set_failure_reason( "Proxy expired" );
//	    ++jobit;
	  }
	  
	  
	  while( IceCore::instance()->get_ice_lblog_pool()->get_command_count() > 2 )
	    sleep(2);
	  
	  IceCore::instance()->get_ice_lblog_pool()->add_request( new iceCommandLBLogging( toRemove ) );
	  
	  // TODO: Bisogna loggare un aborted per tutti i job relativi a questa delega
	  // che non verra' piu' rinnovata ed e' stata rimossa dal DB di ICE.

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
			<< IceUtils::time_t_to_string(thisBetterPrx.get<1>()) << "]"
			);
	try {
	  
	  string thisDelegUrl = thisCEUrl;
	  
	  boost::replace_all( thisDelegUrl, 
			      IceConfManager::instance()->getConfiguration()->ice()->cream_url_postfix(), 
			      IceConfManager::instance()->getConfiguration()->ice()->creamdelegation_url_postfix() 
			      );
	  
	  CreamProxy_ProxyRenew( thisDelegUrl,
				 thisBetterPrx.get<0>(),
				 thisDelegID,
				 false /* ignore blacklisted CE */).execute( 3 );
	  
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
