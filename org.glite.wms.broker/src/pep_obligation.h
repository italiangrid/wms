#ifndef GLITE_GPBOX_PEP_OBLIGATION_H
#define GLITE_GPBOX_PEP_OBLIGATION_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "pep_attribute.h"

namespace glite {
namespace wms {
namespace broker {
namespace gpbox {

enum Answer {
  PERMIT,
  DENY,
  INDETERMINATE,
  NOT_APPLICABLE
};

class Obligation
{
public:
  Obligation(
    std::string const& obligation_id,
    Answer fulFillOn,
    Attributes const& attributes
  );
  std::string const& obligation_id() const;
  Answer fulFillOn() const;
  Attributes const& attributes() const;

private:
  class Impl;
  boost::shared_ptr<Impl> m_impl;
};

typedef std::vector<Obligation> Obligations;

}}}}

#endif
