#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

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

MonitorData::MonitorData(
  boost::shared_ptr<AbortedContainer> aborted,
  boost::shared_ptr<jccommon::IdContainer> container,
  boost::shared_ptr<processer::JobResubmitter> resubmitter,
  boost::shared_ptr<jccommon::EventLogger> logger)
: 
  md_aborted(aborted),
  md_container(container),
  md_resubmitter(resubmitter),
  md_logger(logger)
{ }

namespace processer {

MonitorData::MonitorData( const string &filename, logmonitor::MonitorData &data ) :
  md_isDagLog( false ),
  md_logger( data.md_logger ), md_container( data.md_container ), md_aborted( data.md_aborted ), md_resubmitter( data.md_resubmitter ),
  md_logfile_name( boost::filesystem::normalize_path(filename) ),
  md_dagId(),
  md_timer(), md_sizefile()
{
  if (this->md_logfile_name.length() == 0) {
    throw InvalidFileName(filename);
  }
}

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END;
