#include "glite/wmsutils/tls/socket++/GSISocketClient.h"
#include "glite/wmsutils/tls/socket++/exceptions.h"

#include <iostream>
#include <string>

using namespace std;
using namespace glite::wmsutils::tls::socket_pp;

namespace socket_pp = glite::wmsutils::tls::socket_pp;

int main(int argc, char* argv[])
{
	const string host=argv[1];
	string contact=argv[3];
	int port=atoi(argv[2]);
	int res = 0;
try {
	socket_pp::SocketClient c(host,port);
	//c.ServerContact(contact);
	c.Open();
	try {
		std::string message = "Hello World!";
		c.SetTimeout(5);
		c.Send(message);
		std::cout << "Sent: " << message << std::endl;
		std::string answer = "";
		c.Receive(answer);
		std::cout << "Received: " << answer << std::endl;
		if ( answer != "ok" )
		{
			res = 1;
		}
	}
	catch( std::exception& e) {
	        std::cout << e.what() << std::endl;
		return 1;
	}
} 
catch( std::exception& e) { 
	std::cout << e.what() << std::endl; 
	return 2;
} 
	
if ( res != 0 )
{
	return 3;
}
else
{
 	return 0;
}
}
