/* Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
#include <cstdio>
#include <ctime>
#include <string>
#include <stdint.h>

#include <condor/user_log.c++.h>

#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "common/EventLogger.h"
#include "common/id_container.h"
#include "common/JobFilePurger.h"
#include "common/ProxyUnregistrar.h"
#include "logmonitor/exceptions.h"
#include "logmonitor/JobWrapperOutputParser.h"
#include "logmonitor/AbortedContainer.h"
#include "logmonitor/SizeFile.h"

#include "EventTerminated.h"
#include "MonitorData.h"
#include "JobResubmitter.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

typedef  JobWrapperOutputParser   JWOP;

EventTerminated::EventTerminated( ULogEvent *event, MonitorData *data ) : EventInterface( event, data ),
									  et_event( dynamic_cast<JobTerminatedEvent *>(event) )
{}

EventTerminated::~EventTerminated( void )
{}

void EventTerminated::processNormalJob( jccommon::IdContainer::iterator &position )
{
  int                     retcode, stat;
  string                  errors, sc, done_reason;
  JobWrapperOutputParser  parser( position->edg_id() );
  logger::StatePusher     pusher( elog::cedglog, "EventTerminated::processNormalJob(...)" );

  elog::cedglog << logger::setlevel( logger::info )
		<< ei_s_edgideq << position->edg_id() << endl
		<< "Return value = " << this->et_event->returnValue << endl;
  
  this->ei_data->md_timer->remove_all_timeouts( this->et_event->cluster ); // Remove any installed timeout for this job.
  this->ei_data->md_sizefile->decrement_pending();

  // why aborting a terminated job with a previous error? better getting the job home, errors do not
  // necessarily mean that everything screwed up 
  /* if( this->ei_data->md_aborted->search(this->ei_condor) ) { // Previously aborting job...
    elog::cedglog << logger::setlevel( logger::info )
		  << "This job has got a previous error, aborting it." << endl;

    this->ei_data->md_aborted->remove( this->ei_condor );

    if (this->ei_data->md_logger->have_lbproxy()) {
      this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
    } else {
      this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
    }
    this->ei_data->md_logger->failed_on_error_event( ei_s_joberror );

    jccommon::JobFilePurger( position->edg_id(), this->ei_data->md_logger->have_lbproxy(), false ).do_purge();
    this->ei_data->md_resubmitter->resubmit( position->last_status(), position->edg_id(), position->sequence_code() );

    this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->et_event->eventNumber );
  } else 
  */
  if( (stat = parser.parse_file(retcode, errors, sc, done_reason)) == JWOP::good ) { // Job terminated successfully...
    elog::cedglog << logger::setlevel( logger::info ) << "Real return code: " << retcode << endl;

    if (this->ei_data->md_logger->have_lbproxy()) {
      this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
    } else {
      this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
    }

    if ( !sc.empty() && ( sc != "NoToken" ) )
      this->ei_data->md_logger->job_really_run_event( sc ); // logged really running event

    this->ei_data->md_logger->terminated_event(retcode); // This call discriminates between 0 and all other codes.

    this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->et_event->eventNumber );

    jccommon::ProxyUnregistrar( position->edg_id() ).unregister();
    jccommon::JobFilePurger( position->edg_id(), this->ei_data->md_logger->have_lbproxy()).do_purge();
  }
  else { // Job got an error in the jobwrapper
    elog::cedglog << logger::setlevel( logger::warning ) << "Last job terminated (" << this->ei_condor
		  << ") aborted." << endl
		  << "Reason: \"" << errors << "\"." << endl;

    if (this->ei_data->md_logger->have_lbproxy()) {
      this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
    } else {
      this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
    }

    if ( !sc.empty() && ( sc != "NoToken" ) )
      this->ei_data->md_logger->job_really_run_event( sc ); // logged really running event

    this->ei_data->md_logger->failed_on_error_event(errors + '\n' + done_reason);

    jccommon::JobFilePurger   purger( position->edg_id(), this->ei_data->md_logger->have_lbproxy());
    switch( stat ) {
    case JWOP::resubmit:
      purger.do_purge();
      this->ei_data->md_resubmitter->resubmit( jccommon::undefined_status, position->edg_id(), position->sequence_code() );

      break;
    case JWOP::abort:
      this->ei_data->md_logger->abort_on_error_event( ei_s_jobwrapfail );

      jccommon::ProxyUnregistrar( position->edg_id() ).unregister();
      purger.do_purge( true ); // Remove also Sandboxes...

      break;
    default:
      break;
    }

    this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->et_event->eventNumber );
  }

  return;
}

void EventTerminated::process_event( void )
{
  bool                                   remove = true;
  jccommon::IdContainer::iterator        position;
  logger::StatePusher                    pusher( elog::cedglog, "EventTerminated::process_event()" );

  elog::cedglog << logger::setlevel( logger::info ) << "Got job terminated event for cluster " << this->ei_condor << endl;

  position = this->ei_data->md_container->position_by_condor_id( this->ei_condor );

  if (position == this->ei_data->md_container->end()) {
    elog::cedglog << logger::setlevel( logger::warning ) << ei_s_notsub << endl;
  } else {
    this->processNormalJob(position);
    if( remove && this->ei_data->md_container->remove(position) ) {
      elog::cedglog << logger::setlevel( logger::fatal ) << ei_s_errremcorr << endl
		    << "For job: " << position->edg_id() << endl
		    << "Running in cluster: " << this->ei_condor << endl;

      throw CannotExecute( ei_s_errremcorr );
    }
  }

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END
