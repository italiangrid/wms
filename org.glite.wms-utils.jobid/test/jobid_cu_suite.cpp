#include "jobid_cu_suite.h"


using namespace CppUnit;
using namespace std;
using namespace glite::wmsutils::jobid;


void Jobid_test::setUp()
{}

void Jobid_test::tearDown()
{}

void Jobid_test::Constructor_case()
{
	//EMPTY CONSTRUCTOR
	JobId empty;
	
	CPPUNIT_ASSERT(empty.isSet()==false);
	
	//create a string with cjobid
	string bkserver="grid012g.cnaf.infn.it";
	edg_wlc_JobId jobid;
	int bkport=6000;
	int ok=edg_wlc_JobIdCreate(bkserver.c_str(), bkport, &jobid);
	CPPUNIT_ASSERT(ok == 0);
   if (ok==0) 
	{ 
		string jobstring=edg_wlc_JobIdUnparse(jobid);
		
    	//STRING CONSTRUCTOR
    	JobId stringCons(jobstring);
    	
    	//EDG_WLC CONSTRUCTOR
    	JobId edg_wlc_Cons(jobid);
    	
    	//test copy constructor 
    	JobId copycon(stringCons);
    	
    	CPPUNIT_ASSERT(stringCons.isSet());
		CPPUNIT_ASSERT(edg_wlc_Cons.isSet());
		CPPUNIT_ASSERT(copycon.isSet());
		
		//test =
		JobId testequal;
		testequal=stringCons;
		CPPUNIT_ASSERT(testequal.isSet());
		
		JobId testoperator;
		testoperator=jobid;
		CPPUNIT_ASSERT(testoperator.isSet());
			
		edg_wlc_JobId testget = edg_wlc_Cons.getId();
		char *server;
		unsigned int port;
		edg_wlc_JobIdGetServerParts(testget, &server, &port);
		string serverstring = server;
		CPPUNIT_ASSERT(port==bkport);
		CPPUNIT_ASSERT(serverstring==bkserver);
	} 
	
	CPPUNIT_ASSERT_THROW( JobId stringwrong("grid012"), WrongIdException);
}

void Jobid_test::Clear_case()
{
	JobId *element;
	string jobstring="https://grid012g.cnaf.infn.it:6000/qaKyEoV3G144rmoyXeW6QA";
	CPPUNIT_ASSERT_NO_THROW(element= new JobId(jobstring));

	CPPUNIT_ASSERT(element->isSet());
	element->clear();
	CPPUNIT_ASSERT(element->isSet()==false);
	delete element;
}

void Jobid_test::SetandGet_case()
{
	JobId element;
	string lbserver="grid012g.cnaf.infn.it";
	int port=6000;
	string unique ="qaKyEoV3G144rmoyXeW6QA";
	element.setJobId(lbserver, port, unique);
	
	string server=element.getServer();
	lbserver=lbserver+":6000";
	
	CPPUNIT_ASSERT(server==lbserver);
	string lonely=element.getUnique();
	CPPUNIT_ASSERT(lonely==unique);
	
	string descr=element.toString();
	cout << "!!! BEGIN TEST toString() METHOD!!!"<< endl;
	cout << descr <<endl;
	cout << "!!! END TEST toString() METHOD!!!"<< endl;
	
	JobId wrongelement;
	CPPUNIT_ASSERT_THROW(string server=wrongelement.getServer(), EmptyIdException);
	CPPUNIT_ASSERT_THROW(string server=wrongelement.getUnique(), EmptyIdException);
	
}
