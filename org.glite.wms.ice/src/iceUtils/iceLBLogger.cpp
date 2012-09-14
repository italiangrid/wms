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


#include "iceLBLogger.h"
#include "IceLBContext.h"
#include "IceLBEvent.h"
#include "glite/ce/cream-client-api-c/scoped_timer.h"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"

#include "iceDb/UpdateJob.h"
#include "iceDb/Transaction.h"

#include "CreamJob.h"

#include <list>
#include <utility>

#include <boost/scoped_ptr.hpp>

namespace api_util = glite::ce::cream_client_api::util;
using namespace glite::wms::ice::util;
using namespace std;

iceLBLogger* iceLBLogger::s_instance = 0;

//////////////////////////////////////////////////////////////////////////////
// 
// iceLBLogger
//
//////////////////////////////////////////////////////////////////////////////
iceLBLogger* iceLBLogger::instance( void )
{
    if ( 0 == s_instance ) {
        s_instance = new iceLBLogger();
    }
    return s_instance;
}

iceLBLogger::iceLBLogger( void ) :
    m_log_dev( glite::ce::cream_client_api::util::creamApiLogger::instance()->getLogger() ),
    m_lb_enabled( true )
{
    //
    // To disable logging to LB, just set an environment variable
    //
    // GLITE_WMS_ICE_DISABLE_LB
    //
    // with whatever value you want.
    //
    if ( 0 != getenv( "GLITE_WMS_ICE_DISABLE_LB" ) ) {
        m_lb_enabled = false;
    }
}


iceLBLogger::~iceLBLogger( void )
{

}

CreamJob iceLBLogger::logEvent( IceLBEvent* ev, const bool use_cancel_sequence_code, const bool updatedb )
{
    static const char* method_name = "iceLBLogger::logEvent() - ";
#ifdef ICE_PROFILE_ENABLE
    api_util::scoped_timer T( "iceLBLogger::logEvent()" );
#endif
    // Aborts if trying to log the NULL event
    if ( ! ev ) {
        CREAM_SAFE_LOG(m_log_dev->fatalStream()
                       << method_name
		       << "Cannot log a NULL event"
                       );
        abort();        
    }

    // Destroys the parameter "ev" when exiting this function
    boost::scoped_ptr< IceLBEvent > scoped_ev( ev );

    // Allocates a new (temporary) LB context
    boost::scoped_ptr< IceLBContext > m_ctx( new IceLBContext() );

    std::string new_seq_code;
        
    try {
        m_ctx->setLoggingJob( ev->getJob(), ev->getSrc(), use_cancel_sequence_code );
    } catch( IceLBException& ex ) {
        CREAM_SAFE_LOG(m_log_dev->errorStream()
                       << method_name
                       << ev->describe()
                       << " - [" << ev->getJob().describe() << "]"
                       << ". Caught exception " << ex.what()
                       );
        return ev->getJob();
    }
    
    m_ctx->startLogging();

    int res = 0;
    do {
        CREAM_SAFE_LOG(m_log_dev->infoStream() 
                       << method_name
                       << ev->describe( )
                       << " - [" << ev->getJob().describe() << "]"
                       );
        if ( m_lb_enabled ) {

	  //api_util::scoped_timer T( "logEvent::ev->execute()" );

            res = ev->execute( m_ctx.get() );
            m_ctx->testCode( res );
	}
        
    } while( res != 0 );        

    char* _tmp_seqcode = edg_wll_GetSequenceCode( *(m_ctx->el_context) );

    if ( _tmp_seqcode ) { // update the sequence code only if it is non null
        new_seq_code = _tmp_seqcode;
	free( _tmp_seqcode );
        try { 
	
	  CreamJob theJob( ev->getJob() );
	  
	  if(use_cancel_sequence_code)
	    theJob.set_cancel_sequence_code( new_seq_code );
	  else
	    theJob.set_sequence_code( new_seq_code );

	  if(updatedb) {
	    glite::wms::ice::db::UpdateJob updater( theJob, "iceLBLogger::logEvent" );
	    glite::wms::ice::db::Transaction tnx(false, false);
	    tnx.execute( &updater );
 	  }
          theJob.reset_change_flags( );
	  return theJob;
	  
        } catch( db::DbOperationException& ex ) {
	  
	  CREAM_SAFE_LOG(m_log_dev->errorStream()
			 << method_name
			 << "Error setting new sequence code for job ["
			 << ev->getJob().describe()
			 << "]: "
			 << ex.what()
			 );
	}
    } else {
      return ev->getJob(); // Make the compiler happy
    }
}
