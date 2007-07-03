#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
// This event can be defined only for Condor versions >= 6.7.14
#if CONDORG_AT_LEAST(6,7,14)

#include <cstdio>
#include <ctime>

#include <user_log.c++.h>

#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "common/EventLogger.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"

#include "EventGridResourceDown.h"
#include "MonitorData.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

EventGridResourceDown::EventGridResourceDown( ULogEvent *event, MonitorData *data ) : EventInterface( event, data ), 
  egrd_event( dynamic_cast<GridResourceDownEvent *>(event) )
{}

EventGridResourceDown::~EventGridResourceDown( void )
{}


void EventGridResourceDown::process_event( void )
{
  logger::StatePusher                    pusher( elog::cedglog, "EventGridResourceDown::process_event()" );

  elog::cedglog << logger::setlevel( logger::info ) << "Got a Grid resource down event. No action taken." << endl
		<< "For cluster: " << this->ei_condor << endl
		<< "On ce: " << this->egrd_event->resourceName << endl;
  
  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif // Condor v6.7.14 and beyond.
