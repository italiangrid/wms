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

#include "job_wrapper_cu_suite.h"
#include "jobadapter/url/URL.h"

#include <string>
#include <vector>
#include <iostream>


using namespace std;
using namespace glite::wms::helper::jobadapter::url;
using namespace glite::wms::helper::jobadapter::jobwrapper;


void JobWrapper_test::setUp(){
    cout<<"setUp JobWrapper"<<endl;    
    
}

void JobWrapper_test::tearDown(){
    cout<<"TearDownAd JobWrapper"<<endl;
}

void JobWrapper_test::get_set_methods_case(){
    
    string aJobWrapperFileName("job.sh"); 
    JobWrapper jw(aJobWrapperFileName);  
    
    string standard_input_file_name = "standardInputFileNameJobWrapperTest.in"; 
    string standard_output_file_name = "standardOutputFileNameJobWrapperTest.out"; 
    string standard_error_file_name = "standardErrorFileNameJobWrapperTest.out"; 
    URL input_sandbox_url("http://www.cnaf.infn.it:8080/dir1/dir2/inputFile"); 
    URL output_sandbox_url("http://www.cnaf.infn.it:8080/dir1/dir2/outputFile"); 
    vector<string> input_sandbox_files;
    vector<string> output_sandbox_files;
    input_sandbox_files.push_back("input_sf1");
    output_sandbox_files.push_back("output_sf1");
    string aJobId("https://edt003.cnaf.infn.it:9000/131.154.99.82/092250216745692?edt003.cnaf.infn.it:7771");
    string some_arguments("-i \"\\\"Fabrizio Pacini\" *.txt");
    vector<string> env;
    env.push_back("ENV1=env1");
    env.push_back("ENV2=env2");
    
    string a_brokerInfo(".BrokerInfo");
    
    CPPUNIT_ASSERT_NO_THROW(jw.standard_input(standard_input_file_name));
    CPPUNIT_ASSERT_NO_THROW(jw.standard_output(standard_output_file_name));
    CPPUNIT_ASSERT_NO_THROW(jw.standard_error(standard_error_file_name));
    CPPUNIT_ASSERT_NO_THROW(jw.input_sandbox(input_sandbox_url,input_sandbox_files));
    CPPUNIT_ASSERT_NO_THROW(jw.output_sandbox(input_sandbox_url,output_sandbox_files));
    CPPUNIT_ASSERT_NO_THROW(jw.create_subdir());
    CPPUNIT_ASSERT_NO_THROW(jw.job_Id(aJobId));    

    CPPUNIT_ASSERT_NO_THROW(jw.arguments(some_arguments));
    CPPUNIT_ASSERT_NO_THROW(jw.job_id_to_filename("job_id_to_filename"));           
    CPPUNIT_ASSERT_NO_THROW(jw.maradonaprotocol("protocol","filename"));
    CPPUNIT_ASSERT_NO_THROW(jw.gatekeeper_hostname("hostname"));
    CPPUNIT_ASSERT_NO_THROW(jw.vo("vo"));
    
    CPPUNIT_ASSERT_NO_THROW(jw.environment(env));
    CPPUNIT_ASSERT_NO_THROW(jw.nodes(3));

 
}
