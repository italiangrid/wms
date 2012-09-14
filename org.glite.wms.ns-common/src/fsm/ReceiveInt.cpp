#include "Command.h"

#include "glite/wmsutils/tls/socket++/SocketAgent.h"

#include "ReceiveInt.h"

#include <iostream.h>

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

ReceiveInt::ReceiveInt(const std::string& s) : intName(s)
{
} 

bool ReceiveInt::execute(commands::Command* cmd)
{
  int value = 0;

  if(cmd -> agent().Receive(value)) {
    return cmd -> setParam(intName, value );
  }
  
  return false;
}

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite




