#include "socketClient_cu_suite.h"

using namespace CppUnit;
using namespace std;
using namespace glite::wmsutils::tls::socket_pp;


void socketClient_test::setUp()
{
	srvListeningPort = 55557;
	return;
}

void socketClient_test::tearDown()
{

}

void socketClient_test::OpenClose_case()
{
  srv = new SocketServer(srvListeningPort);
  CPPUNIT_ASSERT(srv->Open());
  srv->Close();
  CPPUNIT_ASSERT(srv->IsConnectionPending()==false);
  delete srv;
}

void socketClient_test::SendReceive_case()
{
	srv = new SocketServer(srvListeningPort);
	srv->Open();
	pthread_create(&thr,NULL,&thrLoop,srv);
	SocketClient c("localhost",srvListeningPort);
	CPPUNIT_ASSERT(c.Open());
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
  	
	long msgLong = LONG_MAX;
	CPPUNIT_ASSERT(c.Send(msgLong));
	long answerLong;
	CPPUNIT_ASSERT(c.Receive(answerLong));
	CPPUNIT_ASSERT_MESSAGE("Failure testing 'long' case", 
			answerLong == LONG_MAX);

	CPPUNIT_ASSERT(c.Close());
	pthread_join (thr,NULL);

	return;
}

void socketClient_test::OpenFailure_case()
{
	CPPUNIT_ASSERT_NO_THROW(
		c = new SocketClient("NotAValidHost", srvListeningPort)
							);
		CPPUNIT_ASSERT_THROW(
				c->Open(),
				IOException); //This Throws
}


void* thrLoop(void* param)
{
	try{
	SocketAgent* a;
	a = ((SocketServer *)param)->Listen();
	a -> SetTimeout(5);
	std::string message;
	msgSendReceive(message,a);
	int messageInt;
	msgSendReceive(messageInt,a);
	long messageLong;
	msgSendReceive(messageLong,a);
	((SocketServer *)param)->KillAgent(a);
	((SocketServer *)param)->Close();
	}
	catch( std::exception& e) {
		std::cout << e.what() << std::endl;
	}	
	return NULL;
}

void socketClient_test::SetTimeout_case()
{
	srv = new SocketServer(srvListeningPort);
	srv->Open();
	//pthread_create(&thr,NULL,&thrLoop,srv);
	SocketClient c("localhost",srvListeningPort);
  CPPUNIT_ASSERT(c.Open());
  CPPUNIT_ASSERT(c.SetTimeout(5));
  CPPUNIT_ASSERT(c.Close());
//	pthread_join (thr,NULL);
  delete srv;
}

void socketClient_test::Agent_case()
{
  	srv = new SocketServer(srvListeningPort);
	srv->Open();
	
	//pthread_create(&thr,NULL,&thrLoop,srv);
	SocketClient c("localhost",srvListeningPort);
  	CPPUNIT_ASSERT(c.Open());
  	SocketAgent *agent = c.getAgent();
  	CPPUNIT_ASSERT(agent->SetRcvTimeout(5));
  	CPPUNIT_ASSERT(agent->SetSndTimeout(5));
  	
  	int i=INT_MAX;
  	srv->Close();
  	CPPUNIT_ASSERT_THROW(agent->Send(i), IOException);
  	
  	int port = agent -> PeerPort();
  	cout << "PORTA" << port<< endl;

	string hname = agent -> PeerName();
  	cout << "NAME" << hname << endl;
  	CPPUNIT_ASSERT(hname == "localhost.localdomain");
	
	string addr = agent -> PeerAddr();
  	cout << "ADDR" << addr<< endl;
  	
	  	
  	CPPUNIT_ASSERT(c.Close());
	//pthread_join (thr,NULL);
  	
  	delete srv;
}

void socketClient_test::OpenFailure2_case()
{
	SocketServer srv(srvListeningPort);
	srv.Open();
	SocketServer* srv2 = new SocketServer(srvListeningPort);
	CPPUNIT_ASSERT_THROW(srv2->Open(),IOException);
	srv.Close();
	delete srv2;
}

void socketClient_test::MultipleClient_case()
{
	srv = new SocketServer(srvListeningPort);
	srv->Open();
	
	SocketClient c1("localhost",srvListeningPort);
	CPPUNIT_ASSERT(c1.Open());
	
	SocketClient c2("localhost",srvListeningPort);
	CPPUNIT_ASSERT(c2.Open());
	
	pthread_create(&thr,NULL,&thrLoop,srv);
	pthread_create(&thr1,NULL,&thrLoop,srv);
	
	string msg = "Test";
	CPPUNIT_ASSERT(c1.Send(msg));
	string answer;
	CPPUNIT_ASSERT(c1.Receive(answer));
	CPPUNIT_ASSERT_MESSAGE("Failure testing 'string' case", 
			answer == "Test");
		
	string msg2 = "Test bis";
	CPPUNIT_ASSERT(c2.Send(msg2));
	string answer2;
	CPPUNIT_ASSERT(c2.Receive(answer2));
	CPPUNIT_ASSERT_MESSAGE("Failure testing 'string' case", 
			answer2 == "Test bis");
	pthread_join (thr,NULL);
	pthread_join (thr1,NULL);
	
	CPPUNIT_ASSERT(c1.Close());
	CPPUNIT_ASSERT(c2.Close());
	delete srv;
}

