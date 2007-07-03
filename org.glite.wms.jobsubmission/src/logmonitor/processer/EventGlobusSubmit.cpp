#include <cstdio>
#include <ctime>

#include <memory>
#include <string>

#include <user_log.c++.h>

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "common/IdContainer.h"
#include "common/EventLogger.h"

#include "EventGlobusSubmit.h"
#include "MonitorData.h"
#include "SubmitReader.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

EventGlobusSubmit::EventGlobusSubmit( ULogEvent *event, MonitorData *data ) : EventInterface( event, data ),
									      egs_event( dynamic_cast<GlobusSubmitEvent *>(event) )
{}

EventGlobusSubmit::~EventGlobusSubmit( void )
{}

void EventGlobusSubmit::process_event( void )
{
  string                                 ce( this->egs_event->rmContact );
  auto_ptr<SubmitReader>                 reader;
  jccommon::IdContainer::iterator        position;
  logger::StatePusher                    pusher( elog::cedglog, "EventGlobusSubmit::process_event()" );

  elog::cedglog << logger::setlevel( logger::info ) << "Got job submit to globus event." << endl
		<< "For cluster " << this->ei_condor << endl
		<< "Contacts " << ce << ", " << this->egs_event->jmContact << '.' << endl;

  position = this->ei_data->md_container->position_by_condor_id( this->ei_condor );

  if( position == this->ei_data->md_container->end() )
    elog::cedglog << logger::setlevel( logger::warning ) << ei_s_notsub << endl;
  else {
    elog::cedglog << logger::setlevel( logger::info ) << ei_s_edgideq << position->edg_id() << endl;

    if( this->ei_data->md_isDagLog )
      elog::cedglog << ei_s_subnodeof << this->ei_data->md_dagId << endl;

    reader.reset( this->createReader(position->edg_id()) );

#ifdef GLITE_WMS_HAVE_LBPROXY
    this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
#else
    this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
#endif    
    this->ei_data->md_logger->globus_submit_event( ce, reader->get_globus_rsl(), this->ei_data->md_logfile_name );

    this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->egs_event->eventNumber );
  }

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;
