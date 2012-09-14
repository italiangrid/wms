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

#include "glite/ce/cream-client-api-c/CreamProxyFactory.h"
#include "glite/ce/cream-client-api-c/scoped_timer.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"

#include "CreamProxyMethod.h"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "IceConfManager.h"
#include "BlackListFailJob_ex.h"

#include <boost/scoped_ptr.hpp>

namespace soap_proxy = glite::ce::cream_client_api::soap_proxy;
namespace cream_ex = glite::ce::cream_client_api::cream_exceptions;
namespace api_util = glite::ce::cream_client_api::util;

using namespace glite::wms::ice::util;
using namespace std;

CreamProxyMethod::CreamProxyMethod( const string& creamurl, const bool honor_blacklist ) :
    m_blacklist( CEBlackList::instance() ),
    m_honor_blacklist( honor_blacklist ),
    m_service( creamurl )
{

}

void CreamProxyMethod::execute( int ntries ) // can throw anything
{
    static const char* method_name = "CreamProxyMethod::execute() - ";
    log4cpp::Category* m_log_dev( api_util::creamApiLogger::instance()->getLogger() );
    bool do_retry = true;
    int retry_count = 1;
    int conn_timeout = IceConfManager::instance()->getConfiguration()->ice()->soap_timeout(); // Timeout to set for each try
    int conn_timeout_delta = conn_timeout / 2; // Timeout increase for each try
    int delay = 1; // How much to sleep after each try

    for ( retry_count = 1; do_retry; ++retry_count ) {

        // First, check whether the service is blacklisted
	if( m_honor_blacklist ) {
          if ( m_blacklist->is_blacklisted( m_service ) ) {
	  
	    if (IceConfManager::instance()->getConfiguration()->ice()->fail_job_blacklisted_ce()) {
	      // FAIL THE JOB!!
	      throw BlackListFailJob_ex( string("The endpoint [") + m_service + "] is blacklisted and ICE is configured to abort the job");
	    }
	  
            throw cream_ex::ConnectionTimeoutException( "The endpoint [" + m_service + "] is blacklisted" ); // FIXME: throw different exception?
          }
	}
        try {
            this->method_call( conn_timeout );            
            do_retry = false; // if everything goes well, do not retry
        } catch( cream_ex::ConnectionTimeoutException& ex ) {
            if ( retry_count < ntries ) {
                CREAM_SAFE_LOG( m_log_dev->warnStream()
                                << method_name 
                                << "Connection timed out to CREAM [" << m_service<<" ]: \""
                                << ex.what()
                                << "\" on try " << retry_count << "/" << ntries
                                << ". Trying again in " << delay << " sec..."
                                 );
                do_retry = true; // superfluous
                sleep( delay );
                delay += 2; // increments delay for each try
                conn_timeout += conn_timeout_delta;
            } else {
                CREAM_SAFE_LOG( m_log_dev->errorStream()
                                << method_name << "Connection timed out to CREAM: \""
                                << ex.what()
                                << "\" on try " << retry_count << "/" << ntries
                                << ". Blacklisting endpoint [" 
				<< m_service << "] and giving up."
                                 );
                m_blacklist->blacklist_endpoint( m_service );
		if (IceConfManager::instance()->getConfiguration()->ice()->fail_job_blacklisted_ce()) {
	          // FAIL THE JOB!!
	          throw BlackListFailJob_ex( string("The endpoint [") + m_service + "] is blacklisted and ICE if configured to abort the job");
	        }
                throw; // rethrow
            }            
        } catch( ... ) {
            throw; // rethrow anything else
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Register
//
//////////////////////////////////////////////////////////////////////////////

CreamProxy_Register::CreamProxy_Register( const string& service_uri,
                                          const string& certfile,
                                          const soap_proxy::AbsCreamProxy::RegisterArrayRequest* req,
                                          soap_proxy::AbsCreamProxy::RegisterArrayResult* res,
					  const string& iceid,
					  const bool honor_blacklist  ) 
  : CreamProxyMethod( service_uri, honor_blacklist ),
    m_certfile( certfile ),
    m_req( req ),
    m_res( res ),
    m_iceid( iceid )  
{
  
}

void CreamProxy_Register::method_call( int timeout ) 
  throw(cream_ex::BaseException&,
	cream_ex::InvalidArgumentException&,
	cream_ex::GridProxyDelegationException&,
	cream_ex::JobSubmissionDisabledException&,
	cream_ex::JobStatusInvalidException&,
	cream_ex::JobUnknownException&,
	cream_ex::GenericException&,
	cream_ex::AuthorizationException&,
	cream_ex::DelegationException&,
	cream_ex::InternalException&,
	cream_ex::ConnectionTimeoutException&,
	soap_proxy::auth_ex&) 
{
  boost::scoped_ptr< soap_proxy::AbsCreamProxy > theProxy( soap_proxy::CreamProxyFactory::make_CreamProxyRegister( m_req, m_res, timeout ) );   
  theProxy->setCredential( m_certfile );
  theProxy->setSoapHeader( m_iceid );
  
#ifdef ICE_PROFILE_ENABLE
  string timer_message = string("CreamProxy_Register::execute() - [")+m_service+"] TIMER";
  api_util::scoped_timer T( timer_message );
#endif
  theProxy->execute( m_service );  
}


//////////////////////////////////////////////////////////////////////////////
//
// Start
//
//////////////////////////////////////////////////////////////////////////////

CreamProxy_Start::CreamProxy_Start( const string& service,
                                    const string& certfile,
                                    const soap_proxy::JobFilterWrapper* req,
                                    soap_proxy::ResultWrapper* res,
				    const bool honor_blacklist  ):
    CreamProxyMethod( service, honor_blacklist ),
    m_certfile( certfile ),
    m_req( req ),
    m_res( res )
{
  
}

void CreamProxy_Start::method_call( int timeout ) 
  throw(cream_ex::BaseException&,
	cream_ex::InvalidArgumentException&,
	cream_ex::GridProxyDelegationException&,
	cream_ex::JobSubmissionDisabledException&,
	cream_ex::JobStatusInvalidException&,
	cream_ex::JobUnknownException&,
	cream_ex::GenericException&,
	cream_ex::AuthorizationException&,
	cream_ex::DelegationException&,
	cream_ex::InternalException&,
	cream_ex::ConnectionTimeoutException&,
	soap_proxy::auth_ex&)  
  
  
{
  boost::scoped_ptr< soap_proxy::AbsCreamProxy > theProxy( soap_proxy::CreamProxyFactory::make_CreamProxyStart( m_req, m_res, timeout ) );
  theProxy->setCredential( m_certfile );

#ifdef ICE_PROFILE_ENABLE
  string timer_message = string("CreamProxy_Start::execute() - [")+m_service+"] TIMER";
  api_util::scoped_timer T( timer_message );
#endif

  theProxy->execute( m_service );
}


//////////////////////////////////////////////////////////////////////////////
//
// Cancel
//
//////////////////////////////////////////////////////////////////////////////
CreamProxy_Cancel::CreamProxy_Cancel( const string& service, 
				      const string& certfile, 
				      const soap_proxy::JobFilterWrapper* req,
				      soap_proxy::ResultWrapper* res,
				      const bool honor_blacklist  ) :
    CreamProxyMethod( service, honor_blacklist ),
    m_certfile( certfile ),
    m_req( req ),
    m_res( res )
{
  
}

void CreamProxy_Cancel::method_call( int timeout )
  throw(cream_ex::BaseException&,
	cream_ex::InvalidArgumentException&,
	cream_ex::GridProxyDelegationException&,
	cream_ex::JobSubmissionDisabledException&,
	cream_ex::JobStatusInvalidException&,
	cream_ex::JobUnknownException&,
	cream_ex::GenericException&,
	cream_ex::AuthorizationException&,
	cream_ex::DelegationException&,
	cream_ex::InternalException&,
	cream_ex::ConnectionTimeoutException&,
	soap_proxy::auth_ex&)   
                                     
{
  boost::scoped_ptr< soap_proxy::AbsCreamProxy > theProxy(soap_proxy::CreamProxyFactory::make_CreamProxyCancel( m_req, m_res, timeout ));
  theProxy->setCredential( m_certfile );

#ifdef ICE_PROFILE_ENABLE
  string timer_message = string("CreamProxy_Cancel::execute() - [")+m_service+"] TIMER";
  api_util::scoped_timer T( timer_message );
#endif

  theProxy->execute( m_service );
}
    
//////////////////////////////////////////////////////////////////////////////
//
// Lease
//
//////////////////////////////////////////////////////////////////////////////
CreamProxy_Lease::CreamProxy_Lease( const std::string& service,
				    const std::string& certfile,
				    const std::pair<std::string, time_t>& lease_IN,
				    std::pair<std::string, time_t>* lease_OUT,
				    const bool honor_blacklist  ) :
    CreamProxyMethod( service, honor_blacklist ),
    m_certfile( certfile ),
    m_lease_IN( lease_IN ),
    m_lease_OUT( lease_OUT )
{
  
}

void CreamProxy_Lease::method_call( int timeout ) 
  throw(cream_ex::BaseException&,
	cream_ex::InvalidArgumentException&,
	cream_ex::GridProxyDelegationException&,
	cream_ex::JobSubmissionDisabledException&,
	cream_ex::JobStatusInvalidException&,
	cream_ex::JobUnknownException&,
	cream_ex::GenericException&,
	cream_ex::AuthorizationException&,
	cream_ex::DelegationException&,
	cream_ex::InternalException&,
	cream_ex::ConnectionTimeoutException&,
	soap_proxy::auth_ex&)   
            
{
  boost::scoped_ptr< soap_proxy::AbsCreamProxy > theProxy(soap_proxy::CreamProxyFactory::make_CreamProxyLease( m_lease_IN, m_lease_OUT, timeout ));
  theProxy->setCredential( m_certfile );

#ifdef ICE_PROFILE_ENABLE
  string timer_message = string("CreamProxy_Lease::execute() - [")+m_service+"] TIMER";
  api_util::scoped_timer T( timer_message );
#endif

  theProxy->execute( m_service );
}
    
//////////////////////////////////////////////////////////////////////////////
//
// Lease
//
//////////////////////////////////////////////////////////////////////////////
CreamProxy_LeaseInfo::CreamProxy_LeaseInfo( const std::string& service,
					    const std::string& certfile,
					    const std::string& lease_IN,
					    std::pair<std::string, time_t>* lease_OUT,
					    const bool honor_blacklist  ) :
    CreamProxyMethod( service, honor_blacklist ),
    m_certfile( certfile ),
    m_lease_IN( lease_IN ),
    m_lease_OUT( lease_OUT )
{
  
}

void CreamProxy_LeaseInfo::method_call( int timeout ) 
  throw(cream_ex::BaseException&,
	cream_ex::InvalidArgumentException&,
	cream_ex::GridProxyDelegationException&,
	cream_ex::JobSubmissionDisabledException&,
	cream_ex::JobStatusInvalidException&,
	cream_ex::JobUnknownException&,
	cream_ex::GenericException&,
	cream_ex::AuthorizationException&,
	cream_ex::DelegationException&,
	cream_ex::InternalException&,
	cream_ex::ConnectionTimeoutException&,
	soap_proxy::auth_ex&)   
            
{
  boost::scoped_ptr< soap_proxy::AbsCreamProxy > theProxy(soap_proxy::CreamProxyFactory::make_CreamProxyLeaseInfo( m_lease_IN, m_lease_OUT, timeout ));
  theProxy->setCredential( m_certfile );

#ifdef ICE_PROFILE_ENABLE
  string timer_message = string("CreamProxy_LeaseInfo::execute() - [")+m_service+"] TIMER";
  api_util::scoped_timer T( timer_message );
#endif

  theProxy->execute( m_service );
}

//////////////////////////////////////////////////////////////////////////////
//
// Info
//
//////////////////////////////////////////////////////////////////////////////
CreamProxy_Info::CreamProxy_Info( const std::string& service, 
				  const std::string& certfile, 
				  const soap_proxy::JobFilterWrapper* req, 
				  soap_proxy::AbsCreamProxy::InfoArrayResult* res,
				  const bool honor_blacklist ) :
    CreamProxyMethod( service, honor_blacklist ),
    m_certfile( certfile ),
    m_req( req ),
    m_res( res )
{
  
}

void CreamProxy_Info::method_call( int timeout )  
  throw(cream_ex::BaseException&,
	cream_ex::InvalidArgumentException&,
	cream_ex::GridProxyDelegationException&,
	cream_ex::JobSubmissionDisabledException&,
	cream_ex::JobStatusInvalidException&,
	cream_ex::JobUnknownException&,
	cream_ex::GenericException&,
	cream_ex::AuthorizationException&,
	cream_ex::DelegationException&,
	cream_ex::InternalException&,
	cream_ex::ConnectionTimeoutException&,
	soap_proxy::auth_ex&) 
{   
  boost::scoped_ptr< soap_proxy::AbsCreamProxy > theProxy( soap_proxy::CreamProxyFactory::make_CreamProxyInfo( m_req, m_res, timeout ) );   
  theProxy->setCredential( m_certfile ); 

#ifdef ICE_PROFILE_ENABLE
  string timer_message = string("CreamProxy_Info::execute() - [")+m_service+"] TIMER";
  api_util::scoped_timer T( timer_message );
#endif

  theProxy->execute( m_service );
}

//////////////////////////////////////////////////////////////////////////////
//
// Purge
//
//////////////////////////////////////////////////////////////////////////////
CreamProxy_Purge::CreamProxy_Purge( const std::string& service,
			  const std::string& certfile,
                          const glite::ce::cream_client_api::soap_proxy::JobFilterWrapper* req, 
			  glite::ce::cream_client_api::soap_proxy::ResultWrapper* res,
			  const bool honor_blacklist  ) :
    CreamProxyMethod( service, honor_blacklist ),
    m_certfile( certfile ),
    m_req( req ),
    m_res( res )
{
  
}

void CreamProxy_Purge::method_call( int timeout ) 
  throw(cream_ex::BaseException&,
	cream_ex::InvalidArgumentException&,
	cream_ex::GridProxyDelegationException&,
	cream_ex::JobSubmissionDisabledException&,
	cream_ex::JobStatusInvalidException&,
	cream_ex::JobUnknownException&,
	cream_ex::GenericException&,
	cream_ex::AuthorizationException&,
	cream_ex::DelegationException&,
	cream_ex::InternalException&,
	cream_ex::ConnectionTimeoutException&,
	soap_proxy::auth_ex&)
{    
  boost::scoped_ptr< soap_proxy::AbsCreamProxy > theProxy( soap_proxy::CreamProxyFactory::make_CreamProxyPurge( m_req, m_res, timeout ) );   
  theProxy->setCredential( m_certfile );

#ifdef ICE_PROFILE_ENABLE
  string timer_message = string("CreamProxy_Purge::execute() - [")+m_service+"] TIMER";
  api_util::scoped_timer T( timer_message );
#endif

  theProxy->execute( m_service );
}


//////////////////////////////////////////////////////////////////////////////
//
// Delegate
//
//////////////////////////////////////////////////////////////////////////////
CreamProxy_Delegate::CreamProxy_Delegate( const std::string& service,
					  const std::string& certfile,
					  const std::string& delegation_id,
					  const bool honor_blacklist  ) :
    CreamProxyMethod( service, honor_blacklist ),
    m_certfile( certfile ),
    m_delegation_id( delegation_id )
{
  
}

void CreamProxy_Delegate::method_call( int timeout ) 
  throw(cream_ex::BaseException&,
	cream_ex::InvalidArgumentException&,
	cream_ex::GridProxyDelegationException&,
	cream_ex::JobSubmissionDisabledException&,
	cream_ex::JobStatusInvalidException&,
	cream_ex::JobUnknownException&,
	cream_ex::GenericException&,
	cream_ex::AuthorizationException&,
	cream_ex::DelegationException&,
	cream_ex::InternalException&,
	cream_ex::ConnectionTimeoutException&,
	soap_proxy::auth_ex&)
{ 
  boost::scoped_ptr< soap_proxy::AbsCreamProxy > theProxy( soap_proxy::CreamProxyFactory::make_CreamProxyDelegate( m_delegation_id, timeout ) );   
  theProxy->setCredential( m_certfile );

#ifdef ICE_PROFILE_ENABLE
  string timer_message = string("CreamProxy_Delegate::execute() - [")+m_service+"] TIMER";
  api_util::scoped_timer T( timer_message );  
#endif

  theProxy->execute( m_service );
}


//////////////////////////////////////////////////////////////////////////////
//
// ProxyRenew
//
//////////////////////////////////////////////////////////////////////////////
CreamProxy_ProxyRenew::CreamProxy_ProxyRenew( const std::string& service,
					      const std::string& certfile,
					      const std::string& delegation_id,
					      const bool honor_blacklist  ) :
    CreamProxyMethod( service, honor_blacklist ),
    m_certfile( certfile ),
    m_delegation_id( delegation_id )
{

}

void CreamProxy_ProxyRenew::method_call( int timeout ) 
  throw(cream_ex::BaseException&,
	cream_ex::InvalidArgumentException&,
	cream_ex::GridProxyDelegationException&,
	cream_ex::JobSubmissionDisabledException&,
	cream_ex::JobStatusInvalidException&,
	cream_ex::JobUnknownException&,
	cream_ex::GenericException&,
	cream_ex::AuthorizationException&,
	cream_ex::DelegationException&,
	cream_ex::InternalException&,
	cream_ex::ConnectionTimeoutException&,
	soap_proxy::auth_ex&)
{    
  boost::scoped_ptr< soap_proxy::AbsCreamProxy > p( soap_proxy::CreamProxyFactory::make_CreamProxy_ProxyRenew( m_delegation_id, timeout ) );   
  p->setCredential( m_certfile );

#ifdef ICE_PROFILE_ENABLE
  string timer_message = string("CreamProxy_ProxyRenew::execute() - [")+m_service+"] TIMER";
  api_util::scoped_timer T( timer_message );
#endif

  p->execute( m_service );
}

//////////////////////////////////////////////////////////////////////////////
//
// Query Event
//
//////////////////////////////////////////////////////////////////////////////

CreamProxy_QueryEvent::CreamProxy_QueryEvent( const std::string& service,
					      const std::string& certfile,
					      const std::string& fromid,
					      const std::string& toid,
					      const time_t       fromDate,
					      const std::string& type,
					      const int maxnum,
					      const vector<pair<string, string> >& properties,
					      std::string& dbid,
					      time_t& etime,
					      std::list<soap_proxy::EventWrapper*>& events,
					      const string& iceid,
					      const bool honor_blacklist  ):
  CreamProxyMethod( service, honor_blacklist ),
  m_certfile( certfile ),
  m_fromid( fromid ),
  m_toid( toid ),
  m_type( type ),
  m_maxnum( maxnum ),
  m_dbid( &dbid ),
  m_events( &events ),
  m_iceid( iceid ),
  m_etime( &etime ),
  m_fromDate( fromDate ),
  m_properties( properties )
{
}
void CreamProxy_QueryEvent::method_call( int timeout ) 
  throw(cream_ex::BaseException&,
	cream_ex::InvalidArgumentException&,
	cream_ex::GenericException&,
	cream_ex::AuthorizationException&,
	cream_ex::InternalException&,
	cream_ex::ConnectionTimeoutException&,
	soap_proxy::auth_ex&)  
{
  boost::scoped_ptr< soap_proxy::AbsCreamProxy > theProxy( soap_proxy::CreamProxyFactory::make_CreamProxy_QueryEvent( make_pair(m_fromid, m_toid), make_pair( m_fromDate, (time_t)-1), m_type, m_maxnum, 0, m_properties, *m_etime, *m_dbid, *m_events, timeout) );;
  
  theProxy->setCredential( m_certfile );
  theProxy->setSoapHeader( m_iceid );
  
  theProxy->execute( m_service );

}
