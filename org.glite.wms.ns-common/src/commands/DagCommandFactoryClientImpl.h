/*
 * DagCommandFactoryServerImpl.h
 * 
 * Copyright (c) 2001, 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */
 
#ifndef _GLITE_WMS_MANAGER_NS_COMMANDS_DAGCOMMANDFACTORYCLIENTIMPL_H_
#define _GLITE_WMS_MANAGER_NS_COMMANDS_DAGCOMMANDFACTORYCLIENTIMPL_H_

namespace commands = glite::wms::manager::ns::commands;

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

class commands::Command;

    /**
     * Compute the sandboxsize for the given node.
     * WARNING: the size value is not inizialized so it can be used for sums into
     * cicles.
     * @param ad a classad containing the subjob whose sandbox size must be calculated.
     * @param size the off_t var to fill with the sandbox size.
     * return true on success, false otherwise.
     */
    // extern bool computeJobSandboxSize(off_t& size);
 
    /**
     * Compute dag's Sandbox Size.
     * This methods gets dag jdl string and determines
     * the sandbox size. For a successful execution command
     * must have "jdl" and "Sandbox" attributes. "Sandboxsize"
     * attribute is written.
     * @param cmd a pointer to the command.
     * @return true on success, false otherwise.
     */
    extern bool computeDagSandboxSize (commands::Command* cmd);

namespace dag {

      extern bool createRemoteDirs(commands::Command* cmd);

      extern bool doSandboxTransfer(commands::Command* cmd);

} // namespace dag


} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite 
 
#endif


