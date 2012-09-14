/***************************************************************************
 *  filename  : SocketServer.cpp
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001 by INFN
 ***************************************************************************/

// $Id$

/**
 * @file SocketServer.cpp
 * @brief The implementation of Socket Server Object.
 * This file contains implemantations for Socket Server used in
 * order to communicate with the Resource Broker.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */

#include <memory.h>
#include <ctime>
#include <algorithm>
#include <unistd.h>
#include <sys/time.h>

#ifdef WITH_SOCKET_EXCEPTIONS
#include "glite/wmsutils/tls/socket++/exceptions.h"
#include <cstdio>
#include <errno.h>
#endif

#include "glite/wmsutils/tls/socket++/SocketAgent.h"
#include "glite/wmsutils/tls/socket++/SocketServer.h"

namespace glite {   
namespace wmsutils { 
namespace tls {
namespace socket_pp {

/**
 * Constructor.
 * @param p the server port.
 * @param b the backlog, that is the maximum number of outstanding connection requests.
 */
SocketServer::SocketServer(int p, int b) : port(p), backlog(b)
{
  memset ((char *)&myaddr_in, 0, sizeof(struct sockaddr_in));
  
  myaddr_in.sin_family = AF_INET;
  myaddr_in.sin_addr.s_addr = INADDR_ANY;
  myaddr_in.sin_port=htons(p);
  sck = -1;

  agent_mutex = new pthread_mutex_t();
  pthread_mutex_init(agent_mutex,0);
}

/**
 * Open the connection.
 * @return whether connection is established or not.
 */
bool SocketServer::Open()
{
  bool result = false;
  
  sck = socket (AF_INET, SOCK_STREAM, 0);
  result = (sck!=-1);
  if( result ) {
  
    int		val = 1;	  
    int         oldval;
    socklen_t   len = sizeof( oldval );
    getsockopt( sck, SOL_SOCKET, SO_REUSEADDR, (void *) &oldval, &len );
    oldval |= val;
    setsockopt( sck, SOL_SOCKET, SO_REUSEADDR, (const void *) &oldval, len);
    result = (bind(sck, (struct sockaddr*)&myaddr_in, sizeof(struct sockaddr_in)) != -1) &&   
      (listen(sck, backlog) != -1);   
  }
#ifdef WITH_SOCKET_EXCEPTIONS
  if( !result ) {  
  char msg[32];
    sprintf(msg,"socket #%d", sck);
    throw IOException ( std::string(msg), std::string("open()"), std::string(strerror(errno)));
  }
#endif
  return result;
}

/**
 * Close the connection.
 */
void SocketServer::Close()
{
  close(sck);
}

/**
 * Destructor.
 * This method must be also implemented by object subclassing server socket.
 */
SocketServer::~SocketServer()
{
  pthread_mutex_lock(agent_mutex);
  while(!agents.empty()) {
    delete agents.front();
    agents.pop_front();
  }
  pthread_mutex_unlock(agent_mutex);
  pthread_mutex_destroy(agent_mutex);
  delete agent_mutex;
  Close();
}

/**
 * Return whether there is any pending connection.
 * @return true if any pending connection exists, false otherwise.
 */
bool SocketServer::IsConnectionPending()
{
  fd_set readfs;
  struct timeval timeout;
  timeout.tv_sec=1;
  timeout.tv_usec=0;
  int retcod=0;
  while (retcod == 0)
    {
      FD_ZERO(&readfs);
      FD_SET(sck,&readfs);
      timeout.tv_sec=1;
      timeout.tv_usec=0;
      retcod = select(FD_SETSIZE,&readfs,(fd_set *)NULL,(fd_set *)NULL, &timeout);
    }
  if (retcod < 0) return(false);
  else            return(true);
}

/**
 * Listen for incoming connection requests.
 * Accept incoming requests and redirect communication on a dedicated port.
 * @param a a reference to the Socket Agent sent by Client.
 * @return the Socket Agent redirecting communication on a dediceted port.
 */
SocketAgent* SocketServer::Listen(SocketAgent* a)
{
  SocketAgent* sa = a;

  if(!sa) sa = new SocketAgent();
 
  socklen_t addrlen = sizeof(struct sockaddr_in);
  int newsck = 0;

  bool pending = IsConnectionPending();

  if (pending && 
      (sa -> sck = newsck = accept(sck, 
			           (struct sockaddr*)& sa -> peeraddr_in, 
				   &addrlen)) == -1) {    
    delete sa;
    sa = 0;

#ifdef WITH_SOCKET_EXCEPTIONS
        char buf[16];
        sprintf(buf,"socket #%d", sck);
        throw IOException(std::string(buf), 
			  std::string("accept()"), 
			  std::string(strerror(errno)));
#endif
  }	
  else { 
    
    struct linger linger;
    
    linger.l_onoff  =1;
    linger.l_linger =1;
  
    if( setsockopt(newsck, SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger)) == -1) {
    
      delete sa;
      sa =0;

#ifdef WITH_SOCKET_EXCEPTIONS
      char buf[16];
        sprintf(buf,"socket #%d", sck);
      throw IOException( std::string(buf),
			 std::string("setsockopt(SO_LINGER)"), 
			 std::string("Unable to set socket options")); 
#endif
    }
  }

  if(sa) {
    pthread_mutex_lock(agent_mutex);
    agents.push_front(sa);
    pthread_mutex_unlock(agent_mutex);
  }

  return sa;
}

/**
 * Kill a Socket Agent.
 * This also close the communication this agent holds.
 * @param a the agent to kill.
 */
void SocketServer::KillAgent(SocketAgent* a)
{
  pthread_mutex_lock(agent_mutex);

  if( find(agents.begin(), agents.end(), a) != agents.end() ) {
    agents.remove(a);
    delete a;
  }
  pthread_mutex_unlock(agent_mutex);
  
}

} // namespace socket_pp
} // namespace tls
} // namespace wmsutils
} // namespace glite


