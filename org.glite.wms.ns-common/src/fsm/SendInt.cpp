#include "Command.h"
#include "SendInt.h"

#include "glite/wmsutils/tls/socket++/SocketAgent.h"

#include <iostream.h>

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

SendInt::SendInt(const std::string& s) : intName(s)
{
} 

bool SendInt::execute(commands::Command* cmd)
{
  int value;
  return cmd -> getParam(intName, value) &&
         cmd -> agent().Send( value );
}

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite


