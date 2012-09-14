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
#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENT_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENT_H

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class Request;
class JobControllerClientImpl;

class JobControllerClient {
public:
  JobControllerClient( void );
  ~JobControllerClient( void );

  void release_request( void );
  void extract_next_request( void );
  const Request *get_current_request( void );

private:
  JobControllerClient( const JobControllerClient & ); // Not implemented
  JobControllerClient &operator=( const JobControllerClient & ); // Not implemented

  JobControllerClientImpl   *jcc_impl;
};

} // Namespace controller

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERCLIENT_H */

// Local Variables:
// mode: c++
// End:
