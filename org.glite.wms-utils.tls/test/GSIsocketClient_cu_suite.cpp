#include "GSIsocketClient_cu_suite.h"

using namespace CppUnit;
using namespace std;
using namespace glite::wmsutils::tls::socket_pp;


void GSIsocketClient_test::setUp()
{
	srvListeningPort = 55557;
	return;
}

void GSIsocketClient_test::tearDown()
{

}

void GSIsocketClient_test::OpenClose_case()
{
  GSISocketServer srv(srvListeningPort);
  CPPUNIT_ASSERT(srv.Open());
  GSISocketClient c("localhost",srvListeningPort);
  c.DelegateCredentials(true);
  CPPUNIT_ASSERT_THROW(c.Open(), glite::wmsutils::tls::socket_pp::AuthenticationException);
  CPPUNIT_ASSERT(c.Close());
  srv.Close();
}

void GSIsocketClient_test::clientwork_case()
{
	GSISocketServer srv(srvListeningPort);
  CPPUNIT_ASSERT(srv.Open());

  GSISocketClient c("localhost",srvListeningPort);
	
  
  c.DelegateCredentials(false);
  c.ServerContact("notlocalhost");
  CPPUNIT_ASSERT_THROW(c.Open(), AuthenticationException);
	CPPUNIT_ASSERT(c.Close());
}

void GSIsocketClient_test::OpenFailure_case()
{
	 	GSISocketClient c("NotAValidHost", srvListeningPort);
    try{
      CPPUNIT_ASSERT_THROW(c.Open(), AuthenticationException); //This Throws
    }catch (const IOException& e) {cout << e.what() << std::endl;}
}

void GSIsocketClient_test::SendReceive_case()
{
  	GSISocketServer* srv= new GSISocketServer(srvListeningPort);
  	CPPUNIT_ASSERT(srv->Open());
 
 	pthread_create(&thr,NULL,&thrLoop,srv);
	GSISocketClient c("localhost",srvListeningPort);
  	try{
      	CPPUNIT_ASSERT_THROW(c.Open(), AuthenticationException); //This Throws
  	}catch (const IOException& e) {cout << e.what() << std::endl;}

	try{
		string msg = "Test 1";
		CPPUNIT_ASSERT(c.Send(msg));
		string answer;
		CPPUNIT_ASSERT(c.Receive(answer));
		CPPUNIT_ASSERT_MESSAGE("Failure testing 'string' case", 
			answer == "Test 1");
	
		int msgInt = INT_MAX;
		CPPUNIT_ASSERT(c.Send(msgInt));
		int answerInt;
		CPPUNIT_ASSERT(c.Receive(answerInt));
		CPPUNIT_ASSERT_MESSAGE("Failure testing 'int' case", 
			answerInt == INT_MAX);
	}catch (const IOException& e) {cout << e.what() << std::endl;}
	
	CPPUNIT_ASSERT(c.Close());
		pthread_join (thr,NULL);

	delete srv;
}

void* thrLoop(void* param)
{
	try{
	GSISocketAgent* a;
	a = ((GSISocketServer *)param)->Listen();

	std::string message;
	msgSendReceive(message,a);
	int messageInt;
	msgSendReceive(messageInt,a);

	((GSISocketServer *)param)->KillAgent(a);
	((GSISocketServer *)param)->Close();
	}
	catch( std::exception& e) {
		std::cout << e.what() << std::endl;
	}	
	return NULL;
}

void GSIsocketClient_test::Agent_case()
{
	GSISocketServer srv(srvListeningPort);
  	CPPUNIT_ASSERT(srv.Open());
	GSISocketAgent* agent =new GSISocketAgent;
  	CPPUNIT_ASSERT_THROW(srv.AuthenticateAgent(agent), AuthenticationException);
	delete agent;
}
