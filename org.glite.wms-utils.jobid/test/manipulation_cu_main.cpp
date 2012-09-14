#include <iostream>
#include <fstream>

#include "manipulation_cu_suite.h"

#include <cppunit/TestResult.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/XmlOutputter.h>

using namespace CppUnit;
using namespace std;

int main (int argc , char** argv)
{
	std::ofstream xml("./cppUnit_output.xml",ios::app);
	
	CppUnit::TestResult controller;
	CppUnit::TestResultCollector result;
	controller.addListener( &result );
	
	TestRunner runner;
	runner.addTest(Manipulation_test::suite());
	runner.run(controller);
	
	CppUnit::XmlOutputter outputter( &result, xml );
	CppUnit::TextOutputter outputter2( &result, std::cerr );
	outputter.write();
	outputter2.write();
	
	return result.wasSuccessful() ? 0 : 1 ;
}
