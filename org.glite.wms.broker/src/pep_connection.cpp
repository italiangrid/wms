#include "pep_connection.h"
#include "pep_request.h"
#include "pep_response.h"
#include "pep_xacml_helper.h"
#include "pep_exceptions.h"

#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <cstdlib>

#include <boost/lexical_cast.hpp>

namespace glite {
namespace wms {
namespace broker {
namespace gpbox {

namespace {

void 
read_timeout(int sock_fd, char* buf, int n, int timeout_sec)
{
  int nread, select_ret;
  struct timeval timeout;
 
  while (n > 0) {
    
    timeout.tv_sec = timeout_sec;
    
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(sock_fd, &read_fds);
    
    if ((select_ret = select(sock_fd + 1, &read_fds, 0, 0, &timeout)) < 0) {
      if (errno == EINTR) {
        continue;
      } else {
        throw ConnectionError(
          "Select failed. Errno " + boost::lexical_cast<std::string>(errno)
        );
      }
    }
    
    if (select_ret) {
      while ((nread = read(sock_fd, buf, n)) < 0) {
        if (errno == EINTR) {
          continue;
        } else {
          throw ConnectionError(
            "Read failed. Errno " + boost::lexical_cast<std::string>(errno)
          );
        }
      }      
      if (nread == 0) {
        throw ConnectionError("Connection closed by peer");
      }
      n -= nread;
      buf += nread;
    } else {
      throw ConnectionError("Connection timed out");
    }
  }
}

void
write_timeout(int sock_fd, char const* buf, int n, int timeout_sec)
{
  int nwrite, select_ret;
  struct timeval timeout;
  
  while (n > 0) {
    
    timeout.tv_sec = timeout_sec;
    
    fd_set write_fds;
    FD_ZERO(&write_fds);
    FD_SET(sock_fd, &write_fds);
    
    if ((select_ret = select(sock_fd + 1, 0, &write_fds, 0, &timeout)) < 0) {
      if (errno == EINTR) {
        continue;
      } else {
        throw ConnectionError(
          "Select failed. Errno " + boost::lexical_cast<std::string>(errno)
        );
      }
    }
    
    if (select_ret) {
      while ((nwrite = write(sock_fd, buf, n)) < 0) {
        if (errno == EINTR) {
          continue;
        } else {
          throw ConnectionError(
            "Write failed. Errno " + boost::lexical_cast<std::string>(errno)
          );
        }
      }
      n -= nwrite;
      buf += nwrite;
      std::cout << nwrite << "\n";
    } else {
      throw ConnectionError("Connection timed out");
    }
  }
}

static std::string const  open_res = "<Responses>";
static std::string const close_res = "</Responses>";

void
add_responses_tag(char* buf, int length)
{
  for (int i = 0; i < open_res.size(); ++i) {
    *buf = open_res[i];
    buf++;
  }
  
  buf += length;
  for (int i = 0; i < close_res.size(); ++i) {
    *buf = close_res[i];
    buf++;
  }
}

}

struct Connection::Impl
{
  int sock_fd;
  struct sockaddr_in pdp_addr;
  bool is_open;
};

Connection::Connection(std::string const& pdp_host,
                       int port,
                       std::string const& cert,
                       bool secure)
  : m_impl(new Impl)
{
  if ((m_impl->sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    throw ConnectionError(
      "Unable to create socket. Errno " + boost::lexical_cast<std::string>(errno)
    );
  }
  struct hostent* host = gethostbyname(pdp_host.c_str());
  if (!host) {
    throw ConnectionError(
      "Host " + pdp_host + " not found"
    );
  }
  memset((void*) &m_impl->pdp_addr, 0, sizeof(m_impl->pdp_addr));
  m_impl->pdp_addr.sin_family = AF_INET;
  m_impl->pdp_addr.sin_port = htons(port);
  memcpy((void*) &m_impl->pdp_addr.sin_addr, host->h_addr, host->h_length);
  m_impl->is_open = false;

  open();
}

Connection::~Connection()
{
  //close();
}

void 
Connection::open()
{
  if (!m_impl->is_open) {
    if (connect(
          m_impl->sock_fd, 
          (struct sockaddr*) &m_impl->pdp_addr, 
          sizeof(m_impl->pdp_addr)
        ) < 0) {
      throw ConnectionError(
        "Unable to open connection. Errno " + boost::lexical_cast<std::string>(errno)
      );
    }
    m_impl->is_open = true;
  }
}

void 
Connection::close()
{
  if (m_impl->is_open) {
    write_timeout(m_impl->sock_fd, "DISCONNECT\n", 11, 5);
    if (::close(m_impl->sock_fd) < 0) {
      throw ConnectionError(
        "Unable to close connection. Errno " + boost::lexical_cast<std::string>(errno)
      );
    }
    m_impl->is_open = false;
  }
}

bool 
Connection::is_open()
{
  return m_impl->is_open;
}

int
read_header(int sock_fd)
{
  const int header_size = 10;
  const int timeout = 5;
  char header_buf[header_size];
  
  read_timeout(sock_fd, header_buf, header_size, timeout);
  std::cout << header_buf << "\n";
  return atoi(header_buf);
}

Buffer
read_body(int sock_fd, int length)
{
  const int timeout = 5;
  Buffer buf(new std::vector<char>(
               length + open_res.size() + close_res.size())
            );
  char buf_c;
  char* start_buf_loca = &(buf->front());
  
  read_timeout(sock_fd, start_buf_loca + open_res.size(), length, timeout);
  read_timeout(sock_fd, &buf_c, 1, timeout);  // a '\n' is appended in the server response
  
  add_responses_tag(start_buf_loca, length);
  
  std::cout << length << " " << buf->size() << "\n";
  
  for (int i = 0; i < buf->size(); i++) {
    std::cout << buf->at(i);
  }
  std::cout << "\n";
  
  return buf;
}

void
write_request(int sock_fd, std::string const& request)
{
  const int header_size = 10;
  const int timeout = 5;

  std::cout << request << request.size() << "\n";
  write_timeout(sock_fd, request.c_str(), request.size(), timeout);
}

boost::shared_ptr<Responses>
Connection::query(Request const& request)
{
  write_request(m_impl->sock_fd, make_request(request));

  int length = read_header(m_impl->sock_fd);

  return boost::shared_ptr<Responses>(get_responses(read_body(m_impl->sock_fd, length)));
}

}}}}
