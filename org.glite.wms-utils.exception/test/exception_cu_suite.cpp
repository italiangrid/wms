#include "exception_cu_suite.h"
#include "glite/wmsutils/exception/Exception.h"
#include <string>
#include <vector>

using namespace CppUnit;
using namespace std;
using namespace glite::wmsutils::exception;

void Exception_test::setUp()
{}

void Exception_test::tearDown()
{}


void Exception_test::constructor_case()
{
  
  //constructor class name, line number, method name, code, exception name
  glite::wmsutils::exception::Exception exc_5("TEST_Class", 3, "test_method", 1, "exception_test");
  
  //constructor class name, method name, code, exception name
  glite::wmsutils::exception::Exception exc_4("TEST_Class", "test_method", 1, "exception_test");
  
  CPPUNIT_ASSERT(exc_5.getExceptionName() == "exception_test");
  CPPUNIT_ASSERT(exc_5.getCode() == 1);
}

void Exception_test::tostring_case()
{
   cout<<"TEST TO STRING METHODS"<<endl;
   glite::wmsutils::exception::Exception exc_5("TEST_Class", 3, "test_method", 1, "exception_test");
   cout << exc_5.dbgMessage() << endl;
   cout << exc_5.printStackTrace() << endl;
   exc_5.log("exception_file");

   cout<<"END TEST TO STRING"<<endl;
}

void Exception_test::stackTrace_case()
{
   glite::wmsutils::exception::Exception exc_5("TEST_Class", 3, "test_method", 1, "exception_test");
   
   exc_5.push_back("Second_Class", 5, "second_method");
   
   vector<string> msgvec = exc_5.getStackTrace();
   
   for (int i=0; i<msgvec.size(); i++)
   {
       cout << msgvec[i] << endl;
   }
   
}


