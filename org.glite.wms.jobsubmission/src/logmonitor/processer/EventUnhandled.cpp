#include <cstdio>
#include <ctime>

#include <user_log.c++.h>

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

} JOBCONTROL_NAMESPACE_END;
