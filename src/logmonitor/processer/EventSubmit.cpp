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

#include <cstdio>
#include <ctime>

#include <memory>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <condor/user_log.c++.h>

#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "common/EventLogger.h"
#include "common/id_container.h"
#include "logmonitor/exceptions.h"
#include "logmonitor/SizeFile.h"
#include "logmonitor/AbortedContainer.h"

#include "EventSubmit.h"
#include "MonitorData.h"
#include "SubmitReader.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

EventSubmit::EventSubmit( ULogEvent *event, MonitorData *data ) : EventInterface( event, data ),
								  es_event( dynamic_cast<SubmitEvent *>(event) )
{}

EventSubmit::~EventSubmit( void )
{}

void EventSubmit::finalProcess( const string &edgid, const string &seqcode )
{
  string                            rsl, globusrsl;
  auto_ptr<SubmitReader>            reader;
  jccommon::IdContainer::iterator   position;
  logger::StatePusher               pusher( elog::cedglog, "EventSubmit::finalProcess(...)" );

  logger::cedglog << logger::setlevel( logger::info ) << ei_s_edgideq << edgid << endl
		  << "Sequence code = " << seqcode << endl;

  if( this->ei_data->md_container->insert(edgid, this->ei_condor, seqcode, this->es_event->eventNumber) )
    elog::cedglog << logger::setlevel( logger::warning ) 
		  << "Error while inserting JobId<->Condorid correspondance." << endl
		  << "Ignoring cluster " << this->ei_condor << endl;
  else {
    position = this->ei_data->md_container->last_inserted();

    reader.reset( this->createReader(edgid) );

    if (this->ei_data->md_logger->have_lbproxy()) {
      this->ei_data->md_logger->set_LBProxy_context( edgid, seqcode, position->proxy_file() );
    } else {
      this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( edgid, seqcode );
    }
   this->ei_data->md_logger->condor_submit_event( this->ei_condor, reader->get_globus_rsl() );

   this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->es_event->eventNumber );
  }

  return;
}

void EventSubmit::process_event( void )
{
  char                  *char_notes( this->es_event->submitEventLogNotes );
  string                edgid, seqcode, rel, buffer, error, islast;
  string                notes( char_notes ? char_notes : "" );
  logger::StatePusher   pusher( elog::cedglog, "EventSubmit::process_event()" );
  boost::match_results<string::const_iterator>   match_pieces;
  jccommon::IdContainer::iterator   position;

  static boost::regex  jobexpr( "^\\s*\\((.*)\\) \\((.*)\\) \\(([01])\\)$" );
  elog::cedglog << logger::setlevel( logger::info ) << "Got job submit event." << endl
		<< "Submitted job " << this->ei_condor << endl;

  if( boost::regex_match(notes, match_pieces, jobexpr) ) { // The event notes are in the EDG format.
    edgid.assign( match_pieces[1].first, match_pieces[1].second );
    seqcode.assign( match_pieces[2].first, match_pieces[2].second );
    islast.assign( match_pieces[3].first, match_pieces[3].second );

    this->ei_data->md_sizefile->set_last( boost::lexical_cast<bool>(islast) ).increment_pending();
    this->finalProcess( edgid, seqcode );
  }
  else
    elog::cedglog << logger::setlevel( logger::warning ) << "Cluster "
      << this->ei_condor << " does not seem a GRID job." << endl;

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END
