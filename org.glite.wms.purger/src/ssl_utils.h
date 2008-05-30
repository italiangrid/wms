// File: ssl_utils.h
// Author: Salvatore Monforte

// $Id$

#include <string>

namespace sslutils
{

bool
proxy_expires_within(
  std::string const& x509_proxy, 
  time_t seconds
);

bool
proxy_init(
  std::string const& certfile,
  std::string const& keyfile,
  std::string const& outfile,
  time_t seconds
);

} // namespace sslutils closure
