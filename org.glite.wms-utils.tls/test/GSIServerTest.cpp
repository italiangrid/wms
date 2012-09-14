
#include "glite/wmsutils/tls/socket++/GSISocketServer.h"
#include "glite/wmsutils/tls/socket++/GSISocketClient.h"
#include "glite/wmsutils/tls/socket++/GSISocketAgent.h"

#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace glite::wmsutils::tls::socket_pp;

int main()
{
    GSISocketServer server(55558);

    try {
    if (!server.Open())
    {
     std::cout << "Server nn aperto\n";
     exit(1);
    }
    GSISocketAgent* agent = server.Listen();

    std::cout << "Connesso\n"; 
    
    int messageInt;
    if (!agent->Receive(messageInt))
    {
      std::cout << "agent nn riceve\n";
    } else 
    {
        if (!agent->Send(messageInt))
        {
            std::cout << "agent nn spedisce\n";
        } 
    }
    
    std::string message;
    if (!agent->Receive(message))
    {
      std::cout << "agent nn riceve\n";
    } else 
    {
        if (!agent->Send(message))
        {
            std::cout << "agent nn spedisce\n";
        }
    }
    
    server.KillAgent(agent);
    
    server.Close();
    std::cout << "Server chiuso\n";
    }catch( std::exception& e) {
	std::cout << e.what() << std::endl;
	exit(1);
    }

}

 
