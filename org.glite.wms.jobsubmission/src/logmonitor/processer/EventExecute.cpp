#include <cstdio>
#include <ctime>

#include <user_log.c++.h>

#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "../../common/EventLogger.h"
#include "../../common/IdContainer.h"

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
		<< "For cluster " << this->ei_condor << " at host " << this->ee_event->executeHost << endl;

  position = this->ei_data->md_container->position_by_condor_id( this->ei_condor );

  if( position == this->ei_data->md_container->end() )
    elog::cedglog << logger::setlevel( logger::warning ) << ei_s_notsub << endl;
  else {
    elog::cedglog << logger::setlevel( logger::info ) << ei_s_edgideq << position->edg_id() << endl;

    if( this->ei_data->md_isDagLog )
      elog::cedglog << ei_s_subnodeof << this->ei_data->md_dagId << endl;
#ifdef GLITE_WMS_HAVE_LBPROXY
    this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
#else
    this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
#endif 
    this->ei_data->md_logger->execute_event( this->ee_event->executeHost );
    
    this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->ee_event->eventNumber );
  }

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;
