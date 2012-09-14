/**
 * @file Label.h
 * Defines Label State.
 *
 * @author Marco Pappalardo marco.pappalardo@ct.infn.it
 */

#ifndef _GLITE_WMS_MANAGER_NS_FSM_LABEL_H_
#define _GLITE_WMS_MANAGER_NS_FSM_LABEL_H_

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
 * This State represents a label within the FSM. 
 * @author Marco Pappalardo marco.pappalardo@ct.infn.it
 */
class Label : public CommandState 
{
 public:
  /**
   * Constructor
   * @param s the attribute representing number of steps to be skipped.
   */
  Label(const std::string& s);

  /**
   * Describes the action this state must perform.
   * @param cmd the command the state belongs to.
   * @return whether an error code indicating whether error occurred or not.
   */
  bool execute (commands::Command* cmd);

  /**
   * Tells that this object is a Label rather than a state.
   * @param lbl the label string to fill with label
   * @return true so the NS knows the state is a label. 
   */
  bool hasLabel (std::string& lbl) const;

 private:
  std::string labelName;
};

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif 







