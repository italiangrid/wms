#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENT_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENT_H

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class Request;
class JobControllerClientImpl;

class JobControllerClient {
public:
  JobControllerClient();
  ~JobControllerClient();

  void release_request();
  void extract_next_request();
  const Request *get_current_request();
  std::string const get_current_request_name() const;

private:
  JobControllerClient(const JobControllerClient &);
  JobControllerClient &operator=(const JobControllerClient &);

  JobControllerClientImpl *jcc_impl;
};

}; // Namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENT_H */

// Local Variables:
// mode: c++
// End:
