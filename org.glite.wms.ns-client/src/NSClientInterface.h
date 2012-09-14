
/*
 * NSClientInterface.h
 * 
 * Copyright (C) 2002 by EU DataGrid.
 * For license coinditions see http://www.eu-datagrid.org/license.html
 */

#ifndef _GLITE_WMS_MANAGER_NS_CLIENT_NSCLIENTINTERFACE_H_
#define _GLITE_WMS_MANAGER_NS_CLIENT_NSCLIENTINTERFACE_H_

#include <string>
#include <list>
#include <vector>

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace client {

/**
 * This is the Network Service Client abstract base definition.
 * 
 * @version 1.0
 * @date September 16 2002
 * @author Marco Pappalardo
 * @author Salvatore Monforte
 */ 
class NSClientInterface
{
public:
  
  /**
   * Destructor.
   */
  virtual ~NSClientInterface() {};
  
  /**
   * Initialize a connection to the remote network server.
   * @return whether the remote NS has been successful contacted or not.
   */ 
  virtual bool connect() = 0;

  /** 
   * Disconnects the client from the remote network server.
   * @return whether the client has been succesful disconnected or not.
   */ 
  virtual bool disconnect() = 0;

  /**
   * Returns whether init procedure is correctly done or not.
   * @return true if init is correctly done, false otherwise.
   */
  virtual bool IsInitDone() const = 0;

  /**
   * Job Submit Command.
   * @param jdl the JDL of the job being submitted.
   * @return whether the request has been correctly sent to the remote NS.
   */
  virtual bool jobSubmit(const std::string& jdl) = 0;
   
  /**
   * List Job Match Command.
   * @param jdl the JDL of the job being submitted.
   * @param l a pointer to a list of strings which will receive CEIds matching the jdl.
   * @return whether the request has been correctly sent to the remote NS.
   */
  virtual bool listJobMatch(const std::string& jdl, std::vector<std::string>& l) = 0;
 
  /**
   * Job Cancel Command.
   * @param joblist a reference to a list containing the CEIds of the jobs to cancel.
   * @return the number of jobs which will be actually cancelled or -1 if errors occurred.
   */
  virtual bool jobCancel(const std::list<std::string>& joblist) = 0;
  
  /**
   * Job Cancel Command, sending notification to the user at the specified recipient.
   * @param joblist a reference to a list containing the CEIds of the jobs to cancel.
   * @param recipient the user's mail box address where the notification will be sent.
   * @return the number of jobs which will be actually cancelled or -1 if error occurred.
   */
  virtual int jobCancel(const std::list<std::string>& joblist, const std::string& recipient) = 0;
   
  /**
   * Job Cancel All Command: removes all the jobs submitted by a given user.
   * @param certificate a reference to user certifiate subject whose jobs should be removed.
   * @return the number of jobs which will be actually cancelled or -1 if error occurred.
   */
  virtual int jobCancelAll(const std::string& certificate) = 0;
  
  /**
   * Job Cancel All Command: sending a notification to the user's recepient.
   * @param certificate a reference to user certifiate subject whose jobs shoul be removed.
   * @return the number of jobs which will be actually cancelled or -1 if error occurred.
   */
  virtual int jobCancelAll(const std::string& certificate, const std::string& recipient) = 0;
  
  /** 
   * Returns a multi-attribute list. 
   * @param l the list of attributes. 
   * @return true on success, false otherwise. 
   */ 
  virtual bool getMultiattributeList(std::vector<std::string>& l) = 0;


protected:
  /**
   * Empty Constructor.
   */
  NSClientInterface() {}
  /**
   * Copy Constructor. 
   * @param client a NSClient object.
   */
  NSClientInterface(NSClientInterface& client) {}
  
};

} // namespace client
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif







