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
#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENTIMPL_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENTIMPL_H

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class Request;

class JobControllerClientImpl {
public:
  JobControllerClientImpl( void ) {}
  virtual ~JobControllerClientImpl( void ) {}

  virtual void release_request( void ) = 0;
  virtual void extract_next_request( void ) = 0;
  virtual const Request *get_current_request( void ) = 0;

private:
  JobControllerClientImpl( const JobControllerClientImpl & ); // Not implemented
  JobControllerClientImpl &operator=( const JobControllerClientImpl & ); // Not implemented
};

} // namespace controller

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENTIMPL_H */

// Local Variables:
// mode: c++
// End:
