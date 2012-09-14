#include "Command.h"
#include "SendString.h"

#include "glite/wmsutils/tls/socket++/SocketAgent.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

SendString::SendString(const std::string& s) : strName(s)
{
} 

bool SendString::execute(commands::Command* cmd)
{
  std::string value;
  cmd -> getParam(strName, value);
  return  cmd -> agent().Send(value);
}

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite


