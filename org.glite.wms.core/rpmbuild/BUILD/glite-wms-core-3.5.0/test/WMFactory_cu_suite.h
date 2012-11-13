/*
  Copyright (c) 2004 on behalf of the EU EGEE Project:
  The European Organization for Nuclear Research (CERN),
  Istituto Nazionale di Fisica Nucleare (INFN), Italy
  Datamat Spa, Italy
  Centre National de la Recherche Scientifique (CNRS), France
  CS Systeme d'Information (CSSI), France
  Royal Institute of Technology, Center for Parallel Computers (KTH-PDC), Sweden
  Universiteit van Amsterdam (UvA), Netherlands
  University of Helsinki (UH.HIP), Finland
  University of Bergen (UiB), Norway
  Council for the Central Laboratory of the Research Councils (CCLRC), United Kingdom
  
  WMFactory header file
  
  Authors: Paolo Andreetto <paolo.andreetto@pd.infn.it>
  
  Version info: $Id$
  Release: $Name$
  
 */

//#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <iostream>
#include "../src/common/WMFactory.h"
#include "../src/common/WMImpl.h"

using namespace CppUnit;
using namespace std;
using namespace glite::wms::manager::common;


class WMTest: public WMImpl
{
public:
  void submit(classad::ClassAd const* request_ad);
  void cancel(glite::wmsutils::jobid::JobId const& request_id);
};






class WMFactory_test : public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( WMFactory_test );
CPPUNIT_TEST( test_create_wm );
CPPUNIT_TEST( test_register_wm );
CPPUNIT_TEST( test_unregister_wm );
CPPUNIT_TEST_EXCEPTION( test_create_unexisting_wm, NoCreateWMException );
CPPUNIT_TEST( test_unregister_unexisting_wm );
CPPUNIT_TEST( test_register_existing_wm );
CPPUNIT_TEST_SUITE_END();

private:
 WMFactory *factory;

public:

 void setUp();
 void tearDown();
 void test_create_wm();
 void test_register_wm();
 void test_unregister_wm();
 void test_create_unexisting_wm();
 void test_unregister_unexisting_wm();
 void test_register_existing_wm();
};




