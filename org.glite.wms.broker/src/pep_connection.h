#ifndef GLITE_GPBOX_PEP_CONNECTION_H
#define GLITE_GPBOX_PEP_CONNECTION_H

#include "pep_response.h"

#include <string>

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

namespace glite {
namespace wms {
namespace broker {
namespace gpbox {

class Request;

class Connection : boost::noncopyable
{
public:
  Connection(std::string const& pdp_host, int  port, std::string const& cert, bool secure);
  ~Connection();
  void open();
  void close();
  bool is_open();
  boost::shared_ptr<Responses> query(Request const& request);
  
private:
  class Impl;
  boost::shared_ptr<Impl> m_impl;
};

}}}}

#endif
