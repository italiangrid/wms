#include "Command.h"
#include "SendLong.h"

#include "glite/wmsutils/tls/socket++/SocketAgent.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

SendLong::SendLong(const std::string& s) : longName(s)
{
} 

bool SendLong::execute(commands::Command* cmd)
{
  double value;
  return cmd -> getParam(longName, value) &&
         cmd -> agent().Send( (long) value );
}

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite


