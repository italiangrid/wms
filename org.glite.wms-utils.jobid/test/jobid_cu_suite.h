#include<iostream>
#include<string>
#include<pthread.h>
                                   
#include <cppunit/extensions/HelperMacros.h>

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/cjobid.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

class Jobid_test : public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(Jobid_test);
	
  	CPPUNIT_TEST(Constructor_case);
  	CPPUNIT_TEST(Clear_case);
  	CPPUNIT_TEST(SetandGet_case);
  	
  CPPUNIT_TEST_SUITE_END();


public:

	void setUp();
	void tearDown();

  	void Constructor_case();
  	void Clear_case();
  	void SetandGet_case();

};



