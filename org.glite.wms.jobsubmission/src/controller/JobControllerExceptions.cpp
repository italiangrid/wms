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
#include <string>

#include "jobcontrol_namespace.h"

#include "JobControllerExceptions.h"

using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

ControllerError::ControllerError( void ) : exception(), ce_what() {}

ControllerError::~ControllerError( void ) throw() {}

const char *ControllerError::what( void ) const throw()
{
  this->ce_what.assign( this->reason() );

  return this->ce_what.c_str();
}

CannotCreate::CannotCreate( const string &reason ) : cc_reason( reason )
{}

CannotCreate::~CannotCreate( void ) throw() {}

const string &CannotCreate::reason( void ) const
{ return this->cc_reason; }

CannotExecute::CannotExecute( const string &reason ) : ce_reason( reason )
{}

CannotExecute::~CannotExecute( void ) throw() {}

const string &CannotExecute::reason( void ) const
{ return this->ce_reason; }

} // namespace controller

} JOBCONTROL_NAMESPACE_END
