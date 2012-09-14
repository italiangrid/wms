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
#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLEREXCEPTIONS_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLEREXCEPTIONS_H

#include <exception>
#include <string>

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class ControllerError : public std::exception {
public:
  ControllerError( void );
  virtual ~ControllerError( void ) throw();

  virtual const std::string &reason( void ) const = 0;
  virtual const char *what( void ) const throw();

private:
  mutable std::string   ce_what;
};

class CannotCreate : public ControllerError {
public:
  CannotCreate( const std::string &reason );
  virtual ~CannotCreate( void ) throw();

  virtual const std::string &reason( void ) const;

private:
  std::string    cc_reason;
};

class CannotExecute : public ControllerError {
public:
  CannotExecute( const std::string &reason );
  virtual ~CannotExecute( void ) throw();

  virtual const std::string &reason( void ) const;

private:
  std::string   ce_reason;
};

} // namespace controller

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLEREXCEPTIONS_H */

// Local Variables:
// mode: c++
// End:
