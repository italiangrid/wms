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
#include "jobcontrol_namespace.h"
#include "common/EventLogger.h"

#include "EventUnhandled.h"
#include "MonitorData.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

EventUnhandled::EventUnhandled( ULogEvent *event, MonitorData *data ) : EventInterface( event, data ), eu_event( event )
{}

EventUnhandled::~EventUnhandled( void )
{}

void EventUnhandled::process_event( void )
{
  logger::StatePusher   pusher( elog::cedglog, "EventUnhandled::process_event()" );

  elog::cedglog << logger::setlevel( logger::warning )
		<< "Got unhandled event: " << this->eu_event->eventNumber << "." << endl
		<< "Meaning: \"" << ULogEventNumberNames[this->eu_event->eventNumber] << "\"." << endl;

  this->ei_data->md_logger->unhandled_event( ULogEventNumberNames[this->eu_event->eventNumber] );

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END
