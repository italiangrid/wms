/*
 * SocketAgent.h
 * 
 * Copyright (C) 2002 EU-Datagrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#ifndef __SOCKETAGENT__
#define __SOCKETAGENT__

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <string>

namespace glite {
namespace wmsutils {
namespace tls { 
namespace socket_pp {

/** 
 * The connection agent.
 * This object acts as agent in message exchange. It joins the server and the
 * client in both connection establishment and message exchange.
 * @author Salvatore Monforte
 * @author comments by Marco Pappalardo and Salvatore Monforte
 */
class SocketAgent
{ 
 public:  

  /**
   * Send a string value.
   * @param s the string value to send.
   * @return true on success, false otherwise.
   */ 
  virtual bool Send(const std::string&);
  /**
   * Send a int value.
   * @param i the int value to send.
   * @return true on success, false otherwise.
   */ 
  virtual bool Send(int);
  /**
   * Send a long value.
   * @param i the long value to send.
   * @return true on success, false otherwise.
   */ 
  virtual bool Send(long);
  
  /**
   * Receive an int value.
   * @param i an int to fill.
   * @return true on success, false otherwise.
   */
  virtual bool Receive(int&);
  /**
   * Receive a long value.
   * @param i a long to fill.
   * @return true on success, false otherwise.
   */
  virtual bool Receive(long&);
  /**
   * Receive a string value.
   * @param s the string to fill.
   * @return true on success, false otherwise.
   */
  virtual bool Receive(std::string&);
  
  /**
   * Returns the host name.
   * @param the string to fill with host name.
   */
  std::string PeerName();
  bool is_recv_pending();
  bool is_send_pending();
  /**
   * Set the connection timeout.
   * @param secs a size_t representing the timeout in seconds.
   * @return tru on success, false otherwise.
   */
  bool SetTimeout(int);
  /**
   * Set the connection timeout.
   * @param secs a size_t representing the timeout in seconds while receiving data.
   * @return tru on success, false otherwise.
   */
  bool SetRcvTimeout(int);
   /**
   * Set the connection timeout.
   * @param secs a size_t representing the timeout in seconds while sending data.
   * @return tru on success, false otherwise.
   */
  bool SetSndTimeout(int);
  /**
   * Returns the host name.
   * @param the string to fill with host name.
   */
  int SocketDescriptor() { return sck; }
  /**                                                                                             
   * Return the peer entity ip address.                                                           
   * @return peer entity ip address.                                                              
   */ 
  std::string PeerAddr(); 
  /**                                                                                             
   * Return the peer entity port.                                                                 
   * @return peer entity port.                                                                    
   */ 
  int PeerPort();

protected:

  /**
   * Constructor.
   */
  SocketAgent();
  /**
   * Destructor.
   */
  virtual ~SocketAgent();
  
 private:
  /**
   * Send chars of a buffer. The max number of chars to be sent is fixed by
   * size parameter. 
   * @param buf the transmission buffer.
   * @param size teh max number of chars to be sent.
   */
  bool sendbuffer(char *, unsigned int);
  /**
   * Receive chars in a buffer. The max number of chars to be received is fixed by
   * size parameter. 
   * @param buf the reception buffer.
   * @param size teh max number of chars to be received.
   */
  bool readbuffer(char *, unsigned int);

  /** The Server. */
  friend class SocketServer;
  /** The Client. */
  friend class SocketClient;
    
  struct sockaddr_in peeraddr_in;	/**< Address for peer socket.*/

protected:
  /** The socket descriptor. */
  int sck;
  int m_recv_timeout;
  int m_send_timeout;
};

} // namespace socket_pp
} // namespace tls
} // namespace wmsutils
} // namespace glite


#endif // __SOCKETAGENT__

/*
  Local Variables:
  mode: c++
  End:
*/



