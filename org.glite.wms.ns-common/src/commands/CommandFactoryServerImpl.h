/*
 * CommandFactoryServerImpl.h
 * 
 * Copyright (c) 2001, 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */
 
#ifndef _ORG_GLITE_WMS_MANAGER_NS_COMMANDS_COMMANDFACTORYSERVERIMPL_H_
#define _ORG_GLITE_WMS_MANAGER_NS_COMMANDS_COMMANDFACTORYSERVERIMPL_H_

#include "CommandFactoryImpl.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {
 
class Command;

/**
 * The Server implementation of the command-factory definition.
 *
 * @version
 * @date September 16 2002
 * @author Salvatore Monforte
 * @author Marco Pappalardo
 */
class CommandFactoryServerImpl : public CommandFactoryImpl
{
public:
  /**
   * Creates a new command of the specified type, using Server view.
   * @return a pointer to a new server-side command.
   */
   Command* create(const std::string& name);

};

} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite 
 
#endif
