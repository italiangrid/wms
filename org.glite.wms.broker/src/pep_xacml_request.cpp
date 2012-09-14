#include "pep_attribute.h"
#include "pep_request.h"

#include <iostream>
#include <sstream>

#include <boost/variant/apply_visitor.hpp>
#include <boost/lexical_cast.hpp>

namespace {

std::string 
start_tag(std::string const& v)
{
  return "<" + v + ">";
}

std::string 
end_tag(std::string const& v)
{
  return "</" + v + ">";
}

std::string 
attribute_entry(
  std::string const& attribute_id,
  std::string const& data_type,
  std::string const& value
)
{
  return "<Attribute AttributeId=\"" + attribute_id + "\""
    + " DataType=\"http://www.w3.org/2001/XMLSchema#" + data_type + "\">"
    + start_tag("AttributeValue") + value + end_tag("AttributeValue") 
    + end_tag("Attribute");
}

}

namespace glite {
namespace wms {
namespace broker {
namespace gpbox {

class GetValueType : public boost::static_visitor<std::string>
{
public:
  std::string operator()(const std::string& str) const { return "string"; }
  std::string operator()(bool v) const { return "boolean"; }
  std::string operator()(int i) const { return "integer"; }
  std::string operator()(URI uri) const { return "anyURI"; }
};

void
add_attributes(std::ostream& ostr, Attributes const& attributes)
{
  std::vector<Attribute>::const_iterator it = attributes.begin();
  std::vector<Attribute>::const_iterator const end = attributes.end();
  
  for ( ; it != end; ++it) {
    ostr << attribute_entry(
      it->name(), 
      boost::apply_visitor(GetValueType(), it->value()),
      boost::lexical_cast<std::string>(it->value())
    );
  }
}

std::string
pack(std::vector<std::string> const& resources)
{
  if (resources.empty()) {
    return std::string();
  }
  std::vector<std::string>::const_iterator i = resources.begin();
  std::vector<std::string>::const_iterator const end = resources.end();
  std::string result(*i);
  ++i;
  for ( ; i != end; ++i) {
    result += '#';
    result += *i;
  }
  return result;
}

void
add_subject_entry(std::ostream& ostr, Request const& request)
{
  const std::string subject = "Subject";
  const std::string attribute_id = "urn:oasis:names:tc:xacml:1.0:subject:subject-id";
  const std::string data_type = "string";
  
  ostr << start_tag(subject);
  ostr << attribute_entry(attribute_id, data_type, request.subject());
  add_attributes(ostr, request.attributes(Request::SUBJECT));
  ostr << end_tag(subject);
}

void
add_resource_entry(std::ostream& ostr, Request const& request)
{
  const std::string resource = "Resource";
  const std::string attribute_id = "urn:oasis:names:tc:xacml:1.0:resource:resource-id";
  const std::string data_type = "string";
  
  ostr << start_tag(resource);
  ostr << attribute_entry(attribute_id, data_type, pack(request.resources()));
  add_attributes(ostr, request.attributes(Request::RESOURCE));
  ostr << end_tag(resource);
}

void
add_action_entry(std::ostream& ostr, Request const& request)
{
  const std::string action = "Action";
  const std::string attribute_id = "urn:oasis:names:tc:xacml:1.0:action:action-id";
  const std::string data_type = "string";
  
  ostr << start_tag(action);
  ostr << attribute_entry(attribute_id, data_type, request.action());
  add_attributes(ostr, request.attributes(Request::ACTION));
  ostr << end_tag(action);
}

std::string
make_request(Request const& request)
{
  std::ostringstream ostr;

  ostr << "XACMLREQ ";
  ostr << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

  ostr << "<Request "
       << "xmlns=\"urn:oasis:names:tc:xacml:1.0:context\" "
       << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
       << "xsi:schemaLocation=\"urn:oasis:names:tc:xacml:1.0:context cs-xacml-schema-context-01.xsd\">";

  add_subject_entry(ostr, request);

  add_resource_entry(ostr, request);

  add_action_entry(ostr, request);
  
  ostr << end_tag("Request")
       << "\n";
  
  return ostr.str();
}

}}}}
