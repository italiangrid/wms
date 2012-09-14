// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.
#include <cstdio>
#include <ctime>
#include <memory>
#include <string>
#include <stdint.h>

#include <condor/user_log.c++.h>

#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "common/id_container.h"
#include "common/EventLogger.h"
#include "common/JobFilePurger.h"
#include "common/ProxyUnregistrar.h"
#include "logmonitor/exceptions.h"
#include "logmonitor/AbortedContainer.h"
#include "logmonitor/SizeFile.h"

#include "EventAborted.h"
#include "MonitorData.h"
#include "JobResubmitter.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

namespace {
  char const * const periodic_remove_msg = "The job attribute PeriodicRemove expression";
}

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

EventAborted::EventAborted( ULogEvent *event, MonitorData *data, bool removeTimer ) : EventInterface( event, data ),
										      ea_removeTimer( removeTimer ),
										      ea_event( dynamic_cast<JobAbortedEvent *>(event) )
{}

EventAborted::~EventAborted( void )
{}

void EventAborted::process_event( void )
{
  jccommon::IdContainer::iterator     position;
  logger::StatePusher                 pusher( elog::cedglog, "EventAborted::process_event()" );

  elog::cedglog << logger::setlevel( logger::info ) << "Got job aborted event." << endl
		<< "For cluster " << this->ei_condor << endl;

  position = this->ei_data->md_container->position_by_condor_id( this->ei_condor );
  
  if( position == this->ei_data->md_container->end() ) {
    elog::cedglog << logger::setlevel( logger::warning ) << ei_s_notsub << endl;
  } else {
    elog::cedglog << logger::setlevel( logger::info ) << ei_s_edgideq << position->edg_id() << endl;

    if( this->ea_removeTimer )
      this->ei_data->md_timer->remove_all_timeouts( this->ea_event->cluster ); // Remove any installed timeout for the job

    this->ei_data->md_sizefile->decrement_pending();
    if (this->ei_data->md_logger->have_lbproxy()) {
      this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
    } else {
      this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
    }

    if( this->ei_data->md_aborted->search(this->ei_condor) ) { // Job got an error
      elog::cedglog << logger::setlevel( logger::debug )
		    << "Job has a previous error, so abort it." << endl; 
      this->ei_data->md_aborted->remove( this->ei_condor );

      this->ei_data->md_logger->aborted_by_system_event( ei_s_joberror );

	jccommon::JobFilePurger( position->edg_id(), this->ei_data->md_logger->have_lbproxy()).do_purge();
	// Only resubmit common jobs...
	this->ei_data->md_resubmitter->resubmit( position->last_status(), position->edg_id(), position->sequence_code(), this->ei_data->md_container );
    }
    else { // Job had a "normal" life cycle...
      elog::cedglog << logger::setlevel( logger::debug )
                    << "Job has been aborted by the user." << endl;

      if (0 == strncmp(ea_event->getReason(), periodic_remove_msg, strlen(periodic_remove_msg)))
        {
          elog::cedglog << logger::setlevel( logger::info ) << " reason: " << ea_event->getReason() << "; periodic remove triggered a resubmit" << endl;

          this->ei_data->md_aborted->remove( this->ei_condor );
          this->ei_data->md_logger->aborted_by_system_event( ei_s_joberror );

	        jccommon::JobFilePurger( position->edg_id(), this->ei_data->md_logger->have_lbproxy()).do_purge();
          this->ei_data->md_resubmitter->resubmit(
            position->last_status(), position->edg_id(), position->sequence_code(), this->ei_data->md_container
          );

          this->ei_data->md_container->remove(position);
          return;
      }

      this->ei_data->md_logger->aborted_by_user_event();

	jccommon::ProxyUnregistrar( position->edg_id() ).unregister();
	jccommon::JobFilePurger( position->edg_id(), this->ei_data->md_logger->have_lbproxy()).do_purge( true ); // Remove also Sandbox
    }

    if( this->ei_data->md_container->remove(position) ) {
      elog::cedglog << logger::setlevel( logger::fatal ) << ei_s_errremcorr << endl
		    << "For job: " << position->edg_id() << endl;

      elog::cedglog << "Running in cluster: " << this->ei_condor << endl;

      throw CannotExecute( ei_s_errremcorr );
    }
  }

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END
