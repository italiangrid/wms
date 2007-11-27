// Copyright (c) Members of the EGEE Collaboration. 2004. 
// See http://www.eu-egee.org/partners/ for details on the copyright
// holders.  
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//
//     http://www.apache.org/licenses/LICENSE-2.0 
//
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

// $Id$

#include "glite/wms/common/utilities/jobid_utils.h"
#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/extensions/HelperMacros.h>

namespace common = glite::wms::common;

class JobId_to_filename_test: public CppUnit::TestFixture
{

CPPUNIT_TEST_SUITE(JobId_to_filename_test);
CPPUNIT_TEST(test_prefix_default);
CPPUNIT_TEST(test_prefix_2);
CPPUNIT_TEST(test_prefix_1);
CPPUNIT_TEST(test_prefix_0);
CPPUNIT_TEST(test_prefix_3);
CPPUNIT_TEST(test_prefix_22);
CPPUNIT_TEST(test_prefix_23);
CPPUNIT_TEST(test_prefix_negative);
CPPUNIT_TEST_SUITE_END();

  glite::jobid::JobId m_id;

public:
  void setUp()
  {
    m_id = glite::jobid::JobId("https://devel12.cnaf.infn.it:9000/zSPXlzpKRvfslWPBnGJxdg");
  }

  void test_prefix_default()
  {
    CPPUNIT_ASSERT_EQUAL(
      std::string("zS/zSPXlzpKRvfslWPBnGJxdg"),
      common::to_filename(m_id)
    );
  }
  void test_prefix_2()
  {
    CPPUNIT_ASSERT_EQUAL(
      std::string("zS/zSPXlzpKRvfslWPBnGJxdg"),
      common::to_filename(m_id, 2)
    );
  }
  void test_prefix_1()
  {
    CPPUNIT_ASSERT_EQUAL(
      std::string("z/zSPXlzpKRvfslWPBnGJxdg"),
      common::to_filename(m_id, 1)
    );
  }
  void test_prefix_0()
  {
    CPPUNIT_ASSERT_EQUAL(
      std::string("zSPXlzpKRvfslWPBnGJxdg"),
      common::to_filename(m_id, 0)
    );
  }
  void test_prefix_3()
  {
    CPPUNIT_ASSERT_EQUAL(
      std::string("zSP/zSPXlzpKRvfslWPBnGJxdg"),
      common::to_filename(m_id, 3)
    );
  }
  void test_prefix_22()
  {
    CPPUNIT_ASSERT_EQUAL(
      std::string("zSPXlzpKRvfslWPBnGJxdg/zSPXlzpKRvfslWPBnGJxdg"),
      common::to_filename(m_id, 22)
    );
  }
  void test_prefix_23()
  {
    CPPUNIT_ASSERT_EQUAL(
      std::string("zSPXlzpKRvfslWPBnGJxdg/zSPXlzpKRvfslWPBnGJxdg"),
      common::to_filename(m_id, 23)
    );
  }
  void test_prefix_negative()
  {
    CPPUNIT_ASSERT_EQUAL(
      std::string("zSPXlzpKRvfslWPBnGJxdg"),
      common::to_filename(m_id, -1)
    );
  }
};

int main()
{
  CppUnit::TestResult controller;
  CppUnit::TestResultCollector result;
  controller.addListener(&result);
  CppUnit::TestRunner runner;
  runner.addTest(JobId_to_filename_test::suite());
  runner.run(controller);
  CppUnit::TextOutputter outputter(&result, std::cerr);
  outputter.write();
  return result.wasSuccessful() ? EXIT_SUCCESS : EXIT_FAILURE;
}
