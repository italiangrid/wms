/***************************************************************************
 *  filename  : GSISocketClient.cpp
 *  author    : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  author    : Marco Pappalardo <marco.pappalardo@ct.infn.it>
 *  copyright : (C) 2001 by INFN
 ***************************************************************************/

// $Id$

#include <errno.h>
/** This class header file. */
#include "glite/wmsutils/tls/socket++/GSISocketClient.h"
/** The communication agent definition file. */
#include "glite/wmsutils/tls/socket++/GSISocketAgent.h"
/** The tokens transission and reception features definitions. */
#include "tokens.h"
/** The error messages file. */
#include "errors.h"

#ifdef WITH_SOCKET_EXCEPTIONS
#include "glite/wmsutils/tls/socket++/exceptions.h"
#endif
#include <iostream>

namespace glite {   
namespace wmsutils { 
namespace tls {
namespace socket_pp {


void GSISocketClient::set_auth_timeout(int to)
{
 this -> m_auth_timeout = to;
}
/**
 * Constructor.
 * @param p the secure server port.
 * @param b the backlog, that is the maximum number of outstanding connection requests.
 */
GSISocketClient::GSISocketClient(const std::string& h, int p) : SocketClient(h,p)
{
  AttachAgent(new GSISocketAgent());
  gss_context = GSS_C_NO_CONTEXT;
  _server_contact = "";
  _delegate_credentials = true;
  m_auth_timeout = -1;
  //_do_mutual_authentication = true;
}

/**
 * Destructor.
 */  
GSISocketClient::~GSISocketClient()
{
  Close();
  SocketClient::Close();
}

/**
 * Initialize GSI Authentication.
 * This method asks the server for authentication.
 * @param sock the socket descriptot
 * @return true on success, false otherwise.
 */
bool GSISocketClient::InitGSIAuthentication(int sock)
{
   OM_uint32                   major_status = 0;
   OM_uint32                   minor_status = 0;
   gss_cred_id_t               credential = GSS_C_NO_CREDENTIAL;
   OM_uint32                   req_flags  = 0;
   OM_uint32                   ret_flags  = 0;
   int                         token_status = 0;
   bool                        return_status = false;
   gss_name_t                  targ_name;
   gss_buffer_desc             name_buffer;
   char                        service[1024];
      
    /* acquire our credentials */
    major_status = globus_gss_assist_acquire_cred(&minor_status,
                                                  GSS_C_BOTH,
                                                  &credential);

    if(major_status != GSS_S_COMPLETE) {

      char buf[32];
      std::string msg(FAILED_ACQ_CRED);
      sprintf(buf, "%d", port);
      msg.append(host + ":" + std::string(buf));
      
#ifdef WITH_SOCKET_EXCEPTIONS
      char *gssmsg = NULL;
      globus_gss_assist_display_status_str( &gssmsg,
					    NULL,
					    major_status,
					    minor_status,
					    token_status);
 
      std::string source(gssmsg);
      free(gssmsg);
      throw AuthenticationException(source,
	std::string("globus_gss_assist_acquire_cred()"),
	std::string(buf));
#else
      globus_gss_assist_display_status(stdout,
				       buf,
				       major_status,
				       minor_status,
				       0);
#endif
      return false;
    }
    
    /* Request that remote peer authenticate tself */
    req_flags = GSS_C_MUTUAL_FLAG;   
    
    if(_delegate_credentials) req_flags |= GSS_C_DELEG_FLAG;  

    snprintf(service, sizeof(service), "host@%s", host.c_str()); /* XXX */
    /* initialize the security context */
    /* credential has to be fill in beforehand */
    std::pair<int,int> arg(sock, m_auth_timeout);
    major_status =
        globus_gss_assist_init_sec_context(&minor_status,
                                           credential,
                                           &gss_context,
                                           _server_contact.empty() ? service :(char*) _server_contact.c_str(), 
                                           req_flags,
                                           &ret_flags,
                                           &token_status,
                                           get_token,
                                           (void *) &arg,
                                           send_token,
                                           (void *) &arg);
    
    gss_release_cred(&minor_status, &credential);

    if(major_status != GSS_S_COMPLETE) {


#ifdef WITH_SOCKET_EXCEPTIONS
      char *gssmsg = NULL;
      globus_gss_assist_display_status_str(&gssmsg,
					   NULL,
					   major_status,
					   minor_status,
					   token_status);
      
      if(gss_context != GSS_C_NO_CONTEXT) {
	gss_delete_sec_context(&minor_status, &gss_context, GSS_C_NO_BUFFER);
      }
      std::string source(gssmsg);
      free(gssmsg);		
      throw AuthenticationException(source, std::string("globus_gss_assist_init_sec_context()"),
	std::string("Failed to establish security context..."));
#else
      globus_gss_assist_display_status(stdout,
				       "Failed to establish security context (init): ",
				       major_status,
				       minor_status,
				       token_status);
#endif
    }
    else {
      
     major_status = gss_inquire_context(&minor_status,
					 gss_context,
					 NULL,
					 &targ_name,
					 NULL,
					 NULL,
					 NULL,
					 NULL,
					 NULL);

     
      return_status = (major_status == GSS_S_COMPLETE); 
      
      major_status = gss_display_name(&minor_status, targ_name, &name_buffer, NULL);
      gss_release_name(&minor_status, &targ_name);
      
    }
    if (return_status == false && gss_context != GSS_C_NO_CONTEXT) {
    
	    gss_delete_sec_context(&minor_status, &gss_context, GSS_C_NO_BUFFER);
   }
#ifdef WITH_SOCKET_EXCEPTIONS
    if (return_status == false) {	  
	    char *gssmsg = NULL;
	    globus_gss_assist_display_status_str( &gssmsg,
			NULL,
			major_status,
			minor_status,
			token_status);

	    std::string source(gssmsg);
	    free(gssmsg);
				   
            throw AuthenticationException(source, std::string("gss_inquire_context"),
	              std::string("Failed to establish security context..."));
    }
#endif		  
    
    return return_status;
}

/**
 * Open the connection.
 * @return true for successful opening, false otherwise.
 */
bool GSISocketClient::Open()
{
  
 bool result = SocketClient::Open() &&
   InitGSIAuthentication(static_cast<GSISocketAgent*>(agent)->socket()); 
 
 if(!result) 
   static_cast<GSISocketAgent*>(agent) -> gss_context = GSS_C_NO_CONTEXT;
 else {
   static_cast<GSISocketAgent*>(agent) -> gss_context = gss_context;

#ifdef WITH_SOCKET_EXCEPTIONS
   try {
#endif
   int ack = 0;  
   static_cast<GSISocketAgent*>(agent) ->  SetRcvTimeout(m_auth_timeout);
   result = static_cast<GSISocketAgent*>(agent) -> Receive( ack );
   static_cast<GSISocketAgent*>(agent) ->  SetRcvTimeout(-1);
#ifdef WITH_SOCKET_EXCEPTIONS
   }  
   catch(...)
   {
	   throw AuthenticationException(
			   std::string("GSI"), 
			   std::string("open()"),
		           std::string("Failed to establish security context..."));
   }
#endif
 }
 return result;
}


/**
 * Close the connection.
 * @return true for successful close, false otherwise.
 */
bool GSISocketClient::Close()
{
  OM_uint32 minor_status = 0;

  if (gss_context != GSS_C_NO_CONTEXT) {
     gss_delete_sec_context(&minor_status, &gss_context, GSS_C_NO_BUFFER);
     gss_context= GSS_C_NO_CONTEXT;
  }
  static_cast<GSISocketAgent*>(agent) -> gss_context = GSS_C_NO_CONTEXT;
 
  return SocketClient::Close();
}

} // namespace socket_pp
} // namespace tls
} // namespace wmsutils
} // namespace glite
