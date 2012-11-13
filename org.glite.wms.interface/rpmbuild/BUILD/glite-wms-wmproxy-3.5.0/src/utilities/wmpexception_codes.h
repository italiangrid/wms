/*
Copyright (c) Members of the EGEE Collaboration. 2004.
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
limitations under the License.
*/

//
// File: wmpexception_codes.h
// Author: Giuseppe Avellino <egee@datamat.it>
//

#ifndef GLITE_WMS_WMPROXY_SERVER_EXCEPTION_CODES_H
#define GLITE_WMS_WMPROXY_SERVER_EXCEPTION_CODES_H

#include "glite/wmsutils/exception/exception_codes.h"


namespace glite
{
namespace wms
{
namespace wmproxy
{
namespace utilities
{

enum {

   WMS_FATAL = glite::wmsutils::exception::WMS_NS_ERROR_BASE + 1, // 1201
   WMS_IS_FAILURE,  // 1202

   WMS_FILE_SYSTEM_ERROR,  // ...
   WMS_ENVIRONMENT_ERROR,
   WMS_CONFIGURATION_ERROR,

   WMS_AUTHENTICATION_ERROR,

   WMS_AUTHORIZATION_ERROR,
   WMS_USERMAP_ERROR,
   WMS_GACL_ERROR,
   WMS_GACL_FILE,
   WMS_GACL_ITEM_NOT_FOUND,

   WMS_PROXY_ERROR,
   WMS_PROXY_EXPIRED,
   WMS_NOT_A_VOMS_PROXY,
   WMS_DELEGATION_ERROR,
   WMS_PROXY_RENEWAL_FAILURE,

   WMS_JOB_NOT_DONE,
   WMS_JOB_NOT_FOUND,

   //WMS_SANDBOX_IO,
   WMS_JDL_PARSING,
   WMS_INVALID_JDL_ATTRIBUTE,
   //WMS_MULTI_ATTRIBUTE_FAILURE,

   WMS_MATCHMAKING,
   WMS_NO_SUITABLE_RESOURCE,

   WMS_NOT_ENOUGH_SPACE,
   WMS_NOT_ENOUGH_QUOTA,

   WMS_INVALID_ARGUMENT,
   WMS_OPERATION_NOT_ALLOWED,

   WMS_LOGGING_ERROR,

   WMS_SERVER_OVERLOADED,

   WMS_OPERATION_TIMEDOUT,

   WMS_NO_ERROR
};

} // namespace utilities
} // namespace wmproxy
} // namespace wms
} // namespace glite

#endif // GLITE_WMS_WMPROXY_SERVER_EXCEPTION_CODES_H

