/***************************************************************************
 *  filename  : GSISocketServer.cpp
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001 by INFN
 ***************************************************************************/

// $Id$

/** The globus secure shell API definitions. */
#include <gssapi.h>
#include <memory.h>
#include <time.h>
#include <stdio.h>

/** Functionalities for transmitting and receiveing tokens. */
#include "tokens.h"
/** The secure Socket Agent used for communication. */
#include "glite/wmsutils/tls/socket++/GSISocketAgent.h"
/** This class header file. */
#include "glite/wmsutils/tls/socket++/GSISocketServer.h"

#ifdef WITH_SOCKET_EXCEPTIONS
#include "glite/wmsutils/tls/socket++/exceptions.h"
#endif

namespace glite {   
namespace wmsutils { 
namespace tls {
namespace socket_pp {

	/** The data struct containing the authentication context. */
	struct GSIAuthenticationContext 
	{
	  std::string 	delegated_credentials_file;  /**< The name or path of the file containing credentialis. */ 
	  std::string	certificate_subject;         /**< The certificate subject. */
	  std::string   gridmap_name;		     /**< The name of the local account the user is mapped to */
	  gss_cred_id_t  credential;                 /**< The credential identifier. */

	  /**
	   * Constructor.
	   */
	  GSIAuthenticationContext() { 
	    delegated_credentials_file="";
	    certificate_subject="";
	    credential = GSS_C_NO_CREDENTIAL;
	  }
	};
        
        void GSISocketServer::set_auth_timeout(int to)
        {
          m_auth_timeout = to;
        }
	/**
	 * Constructor.
	 * @param p the secure server port.
	 * @param b the backlog, that is the maximum number of outstanding connection requests.
	 */
	GSISocketServer::GSISocketServer(int p, int b) : SocketServer(p,b)
	{
	  gsi_logfile = stdout;
	  limited_proxy_mode = normal;
          m_auth_timeout = -1 ;
	}

	/**
	 * Destructor.
	 */
	GSISocketServer::~GSISocketServer()
	{
	  Close();
	}

	/**
	 * Close the connection.
	 */
	void GSISocketServer::Close()
	{
	  SocketServer::Close();
	}

	/**
	 * Accept the GSI Authentication.
	 * @param sock the socket for communication.
	 * @param ctx the authorization context.
	 * @return the context identifier. 
	 */
	gss_ctx_id_t GSISocketServer::AcceptGSIAuthentication(int sock, GSIAuthenticationContext& ctx)
	{
	  OM_uint32      major_status, minor_status;
	  OM_uint32      ret_flags = 0;
	  int            user_to_user = 0;
	  int            token_status = 0;
	  gss_ctx_id_t   context = GSS_C_NO_CONTEXT;
	  char           *name = NULL;
	  bool           return_status = false;
	  gss_cred_id_t  delegated_cred = GSS_C_NO_CREDENTIAL;
	  char           *local_account = NULL; 
	  int 		  rc = 0;
#ifndef WITH_GLOBUS_TOOLKIT_2_2
	  char           *delcname = NULL;
#endif

	  ret_flags =  limited_proxy_mode == normal ? GSS_C_GLOBUS_LIMITED_PROXY_FLAG:GSS_C_GLOBUS_LIMITED_PROXY_MANY_FLAG;
          std::pair<int,int> arg(sock, m_auth_timeout);
	  major_status =
	    globus_gss_assist_accept_sec_context(&minor_status,
						 &context,
						 ctx.credential,
						 &name,
						 &ret_flags,
						 &user_to_user,
						 &token_status,
						 &delegated_cred,
						 &get_token,
						 (void *) &arg,
						 &send_token,
						 (void *) &arg);
	  if( GSS_ERROR(major_status) ) {

#ifndef WITH_SOCKET_EXCEPTIONS      
	    globus_gss_assist_display_status(gsi_logfile,
					     "Failed to establish security context (accept):",
					     major_status,
					     minor_status,
					     token_status);
	    goto end;
#else
	    char *gssmsg;
	    globus_gss_assist_display_status_str( &gssmsg, 
						  NULL,
						  major_status,
						  minor_status,
						  token_status);
	    if( context != GSS_C_NO_CONTEXT ) { 
	      gss_delete_sec_context(&minor_status, &context, GSS_C_NO_BUFFER); 
	      context = GSS_C_NO_CONTEXT; 
	    }
    
	    if (delegated_cred != GSS_C_NO_CREDENTIAL) {
	      gss_release_cred(&minor_status, &delegated_cred);
	      delegated_cred = GSS_C_NO_CREDENTIAL;
	    }
    
	    if( name ) {		
	      free(name);
	      name = NULL;
	    }
	    std::string source(gssmsg);
	    free(gssmsg);	
	    throw AuthenticationException ( source, std::string("globus_gss_assist_acquire_cred()"), std::string("Failed to acquire credentials..."));
#endif
	  }
  
	  /* Do authorization here */
	  rc = globus_gss_assist_gridmap(name, &local_account); 
	  ctx.gridmap_name = std::string(name);
	  if(rc != 0)  {
#ifndef WITH_SOCKET_EXCEPTIONS
	    std::cerr << "User " << name << " cannot be authorized" << std::endl;
	    goto end;
#else
    
	    std::string source("local account: ");
	    source += 
	      local_account ? std::string(local_account) : std::string("unknown"); 
	    std::string msg(std::string("Cannot authorize") + std::string(name));
    
	    if( name ) { 
	      free(name);          
	      name = NULL; 
	    }
	    if( local_account )	{ 
	      free(local_account);
	      local_account = NULL; 
	    } 
    
	    if( context != GSS_C_NO_CONTEXT ) { 
	      gss_delete_sec_context(&minor_status, &context, GSS_C_NO_BUFFER); 
	      context = GSS_C_NO_CONTEXT; 
	    }
    
	    if (delegated_cred != GSS_C_NO_CREDENTIAL) {
	      gss_release_cred(&minor_status, &delegated_cred);
	      delegated_cred = GSS_C_NO_CREDENTIAL;
	    }      
    
	    throw AuthorizationException( source, ("globus_gss_assist_gridmap()"), msg);

#endif
	  } 
	  else {
	    ctx.certificate_subject = name;  
	  }
#ifndef WITH_GLOBUS_TOOLKIT_2_2
	  /* store delegated credentials (see globus_gatekeeper.c:1562) */
	  if (delegated_cred != GSS_C_NO_CREDENTIAL) {
	    minor_status = 0xdee0;
 	    major_status = gss_inquire_cred(&minor_status,
					    delegated_cred,
					    (gss_name_t *) &delcname,
					    NULL,
					    NULL,
					    NULL);
	    if (!GSS_ERROR(major_status)) {
	      if  (minor_status == 0xdee1 && delcname) {

		char* filename = strchr(delcname,'=');
		if( filename ) filename++;
		else filename = delcname;
		ctx.delegated_credentials_file = std::string(filename);  
		free( delcname );
		delcname = NULL;
	      }
#else
	      /* store delegated credentials (see globus_gatekeeper.c:1683) */
	      if( delegated_cred != GSS_C_NO_CREDENTIAL ) {
		
		gss_buffer_desc deleg_proxy_filename;
		OM_uint32       release_major_status, release_minor_status;
		std::string     proxy_filename;

		major_status = gss_export_cred(&minor_status,
					       delegated_cred,
					       NULL,
					       1,
					       &deleg_proxy_filename);
		
		if (major_status == GSS_S_COMPLETE) {
		  proxy_filename.assign( (char*) deleg_proxy_filename.value );
		}
		
		release_major_status = gss_release_buffer(&release_minor_status, &deleg_proxy_filename);

		if (major_status == GSS_S_COMPLETE) {
		  
		  size_t equal_pos = proxy_filename.find('=');
		  if( equal_pos != std::string::npos ) ctx.delegated_credentials_file = proxy_filename.substr( equal_pos+1 );
		  else ctx.delegated_credentials_file = proxy_filename;
#endif  
		}
	    else   {
#ifndef WITH_SOCKET_EXCEPTIONS
	      globus_gss_assist_display_status(gsi_logfile,
					       "Failed to store delegated credentials...",
					       major_status,
					       minor_status,
					       token_status);
#else      
	      char *gssmsg;
	      globus_gss_assist_display_status_str( &gssmsg, 
						    NULL,
						    major_status,
						    minor_status,
						    token_status);
	      if( context != GSS_C_NO_CONTEXT ) { 
		gss_delete_sec_context(&minor_status, &context, GSS_C_NO_BUFFER); 
		context = GSS_C_NO_CONTEXT; 
	      }
      
	      if (delegated_cred != GSS_C_NO_CREDENTIAL) {
		gss_release_cred(&minor_status, &delegated_cred);
		delegated_cred = GSS_C_NO_CREDENTIAL;
	      }
      
	      if( name ) {		
		free(name);
		name = NULL;
	      }
	      std::string source(gssmsg);
	      free(gssmsg);  
	      throw AuthenticationException(source, std::string("gss_inquire_cred()"),
					    std::string("Failed to store delegated credentials"));
#endif
	      goto end;
	    }
	  }
    
	  return_status = true;
  
	end:
  
	  //  if(return_status == false) {
	  //      if (delegated_cred != GSS_C_NO_CREDENTIAL) {
	  //        if (unlink(filename) < 0) /* some more secure way of destroying is needed */
	  //  	perror("unlink()");
	  //      }
	  //    } 
  
	  if (return_status == false && context != GSS_C_NO_CONTEXT){ 
	    gss_delete_sec_context(&minor_status, &context, GSS_C_NO_BUFFER); 
	    context = GSS_C_NO_CONTEXT; 
	  } 
   
	  if( name )		free(name);
#ifndef WITH_GLOBUS_TOOLKIT_2_2
	  if( delcname )	free(delcname);
#endif
	  if( local_account )	free(local_account); 
	  if (delegated_cred != GSS_C_NO_CREDENTIAL)
	    gss_release_cred(&minor_status, &delegated_cred);
   	
	  return context; 
	}

	/**
	 * Listen for incoming connection requests.
	 * Accept incoming requests and redirect communication on a dedicated port.
	 * @param a a reference to the secure GSI Socket Agent sent by Client.
	 * @return the GSI Socket Agent redirecting communication on a dedicated port.
	 */
	GSISocketAgent* GSISocketServer::Listen()
	{
	  GSISocketAgent*  sa;

	  //do {
    
	  sa = static_cast<GSISocketAgent*>
	    (SocketServer::Listen(new GSISocketAgent));
          
          return sa;
        }

        bool GSISocketServer::AuthenticateAgent(GSISocketAgent* sa) {

          gss_ctx_id_t   context = GSS_C_NO_CONTEXT; 
	  OM_uint32      major_status, minor_status;
          int sd = 0;

	  GSIAuthenticationContext ctx;

	  if (sa) {
            sd = sa -> SocketDescriptor();
	    major_status = globus_gss_assist_acquire_cred(&minor_status,
							  GSS_C_BOTH,
							  &(ctx.credential));
	    if(GSS_ERROR(major_status)) {

	      SocketServer::KillAgent(sa);
	      sa = NULL;
      
#ifndef WITH_SOCKET_EXCEPTIONS
	      globus_gss_assist_display_status(gsi_logfile,
					       "Failed to acquire credentials:",
					       major_status,
					       minor_status,
					       0);
#else
	      char *gssmsg;
              char buff[16];
              sprintf(buff, "%d", sd);
	      globus_gss_assist_display_status_str( &gssmsg, 
						    NULL,
						    major_status,
						    minor_status,
						    0);
      
	      std::string source(gssmsg);
	      free(gssmsg);
	      throw AuthenticationException ( source, std::string("globus_gss_assist_acquire_cred()"),
					      std::string("Failed to acquire credentials on socket #") +
                                              std::string(buff));
#endif
	      return false;
	    }
	  }
  
	  if(sa) {
#ifdef WITH_SOCKET_EXCEPTIONS
	    try {
#endif
	      context = AcceptGSIAuthentication(sa->sck, ctx);
	    
	      if( context == GSS_C_NO_CONTEXT) { 
	      
		gss_release_cred(&minor_status, &(ctx.credential)); 
	      	SocketServer::KillAgent(sa);
	        sa = NULL;
	      } 
	      else {
	      
		sa -> gss_context = context;
	        sa -> credential = ctx.credential;
	        sa -> _delegated_credentials_file = ctx.delegated_credentials_file;
	        sa -> _certificate_subject        = ctx.certificate_subject;
	        sa -> _gridmap_name               = ctx.gridmap_name;
	        sa -> SetSndTimeout(m_auth_timeout);
		sa -> Send(1);
		sa -> SetSndTimeout(-1);
	      }
	      
#ifdef WITH_SOCKET_EXCEPTIONS
	    } 
	    catch(...) { 
	      gss_release_cred(&minor_status, &(ctx.credential));
	      SocketServer::KillAgent(sa);
	      sa = NULL;
	      throw;
	    };
#endif
	  }
	  return sa!=NULL;
	}

} // namespace socket_pp
} // namespace tls
} // namespace wmsutils
} // namespace glite


