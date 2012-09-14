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
#ifndef EDG_WORKLOAD_JOBCONTROL_DAEMONS_EXCEPTIONS_H
#define EDG_WORKLOAD_JOBCONTROL_DAEMONS_EXCEPTIONS_H

#include <exception>

JOBCONTROL_NAMESPACE_BEGIN {

namespace daemons {

class DaemonError : public std::exception {
public:
  DaemonError( const std::string &reason );
  virtual ~DaemonError( void ) throw();

  inline const std::string &reason( void ) const { return this->de_reason; }
  virtual const char *what( void ) const throw();

private:
  std::string   de_reason;
};

class CannotStart : public DaemonError {
public:
  CannotStart( const std::string &reason );
  virtual ~CannotStart( void ) throw();
};

class CannotExecute : public DaemonError {
public:
  CannotExecute( const std::string &reason );
  virtual ~CannotExecute( void ) throw();
};

} // Namespace daemons

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_DAEMONS_EXCEPTIONS_H */

// Local Variables:
// mode: c++
// End:
