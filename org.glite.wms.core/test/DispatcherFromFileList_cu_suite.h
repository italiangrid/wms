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
  
  DispatcherFromFileList header file
  
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
#include "../src/server/DispatcherFromFileList.h"
#include "../src/server/DispatcherImpl.h"

using namespace CppUnit;
using namespace std;
using namespace glite::wms::manager::server;

class DispatcherFromFileList_test : public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( DispatcherFromFileList_test );
CPPUNIT_TEST( test_simple_run );
CPPUNIT_TEST_SUITE_END();

private:
 DispatcherFromFileList *dispatcher;

public:

 void setUp();
 void tearDown();
 void test_simple_run();
};


// Local Variables:
// mode: c++
// End:


