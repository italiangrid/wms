#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/lexical_cast.hpp>



#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"
#include "glite/wms/common/utilities/LineParser.h"
#include "jobcontrol_namespace.h"
#include "common/IdContainer.h"

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

    this->jse_parser.reset( new JobWrapperOutputParser(dagid, edgid) );
  }
  else this->jse_parser.reset( new JobWrapperOutputParser(edgid) );
}

JobStatusExtractor::~JobStatusExtractor( void ) {}

int JobStatusExtractor::get_job_status( string &errors )
{
  int                                   retcode;
  string                                sc; // used for really_run event
  JobWrapperOutputParser::status_type   status;

  status = this->jse_parser->parse_file( retcode, errors, sc );

  return retcode;
}

}; // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END;
