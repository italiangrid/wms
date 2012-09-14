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

#include <memory>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

namespace fs = boost::filesystem;

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"

#include "glite/jobid/JobId.h"

#include "glite/wms/common/utilities/boost_fs_add.h"
#include "glite/wms/purger/purger.h"
#include "jobcontrol_namespace.h"
#include "common/files.h"

#include "JobFilePurger.h"

USING_COMMON_NAMESPACE;
using namespace std;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

JobFilePurger::JobFilePurger(
  glite::jobid::JobId const& id,
  bool have_lbproxy
)
  : jfp_have_lbproxy(have_lbproxy), jfp_jobId(id)
{ }

void JobFilePurger::do_purge( bool everything )
{
  const configuration::LMConfiguration    *lmconfig = configuration::Configuration::instance()->lm();
  logger::StatePusher                      pusher( elog::cedglog, "JobFilePurger::do_purge(...)" );

  if( lmconfig->remove_job_files() ) {
    unsigned long int            removed;
    auto_ptr<Files>              files(new Files(this->jfp_jobId));

    try {
      elog::cedglog << logger::setlevel( logger::info ) << "Removing job directory: " << files->output_directory().native_file_string() << endl;
      removed = fs::remove_all( files->output_directory() );
      elog::cedglog << logger::setlevel( logger::ugly ) << "Removed " << removed << " files." << endl;
    }
    catch( fs::filesystem_error &err ) {
      elog::cedglog << logger::setlevel( logger::null ) << "Failed to remove job directory." << endl
		    << "Reason: " << err.what() << endl;
    }

    try {
      elog::cedglog << logger::setlevel( logger::info ) << "Removing submit file: " << files->submit_file().native_file_string() << endl;
      fs::remove( files->submit_file() );
      elog::cedglog << logger::setlevel( logger::ugly ) << "Removed..." << endl;
    }
    catch( fs::filesystem_error &err ) {
      elog::cedglog << logger::setlevel( logger::null ) << "Failed to remove submit file." << endl
		    << "Reason: " << err.what() << endl;
    }

    try {
      elog::cedglog << logger::setlevel( logger::info ) << "Removing classad file: " << files->classad_file().native_file_string() << endl;
      fs::remove( files->classad_file() );
      elog::cedglog << logger::setlevel( logger::ugly ) << "Removed..." << endl;
    }
    catch( fs::filesystem_error &err ) {
      elog::cedglog << logger::setlevel( logger::null ) << "Failed to remove classad file." << endl
		    << "Reason: " << err.what() << endl;
    }

      try {
	elog::cedglog << logger::setlevel( logger::info ) << "Removing wrapper file: " << files->jobwrapper_file().native_file_string() << endl;
	fs::remove( files->jobwrapper_file() );
	elog::cedglog << logger::setlevel( logger::ugly ) << "Removed..." << endl;
      }
      catch( fs::filesystem_error &err ) {
	elog::cedglog << logger::setlevel( logger::null ) << "Failed to remove wrapper file." << endl
		      << "Reason: " << err.what() << endl;
      }
  }
  else
    elog::cedglog << logger::setlevel( logger::info )
		  << "Job files not removed." << endl;

  if( everything ) {
    elog::cedglog << logger::setlevel( logger::ugly ) << "Going to purge job storage..." << endl;

    purger::Purger ThePurger(this->jfp_have_lbproxy);
    ThePurger.force_dag_node_removal()(this->jfp_jobId);
  }

  return;
}

} // jccommon namespace

} JOBCONTROL_NAMESPACE_END
