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

#include <cstring>
#include <cerrno>

#include <fstream>
#include <memory>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"

#include "glite/jobid/JobId.h"

#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "jobcontrol_namespace.h"
#include "common/files.h"

#include "JobWrapperOutputParser.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor {

namespace {

typedef  JobWrapperOutputParser   JWOP;

struct JWErrors {
  const char         *jwe_error;
  JWOP::status_type   jwe_status;
};

} // Anonymous namespace

JobWrapperOutputParser::JobWrapperOutputParser( const string &edgid ) : jwop_edgid( edgid ) {}

JobWrapperOutputParser::~JobWrapperOutputParser( void ) {}

bool JobWrapperOutputParser::parseStream( istream &is, string &errors, int &retcode, status_type &stat, string &sc, string &done_reason)
{
  bool                found = false;
  char                buffer[BUFSIZ];
  struct JWErrors     jwErrors[] = { { "Working directory not writable", resubmit },
				     { "GLOBUS_LOCATION undefined", resubmit },
				     { "/etc/globus-user-env.sh", resubmit },
				     { "not found or unreadable", abort }, // job executable
				     { "Cannot download", resubmit },
				     { "Cannot upload", resubmit },
				     { "Cannot take shallow resubmission token", resubmit },
				     { "prologue failed", resubmit},
				     { "epilogue failed", resubmit},
                                     { "Cannot create directory", resubmit }, // .mpi or 'jobid' directory
                                     { "Job killed", abort }, // proxy expired
                                     { "Job has been terminated", resubmit }, // signalled by the BS
                                     { "No *ftp", resubmit },
                                     { "Cannot find edg-rm", resubmit },
                                     { NULL, unknown }, };
  struct JWErrors    *errIt;
  sc = "NoToken"; // unsure if really needed, so better keep it
  done_reason = "";
  bool first_error_found = false;
  
  if( is.good() ) {
    bool waiting_for_end_tag = false;
    do {
      is.getline( buffer, BUFSIZ );

      if( !is.eof() ) {
	if( is.good() ) {
          if (!first_error_found) {
	    for( errIt = jwErrors; errIt->jwe_error != NULL; ++errIt ) {
	      if( strstr(buffer, errIt->jwe_error) != NULL ) {
	        errors.assign( buffer );
	        stat = errIt->jwe_status;
	        found = true;
                first_error_found = true; // keep parsing done_reason
              }
	    }
          }

          if( strstr(buffer, "job exit status = ") == buffer ) {
            if( sscanf(buffer, "job exit status = %d", &retcode) == 1 ) {
              errors.assign( buffer );
              found = true;
              if (!first_error_found) {
                stat = good;
              }
            }
            else retcode = -1;
          }

          if( strstr(buffer, "jw exit status = ") == buffer ) {
            found = true;
          }

          if (strstr(buffer, "Sequence code: ") == buffer) {
            char s[256];
            if (sscanf(buffer, "Sequence code: %255s", s) == 1) {
              s[255] = '\0';
              sc.assign(s);
            } else {
              sc.assign("");
            }
          }

          static const std::string LM_log_done_begin("LM_log_done_begin");
          static const std::string LM_log_done_end("LM_log_done_end");
          std::string const jw_stdout_err(buffer);
          if (waiting_for_end_tag) {
            size_t const reason_end_tag = jw_stdout_err.find(LM_log_done_end, 0);
            if (reason_end_tag != std::string::npos) {
              waiting_for_end_tag = false;
            } else {
              done_reason += jw_stdout_err + '\n';
            }
          } else {
            size_t const reason_begin_tag = jw_stdout_err.find(LM_log_done_begin, 0);
            if (reason_begin_tag != std::string::npos) {
              waiting_for_end_tag = true;
            }
          }
        } else {
	  errors.assign( "IO error while reading file: " );
	  errors.append( strerror(errno) );
          found = true;
	  retcode = -1;
	  stat = resubmit;

	  break;
	}
      }
    } while( !is.eof() );
  } else {
    errors.assign( "File not available." );
    retcode = -1;
    stat = resubmit;
  }

  return found;
}

JWOP::status_type JobWrapperOutputParser::parse_file( int &retcode, string &errors, string &sc, string& done_reason)
{
  const configuration::LMConfiguration    *lmconfig = configuration::Configuration::instance()->lm();

  bool                       found = false;
  status_type                stat = good;
  glite::jobid::JobId               id( this->jwop_edgid );
  auto_ptr<jccommon::Files>  files( new jccommon::Files(this->jwop_edgid) );
  ifstream                   ifs;
  logger::StatePusher        pusher( elog::cedglog, "JobWrapperOutputParser::parse_file(...)" );

  errors.erase(); retcode = -1;

  elog::cedglog << logger::setlevel( logger::high ) << "Going to parse standard output file." << endl;

  ifs.open( files->standard_output().native_file_string().c_str() );
  found = this->parseStream( ifs, errors, retcode, stat, sc, done_reason);
  ifs.close();
  if( !found ) {
    errors.assign( "Standard output does not contain useful data." );
    elog::cedglog << logger::setlevel( logger::null ) << errors << endl;

    if( lmconfig->use_maradona_file() ) {
      elog::cedglog << logger::setlevel( logger::high )
		    << "Standard output was not useful, passing ball to Maradona..." << endl;

      ifs.clear();
      ifs.open( files->maradona_file().native_file_string().c_str() );
      found = this->parseStream( ifs, errors, retcode, stat, sc, done_reason );
      ifs.close();
      if( found )
	elog::cedglog << logger::setlevel( logger::null )
		      << "Got info from Maradona..." << endl;
      else {
	errors.append( "Cannot read JobWrapper output, both from Condor and from Maradona. " );

	elog::cedglog << logger::setlevel( logger::null ) << errors << endl;

	retcode = -1;
	stat = resubmit;
      }
    }
    else {
      elog::cedglog << logger::setlevel( logger::null )
		    << "Maradona disabled, cannot check for alternate output." << endl;

      retcode = -1;
      stat = resubmit;
    }
  }

  return stat;
}

} // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END
