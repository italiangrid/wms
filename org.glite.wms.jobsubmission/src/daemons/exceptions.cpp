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
#include "exceptions.h"

using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {

namespace daemons {

DaemonError::DaemonError( const string &reason ) : exception(), de_reason( reason ) {}

DaemonError::~DaemonError( void ) throw() {}

const char *DaemonError::what( void ) const throw()
{
  return this->de_reason.c_str();
}

CannotStart::CannotStart( const string &reason ) : DaemonError( reason ) {}

CannotStart::~CannotStart( void ) throw() {}

CannotExecute::CannotExecute( const string &reason ) : DaemonError( reason ) {}

CannotExecute::~CannotExecute( void ) throw() {}

} // Namespace daemons

} JOBCONTROL_NAMESPACE_END
