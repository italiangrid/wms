#include <string>
#include <iostream>
#include <fstream>

#include <boost/regex.hpp>

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "../../jobcontrol_namespace.h"

#include "SubmitReader.h"

USING_COMMON_NAMESPACE;
using namespace std;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

void SubmitReader::internalRead( const glite::wmsutils::jobid::JobId &edgid )
{
  string                buffer;
  ifstream              rsl;
  logger::StatePusher   pusher( elog::cedglog, "SubmitReader::internalRead()" );
  boost::match_results<string::const_iterator>   match_pieces;

  static boost::regex    expression( "^\\s*[Gg]lobus[Rr][Ss][Ll]\\s*=\\s*(.*)$" );

  elog::cedglog << logger::setlevel( logger::info )
		<< "Reading condor submit file of job " << edgid.toString() << endl;

  rsl.open( this->sr_files.submit_file().native_file_string().c_str() );
  if( rsl.good() ) {
    this->sr_submit.erase();

    while( !rsl.eof() ) {
      getline( rsl, buffer );

      if( buffer.length() != 0 ) {
	this->sr_submit.append( buffer );
	this->sr_submit.append( 1, '\n' );

	if( boost::regex_match(buffer, match_pieces, expression) )
	  this->sr_globusRsl.assign( match_pieces[1].first, match_pieces[1].second );
      }
    }

    rsl.close();
  }
  else this->sr_submit.assign( "Submit file not available anymore" );

  return;
}

SubmitReader::SubmitReader( const glite::wmsutils::jobid::JobId &edgid ) : sr_submit(), sr_globusRsl(), sr_files( edgid )
{
  this->internalRead( edgid );
}

SubmitReader::SubmitReader( const glite::wmsutils::jobid::JobId &dagid, const glite::wmsutils::jobid::JobId &edgid ) : sr_submit(), sr_globusRsl(),
											     sr_files( dagid, edgid )
{
  this->internalRead( edgid );
}

SubmitReader::~SubmitReader( void ) {}

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END
