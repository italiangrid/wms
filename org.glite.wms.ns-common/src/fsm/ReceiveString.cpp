/*
 * ReceiveString.cpp
 *
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#include "Command.h"
#include "ReceiveString.h"

#include "glite/wmsutils/tls/socket++/SocketAgent.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

ReceiveString::ReceiveString(const std::string& s) : strName(s)
{
} 

bool ReceiveString::execute(commands::Command* cmd)
{
  std::string strValue;

  return cmd -> agent().Receive(strValue) &&
    cmd -> setParam(strName, strValue);
}

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace edg



