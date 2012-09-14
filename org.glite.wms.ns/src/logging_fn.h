/*
 * File: logging_fn.h
 * Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
 * Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
 *
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$ 

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace daemon {


extern "C++"				
{
  bool LogEnqueuedJob(edg_wll_Context, std::string, std::string, const char*, bool, const char*, bool, bool);
  bool LogEnqueuedJob(edg_wll_Context, const char*, bool, const char*, bool, bool);
  bool LogEnqueuedJob_Start(edg_wll_Context, std::string, std::string, const char*, const char*, bool, bool);
  bool LogEnqueuedJob_Start(edg_wll_Context, const char*, const char*, bool, bool);
}

} // namespace daemon
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite
