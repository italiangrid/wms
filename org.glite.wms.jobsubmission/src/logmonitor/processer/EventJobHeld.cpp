#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <ctime>

#include <string>

#include <user_log.c++.h>
#if (CONDORG_VERSION >= 653)
#include <globus_gram_protocol_constants.h>
#endif

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "../../common/EventLogger.h"
#include "../../common/IdContainer.h"
#include "../../controller/JobController.h"
#include "../../logmonitor/exceptions.h"
#include "../../logmonitor/AbortedContainer.h"

#include "EventJobHeld.h"
#include "MonitorData.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

EventJobHeld::EventJobHeld( ULogEvent *event, MonitorData *data ) : EventInterface( event, data ),
								    ejh_event( dynamic_cast<JobHeldEvent *>(event) )
{}

EventJobHeld::~EventJobHeld( void )
{}

void EventJobHeld::process_event( void )
{
  string                                 reason( this->ejh_event->getReason() );
  jccommon::IdContainer::iterator        position;
  controller::JobController              controller( *this->ei_data->md_logger );
  logger::StatePusher                    pusher( elog::cedglog, "EventJobHeld::process_event()" );

  elog::cedglog << logger::setlevel( logger::info ) << "Got a job held event." << endl
		<< "For cluster: " << this->ei_condor << endl;

  position = this->ei_data->md_container->position_by_condor_id( this->ei_condor );

  if( position == this->ei_data->md_container->end() )
    elog::cedglog << logger::setlevel( logger::warning ) << ei_s_notsub << endl;
  else if( this->ei_data->md_isDagLog && (this->ei_data->md_dagId == position->edg_id()) ) {
    /*
      What to do if holding a DAG job ???
      I don't know: we will cross our arms and happily tweetie
    */

    elog::cedglog << logger::setlevel( logger::error ) << ei_s_dagideq << position->edg_id() << endl
		  << "I really don't know what to do in such a case..." << endl
		  << "Well, I will do nothing, someone will come and help me..." << endl
		  << logger::setlevel( logger::debug ) << "Hissing to the sky, hissing to the moon..." << endl;
  }
  else {
    elog::cedglog << logger::setlevel( logger::info )
		  << "Reason = \"" << reason << "\"." << endl
#if (CONDORG_VERSION >= 653)
		  << "Code = " << this->ejh_event->getReasonCode() << ", SubCode = " << this->ejh_event->getReasonSubCode() << endl
#endif
		  << ei_s_edgideq << position->edg_id() << endl;

    if( this->ei_data->md_isDagLog )
      elog::cedglog << ei_s_subnodeof << this->ei_data->md_dagId << endl;

    if( this->ei_data->md_aborted->search(this->ei_condor) ) // Job got a previous error, ignoring event...
      elog::cedglog << logger::setlevel( logger::info ) << "The job got a previous error, ignoring the held event..." << endl;
#if (CONDORG_VERSION >= 653)
    else if( position->last_status() == GLOBUS_GRAM_PROTOCOL_ERROR_JOB_CANCEL_FAILED ) {
      elog::cedglog << logger::setlevel( logger::warning )
		    << "Got another held event for an already removing job: Hugh !!!" << endl
		    << logger::setlevel( logger::debug )
		    << "I really hate this piece of shit called Condor !!! Sigh !!" << endl
		    << logger::setlevel( logger::info )
		    << "Ignoring this event..." << endl;
    }
    else if( this->ejh_event->getReasonSubCode() == GLOBUS_GRAM_PROTOCOL_ERROR_JOB_CANCEL_FAILED ) {
      elog::cedglog << logger::setlevel( logger::warning )
		    << "This error means that a cancel coming from an external source failed." << endl
		    << logger::setlevel( logger::info )
		    << "Sending a remove request (by condor id) to JC" << endl;

      controller.cancel( this->ejh_event->cluster, this->ei_data->md_logfile_name.c_str(), false );

      this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(),
						   this->ejh_event->eventNumber, this->ejh_event->getReasonSubCode() );
    }
#endif
    else {
      if( this->ei_data->md_aborted->insert(this->ei_condor) ) {
	elog::cedglog << logger::setlevel( logger::fatal ) << ei_s_failedinsertion << endl;

	throw CannotExecute( ei_s_failedinsertion );
      }

#ifdef ENABLE_LBPROXY
      this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
#else
      this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
#endif
      this->ei_data->md_logger->job_held_event( reason );

      elog::cedglog << logger::setlevel( logger::info ) << "Forwarding remove request to JC." << endl;

      controller.cancel( this->ejh_event->cluster, this->ei_data->md_logfile_name.c_str(), false );	


#if (CONDORG_VERSION >= 653)
      this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(),
						   this->ejh_event->eventNumber, this->ejh_event->getReasonSubCode() );
#else
      this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->ejh_event->eventNumber );
#endif
    }
  }

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;
