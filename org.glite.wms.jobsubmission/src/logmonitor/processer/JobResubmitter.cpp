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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <globus_gram_protocol_constants.h>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "jobcontrol_namespace.h"
#include "common/ProxyUnregistrar.h"
#include "common/EventLogger.h"
#include "common/id_container.h"
#include "logmonitor/exceptions.h"
#include "logmonitor/JobWrapperOutputParser.h"

#include <boost/filesystem/path.hpp>
#include <condor/user_log.c++.h>

#include "JobResubmitter.h"

USING_COMMON_NAMESPACE;
using namespace std;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

typedef  JobWrapperOutputParser   JWOP;

JobResubmitter::JobResubmitter( jccommon::EventLogger *logger ) : jr_jobdir(), jr_logger( logger )
{
  const configuration::WMConfiguration       *config = configuration::Configuration::instance()->wm();
  logger::StatePusher                         pusher( elog::cedglog, "JobResubmitter::JobResubmitter(...)" );

  try {
    boost::filesystem::path base( config->input(),  boost::filesystem::native);
    this->jr_jobdir = new utilities::JobDir( base );
  } catch( utilities::JobDirError &error) {
    elog::cedglog << logger::setlevel( logger::fatal )
                    << "Cannot open JobDir to WM." << endl
                    << "Error: \"" << error.what() << "\"." << endl;

    throw CannotOpenFile( config->input() );
  }  
}

JobResubmitter::~JobResubmitter( void )
{
  delete this->jr_jobdir;
}

void JobResubmitter::resubmit( int laststatus, const string &edgid, const string &sequence_code, jccommon::IdContainer *container )
{
  const configuration::WMConfiguration       *config = configuration::Configuration::instance()->wm();
  classad::ClassAd                            command, arguments;
  logger::StatePusher                         pusher( elog::cedglog, "JobResubmitter::resubmit(...)" );
  int                                         retcode;
  string                                      errors, sc, reason;
  JobWrapperOutputParser                      parser( edgid );

  elog::cedglog << logger::setlevel( logger::info )
		<< "Last known status = " << laststatus << endl;

  switch( laststatus ) {
    case GLOBUS_GRAM_PROTOCOL_ERROR_USER_PROXY_EXPIRED:
      elog::cedglog << logger::setlevel( logger::warning ) << "Job has an expiring proxy." << endl
		    << logger::setlevel( logger::info ) << "Must not resubmit, but abort." << endl
		    << "Job id = " << edgid << endl;

      this->jr_logger->abort_on_error_event( string("Job proxy is expired.") );

      jccommon::ProxyUnregistrar( edgid ).unregister();

      break;
  case jccommon::no_resubmission:
    elog::cedglog << logger::setlevel( logger::warning ) << "Job has been cancelled by the user." << endl
		  << "Don't resubmit it." << endl;
    break;
  case GLOBUS_GRAM_PROTOCOL_ERROR_STAGE_OUT_FAILED: // see LCG2 bug 3987
    if (parser.parse_file(retcode, errors, sc, reason) == JWOP::good) { // Job terminated successfully
      
      jccommon::IdContainer::iterator position = container->position_by_edg_id( edgid );
      
      elog::cedglog << logger::setlevel( logger::verylow ) << "Real return code: " << retcode << endl;
      
      if (this->jr_logger->have_lbproxy()) {
        this->jr_logger->set_LBProxy_context( edgid, position->sequence_code(), position->proxy_file() );
      } else {
        this->jr_logger->reset_user_proxy( position->proxy_file() ).reset_context( edgid, position->sequence_code() );
      }
      
      if ( !sc.empty() && ( sc != "NoToken" ) ) 
        this->jr_logger->job_really_run_event( sc ); // logged really running event 
      
      this->jr_logger->terminated_event(retcode); // This call discriminates between 0 and all other codes.
      container->update_pointer( position, this->jr_logger->sequence_code(), ULOG_JOB_TERMINATED );
      
      jccommon::ProxyUnregistrar( edgid ).unregister();
      
      break;
    } else // resubmit it --> default label. Pay attention: don't put any other case under this one!!!
      elog::cedglog << logger::setlevel( logger::verylow ) << "The job is not terminated successfully. Reason: " << errors << endl;
    
  default:
    elog::cedglog << logger::setlevel( logger::info ) << "Resubmitting job to WM." << endl
		  << logger::setlevel( logger::debug ) << "Job id = " << edgid << endl;
    
    command.InsertAttr( "version", string("1.0.0") );
    command.InsertAttr( "command", string("jobresubmit") );
    arguments.InsertAttr( "id", edgid );
    arguments.InsertAttr( "lb_sequence_code", sequence_code );
    command.Insert( "arguments", arguments.Copy() );

     if ( !sc.empty() && ( sc != "NoToken" ) ) 
       this->jr_logger->job_really_run_event( sc ); // logged really running event
    
    this->jr_logger->job_resubmitting_event();
    this->jr_logger->job_wm_enqueued_start_event( config->input(), command );
   
    try {
      std::string req;
      classad::ClassAdUnParser unparser;
      unparser.Unparse(req, &command);

      this->jr_jobdir->deliver(req);
    } catch (utilities::JobDirError const& error) {
      this->jr_logger->job_wm_enqueued_failed_event( config->input(), command, error.what() );

      throw CannotExecute( error.what() );
    }
    this->jr_logger->job_wm_enqueued_ok_event( config->input(), command );
    break;
  }

  return;
}

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END
