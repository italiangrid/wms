/*
 * glite-wms-client test fixture
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 */

#include "glite/wmsutils/exception/Exception.h"
#include "glite/wmsutils/jobid/JobId.h"

#include "glite-wms-client-common.h"
#include "services/listener.h"

using namespace std ;
using namespace glite::wms::client::services ;

const string jobid ="https://glite-wms-client:1234/AoughkatbhaA";
// UNIT TEST
class ClientTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( ClientTest );
	CPPUNIT_TEST( testListener );
	CPPUNIT_TEST( testShadow);
	CPPUNIT_TEST( testUtilities );
	CPPUNIT_TEST_SUITE_END();
public:
	void testListener() {
		Shadow *jobShadow = new Shadow();
		jobShadow->setJobId(jobid);
		jobShadow->setPrefix("../../stage/bin");
		// Insert jdl attributes port/pipe/host inside shadow(if present)

		// Launch console
		if (jobShadow->isLocalConsole()){
			title("Running console shadow");
			jobShadow->console();
		}
		// Display some messages
		if (jobShadow->isLocalConsole()){
			// console-shadow running
			// console-listener running
			cout << "Interactive Session Listener successfully launched"<<"\n";
			jobShadow->setGoodbyeMessage(true);
			cout <<"With the following parameters:"<<"\n";
			cout << "- Host: " << jobShadow->getHost() <<"\n";
			cout << "- Port: " << jobShadow->getPort() <<"\n";
			cout << "- Shadow process Id: " << jobShadow->getPid() << "\n";
			cout << "- Input Stream  location: " << jobShadow->getPipeIn() <<"\n";
			cout << "- Output Stream  location: " << jobShadow->getPipeOut() <<"\n";
		}else {
			// console-shadow NOT running
			cout << "Remote Shadow console set: " << jobShadow->getHost() <<"\n";
		}
		// Run Listener
		Listener listener(jobShadow);
		listener.run();
	}
	void testUtilities() {
		cout << "testUtilities TEST NOT YET IMPLEMENTED" << endl ;
	}
	void testShadow() {

	}
};
int unitTest(){
	/*  CPP UNIT PART  */
	// Debug mode control:
	setWCDM(false);
	CppUnit::TestResult controller ;
	CppUnit::TestResultCollector result ;
	controller.addListener(&result);
	CppUnit::TestRunner runner ;
	/*   ACTUAL CPP UNIT NAME*/
	runner.addTest (ClientTest::suite());

	runner.run (controller);
	CppUnit::TextOutputter outputter (&result , std::cerr);
	outputter.write();
	return   result.wasSuccessful()?0:1;
}

int main(int argc,char *argv[]){
	try{
		if( (argc<2) || (strcmp(argv[1],"--help")==0)|| (strcmp(argv[1],"help")==0)){
			cout << "Usage : " << argv[0] << " <test number> [<JDL file>]" << endl ;
			cout <<"0) Unit Test!!!"<<endl; cout <<" - - - " << endl ;
			cout <<"1) testListener"<<endl;
			cout <<"2) testUtilities"<<endl;
			cout <<"3) testShadow"<<endl;
			return 0;
		}
		if (argc>2){setJdlFile(argv[2]);}
		ClientTest test;
		switch (boost::lexical_cast<int>(argv[1])){
			case 0 : unitTest(); break;
			case 1: test.testListener(); break;
			case 2: test.testUtilities(); break;
			case 3: test.testShadow(); break;
			default: break;
		}
		return 0;
	}catch  (glite::wmsutils::exception::Exception &exc){
		cout << "\nOperation not performed\n" ;
		cout <<"Stack...  " << exc.printStackTrace()<<  endl ;
		cout <<"Exc Name  " << exc.getExceptionName()<< endl;
		cout <<"What      " << exc.what()<< endl;
	}catch  (std::exception &exc){
		cout << "\nOperation not performed. std::exception found:\n" ;
		cout <<  exc.what() << endl ;
		cout << "\nUsage : " << argv[0] << " <test number> [<JDL file>]" << endl ;
	}
};

