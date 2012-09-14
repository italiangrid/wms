/*
 * CommandFactoryClientImpl.h
 * 
 * Copyright (c) 2001, 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */
 
#ifndef _GLITE_WMS_MANAGER_NS_COMMANDS_COMMANDFACTORYCLIENTIMPL_H_
#define _GLITE_WMS_MANAGER_NS_COMMANDS_COMMANDFACTORYCLIENTIMPL_H_

#include "CommandFactoryImpl.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {
 
class Command;

/**
 * The Client implementation of the command-factory definition.
 *
 * @version
 * @date September 16 2002
 * @author Salvatore Monforte
 */
class CommandFactoryClientImpl : public CommandFactoryImpl
{
public:
  /**
   * Creates a new command of the specified type, using Client view.
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
