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
  md_isDagLog( false ),
  md_logger( data.md_logger ), md_container( data.md_container ), md_aborted( data.md_aborted ), md_resubmitter( data.md_resubmitter ),
  md_logfile_name( boost::filesystem::normalize_path(filename) ),
  md_dagId(),
  md_timer(), md_sizefile()
{
  if( this->md_logfile_name.length() == 0 )
    throw InvalidFileName( filename );
}

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;
