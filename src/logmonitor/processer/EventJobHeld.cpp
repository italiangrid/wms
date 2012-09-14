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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <ctime>

#include <string>
#include <stdint.h>

#include <condor/user_log.c++.h>
#include <globus_gram_protocol_constants.h>

#include "glite/jobid/JobId.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"

#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"

#include "glite/wms/common/utilities/manipulation.h"

#include "common/EventLogger.h"
#include "common/id_container.h"
#include "controller/JobController.h"
#include "logmonitor/exceptions.h"
#include "logmonitor/AbortedContainer.h"

#include "EventJobHeld.h"
#include "MonitorData.h"

//namespace {
//  std::string const globus_error10 = "Globus error 10";
//}

using namespace std;
USING_COMMON_NAMESPACE;

RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

EventJobHeld::EventJobHeld( ULogEvent *event, MonitorData *data ) : EventInterface( event, data ),
								    ejh_event( dynamic_cast<JobHeldEvent *>(event) )
{}

EventJobHeld::~EventJobHeld( void )
{}

void EventJobHeld::process_event( void )
{
  string                                 reason(this->ejh_event->getReason());
  jccommon::IdContainer::iterator        position;
  controller::JobController              controller( *this->ei_data->md_logger );
  logger::StatePusher                    pusher( elog::cedglog, "EventJobHeld::process_event()" );

  elog::cedglog << logger::setlevel( logger::info ) << "Got a job held event." << endl
		<< "For cluster: " << this->ei_condor << endl;

  position = this->ei_data->md_container->position_by_condor_id( this->ei_condor );

  if( position == this->ei_data->md_container->end() ) {
    elog::cedglog << logger::setlevel( logger::warning ) << ei_s_notsub << endl;
  } else {
    elog::cedglog << logger::setlevel( logger::info )
		  << "Reason = \"" << reason << "\"." << endl
		  << "Code = " << this->ejh_event->getReasonCode() << ", SubCode = " << this->ejh_event->getReasonSubCode() << endl
		  << ei_s_edgideq << position->edg_id() << endl;

    //if (reason.substr(0, globus_error10.size()) == globus_error10) {

    //  elog::cedglog << logger::setlevel( logger::info ) << "This error is likely to be transient." << endl
    //    << "Let's retry releasing cluster " << this->ei_condor << " before giving up" << endl;

    //  int const MAX_CONDOR_RETRIES = 30;

    //  std::string const jobid(position->edg_id());
    //  configuration::Configuration const& config(*configuration::Configuration::instance());
    //  std::string condor_retries_file(config.ns()->sandbox_staging_path());
    //  condor_retries_file += "/" + glite::wms::common::utilities::get_reduced_part(jobid);
    //  condor_retries_file += "/" + glite::wms::common::utilities::to_filename(jobid);
    //  condor_retries_file += "/Condor.retries";

    //  int condor_retries = 0;
    //  ifstream ifs(condor_retries_file.c_str());
    //  if (ifs) {
    //    ifs >> condor_retries;
    //    ifs.close();
    //  }
    //  ++condor_retries;

    //  ofstream ofs(condor_retries_file.c_str());
    //  if (ofs) {
    //    ofs << condor_retries << '\n';
    //    ofs.close();
    //  }

    //  if (condor_retries <= MAX_CONDOR_RETRIES) {

    //    elog::cedglog << logger::setlevel( logger::info ) << "Forwarding release request to JC." << endl;
    //    sleep(5);
    //    controller.release(this->ejh_event->cluster, this->ei_data->md_logfile_name.c_str());

    //    this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(),
    //    this->ejh_event->eventNumber, this->ejh_event->getReasonSubCode() );
    //   return;
    //  } else {
        //elog::cedglog << logger::setlevel( logger::info ) << "Number of Condor retries exceeded, __NOT__ forwarding release request to JC." << endl;
    // }
    //}

    if (this->ei_data->md_logger->have_lbproxy()) {
      this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
    } else {
      this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
    }
    this->ei_data->md_logger->job_held_event(reason);

    if( this->ei_data->md_aborted->search(this->ei_condor)) {
      elog::cedglog << logger::setlevel( logger::info ) << "The job got a previous error, ignoring the held event..." << endl;
    }

    else if( position->last_status() == GLOBUS_GRAM_PROTOCOL_ERROR_JOB_CANCEL_FAILED ) {
      elog::cedglog << logger::setlevel( logger::warning )
		    << "Got another held event for an already removing job\n"
		    << logger::setlevel( logger::info )
		    << "Ignoring this event...\n";
    }
    else if( this->ejh_event->getReasonSubCode() == GLOBUS_GRAM_PROTOCOL_ERROR_JOB_CANCEL_FAILED ) {
      elog::cedglog << logger::setlevel( logger::warning )
		    << "This error means that a cancel coming from an external source failed." << endl
		    << logger::setlevel( logger::info )
		    << "Sending a remove request (by condor id) to JC" << endl;

      controller.cancel( this->ejh_event->cluster, this->ei_data->md_logfile_name.c_str() );

      this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(),
						   this->ejh_event->eventNumber, this->ejh_event->getReasonSubCode() );
    } else {
      if (this->ei_data->md_aborted->insert(this->ei_condor)) {
        elog::cedglog << logger::setlevel( logger::fatal ) << ei_s_failedinsertion << endl;
	      throw CannotExecute(ei_s_failedinsertion);
      }

      if (this->ei_data->md_logger->have_lbproxy()) {
        this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
      } else {
        this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
      }
      this->ei_data->md_logger->job_held_event( reason );

      elog::cedglog << logger::setlevel( logger::info ) << "Forwarding remove request to JC." << endl;

      controller.cancel( this->ejh_event->cluster, this->ei_data->md_logfile_name.c_str() );	

      this->ei_data->md_container->update_pointer(
         position,
         this->ei_data->md_logger->sequence_code(),
         this->ejh_event->eventNumber,
         this->ejh_event->getReasonSubCode());
    }
  }

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END
