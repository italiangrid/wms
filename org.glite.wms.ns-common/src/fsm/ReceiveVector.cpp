/*
 * ReceiveVector.h
 *
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#include "Command.h"
#include "ReceiveVector.h"

#include "glite/wmsutils/tls/socket++/SocketAgent.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

ReceiveVector::ReceiveVector(const std::string& s) : vectorName(s)
{
} 

bool ReceiveVector::execute(commands::Command* cmd)
{
  std::vector<std::string> v;

  int n = 0;
  if (!cmd -> agent().Receive(n) ) return false;
  for (size_t i=0; i<(size_t)n; i++) {
    std::string strValue;
    if(cmd -> agent().Receive(strValue)) v.push_back(strValue);
  }
  cmd -> setParam(vectorName, v);
  return true;
}

} // namespace fsm
} // namespace networkserver
} // namespace manager
} // namespace wms
} // namespace edg



