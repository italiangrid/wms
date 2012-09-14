/**
 * @file Jump.h
 * Defines Jump State.
 *
 * @author Marco Pappalardo marco.pappalardo@ct.infn.it
 */

#ifndef _GLITE_WMS_MANAGER_NS_FSM_JUMP_H_
#define _GLITE_WMS_MANAGER_NS_FSM_JUMP_H_

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
 * This State represents a Jump in the FSM. 
 * @author Marco Pappalardo marco.pappalardo@ct.infn.it
 */
class Jump : public CommandState 
{
 public:
  /**
   * Constructor
   * @param s the attribute representing number of steps to be skipped.
   */
  Jump(const std::string& s);
  /**
   * Constructor
   * @param steps number of steps to be skipped.
   */
  Jump(const int steps);

  /**
   * Describes the action this state must perform.
   * @param cmd the command the state belongs to.
   * @return whether an error code indicating whether error occurred or not.
   */
  bool execute (commands::Command* cmd);

 private:
  std::string stepsName;
  int steps;
};

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif 







