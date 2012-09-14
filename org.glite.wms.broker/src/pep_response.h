#ifndef GLITE_GPBOX_PEP_RESPONSE_H
#define GLITE_GPBOX_PEP_RESPONSE_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include "pep_obligation.h"

namespace glite {
namespace wms {
namespace broker {
namespace gpbox {

class Response
{
public:
  Response(
    Answer decision,
    std::string const& resource,
    Obligations const& obligations
  );
  Answer decision() const;
  std::string const& resource() const;
  Obligations const& obligations() const;

private:
  class Impl;
  boost::shared_ptr<Impl> m_impl;
};

typedef std::vector<Response> Responses;

}}}}

#endif
