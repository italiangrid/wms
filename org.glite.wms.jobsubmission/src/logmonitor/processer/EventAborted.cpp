#include <cstdio>
#include <ctime>

#include <memory>
#include <string>

#include <user_log.c++.h>

#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wmsutils/jobid/JobId.h"
#include "../../common/IdContainer.h"
#include "../../common/EventLogger.h"
#include "../../common/JobFilePurger.h"
#include "../../common/ProxyUnregistrar.h"
#include "../../logmonitor/exceptions.h"
#include "../../logmonitor/AbortedContainer.h"
#include "../../logmonitor/SizeFile.h"

#include "EventAborted.h"
#include "MonitorData.h"
#include "JobResubmitter.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

EventAborted::EventAborted( ULogEvent *event, MonitorData *data, bool removeTimer ) : EventInterface( event, data ),
										      ea_removeTimer( removeTimer ),
										      ea_event( dynamic_cast<JobAbortedEvent *>(event) )
{}

EventAborted::~EventAborted( void )
{}

void EventAborted::process_event( void )
{
  jccommon::IdContainer::iterator     position;
  logger::StatePusher                 pusher( elog::cedglog, "EventAborted::process_event()" );

  elog::cedglog << logger::setlevel( logger::info ) << "Got job aborted event." << endl
		<< "For cluster " << this->ei_condor << endl;

  position = this->ei_data->md_container->position_by_condor_id( this->ei_condor );
  
  if( position == this->ei_data->md_container->end() )
    elog::cedglog << logger::setlevel( logger::warning ) << ei_s_notsub << endl;
  else if( this->ei_data->md_isDagLog && (this->ei_data->md_dagId == position->edg_id()) ) { // Aborting of a DAG job
    elog::cedglog << logger::setlevel( logger::info ) << ei_s_dagideq << position->edg_id() << endl;
    
    this->ei_data->md_sizefile->set_last( true );

    this->ei_data->md_sizefile->decrement_pending();

#ifdef GLITE_WMS_HAVE_LBPROXY
    this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
#else
    this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
#endif
    this->ei_data->md_logger->aborted_by_user_event();

    if( this->ei_data->md_aborted->search(this->ei_condor) )
      this->ei_data->md_aborted->remove( this->ei_condor );
  }
  else { // Common jobs and DAG nodes...
    elog::cedglog << logger::setlevel( logger::info ) << ei_s_edgideq << position->edg_id() << endl;

    if( this->ei_data->md_isDagLog )
      elog::cedglog << ei_s_subnodeof << this->ei_data->md_dagId << endl;

    if( this->ea_removeTimer )
      this->ei_data->md_timer->remove_all_timeouts( this->ea_event->cluster ); // Remove any installed timeout for the job

    this->ei_data->md_sizefile->decrement_pending();
#ifdef GLITE_WMS_HAVE_LBPROXY
    this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
#else
    this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
#endif

    if( this->ei_data->md_aborted->search(this->ei_condor) ) { // Job got an error
      elog::cedglog << logger::setlevel( logger::debug )
		    << "Job has a previous error, so abort it." << endl; 
      this->ei_data->md_aborted->remove( this->ei_condor );

      this->ei_data->md_logger->aborted_by_system_event( ei_s_joberror );

      if( this->ei_data->md_isDagLog )
	jccommon::JobFilePurger( this->ei_data->md_dagId, position->edg_id() ).do_purge();
      else {
	jccommon::JobFilePurger( position->edg_id() ).do_purge();
	// Only resubmit common jobs...
	this->ei_data->md_resubmitter->resubmit( position->last_status(), position->edg_id(), position->sequence_code(), this->ei_data->md_container );
      }
    }
    else { // Job had a "normal" life cycle...
      elog::cedglog << logger::setlevel( logger::debug )
                    << "Job has been aborted by the user." << endl;
      this->ei_data->md_logger->aborted_by_user_event();

      if( this->ei_data->md_isDagLog )
	jccommon::JobFilePurger( this->ei_data->md_dagId, position->edg_id() ).do_purge( true ); // Remove also Sandbox
      else {
	jccommon::ProxyUnregistrar( position->edg_id() ).unregister();
	jccommon::JobFilePurger( position->edg_id() ).do_purge( true ); // Remove also Sandbox
      }
    }

    if( this->ei_data->md_isDagLog && this->ei_data->md_sizefile->completed() ) {
      elog::cedglog << logger::setlevel( logger::info )
		    << "Seems we are sending the last event during a removal of this DAG." << endl
		    << logger::setlevel( logger::debug )
		    << ei_s_dagideq << this->ei_data->md_dagId << endl
		    << logger::setlevel( logger::info ) << "Removing DAG proxy and files." << endl;

      jccommon::ProxyUnregistrar( this->ei_data->md_dagId ).unregister();
      jccommon::JobFilePurger( this->ei_data->md_dagId, true ).do_purge( true );
    }

    if( this->ei_data->md_container->remove(position) ) {
      elog::cedglog << logger::setlevel( logger::fatal ) << ei_s_errremcorr << endl
		    << "For job: " << position->edg_id() << endl;

      if( this->ei_data->md_isDagLog )
	elog::cedglog << ei_s_subnodeof << this->ei_data->md_dagId << endl;

      elog::cedglog << "Running in cluster: " << this->ei_condor << endl;

      throw CannotExecute( ei_s_errremcorr );
    }
  }

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;
