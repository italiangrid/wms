
#include "glite/wmsutils/tls/socket++/SocketServer.h"
#include "glite/wmsutils/tls/socket++/SocketClient.h"
#include "glite/wmsutils/tls/socket++/SocketAgent.h"

#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace glite::wmsutils::tls::socket_pp;

int main()
{
  pid_t pid = fork();
  
  if (pid == -1) 
  {
    std::cout << "fork failed\n";
    exit(1);
  }
  
  if (pid == 0) {
    SocketClient client("localhost", 55558);
    sleep(5);
    if (!client.Open())
    {
      std::cout << "Connessione nn aperta\n"; 
      exit(1);
    }
    
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
    
    long sendlong=152015100000;
    if(!client.Send(sendlong))
    {
      std::cout << "Send nn riuscita\n"; 
    } else 
    {
        long receivelong;
        if(!client.Receive(receivelong))
        {
            std::cout << "Receive nn riuscita\n"; 
        } else 
        {
            if (sendlong==receivelong)
            {
                std::cout << "long arrivato ok\n";
            } else 
            {
                std::cout << "long arrivato ma diverso dall'originale\n";
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

  }
  
  if (pid > 0) {
    SocketServer server(55558);
    if (!server.Open())
    {
     std::cout << "Server nn aperto\n"; 
     exit(1);
    }
    SocketAgent* agent = server.Listen();

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
       
    long messageLong;
    if (!agent->Receive(messageLong))
    {
      std::cout << "agent nn riceve\n";
    } else 
    {
      if (!agent->Send(messageLong))
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

  }
}

 