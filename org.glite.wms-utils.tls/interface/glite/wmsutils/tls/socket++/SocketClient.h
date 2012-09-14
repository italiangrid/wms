/***************************************************************************
 *  filename  : SocketClient.h
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001 by INFN
 ***************************************************************************/

// $Id$

/**
 * @file SocketClient.h
 * @brief The header file for Socket Client Object.
 * This file contains definitions for Socket Client used in
 * order to communicate with the Resource Broker.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */

#ifndef _SocketClient_h_
#define _SocketClient_h_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <string>

namespace glite {   
namespace wmsutils { 
namespace tls {
namespace socket_pp {

/** The agent used to realize message exchange. */
class SocketAgent;

/** 
 * The connection Client.
 * This object acts as Client in the message exchange. It requests the server for
 * connections and it creates and sends the reference to the
 * agent to be used for message exchange.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */
class SocketClient
{
 public:

  /** 
   * Constructor.
   * @param h the host name.
   * @param p the host port.
   */
  SocketClient(const std::string&, int);
  /**
   * Destructor.
   * This method must be also implemented by object subclassing server socket.
   */
  virtual ~SocketClient();
  
  /**
   * Send a string value.
   * @param s the string value to send.
   * @return true on success, false otherwise.
   */
  bool Send(const std::string&);

  /**
   * Send an int value.
   * @param i the int value to send.
   * @return true on success, false otherwise.
   */
  bool Send(int);
  /**
   * Send a long value.
   * @param i the long value to send.
   * @return true on success, false otherwise.
   */
  bool Send(long); 
  /**
   * Receive an int value.
   * @param i an int to fill.
   * @return true on success, false otherwise.
   */
  bool Receive(int&);
  /**
   * Receive a long value.
   * @param i a long to fill.
   * @return true on success, false otherwise.
   */
  bool Receive(long&);
 
  /**
   * Receive a string value.
   * @param s the string to fill.
   * @return true on success, false otherwise.
   */
  bool Receive(std::string&);

  /**
   * Open a connection to the Server.
   * @return true on success, false otherwise.
   */  
  virtual bool Open();
  /**
   * Close the connection.
   * @return true on success, false otherwise.
   */
  virtual bool Close();

  /**
   * Set the connection timeout.
   * @param secs a size_t representing the timeout in seconds.
   * @return tru on success, false otherwise.
   */
  bool SetTimeout(size_t secs);
  /**
   * Return the host name.
   * @return the host name string.
   */
  std::string Host() const { return host; }
  /**
   * Returns a pointer to the SocketAgent.
   * @return a reference to the related Socket Agent.
   */
  SocketAgent* getAgent() const;

 protected:
  /**  
   * Attach an agent to this client.
   * This method also connects the agent to the proper server.
   * @param a the Socket Agent to attach.
   * @return true for a successful attachment, false otherwise.
   */	
   bool AttachAgent(SocketAgent*);

 public:
  /** The host name. */
  const std::string host;
  /** The host port. */
  const int port;

 protected:
  /** The Socket Agent reference for message exchange. */
  SocketAgent *agent;
};

} // namespace socket_pp
} // namespace tls
} // namespace wmsutils
} // namespace glite

#endif // _SocketClient_h_

/*
  Local Variables:
  mode: c++
  End:
*/

