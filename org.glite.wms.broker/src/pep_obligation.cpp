#include "pep_obligation.h"

namespace glite {
namespace wms {
namespace broker {
namespace gpbox {

struct Obligation::Impl
{
  std::string obligation_id;
  Answer fulFillOn;
  Attributes attributes;
};

Obligation::Obligation(
  std::string const& obligation_id,
  Answer fulFillOn,
  Attributes const& attributes
)
  : m_impl(new Impl)
{
  m_impl->obligation_id = obligation_id;
  m_impl->fulFillOn = fulFillOn;
  m_impl->attributes = attributes;
}

std::string const&
Obligation::obligation_id() const
{
  return m_impl->obligation_id;
}

Answer
Obligation::fulFillOn() const
{
  return m_impl->fulFillOn;
}

Attributes const&
Obligation::attributes() const
{
  return m_impl->attributes;
}

}}}}
