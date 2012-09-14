#ifndef  GLITE_WMS_UTILS_EXCEPTION_EXCEPTION_H
#define GLITE_WMS_UTILS_EXCEPTION_EXCEPTION_H

/*
 * Exception.h
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 */

#include <fstream>
#include <cstdlib>
//#include <list>
#include <syslog.h>  // For logging exceptions to log file
#include <errno.h> // list the exception codes
#include <string>
#include <vector>
#include <exception> // base ancestor stl::exception


namespace glite {
	namespace wmsutils {
		namespace exception {

extern pthread_mutex_t METHOD_MUTEX; //  used in order to store info into a file (rather then syslog)
#define GLITE_STACK_TRY(method_name) std::string METHOD = method_name ;  int LINE = __LINE__ ; try {
#define GLITE_STACK_CATCH() } catch (glite::wmsutils::exception::Exception &exc){ exc.push_back ( __FILE__ , LINE,  METHOD ); throw exc ;  } catch (std::exception &ex){ glite::wmsutils::exception::Exception exc( __FILE__ , LINE,  METHOD, 0, "Standard exception: " + std::string(ex.what()) ); throw exc; }

/**
 * The Exception base classe contains attributes into which are placed exception information and provides
 * constructor that beyond the error code take parameters specifying the source file and line number
 * (e.g. through __FILE__ and __LINE__) where the error has been generated and string messages,
 * allowing an easy way of storing the origin of the exception.
 * Moreover it provides methods for getting all the exception information and for logging them either
 * in a log file or to the syslog daemon.
 * Each of the derived types may contain its private attributes describing the actual error instance in detail.
 * Moreover each exception has an attribute representing the exception identifier that is set by the
 * class constructor and allows the identification of the original exception.
 *
 * @version 0.1
 * @date 22 July 2004
 * @author Alessandro Maraschini <alessandro.maraschini@datamat.it>
*/

class Exception : public std::exception{
   public:
	/**
	*  Constructor Update all mandatory fields
	* @param method the name of the method that raised the exception
	* @param source The source that raised the exception (could be the file path, the class Name, etc etc)
	* @param  exc the previous exception as in the stack trace */
	Exception (  const std::string& source,  const std::string& method,  Exception *exc);
	/**
	*  Constructor Update all mandatory fields
	* @param code the code representing the thrown exception
	* @param exception the name of the thrown exception
	* @param method the name of the method that raised the exception
	* @param source The source that raised the exception (could be the file path, the class Name, etc etc) */
	Exception ( const std::string& source, const std::string& method,  int code,  const std::string& exception);

	/**
	*  Constructor Update all mandatory fields
	* @param source the path of the file that raised the exception
	* @param line_number the number of the line in the file that raised the exception
	* @param method the name of the method that raised the exception
	* @param code the code representing the thrown exception
	* @param exception the name of the thrown exception */
	Exception (const std::string& source,  int line_number,   const std::string& method,  int code,  const std::string& exception);
	/**
	* Default Destructor
	*/
	virtual ~Exception() throw ();
	/**
	*  Return a string debug message containing information about Exception thrown
	* Debug message contains all the attributes stored in an exception instance such as the method, the file and the line
	* that threw the exception.
	*@return the debug message string representation
	*/
	virtual std::string dbgMessage();
	/**
	*  Return the error code
	* @return The integer representing the code of the error that generated the exception
	*/
	virtual int getCode();

	/**
	* return the Error Message associated to the Exception
	* @return The Exception string message representation
	*/
	virtual const char* what() const  throw ();

	/**
	*  Print Exception error information into a log file
	* @param logfile the file where to log exception information
	*/
	virtual void log(const std::string& logfile = "");
	/**
	*   Retrieve the Exception name
	* @return the name of the Exception thrown
	*/
	virtual std::string getExceptionName();

	/**
	*   Retrieve the Stack of the exception as a list of previous generated exceptions
	*@return the string representation of the stack trace: each line correspond to an exception message
	*/
	virtual std::string printStackTrace() ;
	/**
	*   Return the list of methods that caused the Exception
	*/
	virtual std::vector<std::string> getStackTrace() ;
	/**
	* Update stack information
	*/
	virtual void push_back (  const std::string& source, int line_number,  const std::string& method   ) ;
  protected:
		/** Empty constructor*/
		Exception();
		/**  integer error code representing the cause of the error */
		int                   error_code;
		/**  string exception message representation*/
		std::string          error_message ;
		/**  line number where the exception was raised */
		int                   line;
		/** The name of the file where the exception was raised */
		std::string          source_file;
		/** the name of the exception */
		std::string          exception_name;
		/** the name of the method where the expceiton was raised */
		std::string          method_name ;
		/** a string representation of the stacktrace */
		std::string          stack;
		/** the actual internal stacktrace representation */
		std::vector< std::string> stack_strings ;
		/** the name of the ancestor exception */
		std::string          ancestor ;
}; //End  Exception Class
}}}  // Closing namespace
#endif
