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
#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_REQUESTEXCEPTIONS_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_REQUESTEXCEPTIONS_H

#include <exception>

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class RequestException : public std::exception {
public:
  RequestException( void );
  virtual ~RequestException( void ) throw();

  virtual const char *what( void ) const throw() = 0;

protected:
  mutable std::string   re_what;
};

class MalformedRequest : public RequestException {
public:
  MalformedRequest( const classad::ClassAd &ad );
  virtual ~MalformedRequest( void ) throw();

  virtual const char *what( void ) const throw();

  inline const classad::ClassAd &classad( void ) const { return *this->mr_ad; }

private:
  classad::ClassAd     *mr_ad;
};

class UninitializedRequest : public RequestException {
public:
  UninitializedRequest( void );
  virtual ~UninitializedRequest( void ) throw();

  virtual const char *what( void ) const throw();
};

class MismatchedProtocol : public RequestException {
public:
  MismatchedProtocol( const std::string &def, const std::string &current );
  virtual ~MismatchedProtocol( void ) throw();

  virtual const char *what( void ) const throw();

  inline const std::string &default_protocol( void ) const { return this->mp_default; }
  inline const std::string &current_protocol( void ) const { return this->mp_current; }

private:
  std::string   mp_default, mp_current;
};

} // Namespace controller

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_REQUESTEXCEPTIONS_H */

// Local Variables:
// mode: c++
// End:
