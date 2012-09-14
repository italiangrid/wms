#ifndef _SendJDL_h_
#define _SendJDL_h_

#include "edg/workload/networkserver/commands/fsm/CommandState.h"

namespace edg {
namespace workload {
namespace networkserver {
namespace commands {
namespace fsm {
 
class Command;

/**
 * This State sends the job jdl over a connection. 
 * @author Marco Pappalardo marco.pappalardo@ct.infn.it
 * @author Salvatore Monforte salvatore.monforte@ct.infn.it
 */
class SendJDL : public CommandState 
{
 public:
  /**
   * Describes the action this state must perform.
   * @param cmd the command the state belongs to.
   * @return whether an error code indicating whether error occurred or not.
   * @throws edg::workload::common::socket::IOException if error occurs.
   */
  bool execute (commands::Command* cmd);
};

} // namespace fsm
} // namespace comamnds
} // namespace networkserver
} // namespace workload
} // namespace edg

#endif 







