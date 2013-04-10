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


#include "IceLBEventFactory.h"
#include "IceLBEvent.h"

#include <string>

using namespace glite::wms::ice::util;
namespace jobstat = glite::ce::cream_client_api::job_statuses;

//------------------------------------------------------------------------------
IceLBEvent* IceLBEventFactory::mkEvent( const CreamJob& theJob,
					const bool force_donefailed )
{
    switch( theJob.status() ) {
    case jobstat::PENDING:
        // nothing to log
        return 0;
    case jobstat::IDLE:
        // if ( theJob.get_prev_status() == jobstat::RUNNING )
        // return new job_suspended_event( theJob );
        // else
        return 0; // nothing to log
    case jobstat::RUNNING:
        // if ( theJob.get_prev_status() == jobstat::IDLE ||
        if ( theJob.prev_status() == jobstat::HELD )
            return new job_resumed_event( theJob );
        else
            return new job_running_event( theJob ); 
    case jobstat::REALLY_RUNNING:
        return new job_really_running_event( theJob );
    case jobstat::CANCELLED:
        if ( theJob.killed_byice() )
            return new job_aborted_event( theJob );
        else
            return new job_cancelled_event( theJob );
    case jobstat::HELD:
        return new job_suspended_event( theJob ); // FIXME??
    case jobstat::ABORTED:
    	if(force_donefailed)
	  return new job_done_failed_event( theJob );
	  
        return new job_aborted_event( theJob );
    case jobstat::DONE_OK:
        return new job_done_ok_event( theJob );
    case jobstat::DONE_FAILED:
        return new job_done_failed_event( theJob );
    default:
        return 0;
    }
}
