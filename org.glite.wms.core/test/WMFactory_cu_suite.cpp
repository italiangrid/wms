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
  
  WMFactory suite
  
  Authors: Paolo Andreetto <paolo.andreetto@pd.infn.it>
  
  Version info: $Id$
  Release: $Name$
  
 */

#include "WMFactory_cu_suite.h"

using namespace CppUnit;

/* 
    **********************************************************

      Fake WM methods

    ********************************************************** 
*/

void 
WMTest::submit(classad::ClassAd const* request_ad)
{
}

void 
WMTest::cancel(glite::wmsutils::jobid::JobId const& request_id)
{
}

WMImpl* 
create_test_wm()
{
  return new WMTest();
}


/* 
    **********************************************************

      Unit suite methods

    ********************************************************** 
*/

void
WMFactory_test::setUp()
{
  factory = WMFactory::instance();
}

void
WMFactory_test::tearDown()
{
}

void
WMFactory_test::test_create_wm()
{
  string wm_id("test");

  CPPUNIT_ASSERT( factory->register_wm(wm_id,create_test_wm) );

  WMImpl* result = NULL;

  try
  {
    result = factory->create_wm(wm_id);
  }
  catch(NoCreateWMException exception)
  {
    cerr << exception.what() << endl;
  }

  CPPUNIT_ASSERT( result != NULL );
  
  CPPUNIT_ASSERT( factory->unregister_wm(wm_id) );
}

void
WMFactory_test::test_register_wm()
{
  string wm_id("test");

  CPPUNIT_ASSERT( factory->register_wm(wm_id,create_test_wm) );

  factory->unregister_wm(wm_id);

}

void
WMFactory_test::test_unregister_wm()
{
  string wm_id("test");

  if (factory->register_wm(wm_id,create_test_wm)) {

    CPPUNIT_ASSERT( factory->unregister_wm(wm_id) );

  }
}

void
WMFactory_test::test_create_unexisting_wm()
{
  string wm_id("test");

  WMImpl* result = factory->create_wm(wm_id);
}

void
WMFactory_test::test_unregister_unexisting_wm()
{
  string wm_id("test");

  CPPUNIT_ASSERT( !factory->unregister_wm(wm_id) );

}

void
WMFactory_test::test_register_existing_wm()
{
  string wm_id("test");

  if (factory->register_wm(wm_id,create_test_wm)) {

    CPPUNIT_ASSERT( !factory->register_wm(wm_id,create_test_wm) );

  }  
}
