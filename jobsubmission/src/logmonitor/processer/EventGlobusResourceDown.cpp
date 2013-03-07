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
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"

#include "EventGlobusResourceDown.h"
#include "MonitorData.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

EventGlobusResourceDown::EventGlobusResourceDown( ULogEvent *event, MonitorData *data ) : EventInterface( event, data ), 
  egrd_event( dynamic_cast<GlobusResourceDownEvent *>(event) )
{}

EventGlobusResourceDown::~EventGlobusResourceDown( void )
{}


void EventGlobusResourceDown::process_event( void )
{
  const configuration::LMConfiguration   *conf = configuration::Configuration::instance()->lm();

  time_t                                  epoch;
  ULogEvent                              *tmpevent;
  char                                    wbuf[30];
  string                                  when, reason;
  GlobusSubmitFailedEvent                *gsf;
  logger::StatePusher                    pusher( elog::cedglog, "EventGlobusResourceDown::process_event()" );

  elog::cedglog << logger::setlevel( logger::info ) << "Got a Globus resource down event." << endl
		<< "For cluster: " << this->ei_condor << endl
		<< "On ce: " << this->egrd_event->rmContact << endl;
  
  elog::cedglog << logger::setlevel( logger::info )
		<< "Attaching globus timeout to cluster " << this->ei_condor << endl;

  epoch = time( NULL ) + conf->globus_down_timeout();
  asctime_r( &this->egrd_event->eventTime, wbuf );
  when.assign( wbuf, 24 );

  elog::cedglog << logger::setlevel( logger::info )
		<< "Globus timeout will happen in " << conf->globus_down_timeout() << " seconds." << endl
		<< logger::setlevel( logger::debug ) << "At: " << when << endl;

  // Define a fake globus_submit_failed event which will be processed if the timeout expired
  // This event replicates the behaviour that we want when the timer expired
  reason.assign( "LM message: the timeout attached to the globus-down event expired." );
  tmpevent = instantiateEvent( ULOG_GLOBUS_SUBMIT_FAILED );
  tmpevent->cluster = this->egrd_event->cluster;
  tmpevent->proc = tmpevent->subproc = 0;

  localtime_r( &epoch, &tmpevent->eventTime );

  gsf = dynamic_cast< GlobusSubmitFailedEvent* >( tmpevent );
  gsf->reason = new char[128]; 
  strncpy( gsf->reason, reason.c_str(), 128 );
  
  this->ei_data->md_timer->start_timer( epoch, tmpevent );
  
  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END
