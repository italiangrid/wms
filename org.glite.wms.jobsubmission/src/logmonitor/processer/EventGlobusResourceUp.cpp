#include <cstdio>
#include <ctime>

#include <user_log.c++.h>

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

} JOBCONTROL_NAMESPACE_END;
