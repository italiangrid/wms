/**
 * @file ForwardRequest.h
 * Defines ForwardRequest class taking care of 
 * request forwarding through FileList.
 *
 * @author Marco Pappalardo marco.pappalardo@ct.infn.it
 */

#ifndef _GLITE_WMS_MANAGER_NS_FSM_FORWARDREQUEST_H_
#define _GLITE_WMS_MANAGER_NS_FSM_FORWARDREQUEST_H_

#include "CommandState.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {

namespace commands {
class Command;
}

namespace fsm {

/**
 * This State sends a string attribute of the command over a connection. 
 * @author Marco Pappalardo marco.pappalardo@ct.infn.it
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */
class ForwardRequest : public CommandState 
{
 public:
  /**
   * Constructor
   * @param s the name of the attribute whose value is to be sent.
   */
  ForwardRequest();
  
  /**
   * Describes the action this state must perform.
   * @param cmd the command the state belongs to.
   * @return whether an error code indicating whether error occurred or not.
   * @throws Broker::Client::JDLParsingException if parsing error occurs.
   */
  bool execute (commands::Command* cmd);

  /**
   * Overrides the superclass method to forward request.
   * @return true so the request is forwarded to WM.
   */
  bool forwardRequest() const;
};

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif 







