#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENTREAL_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENTREAL_H

#include <memory>

#include "glite/wms/common/utilities/FileList.h"
#include "glite/wms/common/utilities/FileListLock.h"
#include "glite/wms/common/utilities/Extractor.h"

#include "JobControllerClientImpl.h"
#include "Request.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class JobControllerClientReal : public JobControllerClientImpl {
public:
  JobControllerClientReal();
  virtual ~JobControllerClientReal();

  virtual void release_request();
  virtual void extract_next_request();
  virtual Request *get_current_request();
  virtual std::string const get_current_request_name() const { return ""; }

private:
  typedef glite::wms::common::utilities::FileList<classad::ClassAd>   queue_type;

  bool                                                        jccr_currentGood;
  queue_type::iterator                                        jccr_current;
  Request                                                     jccr_request;
  queue_type                                                  jccr_queue;
  std::auto_ptr<glite::wms::common::utilities::FileListDescriptorMutex>   jccr_mutex;
  glite::wms::common::utilities::ForwardExtractor<queue_type>             jccr_extractor;
};

}} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENTREAL_H */

// Local Variables:
// mode: c++
// End:
