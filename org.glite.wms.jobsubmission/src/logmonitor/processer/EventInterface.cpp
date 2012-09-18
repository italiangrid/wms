// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

#include <string>

#include <boost/lexical_cast.hpp>

#include <condor/user_log.c++.h>

#include "glite/jobid/JobId.h"

#include "EventInterface.h"
#include "MonitorData.h"
#include "SubmitReader.h"

using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

const string      EventInterface::ei_s_edgideq( "Job id = " ), EventInterface::ei_s_subnodeof( "Sub job of DAG: " );
const string      EventInterface::ei_s_notsub( "Job seems to be not submitted by the GRID." );
const string      EventInterface::ei_s_joberror( "Job got an error while in the CondorG queue." );
const string      EventInterface::ei_s_jobwrapfail( "Failure while executing job wrapper" );
const string      EventInterface::ei_s_errremcorr( "Error removing CondorId <-> JobId correspondance." );
const string      EventInterface::ei_s_failedinsertion( "Failed insertion of aborting job in aborted list." );

SubmitReader *EventInterface::createReader( const string &edgid )
{
  SubmitReader   *reader;
  reader = new SubmitReader( glite::jobid::JobId(edgid) );
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

} JOBCONTROL_NAMESPACE_END
