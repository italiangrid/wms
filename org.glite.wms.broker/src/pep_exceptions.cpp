#include "pep_exceptions.h"

#include <string>

namespace glite {
namespace wms {
namespace broker {
namespace gpbox {

struct PEPError::Impl
{
  std::string msg;
};

PEPError::PEPError(std::string const& err_msg)
  : m_impl(new Impl)
{
  m_impl->msg = err_msg;
}

PEPError::~PEPError() throw()
{
}

char const*
PEPError::what() const throw()
{
  return m_impl->msg.c_str();
}

ParseError::ParseError(std::string const& err_msg)
  : PEPError(err_msg)
{
}

ConnectionError::ConnectionError(std::string const& err_msg)
  : PEPError(err_msg)
{
}

}}}}
