#include "pep_response.h"

namespace glite {
namespace wms {
namespace broker {
namespace gpbox {

struct Response::Impl
{
  Answer decision;
  std::string resource;
  Obligations obligations;
};

Response::Response(
  Answer decision,
  std::string const& resource,
  Obligations const& obligations
)
  : m_impl(new Impl)
{
  m_impl->decision = decision;
  m_impl->resource = resource;
  m_impl->obligations = obligations;
}

Answer
Response::decision() const
{
  return m_impl->decision;
}

std::string const&
Response::resource() const
{
  return m_impl->resource;
}

Obligations const&
Response::obligations() const
{
  return m_impl->obligations;
}

}}}}
