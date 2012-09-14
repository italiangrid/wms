/*
 * SocketAgent.cpp
 * 
 * Copyright (C) 2002 EU Datagrid
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

/**
 * @file SocketAgent.cpp
 * @brief The file for Socket Agent Object.
 * This file contains implementations for Socket Agent used for message
 * exchange between Client and Server.
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 * @author Marco Pappalardo marco.pappalardo@ct.infn.it
 * @author comments by Marco Pappalardo marco.pappalardo@ct.infn.it and Salvatore Monforte
 */

#include <cstdio>
#include <unistd.h>
#include <memory.h>
#include <sys/time.h>
#include <errno.h>
#include <arpa/inet.h>
/** This class header file. */
#include "glite/wmsutils/tls/socket++/SocketAgent.h"

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
SocketAgent::SocketAgent()
{
  memset ((char *)&peeraddr_in, 0, sizeof(struct sockaddr_in));
  m_recv_timeout = m_send_timeout = -1;
}

/**
 * Destructor.
 */
SocketAgent::~SocketAgent()
{
  close(sck);
}

bool SocketAgent::is_recv_pending()
{
  fd_set readfs;
  struct timeval timeout;
  timeout.tv_sec=m_recv_timeout;
  timeout.tv_usec=0;
  FD_ZERO(&readfs);
  FD_SET(sck,&readfs);
  int result(
    select(sck+1,&readfs,0,0,m_recv_timeout<0?0:&timeout)
  );
  return result==1;
}

bool SocketAgent::is_send_pending()
{
  fd_set sendfs;
  struct timeval timeout;
  timeout.tv_sec=m_send_timeout;
  timeout.tv_usec=0;
  FD_ZERO(&sendfs);
  FD_SET(sck,&sendfs);
   int result(
    select(sck+1,0,&sendfs,0,m_send_timeout<0?0:&timeout)
  );
  return result==1;
}

/**
 * Set the connection timeout.
 * @param secs a size_t representing the timeout in seconds.
 * @return tru on success, false otherwise.
 */
bool SocketAgent::SetTimeout( int secs )
{
   m_send_timeout = m_recv_timeout = secs;
 
   return true;
}

bool SocketAgent::SetSndTimeout( int secs )
{  
   m_send_timeout = secs;
   return true;
}

bool SocketAgent::SetRcvTimeout( int secs )
{
  m_recv_timeout = secs;
  return true;  
}

/**
 * Send a string value.
 * @param s the string value to send.
 * @return true on success, false otherwise.
 */ 
bool SocketAgent::Send(int i)
{  
  unsigned char		int_buffer[4];

  int_buffer[0] = (unsigned char) ((i >> 24) & 0xff);
  int_buffer[1] = (unsigned char) ((i >> 16) & 0xff);
  int_buffer[2] = (unsigned char) ((i >>  8) & 0xff);
  int_buffer[3] = (unsigned char) ((i      ) & 0xff);
  return sendbuffer((char*)int_buffer,4);
}

/**
 * Sends a long value.
 * @param l the value to be sent.
 * @return true on success, false otherwise.
 */
bool SocketAgent::Send(long l)
{
  unsigned char long_buffer[8];
 
  for( int i=0 ; i<8; i++) 
  	long_buffer[i] = (unsigned char) ((l >> ((sizeof(l)-1 -i)*8)) & 0xff);
 
  return sendbuffer((char*)long_buffer,8);
}

/**
 * Send a string value.
 * @param s the string value to send.
 * @return true on success, false otherwise.
 */ 
bool SocketAgent::Send(const std::string& s)
{
  return ( Send( (int) (s.length()) ) &&
    sendbuffer(const_cast<char*>(s.c_str()), s.length()) );
}

/**
 * Receive an int value.
 * @param i an int to fill.
 * @return true on success, false otherwise.
 */
bool SocketAgent::Receive( int& i )
{
  bool result;
  
  unsigned char int_buffer[4];
  
  if( result = readbuffer((char*)int_buffer,4))  {
    i  = (((unsigned int) int_buffer[0]) << 24 ) & 0xffffffff;
    i |= (((unsigned int) int_buffer[1]) << 16 ) & 0xffffffff;
    i |= (((unsigned int) int_buffer[2]) <<  8 ) & 0xffffffff;
    i |= (((unsigned int) int_buffer[3])       ) & 0xffffffff;
    
  }
  return result;
}

bool SocketAgent::Receive( long& l )
{
  bool result;
  l=0;
  unsigned char long_buffer[8];
  
  if( result = readbuffer((char*)long_buffer,8))  {
    for (int i=0; i<8; i++) {
      l  |= (((unsigned long) long_buffer[i]) << ((sizeof(l)-1-i)*8)) & ((1L << (sizeof(l)*8) ) - 1);
    }     
  }
  return result;
}

/**
 * Receive a string value.
 * @param s the string to fill.
 * @return true on success, false otherwise.
 */
bool SocketAgent::Receive(std::string& s)
{
  bool result;

  int length = 0;
  
  if( result = Receive(length) ) {
   
    char *buf = new  char[length+1];
    memset ((char *)buf, 0, length+1);
    if( result = readbuffer(buf, length) ) {
   
      s = std::string(buf);
    }
    delete buf;
  }
  
  return result;
}

/**
 * Send chars of a buffer. The max number of chars to be sent is fixed by
 * size parameter. 
 * @param buf the transmission buffer.
 * @param size teh max number of chars to be sent.
 */
bool SocketAgent::sendbuffer(char* buf, unsigned int size)
{
  bool result = true;
  
  unsigned int tot_written = 0;
  int num_written = 0;
  
  while(tot_written < size &&
        is_send_pending() ) {
    
    num_written = send(sck, buf + tot_written, size - tot_written,0);
    
    if(num_written < 0) {
      
      if(errno == EINTR)
	continue;
      else {
	
	result = false;
	break;
      }
    }
    else
      tot_written += num_written;
  } 

  if (tot_written < size) result=false;

#ifdef WITH_SOCKET_EXCEPTIONS
  if( !result ) {
    
    char msg[32];
    printf(msg,"socket #%d", sck);
    throw IOException ( std::string(buf), 
			std::string("sendbuffer()"), 
			std::string("Unable to send data"));
  }
#endif
  return result;
}

/**
 * Receive chars in a buffer. The max number of chars to be received is fixed by
 * size parameter. 
 * @param buf the reception buffer.
 * @param size teh max number of chars to be received.
 */
bool SocketAgent::readbuffer(char* buf, unsigned int size) 
{
  bool result = true;
  
  int num_read = 0;
  unsigned int tot_read = 0;

  while(tot_read < size &&
        is_recv_pending())
  {
    
    num_read = recv(sck,buf + tot_read, size - tot_read,0);
    
    if(num_read < 0) {
      if(errno == EINTR)
	continue;
      else {
	result = false;
	break;
      }
    } 
    else {
      if(num_read==0) { result = false; break;}
      tot_read += num_read;
    }
  }

  if (tot_read < size) result = false;

  #ifdef WITH_SOCKET_EXCEPTIONS
  if( !result ) {
     char msg[32];
    sprintf(msg,"socket #%d", sck);
    throw IOException ( std::string(msg), 
			std::string("readbuffer()"), 
			std::string("Unable to read data")); 
  } 
#endif

  return result;
}

/**
 * Returns the host name.
 * @param the string to fill with host name.
 */
std::string SocketAgent::PeerName()
{

  struct hostent*
    hp = gethostbyaddr ((char *) &peeraddr_in.sin_addr,
			sizeof (struct in_addr),
			peeraddr_in.sin_family);
  if(hp) return std::string(hp->h_name);
  else return PeerAddr();
}

/**                                                                                               
 * Returns peer entity ip address.                                                                
 * @return peer entity ip address.                                                                
 */ 
std::string SocketAgent::PeerAddr() { 
  return std::string(inet_ntoa(peeraddr_in.sin_addr));
} 
 
/**                                                                                               
 * Returns peer entity ip port.                                                                   
 * @return peer entity ip port.                                                                   
 */ 
int SocketAgent::PeerPort() {
  return peeraddr_in.sin_port; 
} 

} // namespace socket_pp
} // namespace tls
} // namespace wmsutils
} // namespace glite

