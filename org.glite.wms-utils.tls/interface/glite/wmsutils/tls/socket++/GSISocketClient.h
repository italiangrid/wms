/***************************************************************************
 *  filename  : GSISocketClient.h
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

#ifndef _GSISocketClient_h_
#define _GSISocketClient_h_

/** This super class header file. */
#include "glite/wmsutils/tls/socket++/SocketClient.h"
/** Include the secure socket globus definition. */
#include <globus_gss_assist.h>

namespace glite {   
namespace wmsutils { 
namespace tls {
namespace socket_pp {

/** The secure agent used to realize message exchange. */
class GSISocketAgent;

/** 
 * The secure Client.
 * This object acts as Client in the message exchange. It asks the client for
 * connections referencing an agent for secure message exchange.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */
class GSISocketClient : public SocketClient 
{

 public:
  /**
   * Constructor.
   * @param p the secure server port.
   * @param b the backlog, that is the maximum number of outstanding connection requests.
   */
  GSISocketClient(const std::string&, int);
  /**
   * Destructor.
   */  
  virtual ~GSISocketClient();
 
  /**
   * Set the server contact. 
   * @param contact the server contact string to set.
   */
  void ServerContact(const std::string& contact) { _server_contact = contact; }
  /**
   * Set whether the client must delegate credentials or not.
   * @param mode a boolean explainig whether to delegate or not.
   */
  void DelegateCredentials(bool mode) { _delegate_credentials = mode; }
 
  /**
   * Deprecated Function.
   */
  //void DoMutualAuthentication(bool mode) { 
  //  _do_mutual_authentication = mode; 
  //}

  /**
   * Open the connection.
   * @return true for successful opening, false otherwise.
   */
  virtual bool Open();
  /**
   * Close the connection.
   * @return true for successful close, false otehrwise.
   */
  virtual bool Close();
  void set_auth_timeout(int to);

 protected:
  /**
   * Initialize GSI Authentication.
   * This method asks the server for authentication.
   * @param sock the socket descriptot
   * @return true on success, false otherwise.
   */
  bool InitGSIAuthentication(int sock);

 private:
   /** The Secure Shell context identifier. */
   gss_ctx_id_t gss_context; 
   /** The server contact. */
   std::string _server_contact;
   /** Whether to delegate credentials or not. */ 
   bool _delegate_credentials;
   //bool _do_mutual_authentication;
   int m_auth_timeout;
};

} // namespace socket_pp
} // namespace tls
} // namespace wmsutils
} // namespace glite

#endif // _GSISocketClient_h_

/*
  Local Variables:
  mode: c++
  End:
*/






