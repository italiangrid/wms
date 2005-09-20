#include <cstdio>
#include <ctime>

#include <memory>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <user_log.c++.h>

#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wmsutils/jobid/JobId.h"
#include "../../common/EventLogger.h"
#include "../../common/IdContainer.h"
#include "../../logmonitor/exceptions.h"
#include "../../logmonitor/SizeFile.h"
#include "../../logmonitor/AbortedContainer.h"

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

#ifdef ENABLE_LBPROXY
    this->ei_data->md_logger->set_LBProxy_context( edgid, seqcode, position->proxy_file() );
#else
    this->ei_data->md_logger->reset_user_proxy( position->proxy_file() ).reset_context( edgid, seqcode );
#endif 
   this->ei_data->md_logger->condor_submit_event( this->ei_condor, reader->get_globus_rsl() );

   this->ei_data->md_container->update_pointer( position, this->ei_data->md_logger->sequence_code(), this->es_event->eventNumber );
  }

  return;
}

void EventSubmit::process_event( void )
{
  char                 *char_notes( this->ei_data->md_isDagLog ? this->es_event->submitEventUserNotes : 
				    this->es_event->submitEventLogNotes );
  string                edgid, seqcode, rel, buffer, error, islast;
  string                notes( char_notes ? char_notes : "" );
  logger::StatePusher   pusher( elog::cedglog, "EventSubmit::process_event()" );
  boost::match_results<string::const_iterator>   match_pieces;

  static boost::regex  jobexpr( "^\\s*\\((.*)\\) \\((.*)\\) \\(([01])\\)$" );
  static boost::regex  dagexpr( "^DAG job: \\((.*)\\) \\((.*)\\)$" );

  elog::cedglog << logger::setlevel( logger::info ) << "Got job submit event." << endl
		<< "Submitted job " << this->ei_condor << " coming from host: " << this->es_event->submitHost << endl;

  if( boost::regex_match(notes, match_pieces, dagexpr) ) {
    if( this->ei_data->md_sizefile->size_field().position() != 0 ) {
	/*
	  We have matched a DAG job which isn't the first in the log file...
	  This is going to cause many problems: throw an exception and abort the daemon
	*/

      error.assign( "DAG job: " );
      error.append( match_pieces[1].first, match_pieces[1].second );
      error.append( " is not first job in log file, something is going really wrong." );

      throw InvalidLogFile( error );
    }
    else {
      this->ei_data->md_dagId.assign( match_pieces[1].first, match_pieces[1].second );
      this->ei_data->md_isDagLog = true;

      buffer.assign( "Got from events.\nDagId = " );
      buffer.append( this->ei_data->md_dagId );
      this->ei_data->md_sizefile->update_header( buffer );

      seqcode.assign( match_pieces[2].first, match_pieces[2].second );

      elog::cedglog << logger::setlevel( logger::info ) << "First job is a DAG job, entered DAG mode." << endl;

			this->ei_data->md_sizefile->increment_pending().set_last( true );
      //this->ei_data->md_sizefile->increment_pending();
      this->finalProcess( this->ei_data->md_dagId, seqcode );
    }
  }
  else if( boost::regex_match(notes, match_pieces, jobexpr) ) { // The event notes are in the EDG format.
    edgid.assign( match_pieces[1].first, match_pieces[1].second );
    seqcode.assign( match_pieces[2].first, match_pieces[2].second );
    islast.assign( match_pieces[3].first, match_pieces[3].second );

	this->ei_data->md_sizefile->set_last( boost::lexical_cast<bool>(islast) || this->ei_data->md_isDagLog ).increment_pending();


    if( this->ei_data->md_isDagLog )
      elog::cedglog << logger::setlevel( logger::info )
		    << ei_s_subnodeof << this->ei_data->md_dagId << endl;

    this->finalProcess( edgid, seqcode );
  }
  else
    elog::cedglog << logger::setlevel( logger::warning ) << "Cluster " << this->ei_condor << " does not seem a GRID job." << endl
		  << logger::setlevel( logger::info ) << "Event notes = \"" << notes << "\"." << endl;

  return;
}

}} // namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;
