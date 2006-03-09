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

//#include <openssl/pem.h>
//#include <openssl/x509.h>

#include "glite/lb/producer.h"
#include "iceLBContext.h"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "jobCache.h"

#include <netdb.h>

using namespace std;
using namespace glite::wms::ice::util;
namespace configuration = glite::wms::common::configuration;

// static member definitions
unsigned int iceLBContext::el_s_retries = 3;
unsigned int iceLBContext::el_s_sleep = 60;
const char *iceLBContext::el_s_notLogged = "Event not logged, context unset.";
const char *iceLBContext::el_s_unavailable = "unavailable";
const char *iceLBContext::el_s_OK = "OK";
const char *iceLBContext::el_s_failed = "Failed";

//////////////////////////////////////////////////////////////////////////////
//
// iceLogger Exception
//
//////////////////////////////////////////////////////////////////////////////
iceLBException::iceLBException( const char *reason ) : le_reason( reason ? reason : "" )
{}

iceLBException::iceLBException( const string &reason ) : le_reason( reason )
{}

iceLBException::~iceLBException( void ) throw() {}

const char *iceLBException::what( void ) const throw()
{
  return this->le_reason.c_str();
}

//////////////////////////////////////////////////////////////////////////////
// 
// iceLBContext
//
//////////////////////////////////////////////////////////////////////////////
iceLBContext::iceLBContext( void ) :
    el_context( new edg_wll_Context ), 
    el_s_localhost_name( ),
    el_hostProxy( false ),
    el_count( 0 ), 
    log_dev( glite::ce::cream_client_api::util::creamApiLogger::instance()->getLogger() ),
    _cache( jobCache::getInstance() )
{
    edg_wll_InitContext( el_context );
    char name[256];

    if ( gethostname(name, 256) < 0 ) {
        el_s_localhost_name = "(unknown host name )";
    } else {
        struct hostent *H = gethostbyname(name);
        if ( !H ) {
            el_s_localhost_name = "(unknown host name )";            
        } else {
            el_s_localhost_name = H->h_name;
        }
    }
}


iceLBContext::~iceLBContext( void )
{

}

string iceLBContext::getLoggingError( const char *preamble )
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

void iceLBContext::testCode( int &code, bool retry )
{
    if ( code != 0 ) {
        string err( getLoggingError( 0 ) );
        log_dev->errorStream() << "Got error " << err
                               << log4cpp::CategoryStream::ENDLINE;
        
    }

    const configuration::CommonConfiguration *conf = configuration::Configuration::instance()->common();
    int          ret;
    string       cause, host_proxy;

    if( code ) {
        cause = this->getLoggingError( NULL );

        switch( code ) {
        case EINVAL:
            log_dev->errorStream()
                << "Critical error in L&B calls: EINVAL. "
                << "Cause = \"" << cause << "\"."
                << log4cpp::CategoryStream::ENDLINE;
            
            code = 0; // Don't retry...
            break;
        case EDG_WLL_ERROR_GSS:
            log_dev->errorStream()
                << "Severe error in GSS layer while communicating with L&B daemons. " 
                << "Cause = \"" << cause << "\"." 
                << log4cpp::CategoryStream::ENDLINE;

            if( this->el_hostProxy ) {
                log_dev->infoStream()
                    << "The log with the host certificate has just been done. Giving up." 
                    << log4cpp::CategoryStream::ENDLINE;
                
                code = 0; // Don't retry...
            }
            else {
                host_proxy = conf->host_proxy_file();

                log_dev->infoStream()
                    << "Retrying using host proxy certificate [" 
                    << host_proxy << "]" 
                    << log4cpp::CategoryStream::ENDLINE;


                if( host_proxy.length() == 0 ) {
                    log_dev->warnStream()
                        << "Host proxy file not set inside configuration file. " 
                        << "Trying with a default NULL and hoping for the best." 
                        << log4cpp::CategoryStream::ENDLINE;

                    ret = edg_wll_SetParam( *el_context, EDG_WLL_PARAM_X509_PROXY, NULL );
                }
                else {
                    log_dev->infoStream()
                        << "Host proxy file found = [" << host_proxy << "]."
                        << log4cpp::CategoryStream::ENDLINE;

                    ret = edg_wll_SetParam( *el_context, EDG_WLL_PARAM_X509_PROXY, host_proxy.c_str() );
                }

                if( ret ) {
                    log_dev->errorStream()
                        << "Cannot set the host proxy inside the context. Giving up." 
                        << log4cpp::CategoryStream::ENDLINE;

                    code = 0; // Don't retry.
                }
                else this->el_hostProxy = true; // Set and retry (code is still != 0)
            }

            break;
        default:
            if( ++this->el_count > el_s_retries ) {
                log_dev->errorStream()
                    << "L&B call retried " << this->el_count << " times always failed. "
                    << "Ignoring." 
                    << log4cpp::CategoryStream::ENDLINE;

                code = 0; // Don't retry anymore
            }
            else {
                log_dev->warnStream()
                    << "L&B call got a transient error. Waiting " << el_s_sleep << " seconds and trying again. " 
                    << "Try n. " << this->el_count << "/" << el_s_retries 
                    << log4cpp::CategoryStream::ENDLINE;

                sleep( el_s_sleep );
            }
            break;
        }
    }
    else // The logging call worked fine, do nothing
        log_dev->debugStream() 
            << "L&B call succeeded." 
            << log4cpp::CategoryStream::ENDLINE;

    // SignalChecker::instance()->throw_on_signal();

    return;

}

void iceLBContext::registerJob( const util::CreamJob& theJob )
{
    int res;
    edg_wlc_JobId   id;

    setLoggingJob( theJob, EDG_WLL_SOURCE_JOB_SUBMISSION );

    log_dev->infoStream() 
        << "Registering jobid=[" << theJob.getGridJobID() << "]"
        << log4cpp::CategoryStream::ENDLINE;
    
    edg_wlc_JobIdParse( theJob.getGridJobID().c_str(), &id );

    res = edg_wll_RegisterJob( *el_context, id, EDG_WLL_JOB_SIMPLE, theJob.getJDL().c_str(), theJob.getEndpoint().c_str(), 0, 0, 0 );
    edg_wlc_JobIdFree( id );
    if( res != 0 ) {
        log_dev->errorStream() 
            << "Cannot register jobid=[" << theJob.getGridJobID()
            << "]. LB error code=" << res
            << log4cpp::CategoryStream::ENDLINE;
    }
}


void iceLBContext::setLoggingJob( const util::CreamJob& theJob, edg_wll_Source src ) throw ( iceLBException& )
{
    edg_wlc_JobId   id;
    int res = 0;

    res = edg_wlc_JobIdParse( theJob.getGridJobID().c_str(), &id );    

    char *lbserver;
    unsigned int lbport;
    edg_wlc_JobIdGetServerParts( id, &lbserver, &lbport );
    log_dev->infoStream() 
        << "iceLBContext::setLoggingJob: "
        << "Setting log job to jobid=[" << theJob.getGridJobID() << "] "
        << "LB server=[" << lbserver << ":" << lbport << "] "
        << "(port is not used, actually...)"
        << log4cpp::CategoryStream::ENDLINE;
    res |= edg_wll_SetParam( *el_context, EDG_WLL_PARAM_SOURCE, src );        
    res |= edg_wll_SetParam( *el_context, EDG_WLL_PARAM_DESTINATION, lbserver );
    if ( lbserver ) free( lbserver );

    if ( !theJob.getSequenceCode().empty() ) {
        res |= edg_wll_SetLoggingJob( *el_context, id, theJob.getSequenceCode().c_str(), EDG_WLL_SEQ_NORMAL );
    }

    edg_wlc_JobIdFree( id );

    if( res != 0 ) {
        log_dev->errorStream()
            << "iceLBContext::setLoggingJob: "
            << "Unable to set logging job to jobid=["
            << theJob.getGridJobID()
            << "]. LB error is "
            << getLoggingError( 0 )
            << log4cpp::CategoryStream::ENDLINE;
        throw iceLBException( this->getLoggingError("Cannot set logging job:") );
    }

    // Set user proxy for L&B stuff
    fs::path    pf( theJob.getUserProxyCertificate(), fs::native);
    pf.normalize();

    if( fs::exists(pf) ) {

        res = edg_wll_SetParam( *el_context, EDG_WLL_PARAM_X509_PROXY, 
                                theJob.getUserProxyCertificate().c_str() );

        if( res ) {
            log_dev->errorStream()
                << "iceLBContext::setLoggingJob: "
                << "Unable to set logging job to jobid=["
                << theJob.getGridJobID()
                << "]. "
                << getLoggingError( 0 )
                << log4cpp::CategoryStream::ENDLINE;            
            throw iceLBException( this->getLoggingError("Cannot set proxyfile path inside context:") );
        }
    } else {
        log_dev->errorStream()
            << "iceLBContext::setLoggingJob: "
            << "Unable to set logging job to jobid=["
            << theJob.getGridJobID()
            << "]. Proxy file "
            << theJob.getUserProxyCertificate()
            << " does not exist. "
            << "Trying to use the host proxy cert, and hoping for the best..."
            << log4cpp::CategoryStream::ENDLINE;
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////
void iceLBContext::update_and_store_job( CreamJob& theJob )
{
    boost::recursive_mutex::scoped_lock( _cache->mutex );
    string new_seq_code( edg_wll_GetSequenceCode( *el_context ) );
    theJob.setSequenceCode( new_seq_code );    
    _cache->put( theJob );
}
