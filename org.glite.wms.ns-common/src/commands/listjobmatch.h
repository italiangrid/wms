/*
 * File: listjobmatch.h
 * Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$ 

#ifndef _GLITE_WMS_MANAGER_NS_COMMANDS_LISTJOBMATCH_H_
#define _GLITE_WMS_MANAGER_NS_COMMANDS_LISTJOBMATCH_H_

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {
				
extern "C++"
{
  // bool listjobmatch(Command*);
  bool listjobmatchex(Command* cmd);
}

} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif
