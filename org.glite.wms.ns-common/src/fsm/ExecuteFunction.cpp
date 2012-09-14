/*
 * ExecuteFunction.cpp
 *
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 * @author Marco Pappalardo marco.pappalardo@ct.infn.it
 */

#include "commands/Command.h"
#include "ExecuteFunction.h"

#include "glite/wmsutils/tls/socket++/SocketAgent.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

ExecuteFunction::ExecuteFunction(
  boost::function<bool(commands::Command*)> const& f
)
  : m_fn(f)
{
}

bool ExecuteFunction::execute(commands::Command* cmd)
{
  return m_fn(cmd);
}

}}}}} // glite::wms::manager::ns::fsm
