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

#ifndef _LDIF2ClassAdExceptions_h_
#define _LDIF2ClassAdExceptions_h_

/*
 * exceptions.h
 * Copyright (c) 2002 EU DataGrid
 */

#include "glite/wmsutils/exception/Exception.h"
#include "exception_codes.h"

namespace excep = glite::wmsutils::exception;

namespace glite {  
namespace wms {
namespace common {
namespace ldif2classad {
 
    class Exception : public excep::Exception
    {
      protected:
        Exception() {}
        Exception(const std::string& s, const std::string& m, int c, const std::string& n) :
        excep::Exception(s, m, c, n) {}
    };
 
   struct ConnectionException : ldif2classad::Exception
    {
        ConnectionException(const std::string& source, const std::string& method, const std::string& message) :
         ldif2classad::Exception( source, method, LDAP_CONNECTION_ERROR, "ConnectionException" ) {
                this -> error_message = message;
         }
    };

    struct QueryException : ldif2classad::Exception
     {
	QueryException(const std::string& source, const std::string& method, const std::string& message) :
         ldif2classad::Exception( source, method, LDAP_QUERY_ERROR, "QueryException" ) {
                this -> error_message = message;
         }

     };	

    struct UndefinedValueException : ldif2classad::Exception
     {
        UndefinedValueException(const std::string& source, const std::string& method, const std::string& message) :
         ldif2classad::Exception( source, method, LDAP_UNDEFINED_VALUE_ERROR, "UndefinedValueException" ) {
                this -> error_message = message;
         }
 
     };
 
} // namespace ldif2classad
} // namespace common
} // namespace wms
} // namespace glite

#endif
