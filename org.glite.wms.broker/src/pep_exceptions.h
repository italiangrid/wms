#ifndef GLITE_GPBOX_PEP_EXCEPTIONS_H
#define GLITE_GPBOX_PEP_EXCEPTIONS_H

#include <string>
#include <exception>

#include <boost/shared_ptr.hpp>

namespace glite {
namespace wms {
namespace broker {
namespace gpbox {

class PEPError : public std::exception
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;
public:
  PEPError(std::string const& err_msg);
  virtual ~PEPError() throw();
  virtual char const* what() const throw();
};

class ParseError : public PEPError
{
public:
  ParseError(std::string const& err_msg);
};

class ConnectionError : public PEPError
{
public:
  ConnectionError(std::string const& err_msg);
};

}}}}

#endif
