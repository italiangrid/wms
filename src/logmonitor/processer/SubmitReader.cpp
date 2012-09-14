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
#include <iostream>
#include <fstream>

#include <boost/regex.hpp>

#include "glite/jobid/JobId.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "jobcontrol_namespace.h"

#include "SubmitReader.h"

USING_COMMON_NAMESPACE;
using namespace std;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

void SubmitReader::internalRead( const glite::jobid::JobId &edgid )
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

SubmitReader::SubmitReader( const glite::jobid::JobId &edgid ) : sr_submit(), sr_globusRsl(), sr_files( edgid )
{
  this->internalRead( edgid );
}

SubmitReader::SubmitReader( const glite::jobid::JobId &dagid, const glite::jobid::JobId &edgid ) : sr_submit(), sr_globusRsl(),
											     sr_files( edgid )
{
  this->internalRead( edgid );
}

SubmitReader::~SubmitReader( void ) {}

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END
