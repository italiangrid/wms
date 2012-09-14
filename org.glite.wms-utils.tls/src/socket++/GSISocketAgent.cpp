// $Id:

/**
 * @file GSISocketAgent.cpp
 * @brief The implementation for secure Socket Agent Object.
 * This file contains implementations for secure shell based Socket Agent used for message
 * exchange between Client and Server.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */

#include <memory.h>
#include <time.h>
#include <errno.h>
#include <string>
#include <unistd.h>

/** This class header file. */
#include "glite/wmsutils/tls/socket++/GSISocketAgent.h"
/** The tokens transmission and reception functionality definitions file. */
#include "tokens.h"

#ifdef WITH_SOCKET_EXCEPTIONS
#include "glite/wmsutils/tls/socket++/exceptions.h"
#endif

namespace glite {
namespace wmsutils {
namespace tls {
namespace socket_pp {

/**
 * Constructor.
 */
GSISocketAgent::GSISocketAgent() : SocketAgent()
{
  gss_context = GSS_C_NO_CONTEXT;
  credential = GSS_C_NO_CREDENTIAL;
  _delegated_credentials_file = "";
  _certificate_subject = "";
}

/**
 * Destructor.
 */
GSISocketAgent::~GSISocketAgent()
{
  OM_uint32 minor_status;
  gss_release_cred(&minor_status, &credential);

  gss_delete_sec_context(&minor_status, 
			 &gss_context, 
			 GSS_C_NO_BUFFER);
  if (gss_context != GSS_C_NO_CONTEXT) free(gss_context);

  gss_context = GSS_C_NO_CONTEXT;
  if(!_delegated_credentials_file.empty() ) {
    
    unlink( _delegated_credentials_file.c_str() );
  }	
}

/**
 * Send a int value.
 * @param i the int value to send.
 * @return true on success, false otherwise.
 */ 
bool GSISocketAgent::Send(int i)
{
  bool return_status = true;

  unsigned char         int_buffer[4];

  int_buffer[0] = (unsigned char) ((i >> 24) & 0xff);
  int_buffer[1] = (unsigned char) ((i >> 16) & 0xff);
  int_buffer[2] = (unsigned char) ((i >>  8) & 0xff);
  int_buffer[3] = (unsigned char) ((i      ) & 0xff);

  if(return_status = !(gss_context == GSS_C_NO_CONTEXT)) {

    gss_buffer_desc  input_token;
    gss_buffer_desc  output_token;
    OM_uint32        maj_stat, min_stat;
    input_token.value = (void*)int_buffer;
    input_token.length = 4; /* ??? */
    std::pair<int,int> arg(sck, m_send_timeout);
    /* set this flag to 0 if you want to encrypt messages. set it to nonzero
       if only integrity protection is requested */

    int conf_req_flag = 0;

    maj_stat = gss_wrap (&min_stat,
			 gss_context,
			 conf_req_flag,
			 GSS_C_QOP_DEFAULT,
			 &input_token,
			 NULL,
			 &output_token);

    return_status = !GSS_ERROR(maj_stat) &&
      !send_token((void*)&arg, output_token.value, output_token.length);

    gss_release_buffer(&min_stat, &output_token);
  }    
#ifdef WITH_SOCKET_EXCEPTIONS
  if( !return_status ) {
    char buf[32];
    sprintf(buf,"socket #%d", sck);
    throw IOException(std::string(buf), std::string("recv()"), std::string("Unable to receive"));
  }
#endif

  return return_status;
}

/**
 * Send a string value.
 * @param s the string value to send.
 * @return true on success, false otherwise.
 */ 
bool GSISocketAgent::Send(const std::string& s)
{
  bool return_status = true;

  if(return_status = !(gss_context == GSS_C_NO_CONTEXT)) {
	
    gss_buffer_desc  input_token;
    gss_buffer_desc  output_token;
    OM_uint32        maj_stat, min_stat;
    input_token.value = (void*)s.c_str();
    input_token.length = s.length() + 1; /* ??? */
    std::pair<int,int> arg(sck, m_send_timeout); 
    /* set this flag to 0 if you want to encrypt messages. set it to nonzero
       if only integrity protection is requested */
     
    int conf_req_flag = 0;
     
    maj_stat = gss_wrap (&min_stat,
			 gss_context,
			 conf_req_flag,
			 GSS_C_QOP_DEFAULT,
			 &input_token,
			 NULL,
			 &output_token);
     
    return_status = !GSS_ERROR(maj_stat) &&
      !send_token((void*)&arg, output_token.value, output_token.length);
     
    gss_release_buffer(&min_stat, &output_token);
  }	
#ifdef WITH_SOCKET_EXCEPTIONS
  if( !return_status ) {
    char buf[32];
    sprintf(buf,"socket #%d", sck);
    throw IOException (std::string(buf), std::string("send()"), std::string("Unable to send data"));
  } 
#endif
 
  return return_status;
}

/**
 * Receive an int value.
 * @param i an int to fill.
 * @return true on success, false otherwise.
 */
bool GSISocketAgent::Receive(int& i)
{
  unsigned char int_buffer[4];
  
  bool return_status = true;
  OM_uint32 maj_stat, min_stat;
  gss_buffer_desc input_token;
  gss_buffer_desc output_token;

  input_token.value = NULL;
  std::pair<int,int> arg(sck, m_recv_timeout);
  if(return_status = !(gss_context == GSS_C_NO_CONTEXT ||
		       get_token(&arg, &input_token.value, &input_token.length) != 0)) {

    maj_stat = gss_unwrap (&min_stat,
			   gss_context,
			   &input_token,
			   &output_token,
			   NULL,
			   NULL);
    if( (return_status = !GSS_ERROR(maj_stat)) ) {

      memcpy((char*)int_buffer, output_token.value, output_token.length);
	
      i  = ((unsigned int) int_buffer[0]) << 24;
      i |= ((unsigned int) int_buffer[1]) << 16;
      i |= ((unsigned int) int_buffer[2]) <<  8;
      i |= ((unsigned int) int_buffer[3]);

    } 
    gss_release_buffer(&min_stat, &output_token);
    gss_release_buffer(&min_stat, &input_token);
  }
#ifdef WITH_SOCKET_EXCEPTIONS
  if( !return_status ) {
    char buf[32];
    sprintf(buf,"socket #%d", sck);
    throw IOException( std::string(buf),
		       std::string("recv()"),
		       std::string("Unable to receive data") );
  } 
#endif

  return return_status;
}

/**
 * Receive a string value.
 * @param s the string to fill.
 * @return true on success, false otherwise.
 */
bool GSISocketAgent::Receive(std::string& s)
{
  bool return_status = true;
	
  OM_uint32 maj_stat, min_stat;
  gss_buffer_desc input_token;
  gss_buffer_desc output_token;
  std::pair<int,int> arg(sck, m_recv_timeout);
  if(return_status = !(gss_context == GSS_C_NO_CONTEXT ||
		       get_token(&arg, &input_token.value, &input_token.length) != 0)) {
	  
    maj_stat = gss_unwrap (&min_stat,
			   gss_context,
			   &input_token,
			   &output_token,
			   NULL,
			   NULL);
	  
    if (return_status = !GSS_ERROR(maj_stat)) {

      char *buf = new char[output_token.length + 1];
      memset(buf, 0, output_token.length + 1);
      memcpy(buf, output_token.value, output_token.length);
      s = std::string(buf);
      delete[] buf;
    }
	  
    gss_release_buffer(&min_stat, &output_token);
    gss_release_buffer(&min_stat, &input_token);
  }
#ifdef WITH_SOCKET_EXCEPTIONS
  if( !return_status ) {
    char buf[32];
    sprintf(buf,"socket #%d", sck);	
    throw IOException (std::string(buf), std::string("recv()"), std::string("Unable to receive data"));
  } 
#endif

  return return_status;
}

} // namespace socket_pp
} // namespace tls
} // namespace wmsutils
} // namespace glite





























