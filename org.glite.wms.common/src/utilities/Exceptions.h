#ifndef GLITE_WMS_COMMON_CLIENT_EXCEPTIONS_H
#define GLITE_WMS_COMMON_CLIENT_EXCEPTIONS_H

/*
 * Exceptions.h
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 */

#include <fstream>
#include <cstdlib>
//#include <list>
#include <syslog.h>  // For logging exceptions to log file
#include <errno.h> // list the exception codes
#include <string>
#include <exception> // base ancestor
#include "result_codes.h" //base result codes

namespace glite {
namespace wms {
namespace common {
namespace utilities {

extern pthread_mutex_t METHOD_MUTEX; //  used in order to store info into a file (rather then syslog)
#define EDG_STACK_TRY(method_name) std::string METHOD = method_name ; try {
#define EDG_STACK_CATCH() } catch (glite::wms::common::utilities::Exception &exc){ throw  glite::wms::common::utilities::Exception ( __FILE__ , METHOD , &exc)   ;  }

/**
 * The Exception classes contains attributes into which are placed exception information and provides
 * constructors that beyond the error code take parameters specifying the source file and line number
 * (e.g. through __FILE__ and __LINE__) where the error has been generated and string messages,
 * allowing an easy way of storing the origin of the exception.
 * Moreover it provides methods for getting all the exception information and for logging them either
 * in a log file or to the syslog daemon.
 * Each of the derived types may contain its private attributes describing the actual error instance in detail.
 * For example a JdlSyntaxException thrown when parsing the job description could contain additional
 * details indicating exactly on which attributes the error occurred.
 * Moreover each exception has an attribute representing the exception identifier that is set by the
 * class constructor and allows the identification of the original exception.
 *
 * @ingroup Common
 * @version 0.1
 * @date 15 April 2002
 * @author Alessandro Maraschini <alessandro.maraschini@datamat.it>
*/

class Exception : public std::exception{
 public:
  Exception (  const std::string& source,
		  const std::string& method,
                  Exception *e);
  /**
  *  Constructor Update all mandatory fields
  * @param code the code representing the thrown exception
  * @param exception the name of the thrown exception
  * @param method the name of the method that raised the exception
  * @param source The source that raised the exception (could be the file path, the class Name, etc etc)
  * @param line_number the number of the line in the file that raised the exception(if the source has been given as a file)  */

  Exception ( const std::string& source,
			const std::string& method,
			int code,
			const std::string& exception);

  /**
  *  Constructor Update all mandatory fields
  * @param source the path of the file that raised the exception
  * @param line_number the number of the line in the file that raised the exception
  * @param method the name of the method that raised the exception
  * @param code the code representing the thrown exception
  * @param exception the name of the thrown exception */
  Exception (const std::string& source,
			int line_number,
			const std::string& method,
			int code,
			const std::string& exception);

  /**
  * Destructor
  */
  virtual ~Exception() throw ();
  /**
  *  Return a string debug message containing information about Exception thrown
  */
  virtual std::string dbgMessage();
  /**
  *  Return the Code number
  */
  virtual int getCode();

  /**
  * return the Error Message associated to the Exception
  */
  virtual const char* what() const  throw ();

  /**
  *  Print Exception error information into a log file
  */
  virtual void log(const std::string& logfile = "");
  /**
  *   Return the name of the Exception raised
  */
  virtual std::string getExceptionName();

  /**
  *   Return the list of methods that caused the Exception
  */
  virtual std::string printStackTrace() ;

  protected:
       /**
       * Empty Default Constructor
       */
        Exception();
        //Mandatory Information:
        int                   error_code;
        std::string          error_message ;
        int                   line;
        std::string          source_file;
        std::string          exception_name;
        std::string          method_name ;
	std::string          stack;
	std::string          ancestor ;
        // Exception* exc ;
}; //End  Exception Class

/**
 *  This Exception is thrown when a unespected fatal error is found
 */
class FatalErrorException : public Exception {
public:
    FatalErrorException(const std::string& file,
			int line,
			const std::string& method);
};//End CLass      FatalErrorException

/**
 *   StdException remap the Exception thrown by std::exception class into the JSUI WL standard format
 */
class StdException : public Exception {
public:
    StdException(const std::string& file,
		 int line,
		 const std::string& method,
		 int code,
		 const std::string& exception_name);
};//End CLass   StdException

/**
 *   StdException remap the Exception thrown by std::exception class into the JSUI WL standard format
 */
class ThreadException : public Exception {
public:
    ThreadException(const std::string& file,
		    int line,
		    const std::string& method,
		    int code,
		    int jobNumber);
};//End CLass   StdException

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif
