/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef GLITE_WMS_COMMON_LDIF2CLASSAD_EXCEPTION_CODES_H
#define GLITE_WMS_COMMON_LDIF2CLASSAD_EXCEPTION_CODES_H

/*
 * exception_codes.h
 */

#include "glite/wmsutils/exception/exception_codes.h"

namespace glite {
namespace wms {
namespace common {
namespace ldif2classad {

enum {
      LDAP_FATAL_ERROR = glite::wmsutils::exception::WMS_LDAP_ERROR_BASE +1 ,
      LDAP_CONNECTION_ERROR,
      LDAP_QUERY_ERROR,
      LDAP_UNDEFINED_VALUE_ERROR
};

} // namespace ldif2classad
} // namespace common
} // namespace wms
} // namespace glite

#endif

// EOF
