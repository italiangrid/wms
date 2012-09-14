#include<iostream>
#include<string>
#include<pthread.h>
                                   
#include <cppunit/extensions/HelperMacros.h>

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"

class Manipulation_test : public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(Manipulation_test);
	
  	CPPUNIT_TEST(to_fromfile_case);
  	
  CPPUNIT_TEST_SUITE_END();


public:

	void setUp();
	void tearDown();

  	void to_fromfile_case();

};



