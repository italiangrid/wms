#ifndef _GLITE_WMS_MANAGER_NS_CLIENT_CLIENTEXCEPTIONS_H_
#define _GLITE_WMS_MANAGER_NS_CLIENT_CLIENTEXCEPTIONS_H_

#include <vector>
#include <exception>
#include <string> 

#include "glite/wmsutils/exception/Exception.h"
#include "glite/wms/ns-common/exception_codes.h"

namespace excep = glite::wmsutils::exception;

namespace glite {
namespace wms {
namespace manager {
namespace ns {  
namespace client {
 
   class Exception : public excep::Exception
   {
      protected:	
        Exception() {} 
        Exception(const std::string& s, const std::string& m, int c, const std::string& n) :
	excep::Exception(s, m, c, n) {} 
   };
 
   class ConnectionException : public client::Exception
   {
     public: 
        ConnectionException( const std::string& contact ) :
          client::Exception( contact, std::string(""), NSE_CONNECTION_ERROR, "ConnectionException" )
    {
      this -> error_message = std::string( "Unable to contact any networkserver daemon at: " ) +
	std::string( contact );
    }
   protected:
      ConnectionException(const std::string& s, const std::string& m, int c, const std::string& n) :
	client::Exception(s, m, c, n) {} 
      ConnectionException() {} 
  };

  struct AuthenticationException : ConnectionException
  {
    AuthenticationException( const std::string& ucs)  :
	ConnectionException( ucs, std::string(""), NSE_AUTHENTICATION_ERROR, "AuthenticationException" )
      {
        this -> error_message = std::string("Unable to authenticate ") +
	  std::string(ucs) + std::string(" while handshaking...");
      }
  };
  
  class JobNotFoundException : public client::Exception
  {
    public:
    JobNotFoundException ( const std::string& source, const std::string& message ) :
      client::Exception( source, std::string(""), NSE_JOB_NOT_FOUND, "JobNotFoundException" )
    {
      this -> error_message = message;
    }
    protected:
    JobNotFoundException(const std::string& s, const std::string& m, int c, const std::string& n) : client::Exception(s, m, c, n) {} 
    JobNotFoundException() {}
  };

  struct JobNotDoneException : JobNotFoundException
  {
    /**
     * Constructor.
     * @param message the massage explaining this exception.
     */
    JobNotDoneException( const std::string source, const std::string& message ) :
      JobNotFoundException( source , std::string(""), NSE_JOB_NOT_DONE, "JobNotDoneException" )
    {
      this -> error_message = message;
    };
  };
  
  /**
   * An empty job-list Exception.
   * It is thrown when a Client searches for a certain user's jobs
   * and no job is found. 
   * @author Salvo Monforte and Marco Pappalardo.
   */
  struct EmptyJobListException : client::Exception
  {
    EmptyJobListException ( const std::string& source, const std::string& message ) : 
      client::Exception( source , std::string(""), NSE_EMPTY_JOB_LIST, "EmptyJobListException" )
    {
      this -> error_message = message;
    }
  };  
 
  /**
   * Means that no suitable resource is found for the job.
   * It is thrown when a Client searches for resources having specified
   * attributes in order to perform a given job and no resource is found.
   * @author Salvo Monforte and Marco Pappalardo.
   */
  struct NoSuitableResourceException : client::Exception
  {
    /**
     * Constructor.
     * @param cause the job to perform identifier.
     * @param messge a message explaining the exception.
     */
    NoSuitableResourceException ( const std::string& source, const std::string& message ) : 
      client::Exception(source, std::string(""), NSE_NO_SUITABLE_RESOURCE, "NoSuitableResourceException" )
    {
      this -> error_message = message;
    }
  };

  /**
   * User not authorized exception.
   * It is thrown when a user asks for authorization and he has no
   * authorized user profile.
   * @author Salvo Monforte and Marco Pappalardo.
   */
  struct NotAuthorizedUserException : ConnectionException
  {      
    /**
     * Constructor.
     * @param cause the not authorized user identifier.
     * @param message a message explaining the exception.
     */
    NotAuthorizedUserException ( const std::string& source, const std::string& message ) :
      ConnectionException(source, std::string(""), NSE_NOT_AUTHORIZED_USER, "NotAuthorizedUserException" )
    {
      this -> error_message = message;
    }

  };   

  /**
   * Proxy Renewal Exception.
   * It is thrown if an error occurred while registering for proxy renewal.
   * @author Salvo Monforte and Marco Pappalardo.
   */
  struct ProxyRenewalException : client::Exception
  {
    /**
     * Constructor.
     * @param cause the failing proxy renewal source routine.
     * @param message a message explaining the exception.
     */
    ProxyRenewalException ( const std::string& source, const std::string& message ) :
      client::Exception(source, std::string(""), NSE_PROXY_RENEWAL_FAILURE, "ProxyRenewalException" )
    {
      this -> error_message = message;
    }

  };

  /**
   * Sandbox Input/Output Exception.
   * It is thrown if an error occurred while transmitting Input Sandbox
   * to the Server or retrieven OutputSandbox from it.
   * @author Salvo Monforte and Marco Pappalardo.
   */
  class SandboxIOException : public client::Exception
  {      
    public:
    /**
     * Constructor.
     * @param message the message related to this exception.
     */
    SandboxIOException( const std::string& message ) :
      client::Exception( std::string(""), std::string(""), NSE_SANDBOX_IO, "SandboxIOException" )
    {
      this -> error_message = message;
    }

    /**
     * Constructor.
     * @param cause the cause of the IO error. May be a missing file or else.
     * @param message a message explaining the exception.
     */
    SandboxIOException ( const std::string& cause, const std::string& message ) : 
     client::Exception( cause, std::string(""), NSE_SANDBOX_IO, "SandboxIOException" )
    {
      this -> error_message = message;
    }
   protected:
   SandboxIOException(const std::string& s, const std::string& m, int c, const std::string& n) :
	client::Exception(s, m, c, n) {} 
   SandboxIOException() {}
  };

   /**
   * Not Enough Space Exception.
   * It is thrown if not enough space is found on the destination of an Input/Output Sandbox
   * transfer.
   * @author Salvo Monforte and Marco Pappalardo.
   */
  struct NotEnoughSpaceException : SandboxIOException
  {      
    /**
     * Constructor.
     * @param message the message related to this exception.
     */
    NotEnoughSpaceException( const std::string& message ) :
	SandboxIOException( std::string(""), std::string(""), NSE_NOT_ENOUGH_SPACE, "NotEnoughSpaceException" )
    {
      this -> error_message = message;
    }
  };   

   /**
   * Job Size Exception.
   * It is thrown if a job exceeds limit estabilished by quota management, if
   * enabled.
   * @author Marco Pappalardo.
   */
  struct JobSizeException : SandboxIOException
  {      
    /**
     * Constructor.
     * @param message the message related to this exception.
     */
    JobSizeException( const std::string& message ) :
	SandboxIOException( std::string(""), std::string(""), NSE_JOB_SIZE, "JobSizeException" )
    {
      this -> error_message = message;
    }
  };   

  
  /**
   * Not Enough Quota Exception.
   * It is thrown if not enough quota free space is found on the destination for the
   * user requiring an Input/Output Sandbox transfer, if quota management is enabled.
   * @author Salvo Monforte and Marco Pappalardo.
   */
  struct NotEnoughQuotaException : SandboxIOException
  {      
    /**
     * Constructor.
     * @param message the message related to this exception.
     */
    NotEnoughQuotaException( const std::string& message ) :
	SandboxIOException( std::string(""), std::string(""), NSE_NOT_ENOUGH_QUOTA, "NotEnoughQuotaException" )
    {
      this -> error_message = message;
    }
  };   

  
  /**
   * JDL Parsing exception.
   * It is thrown when an error occurred while parsing a JDL expression.
   * @author Salvo Monforte and Marco Pappalardo.
   */
  struct JDLParsingException : client::Exception
  {      
    /**
     * Constructor.
     * @param cause the jdl expression that caused this exception.
     * @param method the NS Client's method called.
     * @param message the error message related to the parsing activity.
     */
    JDLParsingException ( const std::string& cause, const std::string& method, const std::string& message ) : 
	client::Exception ( cause, std::string(""), NSE_JDL_PARSING, "JDLParsingException" )
    {
      this -> error_message = message;
    }
  };   

  /**
   * ListMatch exception.
   * It is thrown when an error occurred while connecting or communicating with Information Service.
   * @author Salvo Monforte and Marco Pappalardo.
   */
  struct ListMatchException : client::Exception
  {      
    /**
     * Constructor.
     * @param cause the jdl expression that caused this exception.
     * @param method the NS Client's method called.
     * @param message the error message related.
     */
    ListMatchException ( const std::string& cause, const std::string& method, const std::string& message ) : 
	client::Exception ( cause, std::string(""), NSE_IS_FAILURE, "ListMatchException" )
    {
      this -> error_message = message;
    }
  };   


  struct MatchMakingException : client::Exception
  {
    /**
     * Constructor.
     * @param cause the cause.
     * @param messge a message explaining the exception.
     */
    MatchMakingException ( const std::string& cause, const std::string& message ) : 
      client::Exception ( cause, std::string(""), NSE_MATCHMAKING, "MatchMakingException" )
    {
      this -> error_message = message;
    }
  };   


  /**
   * A Wrong Command Exception.
   * It is thrown when a Client sends a wrong command 
   * to the Resource Broker Server.
   * @author Salvo Monforte and Marco Pappalardo.
   */
  struct WrongCommandException : client::Exception
  {
    /**
     * Constructor.
     * @param message the massage explaining this exception.
     */
    WrongCommandException( const std::string& message ) :
      client::Exception( std::string(""), std::string(""), NSE_WRONG_COMMAND, "WrongCommandException" )
    {
      this -> error_message = message;
    };
  };

} // namespace client
} // namespace ns
} // namespace manager 
} // namespace wms
} // namespace glite

#endif
