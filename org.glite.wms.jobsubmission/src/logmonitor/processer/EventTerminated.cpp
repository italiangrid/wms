#include <cstdio>
#include <ctime>

#include <string>

#include <user_log.c++.h>

#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wmsutils/jobid/JobId.h"
#include "../../common/EventLogger.h"
#include "../../common/IdContainer.h"
#include "../../common/JobFilePurger.h"
#include "../../common/ProxyUnregistrar.h"
#include "../../logmonitor/exceptions.h"
#include "../../logmonitor/JobWrapperOutputParser.h"
#include "../../logmonitor/AbortedContainer.h"
#include "../../logmonitor/SizeFile.h"

#include "EventTerminated.h"
#include "MonitorData.h"
#include "JobResubmitter.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

typedef  JobWrapperOutputParser   JWOP;

EventTerminated::EventTerminated( ULogEvent *event, MonitorData *data ) : EventInterface( event, data ),
									  et_event( dynamic_cast<JobTerminatedEvent *>(event) )
{}

EventTerminated::~EventTerminated( void )
{}

void EventTerminated::processNormalJob( jccommon::IdContainer::iterator &position )
{
  int                     retcode, stat;
  string                  errors, sc;
  JobWrapperOutputParser  parser( position->edg_id() );
  logger::StatePusher     pusher( elog::cedglog, "EventTerminated::processNormalJob(...)" );

  elog::cedglog << logger::setlevel( logger::info )
		<< ei_s_edgideq << position->edg_id() << endl
		<< "Return value = " << this->et_event->returnValue << endl;
  
  this->ei_data->md_timer->remove_all_timeouts( this->et_event->cluster ); // Remove any installed timeout for this job.

  this->ei_data->md_sizefile->decrement_pending();

  if( this->ei_data->md_aborted->search(this->ei_condor) ) { // Previously aborting job...
    elog::cedglog << logger::setlevel( logger::info )
		  << "This job has got a previous error, aborting it." << endl;

    this->ei_data->md_aborted->remove( this->ei_condor );

#ifdef GLITE_WMS_HAVE_LBPROXY
    this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
#else
    this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
#endif  
    this->ei_data->md_logger->failed_on_error_event( ei_s_joberror );

    jccommon::JobFilePurger( position->edg_id() ).do_purge();
    this->ei_data->md_resubmitter->resubmit( position->last_status(), position->edg_id(), position->sequence_code() );

    this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->et_event->eventNumber );
  }
  else if( (stat = parser.parse_file(retcode, errors, sc)) == JWOP::good ) { // Job terminated successfully...
    elog::cedglog << logger::setlevel( logger::info ) << "Real return code: " << retcode << endl;

#ifdef GLITE_WMS_HAVE_LBPROXY
    this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
#else
    this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
#endif   

    if ( !sc.empty() && ( sc != "NoToken" ) )
      this->ei_data->md_logger->job_really_run_event( sc ); // logged really running event

    this->ei_data->md_logger->terminated_event( retcode ); // This call discriminates between 0 and all other codes.

    this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->et_event->eventNumber );

    jccommon::ProxyUnregistrar( position->edg_id() ).unregister();
    jccommon::JobFilePurger( position->edg_id() ).do_purge();
  }
  else { // Job got an error in the jobwrapper
    elog::cedglog << logger::setlevel( logger::warning ) << "Last job terminated (" << this->ei_condor
		  << ") aborted." << endl
		  << "Reason: \"" << errors << "\"." << endl;

#ifdef GLITE_WMS_HAVE_LBPROXY
    this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
#else
    this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
#endif

    if ( !sc.empty() && ( sc != "NoToken" ) )
      this->ei_data->md_logger->job_really_run_event( sc ); // logged really running event

    this->ei_data->md_logger->failed_on_error_event( errors );

    jccommon::JobFilePurger   purger( position->edg_id() );
    switch( stat ) {
    case JWOP::resubmit:
      purger.do_purge();
      this->ei_data->md_resubmitter->resubmit( jccommon::undefined_status, position->edg_id(), position->sequence_code() );

      break;
    case JWOP::abort:
      this->ei_data->md_logger->abort_on_error_event( ei_s_jobwrapfail );

      jccommon::ProxyUnregistrar( position->edg_id() ).unregister();
      purger.do_purge( true ); // Remove also Sandboxes...

      break;
    default:
      break;
    }

    this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->et_event->eventNumber );
  }

  return;
}

void EventTerminated::process_event( void )
{
  bool                                   remove = true;
  jccommon::IdContainer::iterator        position;
  logger::StatePusher                    pusher( elog::cedglog, "EventTerminated::process_event()" );

  elog::cedglog << logger::setlevel( logger::info ) << "Got job terminated event for cluster " << this->ei_condor << endl;

  position = this->ei_data->md_container->position_by_condor_id( this->ei_condor );

  if( position == this->ei_data->md_container->end() )
    elog::cedglog << logger::setlevel( logger::warning ) << ei_s_notsub << endl;
  else {
    if( this->ei_data->md_isDagLog && (this->ei_data->md_dagId == position->edg_id()) ) {
      /*
	We are in a dag log file... And this is the terminated event for
	the main DAG...
      */
      elog::cedglog << logger::setlevel( logger::info ) << ei_s_dagideq << position->edg_id() << endl
		    << "Return code = " << this->et_event->returnValue << endl;

#ifdef GLITE_WMS_HAVE_LBPROXY
      this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
#else
      this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
#endif
      
      if( this->ei_data->md_aborted->search(this->ei_condor) ) { // The POST script bug
	this->ei_data->md_aborted->remove( this->ei_condor );

	elog::cedglog << logger::setlevel( logger::error )
		      << "Because of the POST script bug, this DAG has terminated while it should be failing." << endl
		      << logger::setlevel( logger::warning ) << "Aborting it." << endl;

	this->ei_data->md_logger->abort_on_error_event( ei_s_dagfailed );
      }
      else
	this->ei_data->md_logger->terminated_event( this->et_event->returnValue );

      this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->et_event->eventNumber );

      jccommon::ProxyUnregistrar( position->edg_id() ).unregister();
      jccommon::JobFilePurger( position->edg_id(), true ).do_purge();

      this->ei_data->md_sizefile->decrement_pending();
    }
    else if( !this->ei_data->md_isDagLog ) // Common jobs only...
      this->processNormalJob( position );
    else { // DAG subjobs...
      elog::cedglog << logger::setlevel( logger::info )
		    << ei_s_edgideq << position->edg_id() << endl
		    << ei_s_subnodeof << this->ei_data->md_dagId << endl
		    << "Ignoring this event, waiting for a POST_TERMINATED one..." << endl;

      remove = false;
    }

    if( remove && this->ei_data->md_container->remove(position) ) {
      elog::cedglog << logger::setlevel( logger::fatal ) << ei_s_errremcorr << endl
		    << "For job: " << position->edg_id() << endl
		    << "Running in cluster: " << this->ei_condor << endl;

      throw CannotExecute( ei_s_errremcorr );
    }
  }

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;
