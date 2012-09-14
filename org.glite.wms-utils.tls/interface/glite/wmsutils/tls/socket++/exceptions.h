#ifndef _SocketExceptions_h_
#define _SocketExceptions_h_

/*
 * Copyright (c) 2002 EU DataGrid
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#include "glite/wmsutils/exception/Exception.h"
#include "glite/wmsutils/tls/socket++/exception_codes.h"

namespace utilities = glite::wmsutils::exception;

namespace glite {  
namespace wmsutils {
namespace tls {
namespace socket_pp {
   
   class Exception : public utilities::Exception
    {
      protected:
        Exception() {}
        Exception(const std::string& s, const std::string& m, int c, const std::string& n) :
        utilities::Exception(s, m, c, n) {}
    };

   struct IOException : socket_pp::Exception
    { 
	IOException( const std::string& source, const std::string& method, const std::string& message) :
	 socket_pp::Exception( source, method, SOCKET_IO_ERROR, "IOException" ) {
	   	this -> error_message = message;
	 }
    };
  
  class GSIException : public socket_pp::Exception
   {
    protected:
        GSIException(const std::string& s, const std::string& m, int c, const std::string& n) : socket_pp::Exception(s,m,c,n) {}
	GSIException() {}
   };
  
  struct AuthorizationException : GSIException
    {   
      AuthorizationException( const std::string& source, const std::string& method, const std::string& message ) : 
           GSIException( source, method, SOCKET_AUTHORIZATION_ERROR, "AuthorizationException" ) {
                this -> error_message = message;
           }
    };

  struct AuthenticationException : GSIException
    {	
      AuthenticationException( std::string source, std::string method, std::string message ) :  
           GSIException( source, method, SOCKET_AUTHENTICATION_ERROR, "AuthenticationException")  {
                this -> error_message = message; 
           }
    };
  
} // namespace socket_pp
} // namespace tls
} // namespace wmsutils 
} // namespace glite

#endif // _SocketExceptions_h_
