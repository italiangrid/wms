/**
 * @file Label.cpp
 * Defines Label State.
 *
 * @author Marco Pappalardo marco.pappalardo@ct.infn.it
 */

#include "Command.h"
#include "Label.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

Label::Label(const std::string& s) : labelName(s)
{
} 

bool Label::hasLabel(std::string& lbl) const
{  
  lbl.assign(labelName);
  return true;
}

bool Label::execute(commands::Command* cmd)
{
  return true;
}

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace edg


