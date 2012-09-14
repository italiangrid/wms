/**
 * @file tokens.cpp
 * @brief The implementation for token transmission and reception.
 * This file implements a couple of methods providing functionality
 * to send and receive tokens.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */

// $Id$

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <globus_gss_assist.h>

#include "glite/wmsutils/tls/socket++/exceptions.h"

const int _TIMEOUT_ = 300;

namespace socket_pp = glite::wmsutils::tls::socket_pp;

namespace {

bool is_recv_pending(int sck, int to)
{
  fd_set readfs;
  struct timeval timeout;
  timeout.tv_sec=to;
  timeout.tv_usec=0;
  FD_ZERO(&readfs);
  FD_SET(sck,&readfs); 
  int result(
    select(sck+1,&readfs,0,0,to<0?0:&timeout)
  );
  return result==1;
}

bool is_send_pending(int sck, int to)
{
  fd_set sendfs;
  struct timeval timeout;
  timeout.tv_sec=to;
  timeout.tv_usec=0;
  FD_ZERO(&sendfs);
  FD_SET(sck,&sendfs);
   int result(
    select(sck+1,0,&sendfs,0,to<0?0:&timeout)
  );
  return result==1;
}

}
/**
 * Send a gss token.
 * This method send gss tokens using GSI socket objects.
 * @param arg pointer to the descriptor of the socket.
 * @param token pointer to the token buffer to be sent.
 * @param token_length token buffer length
 * @returns the number of bytes sent, or -1 on failure.
 */
int send_token(void *arg, void *token, size_t token_length)
{
    size_t			num_written = 0;
    ssize_t			n_written;
    int 			fd = ((std::pair<int,int>*)arg)->first;
    int				to = ((std::pair<int,int>*)arg)->second;
    unsigned char		token_length_buffer[4];

    if( !token ) { 
        char msg[16];
	sprintf(msg,"socket #%d", fd);
	throw socket_pp::IOException(std::string(msg), 
				     std::string("send_token()"), 
				     std::string("Unable to send data")); 
    }

    /* encode the token length in network byte order: 4 byte, big endian */
    token_length_buffer[0] = (unsigned char) ((token_length >> 24) & 0xff);
    token_length_buffer[1] = (unsigned char) ((token_length >> 16) & 0xff);
    token_length_buffer[2] = (unsigned char) ((token_length >>  8) & 0xff);
    token_length_buffer[3] = (unsigned char) ((token_length      ) & 0xff);

    /* send the token length */
    while (num_written < 4 && 
      is_send_pending(fd, to))
    {
      n_written = send(fd, token_length_buffer + num_written, 4 - num_written,0);
      
      if(n_written < 0)
	{
	  if(errno == EINTR)
	    continue;
	  else
	    return -1;
	}
      else
	num_written += n_written;
    }

    if(num_written < 4) return -1;

    /* send the token */

    num_written = 0;
    while (num_written < token_length &&
      is_send_pending(fd,to))
    {
       
       n_written = send(fd, ((u_char *)token) + num_written, token_length - num_written,0);
       
       if(n_written < 0)
	 {
	   if(errno == EINTR)
		continue;
	   else
	     return -1;
	 }
       else
	 num_written += n_written;
    }
 
    if(num_written < token_length) return -1;  
    return 0;
}

/**
 * Receive a gss token.
 * This method receives gss tokens using GSI socket objects.
 * @param arg pointer to the descriptor of the socket.
 * @param token pointer to the token buffer to fill with received token.
 * @param token_length token buffer length
 * @returns the number of bytes recieved, or -1 on failure.
 */
int get_token(void *arg, void **token, size_t *token_length)
{
    size_t			num_read = 0;
    ssize_t			n_read;
    int 			fd = ((std::pair<int,int>*)arg)->first;
    int 			to = ((std::pair<int,int>*)arg)->second;
    unsigned char		token_length_buffer[4];
    
    while (num_read < 4 &&
      is_recv_pending(fd, to))
    {
      
      n_read = recv(fd,token_length_buffer + num_read, 4 - num_read,0);
	
      if(n_read < 0)
	{
	    if(errno == EINTR)
		continue;
	    else
		return -1;
	}
        else if (n_read == 0)
                return GLOBUS_GSS_ASSIST_TOKEN_EOF;
	else
	    num_read += n_read;
    }

    if(num_read < 4) return -1;
    num_read = 0;
    /* decode the token length from network byte order: 4 byte, big endian */

    *token_length  = ((size_t) token_length_buffer[0]) << 24;
    *token_length |= ((size_t) token_length_buffer[1]) << 16;
    *token_length |= ((size_t) token_length_buffer[2]) <<  8;
    *token_length |= ((size_t) token_length_buffer[3]);

    if(*token_length > 1<<24)
    {
	/* token too large */
	return -1;
    }

    /* allocate space for the token */

    *((void **)token) = (void *) malloc(*token_length);

    if(*token == NULL)
    {
	return -1;
    }

    /* receive the token */

    num_read = 0;
    while (num_read < *token_length &&
      is_recv_pending(fd,to))  
    {
      n_read = recv(fd, ((u_char *) (*token)) + num_read,(*token_length) - num_read,0);
	
	if(n_read < 0)
	{
	    if(errno == EINTR)
		continue;
	    else
		return -1;
	}
	else
	    if(n_read == 0) return -1; 
	    num_read += n_read;
    }

    if(num_read<*token_length) return -1;
    return 0;
}

