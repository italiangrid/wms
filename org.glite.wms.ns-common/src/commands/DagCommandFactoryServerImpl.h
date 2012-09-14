/*
 * DagCommandFactoryServerImpl.cpp
 * 
 * Copyright (c) 2001, 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#ifndef _GLITE_WMS_MANAGER_NS_COMMANDS_DAGCOMMANDFACTORYSERVERIMPL_H_
#define _GLITE_WMS_MANAGER_NS_COMMANDS_DAGCOMMANDFACTORYSERVERIMPL_H_

namespace commands  = glite::wms::manager::ns::commands;

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

  class Command;

namespace dag {

  bool createStagingDirectories(commands::Command* cmd);

} // namespace dag  
} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite 

#endif
