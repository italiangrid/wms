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
#include "logmonitor/AbortedContainer.h"
#include "logmonitor/JobWrapperOutputParser.h"
#include "logmonitor/SizeFile.h"

#include "EventPostTerminated.h"
#include "MonitorData.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

typedef  JobWrapperOutputParser   JWOP;

EventPostTerminated::EventPostTerminated( ULogEvent *event, MonitorData *data ) : EventInterface( event, data ), 
  ept_event( dynamic_cast<PostScriptTerminatedEvent *>(event) )
{}

EventPostTerminated::~EventPostTerminated( void )
{}

void EventPostTerminated::process_event( void )
{
  string                             error, sc, done_reason;
  jccommon::IdContainer::iterator    position;
  logger::StatePusher                pusher( elog::cedglog, "EventPostTerminated::process_event()" );

  elog::cedglog << logger::setlevel( logger::info ) << "Got post script terminated event." << endl
		<< "For cluster: " << this->ei_condor << endl;

  position = this->ei_data->md_container->position_by_condor_id( this->ei_condor );

  if( position == this->ei_data->md_container->end() ) {
    elog::cedglog << logger::setlevel( logger::info ) << ei_s_notsub << endl;
  }

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END
