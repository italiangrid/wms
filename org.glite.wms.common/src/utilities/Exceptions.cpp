/* **************************************************************************
*  filename  : Exceptions.cpp
*  author    : Alessandro Maraschini <alessandro.maraschini@datamat.it>
*  copyright : (C) 2002 by DATAMAT
***************************************************************************/

#include "Exceptions.h"
#include "exception_codes.h"
#include "utils.h"

namespace glite {
namespace wms {
namespace common {
namespace utilities {

using namespace std ;
using namespace glite::wms::common::utilities ;
pthread_mutex_t METHOD_MUTEX  ;  // This mutex is used in order to lock the file for writing log infornation
/* *********************************
* Exception Class Implementation
************************************/
//Constructor/Destructor

Exception::Exception () {
   //exc = NULL ;
   stack= "";
   line = 0;
} ;

Exception::~Exception() throw(){ }
/**
* Exception chainig
*/
Exception::Exception (  const string& source,
		  const string& method,
                  Exception *e){
    source_file = source ;
    method_name    = method;
    error_message = "";
    stack= e->printStackTrace();
    ancestor = e->what() ;
    line = 0;
    error_code= 0;
};
Exception::Exception( const std::string& file,
		     int line_number,
		     const std::string& method,
		     int code,
		     const std::string& name)
  : error_code(code), exception_name(name){
    source_file    = file;
    line           = line_number;
    method_name    = method;
    stack= "";
};
Exception::Exception (const string& source,
                const string& method,
                int code,
                const string& exception)
	        : error_code(code), exception_name(exception){
    source_file    = source;
    method_name    = method;
    stack= "";
    line = 0;
};
int Exception::getCode(){
    if  (error_code != 0)
       return error_code ;
    else
       return WMS_COMMON_BASE;
};
const char* Exception::what() const throw(){
  if (!ancestor.empty() )
       return ancestor.c_str() ;
  if  ( error_message != "")
       return error_message.c_str() ;
  else
       return "" ;

};
string Exception::getExceptionName(){
   if  (exception_name!= "")
     return exception_name;
   else
     return "" ;
};
void Exception::log(const std::string& logfile)
{
    if (  logfile == "")
	syslog (  LOG_PERROR,   (char *)  (dbgMessage()).c_str()  );
    else{
	pthread_mutex_lock( &METHOD_MUTEX);                   //   LOCK
	//TBD : test if file exist-->>Create HEADER   ??
	ofstream fout ((char *)   logfile.c_str()  , ios::app );  //Open the file for writing (if it doesn't exist then it will be created)
	fout << "\n" << dbgMessage() ;  //write (append) the message
	fout.close();  //close the file
	pthread_mutex_unlock( &METHOD_MUTEX);               //   UNLOCK
    }
};
string Exception::printStackTrace(){
return stack + "\n" + dbgMessage();
};
string Exception::dbgMessage(){
   // cout << "\nDBM callded for :" << this;
   string result ;
   //Adding exception Name
   if ( exception_name!="")
        result = exception_name ;
   //Adding error msg
   if (error_message!="")
        result +=": " + string(what());
   if (result != "")
      result+="\n";
   //Adding  Source
   result +="         at " + source_file ;
   //Adding line number
   if (line!=0)
        result += " Line: " + inTo(line);
   result +=" " ;
   //Adding Method Name
   if (method_name != "")
      result += "Method: " +method_name ;
   return result;
}
FatalErrorException::FatalErrorException(const std::string& file,
					 int line,
					 const std::string& method)
    : Exception(file, line, method, WMS_FATAL_ERROR, "FatalErrorException")
{
    error_message = "Fatal Error found: system is unable to continue";
}
StdException::StdException(const std::string& file,
			   int line,
			   const std::string& method,
			   int code,
			   const std::string& exception_name)
    : Exception(file, line, method, code, "StdException")
{
    error_message = "std::exception Fatal Error thrown: " + exception_name;
}
ThreadException::ThreadException(const std::string& file,
				 int line,
				 const std::string& method,
				 int code,
				 int jobNumber)
    : Exception(file, line, method, code, "ThreadException")
{

    switch (code){
    case THREAD_INIT:
	error_message = "pthread_attr_init";
	break;
    case THREAD_DETACH :
	error_message = "pthread_attr_setdetachstate";
	break;
    case THREAD_SSL :
	error_message = "SSL multi thread procedure";
	break;
    case THREAD_CREATE :
	error_message = "pthread_create";
	break;
    default:            //THREAD_JOIN
	error_message = "pthread_join";
	break;
    }
    error_message += "pthread Fatal Error thrown for: " + error_message ;
}

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace
