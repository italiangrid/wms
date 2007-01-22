#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENTJD_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENTJD_H

#include <queue>
#include <boost/filesystem/path.hpp>

#include "glite/wms/common/utilities/jobdir.h"

#include "JobControllerClientImpl.h"
#include "Request.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class JobControllerClientJD : public JobControllerClientImpl {
public:
  JobControllerClientJD( void );
  virtual ~JobControllerClientJD( void );

  virtual void release_request( void );
  virtual void extract_next_request( void );
  virtual const Request *get_current_request( void );

private:

  bool                                      jccjd_currentGood;
  boost::filesystem::path                   jccjd_current;
  Request                                   jccjd_request;
  std::queue< boost::filesystem::path >     jccjd_queue;
  glite::wms::common::utilities::JobDir*    jccjd_jd;
};

}; // namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENTJD_H */

// Local Variables:
// mode: c++
// End:
