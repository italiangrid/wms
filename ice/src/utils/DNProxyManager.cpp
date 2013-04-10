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
#include "DelegationManager.h"
#include "DNProxyManager.h"
#include "IceConfManager.h"
#include "IceUtils.h"
#include "common/src/configuration/ICEConfiguration.h"
#include "glite/security/proxyrenewal/renewal.h"

#include "db/GetProxyInfoByDN.h"
#include "db/GetProxyInfoByDN_MYProxy.h"
#include "db/RemoveProxyByDN.h"
#include "db/UpdateProxyFieldsByDN.h"
#include "db/UpdateProxyCounterByDN.h"
#include "db/Transaction.h"
#include "db/CreateProxyField.h"
#include "db/GetAllProxyInfo.h"

#include "glite/ce/cream-client-api-c/VOMSWrapper.h"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"

#include <iostream>
#include <fstream>
#include <istream>
#include <ostream>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <openssl/sha.h> // for using SHA1
#include <map>
#include <string>

#include <boost/filesystem/operations.hpp>
#include <boost/tuple/tuple.hpp>

namespace iceUtil = glite::wms::ice::util;
namespace cream_api = glite::ce::cream_client_api;
using namespace std;

iceUtil::DNProxyManager* iceUtil::DNProxyManager::s_instance = NULL;
boost::recursive_mutex iceUtil::DNProxyManager::s_mutex;

//______________________________________________________________________________
iceUtil::DNProxyManager* iceUtil::DNProxyManager::getInstance() throw()
{
  boost::recursive_mutex::scoped_lock M( s_mutex );

  if( !s_instance ) 
    s_instance = new DNProxyManager();
  return s_instance;
}

//______________________________________________________________________________
iceUtil::DNProxyManager::DNProxyManager( void ) throw()
{
  m_log_dev = cream_api::util::creamApiLogger::instance()->getLogger();
  
}

/***************************************************
 *
 *
 *
 *
 *
 *    'NORMAL' Better Proxy Handling methods
 *
 *
 *
 *
 *
 ***************************************************/

//________________________________________________________________________
bool 
iceUtil::DNProxyManager::setUserProxyIfLonger_Legacy( const string& prx ) 
  throw()
{ 
  boost::recursive_mutex::scoped_lock M( s_mutex );
  
  cream_api::soap_proxy::VOMSWrapper V( prx,  !::getenv("GLITE_WMS_ICE_DISABLE_ACVER") );
  if( !V.IsValid( ) ) {
    CREAM_SAFE_LOG(m_log_dev->errorStream() 
		   << "DNProxyManager::setUserProxyIfLonger_Legacy() - "
		   << "Cannot read the proxy ["
		   << prx << "]. ICE will continue to use the old better proxy. Error is: "
		   << V.getErrorMessage()
		   );
    return false;
  }
  
  return this->setUserProxyIfLonger_Legacy( V.getDNFQAN(), prx, V.getProxyTimeEnd() );
  
}

//________________________________________________________________________
bool 
iceUtil::DNProxyManager::setUserProxyIfLonger_Legacy( const string& dn, 
						      const string& prx ) 
  throw()
{
  boost::recursive_mutex::scoped_lock M( s_mutex );

  time_t newT;

  try {

    newT= glite::ce::cream_client_api::certUtil::getProxyTimeLeft( prx );

  } catch(exception& ex) {
    CREAM_SAFE_LOG(m_log_dev->errorStream() 
		   << "DNProxyManager::setUserProxyIfLonger_Legacy() - "
		   << "Cannot retrieve time left for proxy ["
		   << prx << "]. ICE will continue to use the old better proxy. Error is: "
		   << ex.what()
		   );
    
    return false;
  }

  return this->setUserProxyIfLonger_Legacy( dn, prx, newT );
}

//________________________________________________________________________
bool
iceUtil::DNProxyManager::setUserProxyIfLonger_Legacy( 
						     const string& dn, 
						     const string& prx,
						     const time_t exptime)
  throw()
{ 
  boost::recursive_mutex::scoped_lock M( s_mutex );
  
  string localProxy = iceUtil::IceConfManager::instance()->getConfiguration()->ice()->persist_dir() + "/" + IceUtils::compressed_string( dn ) + ".proxy";
  
  
  bool found = false;
  boost::tuple<string, time_t, long long int> proxy_info;
  try {
    db::GetProxyInfoByDN getter( dn, "DNProxyManager::setUserProxyIfLonger_Legacy" );
    db::Transaction tnx(false, false);
    tnx.execute( &getter );
    found = getter.found();
    if(found)
      proxy_info = getter.get_info();
  } catch( db::DbOperationException& ex ) {

    CREAM_SAFE_LOG(m_log_dev->errorStream() 
		   << "DNProxyManager::setUserProxyIfLonger_Legacy() - "
		   << "Error setting longest proxy "
		   << " for userdn [" << dn 
		   << "]: "
		   << ex.what()
		   );
    return false;
  }

  if( !found ) {
    
    try {
      
      this->copyProxy( prx, localProxy );
      
    } catch(CopyProxyException& ex) {
      CREAM_SAFE_LOG(m_log_dev->errorStream() 
		     << "DNProxyManager::setUserProxyIfLonger_Legacy() - Error copying proxy ["
		     << prx << "] to ["
		     << localProxy << "]: " << ex.what()
		     );
      
      return false;
      
    }
    
    CREAM_SAFE_LOG(m_log_dev->debugStream() 
		   << "DNProxyManager::setUserProxyIfLonger_Legacy() - "
		   << "DN ["
		   << dn << "] not found. Inserting the new proxy ["
		   << prx << "]. Will be Copied into ["
		   << localProxy << "] - New Expiration Time is ["
		   << IceUtils::time_t_to_string(exptime) << "]"
		   );
    
    try {
      db::CreateProxyField creator( dn, "", localProxy, exptime, 0, "DNProxyManager::setUserProxyIfLonger_Legacy" );
      db::Transaction tnx( false, false );
      tnx.execute( &creator );
    } catch( db::DbOperationException& ex ) {
      
      CREAM_SAFE_LOG(m_log_dev->errorStream() 
		     << "DNProxyManager::setUserProxyIfLonger_Legacy() - "
		     << "Error setting longest proxy "
		     << " for userdn [" << dn 
		     << "]: "
		     << ex.what()
		     );
      return false;
    }
    
    return true;
  }
  
  time_t newT, oldT;

  newT = exptime;
  oldT = proxy_info.get<1>();

  if( !oldT ) { // couldn't get the proxy time for some reason
    try {
      this->copyProxy( prx, localProxy );
    } catch(CopyProxyException& ex) {
      CREAM_SAFE_LOG(m_log_dev->errorStream() 
     		     << "DNProxyManager::setUserProxyIfLonger_Legacy() - Error copying proxy ["
		     << prx << "] to ["
		     << localProxy << "]: " << ex.what()
     		     );
      return false;
    }

    CREAM_SAFE_LOG(m_log_dev->debugStream() 
		   << "DNProxyManager::setUserProxyIfLonger_Legacy() - "
		   << "New proxy ["
		   << prx << "] has been copied into ["
		   << localProxy << "] - New Expiration Time is ["
		   << IceUtils::time_t_to_string(exptime) << "]"
		   );
    try {
      db::CreateProxyField creator( dn, "", localProxy, exptime, 0,"DNProxyManager::setUserProxyIfLonger_Legacy" );
      db::Transaction tnx(false, false);
      tnx.execute( &creator );
    } catch( db::DbOperationException& ex ) {
      
      CREAM_SAFE_LOG(m_log_dev->errorStream() 
		     << "DNProxyManager::setUserProxyIfLonger_Legacy() - "
		     << "Error setting longest proxy "
		     << " for userdn [" << dn 
		     << "]: "
		     << ex.what()
		     );
      return false;
    }

    return true;
  }
  
  if(newT > oldT) {
    CREAM_SAFE_LOG(m_log_dev->infoStream() 
		   << "DNProxyManager::setUserProxyIfLonger_Legacy() - "
		   << "Setting user proxy to ["
		   << prx
		   << "] copied to " << localProxy 
		   << "] because the old one is less long-lived."
		   );

    try {

      this->copyProxy( prx, localProxy );

    } catch(CopyProxyException& ex) {
      CREAM_SAFE_LOG(m_log_dev->errorStream() 
		     << "DNProxyManager::setUserProxyIfLonger_Legacy() - Error copying proxy ["
		     << prx << "] to ["
		     << localProxy << "]: " << ex.what()
		     );
	return false;
    }

    CREAM_SAFE_LOG(m_log_dev->debugStream() 
		   << "DNProxyManager::setUserProxyIfLonger_Legacy() - "
		   << "New proxy ["
		   << prx << "] has been copied into ["
		   << localProxy << "] - New Expiration Time is ["
		   << IceUtils::time_t_to_string(newT) << "]"
		   );

    try {
      db::CreateProxyField creator( dn, "",localProxy, exptime, 0, "DNProxyManager::setUserProxyIfLonger_Legacy" );
      db::Transaction tnx(false, false);
      tnx.execute( &creator );
    } catch( db::DbOperationException& ex ) {
      
      CREAM_SAFE_LOG(m_log_dev->errorStream() 
		     << "DNProxyManager::setUserProxyIfLonger_Legacy() - "
		     << "Error setting longest proxy "
		     << " for userdn [" << dn 
		     << "]: "
		     << ex.what()
		     );
      return false;
    }
    return true;
  }
  return true;
}

//________________________________________________________________________
void iceUtil::DNProxyManager::copyProxy( const string& source, const string& target ) 
  throw(CopyProxyException&)
{
   string tmpTarget = target + ".tmp";
   ::unlink( tmpTarget.c_str( ) );
  
   try {
//      std::ifstream in (source.c_str());
//      std::ofstream out(tmpTarget.c_str());
//      out << in.rdbuf();
//      out.close();
//      in.close();
     boost::filesystem::copy_file( boost::filesystem::path(source,boost::filesystem::native), boost::filesystem::path(tmpTarget,boost::filesystem::native) );
   } catch( exception& ex ) {

     ::unlink( tmpTarget.c_str( ) ); // actually this is redundant (see at the beginning of the func)
     
     throw CopyProxyException(string("Couldn't copy [")+source +"] to ["+tmpTarget + "]: " + ex.what());
   }
   
   // copy is ok. Now must rename the tmp to the original proxy file
   
   
   /** 
       from man page (2) of rename:
       If newpath already exists it will be atomically 
       replaced, 
       so that there is no point
       at which another process attempting 
       to access newpath will find it missing.
       
       If newpath exists but the operation fails for some 
       reason rename() guarantees to leave an instance of newpath in place.
   */
   int rc = ::rename( tmpTarget.c_str( ), target.c_str( ) );
   if (rc ) {
     // if something failed, we're sure that original proxy is still there
     // and we must just remote the tmpTarget and NOT update the ICE's database
     ::unlink( tmpTarget.c_str( ) ); // actually this is redundant (see at the beginning of the func)
     throw CopyProxyException(string("Couldn't rename new proxy [")+tmpTarget +"] to ["+target + "]: " + strerror(errno));
   }
   ::chmod( target.c_str( ), S_IRUSR | S_IWUSR );
}

/**********************************************
 *
 *
 *
 *
 *  'SUPER' Better Proxy Handling methods
 *  
 *
 *
 *
 *
 **********************************************/
//________________________________________________________________________
bool 
iceUtil::DNProxyManager::removeBetterProxy( const string& userdn, const string& myproxyname )
  throw()
{
  boost::recursive_mutex::scoped_lock M( s_mutex );

  CREAM_SAFE_LOG(m_log_dev->debugStream() 
		 << "DNProxyManager::removeBetterProxy() - "
		 << "Removing from DB better proxy for userdn ["
		 << userdn << "] MyProxy server ["
		 << myproxyname << "]"
		 );

  try {
    db::RemoveProxyByDN remover( userdn, myproxyname, "DNProxyManager::removeBetterProxy" );
    db::Transaction tnx(false, false);
    tnx.execute( &remover );
  } catch( db::DbOperationException& ex ) {

    CREAM_SAFE_LOG(m_log_dev->errorStream() 
		   << "DNProxyManager::removeBetterProxy() - "
		   << "Error removing better proxy "
		   << " for userdn [" << userdn 
		   << "] myproxy ["
		   << myproxyname << "]: "
		   << ex.what()
		   );
    return false;
  }
  
  string localProxy = this->make_betterproxy_path(userdn, myproxyname); 
  
  CREAM_SAFE_LOG(
		 m_log_dev->debugStream()
		 << "DNProxyManager::removeBetterProxy() - "
		 << "Unlinking proxy file ["
		 << localProxy << "]... "
		 );
  
  if(::unlink( localProxy.c_str() ) < 0 )
    {
      int saveerr = errno;
      CREAM_SAFE_LOG(
		     m_log_dev->errorStream()
		     << "DNProxyManager::removeBetterProxy() - "
		     << "Unlink of file ["
		     << localProxy << "] is failed. Error is: "
		     << strerror(saveerr);
		     );
    }
  return true;
}

//________________________________________________________________________
bool 
iceUtil::DNProxyManager::updateBetterProxy( const string& userDN, 
					    const string& myproxyname,
					    const boost::tuple<string, time_t, long long int>& newEntry )
  throw()
{
  boost::recursive_mutex::scoped_lock M( s_mutex );
  
  string localProxy = this->make_betterproxy_path(userDN, myproxyname );//iceUtil::iceConfManager::getInstance()->getConfiguration()->ice()->persist_dir() + "/" + compressed_string( this->composite( userDN, myproxyname ) ) + ".betterproxy";
  


  try {
    
    this->copyProxy( newEntry.get<0>(), localProxy );
    
  } catch(CopyProxyException& ex) {
    CREAM_SAFE_LOG(m_log_dev->errorStream() 
		   << "DNProxyManager::updateBetterProxy() - Error copying proxy ["
		   << newEntry.get<0>() << "] to ["
		   << localProxy << ".tmp]."
		   );
    
    return false;
  }

//   try {
//     boost::filesystem::path pathSrc( localProxy + ".tmp", boost::filesystem::native );
//     boost::filesystem::path pathTrg( localProxy, boost::filesystem::native );
//     boost::filesystem::rename( pathSrc, pathTrg );  
//   } catch(exception& ex) {
//   
//     CREAM_SAFE_LOG(m_log_dev->fatalStream() 
// 		   << "DNProxyManager::updateBetterProxy() - Error renaming ["
// 		   << localProxy+".tmp" << "] to ["
// 		   << localProxy << "]: " << ex.what( )
// 		   );
//     return false;
//   }

  try {
    db::UpdateProxyFieldsByDN updater( userDN, myproxyname, localProxy, newEntry.get<1>(), "DNProxyManager::updateBetterProxy" );
    db::Transaction tnx(false, false);
    tnx.execute( &updater );
  } catch( db::DbOperationException& ex ) {
    
    CREAM_SAFE_LOG(m_log_dev->errorStream() 
		   << "DNProxyManager::updateBetterProxy() - "
		   << "Error updating better proxy "
		   << " for userdn [" << userDN 
		   << "] myproxy ["
		   << myproxyname << "]: "
		   << ex.what()
		   );
    return false;
  }
  return true;
}

//________________________________________________________________________
bool
iceUtil::DNProxyManager::setBetterProxy( const string& dn, 
					 const string& proxyfile,
					 const string& myproxyname,
					 const time_t proxy_time_end,
					 const unsigned long long init_counter)
  throw()
{
  boost::recursive_mutex::scoped_lock M( s_mutex );
  
  string localProxy = this->make_betterproxy_path(dn, myproxyname);//iceUtil::iceConfManager::getInstance()->getConfiguration()->ice()->persist_dir() + "/" + compressed_string( this->composite( dn, myproxyname ) ) + ".betterproxy";
  
  try {
    
    this->copyProxy( proxyfile, localProxy );
    
  } catch(CopyProxyException& ex) {
    CREAM_SAFE_LOG(m_log_dev->errorStream() 
		   << "DNProxyManager::setBetterProxy() - Error copying proxy ["
		   << proxyfile<< "] to ["
		   << localProxy << "]."
		   );
    
    return false;
    
  }
  
  //string mapKey = this->composite( dn, myproxyname );

  CREAM_SAFE_LOG(m_log_dev->debugStream() 
		 << "DNProxyManager::setBetterProxy() - "
		 << "Inserting proxy entry into DB for DN ["
		 << dn 
		 << "] and MyProxy ["
		 << myproxyname
		 << "] Proxy file ["
		 << localProxy << "] counter=["
		 << init_counter << "]"
		 );

  try {
    db::CreateProxyField creator( dn, myproxyname, localProxy, proxy_time_end, init_counter, "DNProxyManager::setBetterProxy" );
    db::Transaction tnx(false, false);
    tnx.execute( &creator );    
  } catch( db::DbOperationException& ex ) {
    
    CREAM_SAFE_LOG(m_log_dev->errorStream() 
		   << "DNProxyManager::setBetterProxy() - "
		   << "Error updating better proxy "
		   << " for userdn [" << dn 
		   << "] myproxy ["
		   << myproxyname << "]: "
		   << ex.what()
		   );
    return false;
  }

  return true;
}

//________________________________________________________________________
boost::tuple<string, time_t, long long int> 
iceUtil::DNProxyManager::getAnyBetterProxyByDN( const string& dn )
  const throw() 
{
  boost::recursive_mutex::scoped_lock M( s_mutex );
  
	// FIXME: controllare la validita' prima di tornare il proxy

  boost::tuple<std::string, time_t, long long int> result;
  try {
    db::GetProxyInfoByDN getter( dn, "DNProxyManager::getAnyBetterProxyByDN");
    db::Transaction tnx(false, false);
    tnx.execute( &getter );
    if( !getter.found( ) )
      return boost::make_tuple("", 0, 0);
    result = getter.get_info( );
  } catch( db::DbOperationException& ex ) {
    
    CREAM_SAFE_LOG(m_log_dev->errorStream() 
		   << "DNProxyManager::getAnyBetterProxyByDN() - "
		   << "Error getting a proxy"
		   << " for userdn [" << dn 
		   << "]: "
		   << ex.what()
		   );
    //return false;
  }
  
  return result;

}

//________________________________________________________________________
boost::tuple<string, time_t, long long int> 
iceUtil::DNProxyManager::getExactBetterProxyByDN( const string& dn,
						  const string& myproxyname)
  const throw() 
{
  boost::recursive_mutex::scoped_lock M( s_mutex );
  
  
  boost::tuple< string, time_t, long long> proxy_info;

  try {
    db::GetProxyInfoByDN_MYProxy getter( dn, myproxyname, "DNProxyManager::getExactBetterProxyByDN" );
    db::Transaction tnx(false, false);
    tnx.execute( &getter );
    //    found = getter.found();
    if( !getter.found() )
      return boost::make_tuple("", 0, 0);
    
    proxy_info = getter.get_info();
  } catch( db::DbOperationException& ex ) {
    
    CREAM_SAFE_LOG(m_log_dev->errorStream() 
		   << "DNProxyManager::getExactBetterProxyByDN() - "
		   << "Error getting a proxy"
		   << " for userdn [" << dn 
		   << "] myproxy ["
		   << myproxyname
		   << "]: "
		   << ex.what()
		   );
    //return false;
  }

  return proxy_info;
  
}

//________________________________________________________________________
bool 
iceUtil::DNProxyManager::incrementUserProxyCounter( const CreamJob& aJob,
                                                    const time_t proxy_time_end)
  throw()
{
  boost::recursive_mutex::scoped_lock M( s_mutex );
  
  CREAM_SAFE_LOG(
		   m_log_dev->debugStream()
		   << "DNProxyManager::incrementUserProxyCounter() - "
		   << "Looking for DN ["
		   << aJob.user_dn() << "] MyProxy server ["
		   << aJob.myproxy_address() << "] in the DB..."
		   );
  
  bool found = false;
  boost::tuple<string, time_t, long long int> proxy_info;
  try {
    db::GetProxyInfoByDN_MYProxy getter( aJob.user_dn(), aJob.myproxy_address(), 
					 "DNProxyManager::incrementUserProxyCounter" );
    db::Transaction tnx(false, false);
    tnx.execute( &getter );
    found = getter.found();
    if(found)
      proxy_info = getter.get_info();
  } catch( db::DbOperationException& ex ) {
    
    CREAM_SAFE_LOG(m_log_dev->errorStream() 
		   << "DNProxyManager::incrementUserProxyCounter() - "
		   << "Error executing GetProxyInfoByDN_MYProxy for userdn ["
		   << aJob.user_dn()
		   << "] myproxy ["
		   << aJob.myproxy_address()
		   << "]: "
		   << ex.what()
		   );
    return false;
  }

  if( found ) {

    CREAM_SAFE_LOG(
		   m_log_dev->debugStream()
		   << "DNProxyManager::incrementUserProxyCounter() - "
		   << "Incrementing proxy counter for DN ["
		   << aJob.user_dn() << "] MyProxy server ["
		   << aJob.myproxy_address() <<"] from [" << proxy_info.get<2>()
		   << "] to [" << (proxy_info.get<2>() +1 ) << "]"
		   );

    try {
      
      db::UpdateProxyCounterByDN updater( aJob.user_dn(), aJob.myproxy_address(), proxy_info.get<2>() + 1, "DNProxyManager::incrementUserProxyCounter" );
      db::Transaction tnx(false, false);
      tnx.execute( &updater );
      
    } catch( db::DbOperationException& ex ) {
      
      CREAM_SAFE_LOG(m_log_dev->errorStream() 
		     << "DNProxyManager::incrementUserProxyCounter() - "
		     << "Error updating proxy counter for userdn ["
		     << aJob.user_dn()
		     << "] myproxy ["
		     << aJob.myproxy_address()
		     << "]: "
		     << ex.what()
		     );
      return false;
    }
    
    return true;

  } else {
    return this->setBetterProxy( aJob.user_dn(), 
				 aJob.user_proxyfile(),
				 aJob.myproxy_address(),
				 proxy_time_end,
				 (unsigned long long)1);
  }
  return true;
}

//________________________________________________________________________
bool 
iceUtil::DNProxyManager::decrementUserProxyCounter( const string& userDN, 
						    const string& myproxy_name ) 
  throw()
{
  boost::recursive_mutex::scoped_lock M( s_mutex );
  
  CREAM_SAFE_LOG(
		   m_log_dev->debugStream()
		   << "DNProxyManager::decrementUserProxyCounter() - "
		   << "Looking for DN ["
		   << userDN << "] MyProxy server ["
		   << myproxy_name << "] in the DB..."
		   );
  
  bool found = false;
  boost::tuple<string, time_t, long long int> proxy_info;
  try {
    db::GetProxyInfoByDN_MYProxy getter( userDN, myproxy_name, "DNProxyManager::decrementUserProxyCounter" );
    db::Transaction tnx(false, false);
    tnx.execute( &getter );
    found = getter.found();
    if(found)
      proxy_info = getter.get_info();
  } catch( db::DbOperationException& ex ) {
      
    CREAM_SAFE_LOG(m_log_dev->errorStream() 
		   << "DNProxyManager::decrementUserProxyCounter() - "
		   << "Error executing GetProxyInfoByDN_MYProxy for userdn ["
		   << userDN
		   << "] myproxy ["
		   << myproxy_name
		   << "]: "
		   << ex.what()
		   );
    return false;
  }
  
  if( found ) {

    CREAM_SAFE_LOG(
		   m_log_dev->debugStream()
		   << "DNProxyManager::decrementUserProxyCounter() - "
		   << "Decrementing proxy counter for DN ["
		   << userDN << "] MyProxy server ["
		   << myproxy_name <<"] from [" << proxy_info.get<2>()
		   << "] to [" << (proxy_info.get<2>() - 1) << "]"
		   );

    try {
      //list<pair<string, string> > params;
      //ostringstream tmp;
      //tmp << (proxy_info.get<2>() - 1);
      //params.push_back( make_pair("counter", tmp.str()) );
      db::UpdateProxyCounterByDN updater( userDN, myproxy_name, proxy_info.get<2>() - 1, "DNProxyManager::decrementUserProxyCounter" );
      db::Transaction tnx(false, false);
      tnx.execute( &updater );
    } catch( db::DbOperationException& ex ) {
      
      CREAM_SAFE_LOG(m_log_dev->errorStream() 
		     << "DNProxyManager::decrementUserProxyCounter() - "
		     << "Error updating proxy counter for userdn ["
		     << userDN
		     << "] myproxy ["
		     << myproxy_name
		     << "]: "
		     << ex.what()
		     );
      return false;
    }
    

    if( proxy_info.get<2>() == 1 ) {

      CREAM_SAFE_LOG(
		   m_log_dev->debugStream()
		   << "DNProxyManager::decrementUserProxyCounter() - "
		   << "Proxy Counter is ZERO for DN ["
		   << userDN << "] MyProxy server ["
		   << myproxy_name<<"]. Removing better proxy ["
		   << proxy_info.get<0>()
		   << "] from persist_dir..."
		   );
      /**
	 If the counter is 0 deregister the current proxy and clear the map and delete the file.
      */

      bool ok = this->removeBetterProxy( userDN, myproxy_name );
    
      try {
	iceUtil::Delegation_manager::instance()->removeDelegation( userDN, myproxy_name );
      } catch( std::exception& ex ) {
	
	CREAM_SAFE_LOG(m_log_dev->errorStream() 
		       << "DNProxyManager::decrementUserProxyCounter() - "
		       << "Delegation_manager::removeDelegation returned an error for "
		       << "userdn ["
		       << userDN
		       << "] myproxy ["
		       << myproxy_name
		       << "]: "
		       << ex.what()
		       );
	return false;
      }
      
      return ok;
    }

  }
  return true;
}

//________________________________________________________________________
string 
iceUtil::DNProxyManager::make_betterproxy_path( const string& dn, 
						const string& myproxy )
  throw()
{
  return iceUtil::IceConfManager::instance()->getConfiguration()->ice()->persist_dir() + "/" + IceUtils::compressed_string( this->composite( dn, myproxy ) ) + ".betterproxy";
}
