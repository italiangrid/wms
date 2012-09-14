#include<iostream>
#include<string>
#include<pthread.h>
                                   
#include <cppunit/extensions/HelperMacros.h>

#include "glite/wmsutils/tls/socket++/GSISocketServer.h"
#include "glite/wmsutils/tls/socket++/GSISocketClient.h"
#include "glite/wmsutils/tls/socket++/GSISocketAgent.h"
#include "glite/wmsutils/tls/socket++/exceptions.h" 


class GSIsocketClient_test : public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(GSIsocketClient_test);

  CPPUNIT_TEST(OpenClose_case);
  CPPUNIT_TEST(clientwork_case);
  CPPUNIT_TEST(SendReceive_case);
  CPPUNIT_TEST(OpenFailure_case); 
  CPPUNIT_TEST(Agent_case); 

  CPPUNIT_TEST_SUITE_END();

private:
	int srvListeningPort;
	pthread_t thr;

public:

	void setUp();
	void tearDown();

  	void OpenClose_case();
	void clientwork_case();
	void SendReceive_case();
	void OpenFailure_case();
	void Agent_case();

	friend void* thrLoop(void *);

};

template <class Type>
void msgSendReceive ( Type message, glite::wmsutils::tls::socket_pp::SocketAgent* a)
{
	a -> Receive(message);
  a -> Send(message);
 	return;	 
}

