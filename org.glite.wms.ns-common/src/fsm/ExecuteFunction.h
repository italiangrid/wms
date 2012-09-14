/*
 * ExecuteFunction.h
 *
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 * @author Marco Pappalardo marco.pappalardo@ct.infn.it
 */

#ifndef GLITE_WMS_MANAGER_NS_FSM_EXECUTEFUNCTION_H
#define GLITE_WMS_MANAGER_NS_FSM_EXECUTEFUNCTION_H

#include "CommandState.h"
#include <boost/function.hpp>

namespace commands = glite::wms::manager::ns::commands;

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

class Command;

/**
 * This State receives a string command attribute over a connection.
 *
 * @version 1.0
 * @date September 16 2002
 * @author Salvatore Monforte, Marco Pappalardo
 */
class ExecuteFunction : public CommandState
{
public:
  /**
   * Constructor
   * @param s the name of the attribute whose value is to be sent.
   */
  ExecuteFunction(boost::function<bool(commands::Command*)> const& f);

  /**
   * Describes the action this state must perform.
   * @param cmd the command the state belongs to.
   * @return whether an error code indicating whether error occurred or not.
   * @throws JDLParsingException if parsing error occurs.
   */
  bool execute(commands::Command* cmd);

private:
  boost::function<bool(commands::Command*)> m_fn;
};

}}}}} // glite::wms::manager::ns::fsm

#endif
