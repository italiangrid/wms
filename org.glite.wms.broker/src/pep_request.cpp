#include "pep_request.h"

namespace glite {
namespace wms {
namespace broker {
namespace gpbox {

struct Request::Impl
{
  std::vector<std::string> resources;
  std::string action;
  std::string subject;
  Attributes attributes[3];
};

Request::Request(
  std::vector<std::string> const& resources,
  std::string const& action,
  std::string const& subject
)
  : m_impl(new Impl)
{
  m_impl->resources = resources;
  m_impl->action = action;
  m_impl->subject = subject;
}

Request&
Request::attributes(Attributes const& attributes, Scope scope)
{
  m_impl->attributes[scope] = attributes;
  return *this;
}

std::vector<std::string> const&
Request::resources() const
{
  return m_impl->resources;
}

std::string const&
Request::subject() const
{
  return m_impl->subject;
}

std::string const&
Request::action() const
{
  return m_impl->action;
}

Attributes const&
Request::attributes(Scope scope) const
{
  return m_impl->attributes[scope];
}

}}}}
