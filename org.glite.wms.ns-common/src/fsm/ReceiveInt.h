
#ifndef _GLITE_WMS_MANAGER_NS_FSM_RECEIVEINT_H_
#define _GLITE_WMS_MANAGER_NS_FSM_RECEIVEINT_H_

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
 * This State sends a double attribute of the command over a connection. 
 * @author Marco Pappalardo marco.pappalardo@ct.infn.it
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */
class ReceiveInt : public CommandState 
{
 public:
  /**
   * Constructor
   * @param s the name of the attribute whose value is to be sent.
   */
  ReceiveInt(const std::string& s);
  
  /**
   * Describes the action this state must perform.
   * @param cmd the command the state belongs to.
   * @return whether an error code indicating whether error occurred or not.
   */
  bool execute (commands::Command* cmd);

 private:
  std::string intName;
};

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif 







