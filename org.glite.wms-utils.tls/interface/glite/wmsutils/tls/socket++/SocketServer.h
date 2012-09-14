/***************************************************************************
 *  filename  : SocketServer.h
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001 by INFN
 ***************************************************************************/

// $Id$

/**
 * @file SocketServer.h
 * @brief The header file for Socket Server Object.
 * This file contains definitions for Socket Server used in
 * order to communicate with the Resource Broker.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */

#ifndef _SocketServer_h__
#define _SocketServer_h__

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include <list>
#include <string>

namespace glite {   
namespace wmsutils { 
namespace tls {
namespace socket_pp {

/** The agent used to realize message exchange. */
class SocketAgent;

/** 
 * The connection Server.
 * This object acts as Server in the message exchange. It listens for client
 * connections and, when asked for, it receives, sets and sends back the reference to the
 * agent to be used for message exchange.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */
class SocketServer
{
 public:

  /**
   * Constructor.
   * @param p the server port.
   * @param b the backlog, that is the maximum number of outstanding connection requests.
   */
  SocketServer(int, int=5);
  /**
   * Destructor.
   * This method must be also implemented by object subclassing server socket.
   */
  virtual ~SocketServer();

  /**
   * Open the connection.
   * @return whether connection is established or not.
   */
  bool Open();
  
  /**
   * Return whether there is any pending connection.
   * @return true if any pending connection exista, false otherwise.
   */
  bool IsConnectionPending();

  /**
   * Close the connection.
   */
  virtual void Close();
  /**
   * Listen for incoming connection requests.
   * Accept incoming requests and redirect communication on a dedicated port.
   * @param a a reference to the Socket Agent sent by Client.
   * @return the Socket Agent redirecting communication on a dediceted port.
   */
  virtual SocketAgent* Listen(SocketAgent* = 0);

 /**
  * Kill a Socket Agent.
  * This also close the communication this agent holds.
  * @param a the agent to kill.
  */
  void KillAgent(SocketAgent*);

  private:
  
  struct sockaddr_in myaddr_in;	    /**< the socket address for local socket address. */
  pthread_mutex_t* agent_mutex;     /**< a mutex for access to agent. */ 
  std::string hostname;             /**< the host name. */
  int port;                         /**< the host port. */
  int backlog;                      /**< the backlog. */
  std::list<SocketAgent*> agents;   /**< the list of agents managed by this server. */

 protected:
  /** The socket descriptor. */
  int sck;	
};

} // namespace socket_pp
} // namespace tls
} // namespace wmsutils
} // namespace glite


#endif // _SocketServer_h__

/*
  Local Variables:
  mode: c++
  End:
*/



