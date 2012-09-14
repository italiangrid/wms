/***************************************************************************
 *  filename  : GSISocketServer.h
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001 by INFN
 ***************************************************************************/

// $Id$

/**
 * @file GSISocketServer.h
 * @brief The header file for ssh based Socket Server Object.
 * This file contains definitions for secure Socket Server used in
 * order to communicate with the Resource Broker.\ It uses SSH standard.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */

#ifndef _GSISocketServer_h_
#define _GSISocketServer_h_

/** Include the super class header. */
#include "glite/wmsutils/tls/socket++/SocketServer.h"
/** Include the secure socket globus definition. */
#include <globus_gss_assist.h>

namespace glite {   
namespace wmsutils { 
namespace tls {
namespace socket_pp {

/** The secure agent used to realize message exchange. */
class GSISocketAgent;
/** The data struct containing the authentication context. */
struct GSIAuthenticationContext;

/** 
 * The secure Server.
 * This object acts as Server in the message exchange. It listens for client
 * connections and, when asked for, it receives, sets and sends back the reference to the
 * agent to be used for secure message exchange.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */
class GSISocketServer : public SocketServer
{
 public:
  enum limited_proxy_mode_t { normal, multi };	
  /**
   * Constructor.
   * @param p the secure server port.
   * @param b the backlog, that is the maximum number of outstanding connection requests.
   */
  GSISocketServer(int, int=5);
  /**
   * Destructor.
   * This method must be also implemented by object subclassing server socket.
   */
  virtual ~GSISocketServer();

  /**
   * Close the connection.
   */
  virtual void Close();
  /**
   * Listen for incoming connection requests.
   * Accept incoming requests and redirect communication on a dedicated port.
   * @param a a reference to the secure GSI Socket Agent sent by Client.
   * @return the GSI Socket Agent redirecting communication on a dedicated port.
   */
  virtual GSISocketAgent* Listen();
  /**
   * 
   */
  bool GSISocketServer::AuthenticateAgent(GSISocketAgent* sa);
  /**
   * Redirects the GSI output.
   * This method allows to define a logging file for GSI.
   * @param fp a pinter to a file.
   */ 
  void RedirectGSIOutput(FILE *fp) { if(fp!=NULL) gsi_logfile = fp; }
 
  void LimitedProxyMode(limited_proxy_mode_t mode) { limited_proxy_mode = mode; }
  void set_auth_timeout(int to);
		  
 private:
  /**
   * Accept the GSI Authentication.
   * @param sock the socket for communication.
   * @param ctx the authorization context.
   * @return the context identifier. 
   */
  gss_ctx_id_t AcceptGSIAuthentication(int sock, GSIAuthenticationContext&); 
  /** The reference to the log file. */
  FILE *gsi_logfile;
  limited_proxy_mode_t limited_proxy_mode;
  int m_auth_timeout;
};

} // namespace socket_pp
} // namespace tls
} // namespace wmsutils
} // namespace glite

#endif // _GSISocketServer_h_

/*
  Local Variables:
  mode: c++
  End:
*/



