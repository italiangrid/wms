#include "Command.h"
#include "SendBoolean.h"

#include "glite/wmsutils/tls/socket++/SocketAgent.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

SendBoolean::SendBoolean(const std::string& s) : boolName(s)
{
} 

bool SendBoolean::execute(commands::Command* cmd)
{
  bool value;
  return cmd -> getParam(boolName, value) &&
    cmd -> agent().Send( value ? 1 : 0 );
}

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite


