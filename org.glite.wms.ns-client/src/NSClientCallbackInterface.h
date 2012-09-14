
/*
 * NSClientCallbackInterface.h
 * 
 * Copyright (C) 2002 by EU DataGrid.
 * For license coinditions see http://www.eu-datagrid.org/license.html
 */

#ifndef _GLITE_WMS_MANAGER_NS_CLIENT_NSCLIENTCALLBACKINTERFACE_H_
#define _GLITE_WMS_MANAGER_NS_CLIENT_NSCLIENTCALLBACKINTERFACE_H_

#include "NSClientInterface.h"

namespace classad {
  class ClassAd;
};

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace client {

/**
 * This is the Network Service Client providing callback notification
 * abstract base definition.
 * 
 * @version
 * @date September 16 2002
 * @author Marco Pappalardo
 * @author Salvatore Monforte
 */ 
class NSClientCallbackInterface : public virtual NSClientInterface
{
public:
  
  /**
   * Wrapper type for a pointer to callback function.
   */
  typedef void (*callback_t)(classad::ClassAd*);

  /**
   * Callback registration.
   * @param ncode The notification code to register the callback for.
   * @param dg_jobig A string containing the job id the notification refers to.
   * @param callback A pointer to a callback function.
   */
  virtual bool registerCallback(int ncode, const std::string& dg_jobid, callback_t callback ) = 0;

  /**
   * Callback de-registration: remove a previously registered entry for the 
   * given call-back function, notification events and dg_jobid.
   * @param ncode The notification code the callback to unregister refers to.
   * @param dg_jobig A string containing the job id the notification refers to.
   * @param callback A pointer to a callback function.
   */
  virtual bool unregisterCallback(int ncode, const std::string& dg_jobid, callback_t = NULL) = 0;
  
  /**
   * Callback de-registration: remove all previously registered entries for a job. 
   * @param dg_jobid A string containing the job id the notification refers to.
   * @return whether the remote callback function(s) has(have) been successful un-registered.
   */
  virtual bool unregisterCallback( const std::string& dg_jobid ) = 0;
  
  /**
   * Searches the callback registry for the given notification code.
   * @param notification_code the code to look up.
   * @return the callback related to the code.
   */
  virtual callback_t lookupCallbackRegistry( int notification_code ) = 0;
  
private:
  /** 
   * Executes the notifications listener thread. 
   */  
  virtual void runListenerThread() = 0;

  /** 
   * Executes the notifications dispatcher thread.
   */
  virtual void runDispatcherThread() = 0;

  /** 
   * Checks if the condition needed to enable the listener is met.
   * This function SHOULD be called with callback_registry_mutex locked.
   */
  virtual void checkEnableListenerCond() = 0;

  /** 
   * Checks if the condition needed to enable the dispatcehr is met.
   * This function SHOULD be called with notifications_mutex locked.
   */
  virtual void checkEnableDispatcherCond() = 0;

  /** 
   * Registry lookup. Collects any registered callbacks matching a given notification code and job id.
   * @param id the data grid job identifier
   * @param n the notification code
   * @param l a reference to callback list which will be filled with callbacks matching search parameters.
   * @return whether at least one matching call back has been found, or not.
   */
  virtual bool lookupCallbackRegistry( const string& id, int n, list<callback_t>& l ) = 0;

  /** 
   * Execute a callback. 
   * @param callback teh callback to be executed
   * @param ad the notification ad to pass as argument
   * @return the thread id on success 0 otherwise
   */
  virtual pthread_t executeCallback(callback_t callback, ClassAd* ad) = 0;

  /** 
   * Release a callback. 
   * Update the reference count to running thread performing call-back call.
   */
  virtual void releaseCallback() = 0;

  /**
   * Compose callbacks.
   * Compose the callback registered for the given jobid and returns in the
   * code parameter an ordered list of all registered ones.
   * @param code The return code (reference).
   * @param jobid The job id.
   * @param nt Notification type.
   * @return true when it could make the required job, false if the
   * job is not present or if there aren't notifications registered.
   */
  virtual bool composeNotifications( int &code, const std::string &jobid, notification_type_t nt ) = 0;

};

} // namespace client
} // namespace ns
} // namespace manager
} // namespace wms 
} // namespace glite

#endif


