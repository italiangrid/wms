
/*
 * NSClient.h
 * 
 * Copyright (C) 2002 by EU DataGrid.
 * For license coinditions see http://www.eu-datagrid.org/license.html
 */

#ifndef _GLITE_WMS_MANAGER_NS_CLIENT_NSCLIENT_H_
#define _GLITE_WMS_MANAGER_NS_CLIENT_NSCLIENT_H_

#include "NSClientInterface.h" 
#include "glite/wms/common/logger/common.h"

#include <string>

namespace glite {
namespace wmsutils {
namespace tls {
namespace socket_pp {
class GSISocketClient;
}
}
}

namespace socket_pp = wmsutils::tls::socket_pp;

namespace wms {

namespace logger = common::logger;

namespace manager {
namespace ns {
namespace commands {
class Command;
}
namespace client {

/**
 * This is the Network Service Client application.
 * 
 * @version
 * @date September 16 2002
 * @author Salvatore Monforte
 * @author Marco Pappalardo
 */ 
class NSClient : public virtual NSClientInterface
{
public:

  /**
   * Costructor.
   * @param h hostname where the Network Server is running at.
   * @param p portnumer the Network Server is listening at.
   * @param lvl log level for logger, optional. 
   * @param auth_to authentication timeout for GSI connection, optional.
   */
  NSClient(const std::string& h, int p, logger::level_t lvl = logger::null, int auth_to = 25);

  /**
   * Destructor.
   */
  ~NSClient();  

  /**
   * Initialize a connection to the remote network server.
   * @return whether the remote NS has been successful contacted or not.
   */
  bool connect();
 
  /**
   * Disconnects the client from the remote network server.
   * @return whether the client has been succesful disconnected or not.
   */
   bool disconnect();
 
  /**
   * Returns whether init procedure is correctly done or not.
   * @return true if init is correctly done, false otherwise.
   */
   bool IsInitDone() const;
 
  /**
   * Returns NSClient connection port.
   * @return the connection port, -1 if unset.
   */
   int getPort();

  /**
   * Returns NSClient host address.
   * @return the host address, empty string if unset.
   */
   std::string getHost();

  /**
   * Job Submit Command.
   * @param jdl the JDL of the job being submitted.
   * @return whether the request has been correctly sent to the remote NS.
   */
   bool jobSubmit(const std::string& jdl);
 
   /**
    * Submit a dag to broker.
    * @param s a jdl string representing a dag.
    * @return true on successful submission, false otherwise.
    * @throws TimeoutException if timeouts expire.
    * @throws ConnectionException if error occurs while connecting.
    * @throws JDLParsingException if error occurs while parsing jdl string.
    * @throws SandboxIOException when error occurs while transferring sandbox.
    * @throws AuthenticationException for authentication failure.
    * @throws NotEnoughQuotaException for leaks of quota space.
    * @throws JobSizeException for job exceeding allowed job size. 
    */ 
   bool dagSubmit(const std::string& s);

  /**
   * List Job Match Command.
   * @param jdl the JDL of the job being submitted.
   * @param l a pointer to a vector of strings which will receive CEIds matching the jdl.
   * @return whether the request has been correctly executed.
   */
   bool listJobMatch(const std::string& jdl, std::vector<std::string>& l);

  /**
   * List Job Match Command.
   * @param jdl the JDL of the job being submitted.
   * @param l a pointer to a vector of pairs which will receive CEIds matching the jdl and rank.
   * @return whether the request has been correctly executed.
   */
   bool listJobMatch(const std::string& jdl, std::vector<std::pair<std::string, double> >& l);

  /**
   * List Job Match Command with Forward action to the WM.
   * @param jdl the JDL of the job being submitted.
   * @param l a pointer to a vector of strings which will receive CEIds matching the jdl.
   * @return whether the request has been correctly executed.
   */
   bool listJobMatchEx(const std::string& jdl, std::vector<std::string>& l);

  /**
   * List Job Match Command with Forward action to the WM.
   * @param jdl the JDL of the job being submitted.
   * @param l a pointer to a vector of pairs which will receive CEIds matching the jdl and rank.
   * @return whether the request has been correctly executed.
   */
   bool listJobMatchEx(const std::string& jdl, std::vector<std::pair<std::string, double> >& l);

  /**
   * Job Purge Command.
   * @param dg_jobid the identifier of the job whose repository has to be purged.
   * @return true on success, false otherwise.
   */ 
   bool jobPurge(const std::string& dg_jobid);
   
  /**
   * Job Cancel Command.
   * @param joblist a reference to a list containing the CEIds of the jobs to cancel.
   * @return the number of jobs which will be actually cancelled or -1 if errors occurred.
   */
   bool jobCancel(const std::list<std::string>& joblist);

   /**
   * Job Cancel Command, sending notification to the user at the specified recipient.
   * @param joblist a reference to a list containing the CEIds of the jobs to cancel.
   * @param recipient the user's mail box address where the notification will be sent.
   * @return the number of jobs which will be actually cancelled or -1 if error occurred.
   */
   int jobCancel(const std::list<std::string>& joblist, const std::string& recipient);
  
  /**
   * Job Cancel All Command: removes all the jobs submitted by a given user.
   * @param certificate a reference to user certifiate subject whose jobs should be removed.
   * @return the number of jobs which will be actually cancelled or -1 if error occurred.
   */
  int jobCancelAll(const std::string& certificate);
 
  /**
   * Job Cancel All Command: sending a notification to the user's recepient.
   * @param certificate a reference to user certifiate subject whose jobs shoul be removed.
   * @return the number of jobs which will be actually cancelled or -1 if error occurred.
   */
  int jobCancelAll(const std::string& certificate, const std::string& recipient);
 
  /**
   * Returns a multi-attribute list.
   * @param l the list of attributes.
   * @return true on success, false otherwise.
   */
  bool getMultiattributeList(std::vector<std::string>& l);
 
  /**
   * Get Output Files List Command.
   * @param edg_jobid the jobid of the job whose output sandbox is to be retrieved.
   * @param l a pointer to a vector of strings which will receive output sandbox files list.
   * @return whether the request has been correctly sent to the remote NS or not.
   */
  bool getOutputFilesList(const std::string& edg_jobid, std::vector<std::string>& l);

  /**
   * Get Output Files List Size Command.
   * @param edg_jobid the jobid of the job whose output sandbox is to be retrieved.
   * @param i an int to fill with output sanbox size.
   * @return whether the sandbox size has correctly retrieved or not.
   */
  bool getOutputFilesListSize(const std::string& edg_jobid, int& i);

  /**
   * Returns the quota defined for the current user.
   * This method fills up quota param with soft and hard limit quota values,
   * usign the same criteria quota management uses: <-1, -1> when error handling
   * quota occurs; <0, 0> when quota is undefined for the user; any other pair
   * of non negative longs if quota is retrieved.
   * @param quota a pair of long values corresponding to soft and hard quota limit.
   * @return true on success, false otherwise.
   */
  bool getQuota(std::pair<long, long>& quota);


  /**
   * Returns the quota defined for the current user.
   * This method fills up quota param with free soft and hard quota values,
   * usign the same criteria quota management uses: <-1, -1> when error handling
   * quota occurs; <0, 0> when quota is undefined for the user; any other pair
   * of non negative longs if free quota is retrieved.
   * @param quota a pair of long values corresponding to free soft and hard quota limit.
   * @return true on success, false otherwise.
   */
  bool getFreeQuota(std::pair<long, long>& quota);


  /**
   * Fills a bool variable indicating whether the quota management is enabled or not.
   * @param status a bool to fill up with the quota management status.
   * @return true on success, false otherwise.
   */
  bool getQuotaManagementStatus(bool& status);

  
  /**
   * Fills a long variable with the max job size allowed for a submit.
   * @param maxjobsize the long to fill.
   * @return true on success, false otherwise.
   */
  bool getMaxInputSandboxSize(long& maxinputsandboxsize);
 
  
  /**
   * Returns the leading path to use for
   * Sandboxes retrivals.
   * @return the sandbox repository leading path.
   */
  std::string              getSandboxRootPath();

protected:
  /**
   * Constructor.
   */
   NSClient() {}
private:
  /** 
   * Executes a Command.
   * @param cmd a pointer to the Command to be done.
   * @return true on success, false otherwise.
   */
   bool runCommand(commands::Command* cmd);

protected:
   /** Tells if error occurred during init. */
   bool _initerror;
   /** The Client Socket reference. */
   socket_pp::GSISocketClient* connection;
   /** Authentication timeout. */
   int auth_timeout;
public:
   /** The IP address for this object. */
   std::string this_ip;	

};

} // namespace client
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite
#endif
