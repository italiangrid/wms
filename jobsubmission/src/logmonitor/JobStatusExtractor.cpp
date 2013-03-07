/* Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/lexical_cast.hpp>



#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"
#include "glite/wms/common/utilities/LineParser.h"
#include "jobcontrol_namespace.h"
#include "common/id_container.h"

#include "exceptions.h"
#include "JobStatusExtractor.h"
#include "JobWrapperOutputParser.h"

USING_COMMON_NAMESPACE;
using namespace std;
namespace fs = boost::filesystem;

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor {

JobStatusExtractor::JobStatusExtractor( const utilities::LineParser &options ) : jse_parser()
{
  const configuration::LMConfiguration    *config = configuration::Configuration::instance()->lm();
  string                                   cid, edgid;
  auto_ptr<jccommon::IdContainer>          container;

  if( options.is_present('c') ) {
    fs::path                  idrep( config->monitor_internal_dir(), fs::native );

    idrep /= config->id_repository_name();
    container.reset( new jccommon::IdContainer(idrep.native_file_string()) );

    cid.assign( options['c'].getStringValue() );
    edgid.assign( container->condor_id(cid) );

    if( edgid.size() == 0 ) throw InvalidJobId( cid );
  }
  else edgid.assign( options['e'].getStringValue() );

  if( options.is_present('d') ) {
    string    dagid( options['d'].getStringValue() );

    this->jse_parser.reset( new JobWrapperOutputParser(edgid) );
  }
  else this->jse_parser.reset( new JobWrapperOutputParser(edgid) );
}

JobStatusExtractor::~JobStatusExtractor( void ) {}

int JobStatusExtractor::get_job_status( string &errors )
{
  int                                   retcode;
  string                                sc, reason; // used for really_run event
  JobWrapperOutputParser::status_type   status;

  status = this->jse_parser->parse_file( retcode, errors, sc, reason );

  return retcode;
}

} // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END
