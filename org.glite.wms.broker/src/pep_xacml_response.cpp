#include <map>
#include <string>
#include <stack>

#include <boost/lexical_cast.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/util/XMLString.hpp>

#include "pep_attribute.h"
#include "pep_request.h"
#include "pep_response.h"
#include "pep_xacml_helper.h"
#include "pep_exceptions.h"

XERCES_CPP_NAMESPACE_USE

namespace glite {
namespace wms {
namespace broker {
namespace gpbox {

Answer 
string_to_answer(std::string const& in_string)
{
  if (in_string == "Permit") {
    return PERMIT;
  } else if (in_string == "Deny") {
    return DENY;
  } else if (in_string == "NotApplicable") {
    return NOT_APPLICABLE;
  } else {
    return INDETERMINATE;
  }
}

std::string 
get_string_from(XMLCh const* const XMLBuf)
{
  char* buf_ptr = XMLString::transcode(XMLBuf);
  std::string buf_string = buf_ptr;
  XMLString::release(&buf_ptr);
  
  return buf_string;
}

Attribute::DataType
create_type(
  std::string const& type,
  std::string const& value
)
{
  if (type == "http://www.w3.org/2001/XMLSchema#string") {
    return boost::lexical_cast<std::string>(value);
  } else if (type == "http://www.w3.org/2001/XMLSchema#integer") {
    return boost::lexical_cast<int>(value);
  } else if (type == "http://www.w3.org/2001/XMLSchema#anyURI") {
    return boost::lexical_cast<URI>(value);
  } else if (type == "http://www.w3.org/2001/XMLSchema#boolean") {
    return boost::lexical_cast<bool>(value);
  } else {
    throw ParseError("create_type error: undefined DataType: " + type);
  }
}

XMLCh const*
get_attribute(std::string const& name, AttributeList& attr)
{
  XMLCh const* buf = attr.getValue(name.c_str());
  if (!buf) throw ParseError("Needed attribute " + name + " not found");

  return buf;
}

class ParseHandler : public HandlerBase
{
public:
  ParseHandler() : m_responses(new Responses) { }
  void startElement(XMLCh const* const name, AttributeList& attr)
  {
    std::string tag = get_string_from(name);
    m_tag_level.push(tag);

    if (tag == "Response") {
      m_decision = string_to_answer("NotApplicable");
      m_resource = "Not defined";
      m_obligations.clear();
    } else if (tag == "Result") {
      m_resource = get_string_from(
        get_attribute("ResourceId", attr)
      );
    } else if (tag == "Obligation") {
      m_obligation.attributes.clear();
      m_obligation.obligation_id = get_string_from(
        get_attribute("ObligationId", attr)
      );
      m_obligation.ful_fill_on = string_to_answer(
        get_string_from(get_attribute("FulfillOn", attr))
      );
    } else if (tag == "AttributeAssignment") {
      m_attribute.attribute_id = get_string_from(
        get_attribute("AttributeId", attr)
      );
      m_attribute.type = get_string_from(
        get_attribute("DataType", attr)
      );
    }
  }
  
  void endElement(XMLCh const* const name)
  {
    std::string tag = get_string_from(name);
    m_tag_level.pop();
    
    if (tag == "Obligation") {
      m_obligations.push_back(
        Obligation(
          m_obligation.obligation_id,
          m_obligation.ful_fill_on,
          m_obligation.attributes
        )
      );
    } else if (tag == "AttributeAssignment") {
      m_obligation.attributes.push_back(
        Attribute(
          m_attribute.attribute_id,
          create_type(m_attribute.type, m_attribute.value)
        )
      );                                       
    } else if (tag == "Response") {
      m_responses->push_back(
        Response(m_decision, m_resource, m_obligations)
      );
    }
  }
  
  void characters(XMLCh const* const name, const unsigned int len)
  {
    std::string tag = m_tag_level.top();

    if (tag == "Decision") {
      m_decision = string_to_answer(get_string_from(name));
    } else if (tag == "AttributeAssignment") {
      m_attribute.value = get_string_from(name);
    }
  }

  void error(SAXParseException const& e)
  {
    throw ParseError("Parse error in " + m_tag_level.top() + " tag");
  }

  void fatalError(SAXParseException const& e)
  {
    throw ParseError("Fatal parse error in " + m_tag_level.top() + " tag");
  }

  boost::shared_ptr<Responses>
  responses()
  {
    return m_responses;
  }

private:
  Answer m_decision;
  std::string m_resource;
  Obligations m_obligations;
  
  struct 
  {
    std::string obligation_id;
    Answer ful_fill_on;
    Attributes attributes;
  } 
  m_obligation;
  
  struct
  {
    std::string attribute_id;
    std::string type;
    std::string value;
  }
  m_attribute;
  
  boost::shared_ptr<Responses> m_responses;
  std::stack<std::string> m_tag_level;
};

boost::shared_ptr<Responses>
get_responses(Buffer const& pdp_answer)
{
  XMLPlatformUtils::Initialize();
  std::auto_ptr<SAXParser> parser(new SAXParser());
  ParseHandler handler;
  MemBufInputSource buffer(
    (const XMLByte*) &(pdp_answer->front()), 
    (unsigned int) pdp_answer->size(),
    "XMLBuffer"
  );
  
  parser->setDocumentHandler(&handler);
  parser->setErrorHandler(&handler);
  
  parser->parse(buffer);
  return handler.responses();
}

}}}}
