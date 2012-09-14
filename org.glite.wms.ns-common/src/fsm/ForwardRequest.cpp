/**
 * @file ForwardRequest.cpp
 * Defines ForwardRequest class taking care of
 * request forwarding through FileList.
 * 
 * @author Marco Pappalardo marco.pappalardo@ct.infn.it
 */ 

#include "Command.h"
#include "ForwardRequest.h"

#include "glite/wmsutils/tls/socket++/SocketAgent.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

ForwardRequest::ForwardRequest()
{
} 

bool ForwardRequest::execute(commands::Command* cmd)
{
  return true;
}

bool ForwardRequest::forwardRequest() const
{
  return true;
}

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite


