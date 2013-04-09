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

#include "IceCommandLBLogging.h"
#include "iceUtils/IceUtils.h"
#include "iceDb/GetJobByGid.h"
#include "iceDb/Transaction.h"
#include "iceDb/RemoveJobByGid.h"
#include "iceUtils/iceLBLogger.h"
#include "iceUtils/IceLBEvent.h"
#include "iceUtils/iceLBEventFactory.h"
#include "glite/security/proxyrenewal/renewal.h"
#include "common/src/configuration/ICEConfiguration.h"
#include "common/src/configuration/CommonConfiguration.h"
/**
 *
 * Cream Client API C++ Headers
 *
 */
#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/job_statuses.h"

#include "iceUtils/DNProxyManager.h"

#include <boost/lexical_cast.hpp>
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
using namespace glite::wms::ice;

//______________________________________________________________________________
util::IceCommandLBLogging::IceCommandLBLogging( const list<CreamJob>& jobs ) :
    IceAbstractCommand( "IceCommandLBLogging", "" ),
    m_log_dev( cream_api::util::creamApiLogger::instance()->getLogger() ),
    m_jobs_to_remove( jobs ),
    m_lb_logger( ice::util::iceLBLogger::instance() )
{
}

//______________________________________________________________________________
util::IceCommandLBLogging::~IceCommandLBLogging( )
{
}

//______________________________________________________________________________
string util::IceCommandLBLogging::get_grid_job_id( ) const
{
  string randid( "" );
  struct timeval T;
  gettimeofday( &T, 0 );
  randid += boost::lexical_cast<string>( T.tv_sec ) + "." + boost::lexical_cast<string>( T.tv_usec );
  return randid;
}

//______________________________________________________________________________
void util::IceCommandLBLogging::execute( const std::string& tid ) throw()
{  
  list<CreamJob>::iterator jobit;
  
  for( jobit = m_jobs_to_remove.begin(); jobit != m_jobs_to_remove.end(); ++jobit ) {
    
    /**
     * The following if is for the feedback
     */
    //if( util::IceUtils::is_rescheduled_job( *jobit ) )
    CreamJob _tmp;
    string ignore_reason;
    if( glite::wms::ice::util::IceUtils::ignore_job( jobit->complete_cream_jobid(), _tmp, ignore_reason ) ) {
      CREAM_SAFE_LOG(m_log_dev->debugStream() << "IceCommandLBLogging::execute - TID=[" << getThreadID() << "] "
                      << "Will not LOG anything to LB for Job ["
		      << jobit->grid_jobid() << "] for reason: "
		      << ignore_reason
		      );
      continue;
    }
      
    IceLBEvent* ev = iceLBEventFactory::mkEvent( *jobit );
    if ( ev ) {
      m_lb_logger->logEvent( ev, false, false );
    }
    
    if( cream_api::job_statuses::DONE_OK == jobit->status() ||
        cream_api::job_statuses::DONE_FAILED == jobit->status() ||
	cream_api::job_statuses::CANCELLED == jobit->status() ||
	cream_api::job_statuses::ABORTED == jobit->status() )
      {	
	CREAM_SAFE_LOG(m_log_dev->debugStream() << "IceCommandLBLogging::execute - TID=[" << getThreadID() << "] "
		       << "Removing job [" << jobit->grid_jobid( )
		       << "] from ICE's database"
		       );
	
	{
	  db::RemoveJobByGid remover( jobit->grid_jobid(), "IceCommandLBLogging::execute" );
	  db::Transaction tnx( false, false );
	  tnx.execute( &remover );
	  if( jobit->proxy_renewable() )
	    glite::wms::ice::util::DNProxyManager::getInstance()->decrementUserProxyCounter( jobit->user_dn(), jobit->myproxy_address() );
	}
	
      }
  }
}
