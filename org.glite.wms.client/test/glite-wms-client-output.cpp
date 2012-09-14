/*
 * glite-wms-client test fixture
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 */

#include "glite/wmsutils/exception/Exception.h"
#include "glite/wmsutils/jobid/JobId.h"

#include "glite-wms-client-common.h"
#include "glite/wms/wmproxyapi/wmproxy_api_utilities.h"

using namespace std ;
// using namespace glite::wms::client::services ;
using namespace glite::wms::wmproxyapiutils;
using namespace glite::wms::wmproxyapi;

// TB inserted inside COMMON:
ConfigContext *cfs=NULL;
string wmproxyEndPoint ="https://ghemon.cnaf.infn.it:7443/glite_wms_wmproxy_server";
string jobid ="https://ghemon.cnaf.infn.it:9000/8dyWbQFrf-Rb8EtIOt5aAA";
string protocol = "gsiftp";
void initContext(){
	string proxyFile   = getProxyFile();
	string trustedCerts= getTrustedCert();
	cfs= new ConfigContext (proxyFile,wmproxyEndPoint,trustedCerts);
	string msg="VALUES USED: \n- Wmproxy= " +cfs->endpoint;
	msg+="\n- Proxy= "+ cfs->proxy_file;
	msg+="\n- trustedCerts= "+cfs->trusted_cert_dir;
	msg+="\n- jobid= "+jobid;
	if (jobid==""){
		cerr << "WARNING!!! JOBID NOT INITIALIZED!!!!!" << endl ;
	}
	title(msg);
}


string printVectStr(std::vector<std::string> vect){
	string msg = "";
	for (unsigned int i=0;i<vect.size();i++){
		msg+="- "+vect[i] +"\n";
	}
	return msg;
}

string printVectPair(vector <pair<string , long> > vect){
	string msg = "";
	for (unsigned int i=0;i<vect.size();i++){
		msg+="- "+vect[i].first+" ..."+ boost::lexical_cast<string>(vect[i].second)+"\n";
	}
	return msg;
}
// TB inserted inside END



// UNIT TEST
class OutputTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( OutputTest );
	CPPUNIT_TEST( testgetSandboxDestURI );
	CPPUNIT_TEST( testgetSandboxBulkDestURI );
	CPPUNIT_TEST( testgetOutputFileList);
	CPPUNIT_TEST( testgetTransferProtocols );
	CPPUNIT_TEST_SUITE_END();
public:
	void testgetSandboxDestURI() {
		title("testgetSandboxDestURI = \n" + printVectStr(getSandboxDestURI (jobid,cfs)) );
	}
	void testgetSandboxBulkDestURI() {
		cerr << "testgetSandboxBulkDestURI NOT YET IMPLEMENTED" << endl ;
	}
	void testgetTransferProtocols() {
		cerr << "testgetTransferProtocols NOT YET IMPLEMENTED" << endl ;
	}
	void testgetOutputFileList() {
		title("testgetOutputFileList(no proto)  = \n" + printVectPair(getOutputFileList (jobid, cfs)));
		title("testgetOutputFileList(WITH proto)= \n" + printVectPair(getOutputFileList (jobid, cfs, protocol)));
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
	runner.addTest (OutputTest::suite());
	runner.run (controller);
	CppUnit::TextOutputter outputter (&result , std::cerr);
	outputter.write();
	return   result.wasSuccessful()?0:1;
}

int main(int argc,char *argv[]){
	try{
		if( (argc<2) || (strcmp(argv[1],"--help")==0)|| (strcmp(argv[1],"help")==0)){
			cout << "Usage : " << argv[0] << " <test number> [<WMPROXY endpoint>] [<JobId>]" << endl ;
			cout <<"0) Unit Test!!!"<<endl; cout <<" - - - " << endl ;
			cout <<"1) testgetSandboxDestURI"<<endl;
			cout <<"1) testgetSandboxBulkDestURI"<<endl;
			cout <<"3) testgetTransferProtocols"<<endl;
			cout <<"4) testgetOutputFileList"<<endl;
			return 0;
		}
		if (argc>2){wmproxyEndPoint=argv[2];}
		if (argc>3){jobid=argv[3];}
		OutputTest test;
		initContext();
		switch (boost::lexical_cast<int>(argv[1])){
			case 0 : unitTest(); break;
			case 1: test.testgetSandboxDestURI();     break;
			case 2: test.testgetSandboxBulkDestURI(); break;
			case 3: test.testgetTransferProtocols();  break;
			case 4: test.testgetOutputFileList();     break;
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
		cout << "\nUsage : " << argv[0] << " <test number> [<WMPROXY endpoint>] [<JobId>]" << endl ;
	}catch (...){
		cout << "\nOperation not performed. UNEXPECTED EXCEPTION RAISED"<< endl  ;
	}
	return 1;
};

