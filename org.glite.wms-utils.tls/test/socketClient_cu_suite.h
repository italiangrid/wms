#include<iostream>
#include<string>
#include<pthread.h>
                                   
#include <cppunit/extensions/HelperMacros.h>

#include "glite/wmsutils/tls/socket++/SocketServer.h"
#include "glite/wmsutils/tls/socket++/SocketClient.h"
#include "glite/wmsutils/tls/socket++/SocketAgent.h"
#include "glite/wmsutils/tls/socket++/exceptions.h" 

//using namespace std;

//using namespace glite::wmsutils::tls;
//class glite::wmsutils::tls::socket_pp::SocketAgent;

class socketClient_test : public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(socketClient_test);
	
  	CPPUNIT_TEST(OpenClose_case);
  	CPPUNIT_TEST(SendReceive_case);
  	CPPUNIT_TEST(Agent_case);
  	CPPUNIT_TEST(MultipleClient_case);
	CPPUNIT_TEST(OpenFailure_case);
	CPPUNIT_TEST(OpenFailure2_case);  
	CPPUNIT_TEST(SetTimeout_case);
	

  CPPUNIT_TEST_SUITE_END();
private:
	int srvListeningPort;
	pthread_t thr, thr1;
	glite::wmsutils::tls::socket_pp::SocketServer *srv;
	glite::wmsutils::tls::socket_pp::SocketClient *c;

public:

	void setUp();
	void tearDown();

  	void OpenClose_case();
	void SendReceive_case();
  
	void OpenFailure_case();
	void OpenFailure2_case();

	
  	void Agent_case();
  	void MultipleClient_case();
	void SetTimeout_case();


	friend void* thrLoop(void *);
	

};

template <class Type>
void msgSendReceive ( Type message, glite::wmsutils::tls::socket_pp::SocketAgent* a)
{
	a -> Receive(message);
  a -> Send(message);
 	return;	 
}

