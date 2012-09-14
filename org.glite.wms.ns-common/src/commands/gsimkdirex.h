/*
 * File: gsimkdirex.h
 * Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$

#ifndef _GLITE_WMS_MANAGER_NS_COMMANDS_GSIMKDIREX_H_
#define _GLITE_WMS_MANAGER_NS_COMMANDS_GSIMKDIREX_H_

#include <string>

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

extern "C++"
{
   bool gsimkdirex(const std::string& destination, const std::string& stub = std::string(""));
}

#endif

} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite
