#include "Command.h"

#include "glite/wmsutils/tls/socket++/SocketAgent.h"

#include "ReceiveBoolean.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

ReceiveBoolean::ReceiveBoolean(const std::string& s) : boolName(s)
{
} 

bool ReceiveBoolean::execute(commands::Command* cmd)
{
  int intValue;
  if(cmd -> agent().Receive(intValue))
    return cmd -> setParam(boolName, intValue ? true : false);
   return false;	
}

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite


