#include "Command.h"

#include "glite/wmsutils/tls/socket++/SocketAgent.h"

#include "ReceiveLong.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

ReceiveLong::ReceiveLong(const std::string& s) : longName(s)
{
} 

bool ReceiveLong::execute(commands::Command* cmd)
{
  long value;
  if(cmd -> agent().Receive(value)) {
    return cmd -> setParam(longName, (double)value );
  }
  return false;	
}

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite


