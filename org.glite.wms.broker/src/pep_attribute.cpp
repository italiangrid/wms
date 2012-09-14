#include "pep_attribute.h"

namespace glite {
namespace wms {
namespace broker {
namespace gpbox {

Attribute::Attribute(
  std::string const& name,
  DataType const& value
)
  : m_name(name), m_value(value)
{
}

std::string const&
Attribute::name() const
{
  return m_name;
}

Attribute::DataType const&
Attribute::value() const
{
  return m_value;
}

}}}}
