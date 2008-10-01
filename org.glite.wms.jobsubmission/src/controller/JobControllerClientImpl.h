#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENTIMPL_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENTIMPL_H

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class Request;

class JobControllerClientImpl {
public:
  JobControllerClientImpl( void ) {}
  virtual ~JobControllerClientImpl( void ) {}

  virtual void release_request() = 0;
  virtual void extract_next_request() = 0;
  virtual const Request *get_current_request() = 0;
  virtual std::string const get_current_request_name() const = 0;

private:
  JobControllerClientImpl( const JobControllerClientImpl & ); // Not implemented
  JobControllerClientImpl &operator=( const JobControllerClientImpl & ); // Not implemented
};

}; // namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENTIMPL_H */

// Local Variables:
// mode: c++
// End:
