#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
// This event can be defined only for Condor versions >= 6.7.14
#if CONDORG_AT_LEAST(6,7,14)

#include <cstdio>
#include <ctime>

#include <memory>
#include <string>

#include <user_log.c++.h>

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "../../common/IdContainer.h"
#include "../../common/EventLogger.h"

#include "EventGridSubmit.h"
#include "MonitorData.h"
#include "SubmitReader.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

EventGridSubmit::EventGridSubmit( ULogEvent *event, MonitorData *data ) : EventInterface( event, data ),
									      egs_event( dynamic_cast<GridSubmitEvent *>(event) )
{}

EventGridSubmit::~EventGridSubmit( void )
{}

void EventGridSubmit::process_event( void )
{
  string                                 ce( this->egs_event->resourceName );
  auto_ptr<SubmitReader>                 reader;
  jccommon::IdContainer::iterator        position;
  logger::StatePusher                    pusher( elog::cedglog, "EventGridSubmit::process_event()" );

  string log_message;
  bool ignore_globus_event = false;
 
// GridSubmit events will be generated along with GlobusSubmit throughout
// Condor version 6.8 for Globus jobs. 
// Globus events are supposed to disappear somewhere in the 6.9 series.
// At that time, code from GlobusSubmit will have to be moved here,
// if still applicable.

  if (ce.find("gt") == 0 || ce.find("globus") == 0) ignore_globus_event = true;

  if (ignore_globus_event) {
    elog::cedglog << logger::setlevel( logger::info ) << "Got job submit to grid event. "
                  << this->ei_condor << ". Ignoring for Globus jobs." << endl;
  } else {
    elog::cedglog << logger::setlevel( logger::info ) << "Got job submit to grid event." << endl;

    elog::cedglog << "For cluster " << this->ei_condor << endl
         	  << "Contacts " << ce << '.' << endl;

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
      this->ei_data->md_logger->grid_submit_event( ce, this->ei_data->md_logfile_name );
    

      this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->egs_event->eventNumber );
    }
  }

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif // Condor v6.7.14 and beyond.
