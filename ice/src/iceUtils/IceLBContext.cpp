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

//
// This file is heavily based on org.glite.wms.jobsubmission/src/common/EventLogger.cpp
//
// See also:
// org.glite.lb.client-interface/build/producer.h
// org.glite.lb.client-interface/doc/C/latex/refman.pdf

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <iostream>
#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace fs = boost::filesystem;

#include "glite/lb/producer.h"
#include "IceLBContext.h"
#include "DNProxyManager.h"
#include "IceConfManager.h"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "common/src/configuration/ICEConfiguration.h"
#include "common/src/configuration/CommonConfiguration.h"
// #include "jobCache.h"
#include "IceUtils.h"

//#include <netdb.h>

using namespace std;
using namespace glite::wms::ice::util;
namespace configuration = glite::wms::common::configuration;

// static member definitions
unsigned int IceLBContext::s_el_s_retries = 3;
unsigned int IceLBContext::s_el_s_sleep = 5; // Delay between retries
const char *IceLBContext::el_s_notLogged = "Event not logged, context unset.";
const char *IceLBContext::el_s_unavailable = "unavailable";
const char *IceLBContext::el_s_OK = "OK";
const char *IceLBContext::el_s_failed = "Failed";
const char *IceLBContext::el_s_succesfully = "Job Terminated Successfully";

//#ifdef GLITE_WMS_HAVE_LBPROXY

#include <openssl/pem.h>
#include <openssl/x509.h>

namespace {
    
    // retrieve the subject_name from a given x509_proxy (thx to giaco)
    std::string get_proxy_subject(std::string const& x509_proxy)
    {
        static std::string const null_string;
        
        std::FILE* fd = std::fopen(x509_proxy.c_str(), "r");
        if (!fd) return null_string;
        boost::shared_ptr<std::FILE> fd_(fd, std::fclose);
        
        ::X509* const cert = ::PEM_read_X509(fd, 0, 0, 0);
        if (!cert) return null_string;
        boost::shared_ptr< ::X509> cert_(cert, ::X509_free);
        
        char* const s = ::X509_NAME_oneline(::X509_get_subject_name(cert), 0, 0);
        if (!s) return null_string;
        boost::shared_ptr<char> s_(s, ::free);
        
        return std::string(s);
    }

}

//#endif
//////////////////////////////////////////////////////////////////////////////
//
// iceLogger Exception
//
//////////////////////////////////////////////////////////////////////////////
IceLBException::IceLBException( const char *reason ) : m_le_reason( reason ? reason : "" )
{}

IceLBException::IceLBException( const string &reason ) : m_le_reason( reason )
{}

IceLBException::~IceLBException( void ) throw() {}

const char *IceLBException::what( void ) const throw()
{
  return this->m_le_reason.c_str();
}

string IceLBContext::s_localHostName( "" );

//////////////////////////////////////////////////////////////////////////////
// 
// iceLBContext
//
//////////////////////////////////////////////////////////////////////////////

//____________________________________________________________________________
IceLBContext::IceLBContext( void ) :
    el_context( new edg_wll_Context ), 
    el_s_localhost_name( ),
    m_el_hostProxy( false ),
    m_el_count( 0 ), 
    m_log_dev( glite::ce::cream_client_api::util::creamApiLogger::instance()->getLogger() )
    // m_cache( jobCache::getInstance() )
{

  {
    edg_wll_InitContext( el_context );
    
  }

    try {
      if( s_localHostName.empty() ) {
        el_s_localhost_name = IceUtils::get_host_name();  
        s_localHostName = el_s_localhost_name;
      } else {
        el_s_localhost_name = s_localHostName;
      }
    } catch( runtime_error& ex) {
        CREAM_SAFE_LOG(m_log_dev->errorStream() 
                       << "IceLBContext::CTOR - getHostName() returned an ERROR: "
                       << ex.what()
                       );
        el_s_localhost_name = "(unknown host name )"; 
	s_localHostName = "";
    }
}

IceLBContext::~IceLBContext( void )
{
  edg_wll_FreeContext( *el_context );
  delete el_context;
}

string IceLBContext::getLoggingError( const char *preamble )
{
  string       cause( preamble ? preamble : "" );

  if( preamble ) cause.append( 1, ' ' ); 

  char        *text, *desc;

  edg_wll_Error( *this->el_context, &text, &desc );
  cause.append( text );
  cause.append( " - " ); cause.append( desc );

  free( desc ); free( text );

  return cause;
}

void IceLBContext::testCode( int &code, bool retry )
{
    if ( code != 0 ) {
        string err( getLoggingError( 0 ) );
        CREAM_SAFE_LOG(m_log_dev->errorStream() << "IceLBContext::testCode - Got error " << err
		       );
        
    }

    const configuration::ICEConfiguration *conf = configuration::Configuration::instance()->ice();
    int          ret;
    string       cause, host_proxy;

    if( code ) {
        cause = this->getLoggingError( NULL );

        switch( code ) {
        case EINVAL:
	  CREAM_SAFE_LOG(m_log_dev->errorStream()
			 << "IceLBContext::testCode - Critical error in L&B calls: EINVAL. "
			 << "Cause = \"" << cause << "\"."
			 );
            
            code = 0; // Don't retry...
            break;
        case EDG_WLL_ERROR_GSS:
	  CREAM_SAFE_LOG(m_log_dev->errorStream()
			 << "IceLBContext::testCode - Severe error in GSS layer while communicating with L&B daemons. " 
			 << "Cause = \"" << cause << "\"." 
			 );

            if( this->m_el_hostProxy ) {
	      CREAM_SAFE_LOG(m_log_dev->debugStream()
			     << "IceLBContext::testCode - The log with the host certificate has just been done. Giving up." 
			     );
                
                code = 0; // Don't retry...
            }
            else {
	      host_proxy = conf->ice_host_cert();

                CREAM_SAFE_LOG(m_log_dev->debugStream()
			       << "Retrying using host proxy certificate [" 
			       << host_proxy << "]" 
			       );


                if( host_proxy.length() == 0 ) {
		  CREAM_SAFE_LOG(m_log_dev->warnStream()
				 << "IceLBContext::testCode - Host proxy file not set inside configuration file. " 
				 << "Trying with a default NULL and hoping for the best." 
				 );

                    ret = edg_wll_SetParam( *el_context, EDG_WLL_PARAM_X509_PROXY, NULL );
                }
                else {
		  CREAM_SAFE_LOG(m_log_dev->debugStream()
				 << "IceLBContext::testCode - Host proxy file found = [" << host_proxy << "]."
				 );

                    ret = edg_wll_SetParam( *el_context, EDG_WLL_PARAM_X509_PROXY, host_proxy.c_str() );
                }

                if( ret ) {
		  CREAM_SAFE_LOG(m_log_dev->errorStream()
				 << "IceLBContext::testCode - Cannot set the host proxy inside the context. Giving up." 
				 );

                    code = 0; // Don't retry.
                }
                else this->m_el_hostProxy = true; // Set and retry (code is still != 0)
            }

            break;
        default:
            if( ++this->m_el_count > s_el_s_retries ) {
	      CREAM_SAFE_LOG(m_log_dev->errorStream()
			     << "IceLBContext::testCode - L&B call retried " << this->m_el_count << " times always failed. "
			     << "Ignoring." 
			     );

                code = 0; // Don't retry anymore
            }
            else {
	      CREAM_SAFE_LOG(m_log_dev->warnStream()
			     << "IceLBContext::testCode - L&B call got a transient error (code=" << code << "). Waiting " << s_el_s_sleep << " seconds and trying again. " 
			     << "Try n. " << this->m_el_count << "/" << s_el_s_retries 
			     );

                sleep( s_el_s_sleep );
            }
            break;
        }
    }
    else // The logging call worked fine, do nothing
      CREAM_SAFE_LOG(m_log_dev->debugStream() 
		     << "IceLBContext::testCode - L&B call succeeded." 
		     );

    return;

}

void IceLBContext::setLoggingJob( const util::CreamJob& theJob, edg_wll_Source src, const bool use_cancel_seq_code ) throw ( IceLBException& )
{
    static const char* method_name = "IceLBContext::setLoggingJob - ";
    string _gid( theJob.grid_jobid() );
    edg_wlc_JobId   id;
    int res = 0;

    res = edg_wlc_JobIdParse( _gid.c_str(), &id );    

    char *lbserver;
    
    unsigned int lbport;
    edg_wlc_JobIdGetServerParts( id, &lbserver, &lbport );

    res |= edg_wll_SetParam( *el_context, EDG_WLL_PARAM_SOURCE, src );        
    res |= edg_wll_SetParam( *el_context, EDG_WLL_PARAM_DESTINATION, lbserver );
    //if ( lbserver ) free( lbserver );

    boost::tuple<string, time_t, long long int> result = DNProxyManager::getInstance()->getAnyBetterProxyByDN(theJob.user_dn());

    string seq_code;
    if( use_cancel_seq_code ) {
      seq_code = theJob.cancel_sequence_code( );
    } else {
      seq_code = theJob.sequence_code( );
    }
    
    CREAM_SAFE_LOG(m_log_dev->infoStream() << method_name
		   << "Setting log job to jobid=[" << _gid << "] "
		   << "LB server=[" << lbserver << ":" << lbport << "] SEQUENCE CODE ["
		   << seq_code <<"]"
		   << "(port is not used, actually...)"
		   );
		   
    if ( lbserver ) free( lbserver );
		   
    if ( !seq_code.empty() ) {
        if(IceConfManager::instance()->getConfiguration()->common()->lbproxy()) {
          string const user_dn( get_proxy_subject( result.get<0>()) );

          res |= edg_wll_SetLoggingJobProxy( *el_context, id, seq_code.c_str(), user_dn.c_str(), EDG_WLL_SEQ_NORMAL );
        } else 
          res |= edg_wll_SetLoggingJob( *el_context, id, seq_code.c_str(), EDG_WLL_SEQ_NORMAL );
    }
    
    edg_wlc_JobIdFree( id );

    if( res != 0 ) {
        CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name
		     << "Unable to set logging job to jobid=["
		     << _gid
		     << "]. LB error is "
		     << getLoggingError( 0 )
		     );
        throw IceLBException( this->getLoggingError("Cannot set logging job:") );
    }

    // Set user proxy for L&B stuff
    string betterproxy = result.get<0>();
    fs::path    pf( betterproxy, fs::native);
    pf.normalize();

    if( fs::exists(pf) ) {
        res = edg_wll_SetParam( *el_context, EDG_WLL_PARAM_X509_PROXY, 
                                betterproxy.c_str() );

        if( res ) {
            CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name
			 << "Unable to set logging job to jobid=["
			 << _gid
			 << "]. "
			 << getLoggingError( 0 )
			 );
            throw IceLBException( this->getLoggingError("Cannot set proxyfile path inside context:") );
        }
    } else {
        CREAM_SAFE_LOG(m_log_dev->errorStream() << method_name
		     << "Unable to set logging job to jobid=["
		     << _gid
		     << "]. Proxy file ["
		     << betterproxy
		     << "] does not exist. "
		     << "Trying to use the host proxy cert, and hoping for the best..."
		     );
    }
}
