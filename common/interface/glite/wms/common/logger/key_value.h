#ifndef GLITE_LOGGING_KEY_VALUE_H
#define GLITE_LOGGING_KEY_VALUE_H

#include <string>
#include <iosfwd>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace glite {
namespace logging {

struct BadKey: public std::runtime_error
{
  BadKey(std::string const& key)
    : std::runtime_error("bad key (" + key + ")")
  {
  }
};

struct BadValue: public std::runtime_error
{
  BadValue(std::string const& value)
    : std::runtime_error("bad value (" + value + ")")
  {
  }
};

class KeyValue
{
  std::string m_key;
  std::string m_value;

public:
  template<typename V>
  KeyValue(std::string const& k, V const& v)
    : m_key(k), m_value(boost::lexical_cast<std::string>(v))
  {
    check_key();
    check_value();
  }
  std::string const& key() const;
  std::string const& value() const;

private:
  void check_key();
  void check_value();
};

std::ostream& operator<<(std::ostream& os, KeyValue const& kv);

}}

#endif
