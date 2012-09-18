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

#include "EventGlobusResourceUp.h"
#include "MonitorData.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

EventGlobusResourceUp::EventGlobusResourceUp( ULogEvent *event, MonitorData *data ) : EventInterface( event, data ), 
  egru_event( dynamic_cast<GlobusResourceUpEvent *>(event) )
{}

EventGlobusResourceUp::~EventGlobusResourceUp( void )
{}

void EventGlobusResourceUp::process_event( void )
{

  logger::StatePusher                    pusher( elog::cedglog, "EventGlobusResourceUp::process_event()" );

  elog::cedglog << logger::setlevel( logger::info ) << "Got a Globus resource up event." << endl
		<< "For cluster: " << this->ei_condor << endl;

  elog::cedglog << logger::setlevel( logger::info ) 
		<< "Remove globus timeout from cluster: " << this->ei_condor << " if any." << endl;

  // Remove the timer looking for a ULOG_GLOBUS_SUBMIT_FAILED event associated with the given condorid
  this->ei_data->md_timer->remove_timeout( this->egru_event->cluster, ULOG_GLOBUS_SUBMIT_FAILED );

  
  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END
