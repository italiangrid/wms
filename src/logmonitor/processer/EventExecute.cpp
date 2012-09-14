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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <ctime>
#include <config.h>
#include <stdint.h>
#include <condor/user_log.c++.h>

#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "common/EventLogger.h"
#include "common/id_container.h"

#include "EventExecute.h"
#include "MonitorData.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

EventExecute::EventExecute( ULogEvent *event, MonitorData *data ) : EventInterface( event, data ),
								    ee_event( dynamic_cast<ExecuteEvent *>(event) )
{}

EventExecute::~EventExecute( void )
{}

void EventExecute::process_event( void )
{
  jccommon::IdContainer::iterator        position;
  logger::StatePusher                    pusher( elog::cedglog, "EventExecute::process_event()" );

  elog::cedglog << logger::setlevel( logger::info ) << "Got job executing event." << endl
		<< "For cluster " << this->ei_condor << endl;

  position = this->ei_data->md_container->position_by_condor_id( this->ei_condor );

  if( position == this->ei_data->md_container->end() )
    elog::cedglog << logger::setlevel( logger::warning ) << ei_s_notsub << endl;
  else {
    elog::cedglog << logger::setlevel( logger::info ) << ei_s_edgideq << position->edg_id() << endl;

      if (this->ei_data->md_logger->have_lbproxy()) {
        this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
      } else {
        this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
      }
    this->ei_data->md_logger->execute_event(this->ee_event->getExecuteHost());
    
    this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->ee_event->eventNumber );
  }

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END
