#include "glite/wms/common/logger/key_value.h"
#include <iostream>
#include <boost/regex.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace glite {
namespace logging {

void KeyValue::check_key()
{
  static boost::regex const key_re("^[[:alpha:]][_\\-[:alnum:]]*$");

  if (!regex_match(m_key, key_re)) {
    throw BadKey(m_key);
  }
}

void KeyValue::check_value()
{
  static boost::regex const value_re("^[[:graph:] ]*$");
  static boost::regex const blank_re(" +");

  boost::replace_all(m_value, "\"", "\\\"");
  if (!regex_match(m_value, value_re)) {
    throw BadValue(m_value);
  }
  if (regex_search(m_value, blank_re)) {
    m_value = '"' + m_value + '"';
  }
}

std::string const&
KeyValue::key() const
{
  return m_key;
}

std::string const&
KeyValue::value() const
{
  return m_value;
}

std::ostream& operator<<(std::ostream& os, KeyValue const& kv)
{
  return os << kv.key() << '=' << kv.value();
}

}}
