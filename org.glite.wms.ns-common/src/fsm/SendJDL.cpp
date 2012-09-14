#include "Command.h"
#include "SendJDL.h"
#include "glite/wms/tsl/socket++/SocketAgent.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

bool SendJDL::execute(commands::Command* cmd)
{
  string jdl;
  if( !cmd -> getParam("jdl", jdl) ) return false;
  
  return ( cmd -> agent().Send(jdl) );
}

} // namespace fsm
} // namespace networkserver
} // namespace manager
} // namespace workload
} // namespace edg



