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

#include "glite/wms/common/utilities/boost_fs_add.h"
#include "jobcontrol_namespace.h"
#include "logmonitor/exceptions.h"
#include "logmonitor/Timer.h"
#include "logmonitor/SizeFile.h"

#include "MonitorData.h"

using namespace std;
USING_COMMON_NAMESPACE;

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { 

MonitorData::MonitorData( AbortedContainer *aborted, jccommon::IdContainer *container, processer::JobResubmitter *resubmitter, jccommon::EventLogger *logger ) : 
  md_logger( logger ), md_container( container ), md_aborted( aborted ), md_resubmitter( resubmitter )
{}

namespace processer {

MonitorData::MonitorData( const string &filename, logmonitor::MonitorData &data ) :
  md_logger( data.md_logger ), md_container( data.md_container ), md_aborted( data.md_aborted ), md_resubmitter( data.md_resubmitter ),
  md_logfile_name( utilities::normalize_path(filename) ),
  md_dagId(),
  md_timer(), md_sizefile()
{
  if( this->md_logfile_name.length() == 0 )
    throw InvalidFileName( filename );
}

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END
