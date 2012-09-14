/*
 * SendVector.h
 *
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#include "Command.h"
#include "SendVector.h"

#include "glite/wmsutils/tls/socket++/SocketAgent.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace fsm {

SendVector::SendVector(const std::string& s) : vectorName(s)
{
} 

bool SendVector::execute(commands::Command* cmd)
{
  std::vector<std::string> v;
  if ( cmd -> getParam(vectorName, v) ) {
      int size = (int)v.size();
      if ( cmd -> agent().Send(size) ) {
      for(size_t i=0; i<(size_t)size; i++) {
	if ( ! cmd -> agent().Send(v[i]) ) {
	  return false;
	}
      }
      return true;
    } 
  } else {
    cmd -> agent().Send(0);
  }
  return false; 
}

} // namespace fsm
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite



