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
  
  WMReal header file
  
  Authors: Paolo Andreetto <paolo.andreetto@pd.infn.it>
  
  Version info: $Id$
  Release: $Name$
  
 */

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
#include "../src/common/WMImpl.h"
#include "../src/server/WMReal.h"
#include "classad_distribution.h"

using namespace CppUnit;
using namespace std;
using namespace glite::wms::manager::common;
using namespace glite::wms::manager::server;


class WMReal_test : public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( WMReal_test );
CPPUNIT_TEST( test_submit );
CPPUNIT_TEST( test_cancel );
CPPUNIT_TEST_SUITE_END();

private:
 WMImpl *wm_real; 
 
public:

 void setUp();
 void tearDown();
 void test_submit();
 void test_cancel();
};




