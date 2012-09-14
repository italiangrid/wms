#ifndef GLITE_GPBOX_PEP_REQUEST_H
#define GLITE_GPBOX_PEP_REQUEST_H

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>

#include "pep_attribute.h"

namespace glite {
namespace wms {
namespace broker {
namespace gpbox {

class Request
{
public:
  enum Scope {
    SUBJECT,
    RESOURCE,
    ACTION
  };
  
  Request(
    std::vector<std::string> const& resources,
    std::string const& action,
    std::string const& subject
  );
  Request& attributes(Attributes const& attributes, Scope scope);
  std::vector<std::string> const& resources() const;
  std::string const& subject() const;
  std::string const& action() const;
  Attributes const& attributes(Scope scope) const;
  
private:
  class Impl;
  boost::shared_ptr<Impl> m_impl;
};

}}}}

#endif
