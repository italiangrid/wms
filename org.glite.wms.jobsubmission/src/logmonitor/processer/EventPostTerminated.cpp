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
#include "../../logmonitor/AbortedContainer.h"
#include "../../logmonitor/JobWrapperOutputParser.h"
#include "../../logmonitor/SizeFile.h"

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
  int                                retcode, stat;
  string                             error, sc;
  jccommon::IdContainer::iterator    position, dagposition;
  logger::StatePusher                pusher( elog::cedglog, "EventPostTerminated::process_event()" );

  elog::cedglog << logger::setlevel( logger::info ) << "Got post script terminated event." << endl
		<< "For cluster: " << this->ei_condor << endl;

  position = this->ei_data->md_container->position_by_condor_id( this->ei_condor );

  if( position == this->ei_data->md_container->end() )
    elog::cedglog << logger::setlevel( logger::info ) << ei_s_notsub << endl;
  else {
    if( !this->ei_data->md_isDagLog ) {
      error.assign( "DAG subjob: " );
      error.append( position->edg_id() );
      error.append( " appeared in a log file not associated with a DAG." );

      elog::cedglog << logger::setlevel( logger::fatal )
		    << "DAG subjob not inside a DAG log file !!!" << endl
		    << ei_s_edgideq << position->edg_id() << endl
		    << "Unhandable situation, aborting the daemon." << endl;

      throw InvalidLogFile( error );
    }
    else {
      JobWrapperOutputParser     parser( this->ei_data->md_dagId, position->edg_id() );

      this->ei_data->md_sizefile->decrement_pending();

      elog::cedglog << logger::setlevel( logger::info ) << ei_s_edgideq << position->edg_id() << endl
		    << ei_s_subnodeof << this->ei_data->md_dagId << endl
		    << ( this->ept_event->normal ? "Normal" : "Abnormal" ) << " job termination. Post return code = "
		    << this->ept_event->returnValue << endl
		    << "This value cannot be trusted, checking the output of the wrapper." << endl;

      if( !this->ept_event->normal )
	elog::cedglog << logger::setlevel( logger::warning )
		      << "Signal number for abnormal termination: " << this->ept_event->signalNumber << endl;

#ifdef ENABLE_LBPROXY
      this->ei_data->md_logger->set_LBProxy_context( position->edg_id(), position->sequence_code(), position->proxy_file() );
#else
      this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( position->edg_id(), position->sequence_code() );
#endif

      if( this->ei_data->md_aborted->search(this->ei_condor) ) { // Aborting node ??? Huh ???
	elog::cedglog << logger::setlevel( logger::info )
		      << "This subnode has got a previous error, aborting it." << endl;

	this->ei_data->md_aborted->remove( this->ei_condor );

	this->ei_data->md_logger->failed_on_error_event( ei_s_joberror );
      }
      else if( (stat = parser.parse_file(retcode, error, sc)) == JWOP::good ) { // Node terminated successfully
	elog::cedglog << "Return code of the node: " << retcode << endl;

	if ( sc != "NoToken" ) 
          this->ei_data->md_logger->job_really_run_event( sc ); // logged really running event
	
	this->ei_data->md_logger->terminated_event( retcode ); // This call will also check if the retcode is == 0
      }
      else { // Node got an error in the JobWrapper
	elog::cedglog << logger::setlevel( logger::info )
		      << "Last node terminated (" << this->ei_condor << ") aborted." << endl
		      << "Reason: " << error << endl;

	if ( sc != "NoToken" ) 
          this->ei_data->md_logger->job_really_run_event( sc ); // logged really running event

	this->ei_data->md_logger->failed_on_error_event( error );

	if( stat == JWOP::resubmit )
	  elog::cedglog << logger::setlevel( logger::warning )
			<< "I should resubmit such job... But this is a DAG node..." << endl
			<< logger::setlevel( logger::warning ) << "Ignoring..." << endl;

	this->ei_data->md_logger->abort_on_error_event( ei_s_jobwrapfail );

	if( this->ept_event->returnValue == 0 ) {
	  /*
	    The POST script is not aware of all bad return code for
	    the node...
	    So, in that case, I set the DAG as failing, and then,
	    when ending, I log an abort...

	    Ask Francesco Giacomini for more informations about this...
	  */

	  elog::cedglog << logger::setlevel( logger::warning )
			<< "Setting the DAG as aborting." << endl
			<< "As of the form of the POST script, DAGMan isn't able to detect this failure." << endl
			<< logger::setlevel( logger::debug )
			<< "Ask Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it> for details." << endl;

	  dagposition = this->ei_data->md_container->position_by_edg_id( this->ei_data->md_dagId );
	  if( this->ei_data->md_aborted->insert(dagposition->condor_id()) ) { // Insert the condor id of the DAG...
	    elog::cedglog << logger::setlevel( logger::fatal ) << ei_s_failedinsertion << endl;

	    throw CannotExecute( ei_s_failedinsertion );
	  }
	}
	else
	  elog::cedglog << logger::setlevel( logger::info )
			<< "We are agreeing with the post script: how lucky !" << endl;
      }

      this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->ept_event->eventNumber );
      jccommon::JobFilePurger( this->ei_data->md_dagId, position->edg_id() ).do_purge();

      if( this->ei_data->md_container->remove(position) ) {
	elog::cedglog << logger::setlevel( logger::null ) << ei_s_errremcorr << endl
		      << "For job: " << position->edg_id() << endl
		      << ei_s_subnodeof << this->ei_data->md_dagId << endl
		      << "Running in cluster: " << this->ei_condor << endl;

	throw CannotExecute( ei_s_errremcorr );
      }
    }
  }

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;
