/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef URL_CU_SUTE_H
#define URL_CU_SUTE_H

#include "jobadapter/url/URL.h"
#include <cppunit/extensions/HelperMacros.h>
#include <string>

class Url_test: public CppUnit::TestFixture {
   
static std::string ok_no_port_no_path;
static std::string ok_with_port_with_path;
static std::string ok_no_port_with_path;
static std::string ok_with_port_no_path;

static std::string ko_without_protocol;
static std::string ko_without_path;
static std::string ko_with_alfanum_port;

glite::wms::helper::jobadapter::url::URL url_ok_no_port_no_path;
glite::wms::helper::jobadapter::url::URL url_ok_with_port_with_path;
glite::wms::helper::jobadapter::url::URL url_ok_no_port_with_path;
glite::wms::helper::jobadapter::url::URL url_ok_with_port_no_path;

glite::wms::helper::jobadapter::url::URL url_ko_without_protocol;
glite::wms::helper::jobadapter::url::URL url_ko_without_path;
glite::wms::helper::jobadapter::url::URL url_ko_with_alfanum_port;

glite::wms::helper::jobadapter::url::URL url_empty;
glite::wms::helper::jobadapter::url::URL url_void;
   
    CPPUNIT_TEST_SUITE(Url_test);
        CPPUNIT_TEST(is_empty_case);
        CPPUNIT_TEST(constructors_case);
        CPPUNIT_TEST(protocol_case);        
        CPPUNIT_TEST(host_case);
        CPPUNIT_TEST(port_case);
        CPPUNIT_TEST(path_case);
        CPPUNIT_TEST(as_string_case);
    CPPUNIT_TEST_SUITE_END();   

public:    
    void setUp();
    void tearDown();
    void constructors_case();    
    void is_empty_case();
    void protocol_case();        
    void host_case();
    void port_case();
    void path_case();
    void as_string_case();
    
};


#endif // URL_CU_SUTE_H
