/* **************************************************************************
*  filename  : Exceptions.cpp
*  author    : Alessandro Maraschini <alessandro.maraschini@datamat.it>
*  copyright : (C) 2002 by DATAMAT
***************************************************************************/

#include "glite/wmsutils/exception/Exception.h"
#include "glite/wmsutils/exception/exception_codes.h"

namespace glite {
namespace wmsutils{
namespace exception {
using namespace std ;
pthread_mutex_t METHOD_MUTEX  ;  // This mutex is used in order to lock the file for writing log infornation

/* *********************************
* Exception Class Implementation
************************************/
//Constructor/Destructor

Exception::Exception () {
	line = 0;
};

Exception::~Exception() throw(){ }

/**
* Exception chainig
*/
void Exception::push_back (const string& source, int line_number,  const string& method){
	stack_strings.push_back (dbgMessage());
	ancestor = what();
	source_file = source;
	line = line_number;
	method_name = method;
	error_message = "";
	exception_name = "";
}

Exception::Exception( const std::string& file, int line_number,  const std::string& method,  int code, const std::string& name)
	: error_code(code), exception_name(name){
	source_file    = file;
	line           = line_number;
	method_name    = method;
};

Exception::Exception (const string& source, const string& method, int code, const string& exception)
	: error_code(code), exception_name(exception){
	source_file    = source;
	method_name    = method;
	// stack= "";
	line = 0;
};

int Exception::getCode(){
    if  (error_code != 0) return error_code;
    else
       return WMS_COMMON_BASE;
};

const char* Exception::what() const throw(){
  if (!ancestor.empty()) return ancestor.c_str();

  return error_message.c_str();
};

string Exception::getExceptionName(){
   return exception_name;
};

void Exception::log(const std::string& logfile)
{
    if (logfile == "")
	syslog (LOG_PERROR, (char *)(dbgMessage()).c_str());
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
	string stack = "" ;
	for (unsigned int i = 0 ; i < stack_strings.size() ; i++ ){
		stack+=stack_strings[i] +"\n" ;
	}
	return stack +dbgMessage();
};

vector<string> Exception::getStackTrace(){
	// make a copy of the stack
	vector<string> stack = stack_strings ;
	stack.push_back(dbgMessage()) ;
	return stack;
};

string Exception::dbgMessage(){
   string result ;
   //Adding exception Name
   result = exception_name;

   //Adding error msg
   if (error_message!="") result +=": " + string(what());

   if (result != "") result+="\n";

   //Adding  Source
   result +="\tat " + method_name  +"[" +source_file;

   //Adding line number
   if (line!=0){
      char buffer [1024] ;
      sprintf (buffer, "%i" ,  line) ;
      result += ":" + string ( buffer );
   }
   result +="]" ;
   return result;
}

}}}  // Closing namespace

