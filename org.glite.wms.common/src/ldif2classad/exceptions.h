#ifndef _LDIF2ClassAdExceptions_h_
#define _LDIF2ClassAdExceptions_h_

/*
 * exceptions.h
 * Copyright (c) 2002 EU DataGrid
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#include "glite/wms/common/utilities/Exceptions.h"
#include "exception_codes.h"

namespace utilities = glite::wms::common::utilities;

namespace glite {  
namespace wms {
namespace common {
namespace ldif2classad {
 
    class Exception : public utilities::Exception
    {
      protected:
        Exception() {}
        Exception(const std::string& s, const std::string& m, int c, const std::string& n) :
        utilities::Exception(s, m, c, n) {}
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
