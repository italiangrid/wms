#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENTJD_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENTJD_H

#include <queue>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>

#include "glite/wms/common/utilities/jobdir.h"

#include "JobControllerClientImpl.h"
#include "Request.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class JobControllerClientJD : public JobControllerClientImpl {
public:
  JobControllerClientJD();
  virtual ~JobControllerClientJD() { }

<<<<<<< JobControllerClientJD.h
  virtual void release_request();
  virtual void extract_next_request();
  virtual const Request *get_current_request();
  virtual std::string const get_current_request_name() const;
=======
  virtual void release_request();
  virtual void extract_next_request();
  virtual const Request *get_current_request();
>>>>>>> 1.3

private:
  bool                                      jccjd_currentGood;
  boost::filesystem::path                   jccjd_current;
  Request                                   jccjd_request;
  std::queue<boost::filesystem::path>       jccjd_queue;
  boost::shared_ptr<glite::wms::common::utilities::JobDir> jccjd_jd;
};

}; // namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENTJD_H */

// Local Variables:
// mode: c++
// End:
