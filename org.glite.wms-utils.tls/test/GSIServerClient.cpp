
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
    GSISocketClient client("localhost", 55558);
    client.ServerContact("localhost");
    std::cout << "sonoqui\n";

    try
    {
    	if (!client.Open())
    	{
      	  std::cout << "Connessione nn aperta\n";
      	  exit(1);
    	}

    	std::cout << "Connesso\n";


    	int sendint=230000001;
	if(!client.Send(sendint))
    	{
          std::cout << "Send nn riuscita\n";
    	} else
    	{
          int receiveint;
          if(!client.Receive(receiveint))
          {
            std::cout << "Receive nn riuscita\n";
          } else
          {
            if (sendint==receiveint)
            {
                std::cout << "int arrivato ok\n";
            } else
            {
               std::cout << "int arrivato ma diverso dall'originale\n";
            }
          }
    	}

    std::string sendmsg;
    std::ifstream file("stringa.txt");
    while (!file.eof())
    {
      std::string tmp;
      std::getline(file, tmp);
      sendmsg += tmp;
    }
    file.close();

    if(!client.Send(sendmsg))
    {
      std::cout << "Send nn riuscita\n"; 
    } else 
    {
        std::string receivemsg;

        if(!client.Receive(receivemsg))
        {
            std::cout << "Receive nn riuscita " << receivemsg <<std::endl; 
        } else
        {
            if (sendmsg==receivemsg)
            {
                std::cout << "msg arrivato ok\n";
            } else 
            {
                std::cout << "msg arrivato ma diverso dall'originale\n";
            }
        }
    }
    
    if (!client.Close())
    {
      std::cout << "Connessione nn chiusa\n"; 
    }
    } catch ( std::exception& e) {
	std::cout << e.what() << std::endl;
	exit(1);
    }
}


