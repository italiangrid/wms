/*
 * File: Command.h
 * 
 * Copyright (c) 2001, 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */
 
// $Id$

#ifndef _GLITE_WMS_MANAGER_NS_COMMANDS_COMMAND_H_
#define _GLITE_WMS_MANAGER_NS_COMMANDS_COMMAND_H_

#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>

#include "CommandState.h"

#include "glite/wmsutils/jobid/cjobid.h"
#include "glite/lb/producer.h"

namespace classad {
  class ClassAd;
};

namespace glite {

namespace wmsutils {
namespace tls {
namespace socket_pp {
class SocketAgent;
} // namespace socket_pp
} // namespace tls
} // namespace wmsutils

namespace socket_pp = wmsutils::tls::socket_pp;

namespace wms {
namespace manager {
namespace ns {

namespace fsm {
	class Jump;
}// namespace fsm

namespace commands {

struct bad {};


/**
 * A Command.
 *
 * @version
 * @date September 16 2002
 * @author Salvatore Monforte
 * @author Marco Pappalardo
 */
class Command
{
 protected:
 friend class CommandFactoryClientImpl;
 friend class CommandFactoryServerImpl; 
 friend bool serializeServerImpl(socket_pp::SocketAgent*, Command*);

  /**
   * Constructor.
   */
  Command();
 
 public:
  /**
   * Destructor
   */
  virtual ~Command();
  /**
   * Returns a classad view of the command
   * @return a classad representing the command
   */
  classad::ClassAd& asClassAd() const;
  
  /**
   * Serialize the classad view through a socket.
   * @param sck a pointer to a GSISocketAgent.
   * @return whether the command has been serialized, or not.
   * @throws
   */
  bool serialize(socket_pp::SocketAgent*);
  
  /**
   * Executes a Command.
   * @return true for successful execution, false otherwise.
   */
  bool execute();
  
  /**
   * Returns whether the command is done or not.
   * @return a boolean value saying whether the command is done or not.
   */
  bool isDone() const;
  
  /**
   * Returns current state for command.
   * @return the state the command actually stays.
   */
  const ns::fsm::CommandState& state();
  
  /**
   * Returns the agent for this command.
   * @return the socket agent.
   */
  socket_pp::SocketAgent& agent()  const;
  
  /**
   * Retrieves a boolean command's parameter.
   * @param name the param name.
   * @param b the boolean to fill with found value.
   * @return true if param is found, false otherwise.
   */
  bool getParam(const std::string& name, bool& b);

  /**
   * Retrieves an int command's parameter.
   * @param name the param name.
   * @param i the int to fill with found value.
   * @return true if param is found, false otherwise.
   */  
  bool getParam(const std::string& name, int& i);

  /**
   * Retrieves a double command's parameter.
   * @param name the param name.
   * @param d the double to fill with found value.
   * @return true if param is found, false otherwise.
   */  
  bool getParam(const std::string& name, double& d);

  /**
   * Retrieves a string command's parameter.
   * @param name the param name.
   * @param s the string to fill with found value.
   * @return true if param is found, false otherwise.
   */
  bool getParam(const std::string& name, std::string& s);

  /**
   * Retrieves a vector command's parameter.
   * @param name the param name.
   * @param v the vector of strings to fill with found value.
   * @return true if param is found, false otherwise.
   */
  bool getParam(const std::string& name, std::vector<std::string>& v);
  
  /**
   * Sets a boolean command's parameter.
   * @param name the param name.
   * @param b the boolean value.
   * @return true if param is set, false otherwise.
   */
  bool setParam(const std::string& name, bool b);

  /**
   * Sets an int command's parameter.
   * @param name the param name.
   * @param i thr int value.
   * @return true if param is set, false otherwise.
   */
  bool setParam(const std::string& name, int i);

  /**
   * Sets a double command's parameter.
   * @param name the param name.
   * @param d the double value.
   * @return true if param is set, false otherwise.
   */
  bool setParam(const std::string& name, double d);

  /**
   * Sets a string command's parameter.
   * @param name the param name.
   * @param s the string value.
   * @return true if param is set, false otherwise.
   */
  bool setParam(const std::string& name, const std::string& s);

  /**
   * Sets a classad command's parameter.
   * @param name the param name.
   * @param pad a pointer to class ad value to add.
   * @return true if param is set, false otherwise.
   */
  bool setParam(const std::string& name, classad::ClassAd *pad);

  /**
   * Sets a vector command's parameter.
   * @param name the param name.
   * @param v the vector of strings value.
   * @return true if param is set, false otherwise.
   */
  bool setParam(const std::string& name, const std::vector<std::string>& v);

  /**
   * Returns the name of the command.
   * @retunrn the name of the command.
   */
  std::string name();

  /**
   * Return the version of the command.
   * @return the version of the command.
   */
  std::string version();

  /**
   * Returns a reference to the log context.
   * @return the pointer to the log context.
   */
  edg_wll_Context* getLogContext();

  
  /**
   * Returns the reference to the inner jobId.
   * @return the pointer to jobId object.
   */
  edg_wlc_JobId* getLogJobId();


private:
  friend class ns::fsm::Jump;
  /** A pointer to the ClassAd representation of the command. */
  classad::ClassAd      *ad;
  /** A pointer to the FiniteStateMachine */
  ns::fsm::state_machine_t *fsm;
  /** A pointer to a socket agent. */
  socket_pp::SocketAgent*                 sck;
  /** A scoped pointed Log Context. */
  boost::scoped_ptr<edg_wll_Context>      ctx;
  /** A scoped pointed JobId. */
  boost::scoped_ptr<edg_wlc_JobId>              jobId;
  bool (*serializeImpl)(socket_pp::SocketAgent*, Command*);


};
 
} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif
