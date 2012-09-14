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

#include <iostream>
#include <fstream>

#include <cppunit/TestResult.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/XmlOutputter.h>

#include "url_cu_suite.h"
#include "job_wrapper_cu_suite.h"

int main (int argc, char* argv[]) {
    //TODO
   // std::ofstream xml("./cppUnit_output.xml", ios::app);
    
    CppUnit::TestResult controller;
    CppUnit::TestResultCollector result;
    controller.addListener( &result );
    
    CppUnit::TestRunner runner;
    runner.addTest(Url_test::suite()); 
    runner.addTest(JobWrapper_test::suite()); 
//    runner.addTest();  a new test
    runner.run(controller);
 
  // to do: da sistemare output in xml
   // CppUnit::XmlOutputter xmlOutputter( &result, xml );
  CppUnit::TextOutputter textOutputter( &result, std::cerr ); 
    textOutputter.write();
    //xmlOutputter.write();
    return result.wasSuccessful() ? 0 : 1 ;
}
