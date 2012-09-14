#include<iostream>
#include<string>
#include<pthread.h>
                                   
#include <cppunit/extensions/HelperMacros.h>


class Exception_test : public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(Exception_test);
    CPPUNIT_TEST(constructor_case);
    CPPUNIT_TEST(tostring_case);
    CPPUNIT_TEST(stackTrace_case);	
  CPPUNIT_TEST_SUITE_END();


public:

	void setUp();
	void tearDown();
  
    void constructor_case();
    void tostring_case();
    void stackTrace_case();

};



