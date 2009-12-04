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

#include <user_log.c++.h>

#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "common/IdContainer.h"
#include "common/EventLogger.h"
#include "controller/JobController.h"
#include "logmonitor/exceptions.h"
#include "logmonitor/AbortedContainer.h"

#include "EventGlobusSubmitFailed.h"
#include "MonitorData.h"
#include "SubmitReader.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

//namespace {
//  std::string globus_error10 = "10 data transfer to the server failed";
//reason: 37 the provided RSL 'queue' parameter
//reason: 10 data transfer to the server failed
//}

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

EventGlobusSubmitFailed::EventGlobusSubmitFailed( ULogEvent *event, MonitorData *data ) : EventInterface( event, data ), 
  egsf_event( dynamic_cast<GlobusSubmitFailedEvent *>(event) )
{}

EventGlobusSubmitFailed::~EventGlobusSubmitFailed( void )
{}

void EventGlobusSubmitFailed::process_event( void )
{
  auto_ptr<SubmitReader>                 reader;
  jccommon::IdContainer::iterator        position;
  controller::JobController              controller( *this->ei_data->md_logger );
  logger::StatePusher                    pusher( elog::cedglog, "EventGlobusSubmitFailed::process_event()" );

  elog::cedglog << logger::setlevel( logger::info ) << "Got globus submit failed event." << endl
		<< "For cluster: " << this->ei_condor << ", reason: " << this->egsf_event->reason << endl;

  position = this->ei_data->md_container->position_by_condor_id( this->ei_condor );

  if( position == this->ei_data->md_container->end() )
    elog::cedglog << logger::setlevel( logger::warning ) << ei_s_notsub << endl;
  else {
    elog::cedglog << logger::setlevel( logger::info ) << ei_s_edgideq << position->edg_id() << endl;

    if( this->ei_data->md_isDagLog )
      elog::cedglog << ei_s_subnodeof << this->ei_data->md_dagId << endl;

    if( this->ei_data->md_aborted->insert(this->ei_condor) ) {
      elog::cedglog << logger::setlevel( logger::fatal ) << ei_s_failedinsertion << endl;

      throw CannotExecute( ei_s_failedinsertion );
    }

    reader.reset( this->createReader(position->edg_id()) );

    if (this->ei_data->md_logger->have_lbproxy()) {
      this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
    } else {
      this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
    }
    this->ei_data->md_logger->globus_submit_failed_event( reader->to_string(), this->egsf_event->reason, this->ei_data->md_logfile_name );

    //if (std::string(this->egsf_event->reason).substr(0, globus_error10.size()) == globus_error10) {
      //elog::cedglog << logger::setlevel( logger::info ) << "Here it comes! The infamous globus error 10: __NOT__ forwarding remove request to JC." << endl;
    //} else {
      elog::cedglog << logger::setlevel( logger::info ) << "Forwarding remove request to JC." << endl;
      controller.cancel( this->egsf_event->cluster, this->ei_data->md_logfile_name.c_str() );
      this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->egsf_event->eventNumber );
    //}
  }

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END
