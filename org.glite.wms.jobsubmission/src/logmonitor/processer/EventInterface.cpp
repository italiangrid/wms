#include <string>

#include <boost/lexical_cast.hpp>

#include <user_log.c++.h>

#include "glite/wmsutils/jobid/JobId.h"

#include "EventInterface.h"
#include "MonitorData.h"
#include "SubmitReader.h"

USING_COMMON_NAMESPACE;
using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

const string      EventInterface::ei_s_edgideq( "Job id = " ), EventInterface::ei_s_subnodeof( "Sub job of DAG: " );
const string      EventInterface::ei_s_notsub( "Job seems to be not submitted by the GRID." );
const string      EventInterface::ei_s_dagfailed( "DAG failed as one of the node failed.\n"
						  "DAGMan was not able to intercept this status as of the bug in the POST script.\n"
						  "Ask Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it> for more details." );
const string      EventInterface::ei_s_dagideq( "DAG id = " );
const string      EventInterface::ei_s_joberror( "Job got an error while in the CondorG queue." );
const string      EventInterface::ei_s_jobwrapfail( "Failure while executing job wrapper" );
const string      EventInterface::ei_s_errremcorr( "Error removing CondorId <-> JobId correspondance." );
const string      EventInterface::ei_s_failedinsertion( "Failed insertion of aborting job in aborted list." );

SubmitReader *EventInterface::createReader( const string &edgid )
{
  SubmitReader   *reader;

  if( !this->ei_data->md_isDagLog || (edgid == this->ei_data->md_dagId) )
    reader = new SubmitReader( glite::wmsutils::jobid::JobId(edgid) );
  else
    reader = new SubmitReader( glite::wmsutils::jobid::JobId(this->ei_data->md_dagId), glite::wmsutils::jobid::JobId(edgid) );

  return reader;
}

EventInterface::EventInterface( ULogEvent *event, MonitorData *data ) : ei_data( data ),
									ei_condor( boost::lexical_cast<string>(event->cluster) )
{
  if( event->proc != 0 ) {
    this->ei_condor.append( 1, '.' );
    this->ei_condor.append( boost::lexical_cast<string>(event->proc) );
  }
}

EventInterface::~EventInterface( void )
{}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;
