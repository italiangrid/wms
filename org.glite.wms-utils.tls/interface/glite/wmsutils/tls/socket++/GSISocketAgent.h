/***************************************************************************
 *  filename  : GSISocketAgent.h
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001 by INFN
 ***************************************************************************/

// $Id$

/**
 * @file GSISocketAgent.h
 * @brief The header file for secure Socket Agent Object.
 * This file contains definitions for secure shell based Socket Agent used for message
 * exchange between Client and Server.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */

#ifndef _GSISocketAgent_h_
#define _GSISocketAgent_h_

/** The superclass definition file. */
#include "glite/wmsutils/tls/socket++/SocketAgent.h"

/** The globus secure shell definitions file. */
#include <globus_gss_assist.h>

namespace glite {
namespace wmsutils {
namespace tls { 
namespace socket_pp {

/** 
 * The secure connection agent.
 * This object acts as agent in the secure-shell-based message exchange. 
 * It joins the server and the client
 * in both connection establishment and message exchange.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */
class GSISocketAgent : public SocketAgent
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
   * Receive an int value.
   * @param i an int to fill.
   * @return true on success, false otherwise.
   */
  virtual bool Receive(int&);
  /**
   * Receive a string value.
   * @param s the string to fill.
   * @return true on success, false otherwise.
   */
  virtual bool Receive(std::string&);
  /**
   * Return the delegate credential file name.
   * @return a string containing credential file name.
   */
  const std::string& CredentialsFile() const { return _delegated_credentials_file; }
  /**
   * Return the certificate subject.
   * @return a string containing the certificate subject.
   */
  const std::string& CertificateSubject() const { return _certificate_subject; }
  /**
   * Return the local account name the user is mapped to.
   * @return a string containing the local account name.
   *
   */
   const std::string& GridmapName() const { return _gridmap_name; } 
  /**
   * Constructor.
   */
  GSISocketAgent();
  /**
   * Distructor.
   */
  virtual ~GSISocketAgent();

private:
  /**
   * Return the socket descriptor.
   * @return the socket descriptor.
   */
  int socket() const { return sck; } 

private:
  /** The secure server. */
  friend class GSISocketServer;
  /** The secure client. */
  friend class GSISocketClient;

  /** The Secure Shell context identifier. */
  gss_ctx_id_t gss_context; 
  /** The Secure Shell credential identifier. */
  gss_cred_id_t credential; 
  /** The file containing delegated credentials. */ 
  std::string _delegated_credentials_file;
  /** The certificate subject. */
  std::string _certificate_subject;
  /** The local account the user is mapped to. */
  std::string _gridmap_name;
};

} // namespace socket_pp
} // namespace tls
} // namespace wmsutils
} // namespace glite

#endif // _GSISocketAgent_h_

/*
  Local Variables:
  mode: c++
  End:
*/




