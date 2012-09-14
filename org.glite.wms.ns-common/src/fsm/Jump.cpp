/**
 * @file Jump.cpp
 * Defines Jump State.
 *
 * @author Marco Pappalardo marco.pappalardo@ct.infn.it
 */

#include "Command.h"
#include "Jump.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

Jump::Jump(const std::string& s) : stepsName(s)
{  
  steps = -1;
} 

Jump::Jump(const int i)
{  
  steps = i;
}

bool Jump::execute(commands::Command* cmd)
{
  if (steps == -1) {
    if (!cmd->getParam(stepsName, steps)) return false;
  }
  
  for (int i = 0; i < steps; i++) {
    if( cmd -> fsm -> empty() ) { return false; }
    cmd -> fsm -> pop();
  }

  return true;
}

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite


