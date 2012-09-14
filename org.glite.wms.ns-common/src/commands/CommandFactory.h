/*
 * File: CommandFactory.h
 *
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#ifndef _GLITE_WMS_MANAGER_NS_COMMANDS_COMMANDFACTORY_H_
#define _GLITE_WMS_MANAGER_NS_COMMANDS_COMMANDFACTORY_H_

#include <boost/scoped_ptr.hpp>
#include "Command.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

class CommandFactoryImpl;

/**
 * The command factory class template.
 * 
 * @version 1.0
 * @date September 16 2002
 * @author Salvatore Monforte, Marco Pappalardo.
 */
template<class implementation>
class CommandFactory 
{
 public:
   CommandFactory() : impl( new implementation ) {}
  
  /**
   * Creates a Command by name.
   * @return a pointer to new Command.
   */
  Command* create(const std::string& name) { return impl -> create( name ); }
  
private:
  boost::scoped_ptr<CommandFactoryImpl> impl;
};

} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif
