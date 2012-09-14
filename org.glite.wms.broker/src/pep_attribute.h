#ifndef GLITE_GPBOX_PEP_ATTRIBUTES_H
#define GLITE_GPBOX_PEP_ATTRIBUTES_H

#include <string>
#include <vector>
#include <iostream>

#include <boost/variant/variant.hpp>

namespace glite {
namespace wms {
namespace broker {
namespace gpbox {

class URI
{
public:
  URI() {}
  URI(std::string const& value)
    : m_value(value) {}
  std::string value() const { return m_value; }
  friend std::istream& operator>>(std::istream& is, URI& uri);
  
private:
  std::string m_value;
};

inline std::istream& operator>>(std::istream& is, URI& uri)
{
  return is >> uri.m_value;
}
inline std::ostream& operator<<(std::ostream& os, URI const& uri)
{
  return os << uri.value();
}

class Attribute
{
public:
  typedef boost::variant<
    std::string,
    bool,
    int,
    URI
    > DataType;  
  
  Attribute(std::string const& name, DataType const& value);
  std::string const& name() const;
  DataType const& value() const;
  
private:
  std::string m_name;
  DataType m_value;
};

typedef std::vector<Attribute> Attributes;

}}}}

#endif
