
/*
 * NSClientCallback.h
 * 
 * Copyright (C) 2002 by EU DataGrid.
 * For license coinditions see http://www.eu-datagrid.org/license.html
 */

#ifndef _GLITE_WMS_MANAGER_NS_CLIENT_NSCLIENTCALLBACK_H_
#define _GLITE_WMS_MANAGER_NS_CLIENT_NSCLIENTCALLBACK_H_

#include "glite/wms/common/triple.h"
#include "NSClientCallbackInterface.h"
#include "NSClient.h"

#include <queue>

namespace glite {

namespace wmsutils {
namespace tls {
namespace socket_pp {
class GSISocketServer;
}
}
}

namespace socket_pp = wmsutils::tls::socket_pp;

namespace wms {
namespace manager {
namespace ns {
namespace client {

/**
 * This is the Network Service Client application using callback notification.
 * 
 * @version
 * @date September 16 2002
 * @author Salvatore Monforte
 * @author Marco Pappalardo
 */ 
class NSClientCallback : public NSClientCallbackInterface, public NSClient
  {
  public:
    /**
     * Costructor.
     * @param h hostname where the Network Server is running at.
     * @param p portnumer the Network Server is listening at.
     */
    NSClientCallback(const std::string& h, int p);
    ~NSClientCallback();   
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
     * Job Submit Command.
     * @param jdl the JDL of the job being submitted.
     * @return whether the request has been correctly sent to the remote NS.
     */
    bool jobSubmit(const std::string& jdl);
    
    /**
     * List Job Match Command.
     * @param jdl the JDL of the job being submitted.
     * @param l a pointer to a list of strings which will receive CEIds matching the jdl.
     * @return whether the request has been correctly sent to the remote NS.
     */
    bool listJobMatch(const std::string& jdl, std::list<std::string>* l = NULL);
    
    /**
     * Job Cancel Command.
     * @param joblist a reference to a list containing the CEIds of the jobs to cancel.
     * @return the number of jobs which will be actually cancelled or -1 if errors occurred.
     */
    int jobCancel(const std::list<std::string>& joblist);
    
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
     * Returns the Output Sandbox for a job.
     * @param dg_job_id the job identifier.
     * @param dg_path the pathname for sandbox.
     * @return true on success, false otherwise.
     */
    bool getOutputSandbox(const std::string& dg_jobid, const std::string& dg_path);
    
    /**
     * Returns the logger contact string.
     * @return the logger contact string.
     */
    std::string getLBContact();
    
  private:
    friend void listener(void*);
    friend void dispatcher(void*);
    friend void callback_wrapper(void*);
    friend class find_callback_by_notification_t;
    
    /** Defines the registry content. **/
    typedef pair<string, callback_t> registration_entry_t;
    /** The notification registry. */
    typedef map<int,registration_entry_t> notification_registry_t; 
    
    /** The notification registry. */
    notification_registry_t notification_registry;
    
    /** 
     * Executes the notifications listener thread. 
     */  
    void runListenerThread();
    
    /** 
     * Executes the notifications dispatcher thread.
     */
    void runDispatcherThread();
    
    /** 
     * Checks if the condition needed to enable the listener is met.
     * This function SHOULD be called with callback_registry_mutex locked.
     */
    void checkEnableListenerCond();
    
    /** 
     * Checks if the condition needed to enable the dispatcehr is met.
     * This function SHOULD be called with notifications_mutex locked.
     */
    void checkEnableDispatcherCond();
    
    /** 
     * Callback registration. 
     * @param ncode The notification code to register the callback for.
     * @param dg_jobig A string containing the job id the notification refers to.
     * @param callback A pointer to a callback function.
     */ 
    bool registerCallback(int ncode, const string& dg_jobid, callback_t callback );

    /** 
     * Callback de-registration: remove a previously registered entry for the
     * given call-back function, notification events and dg_jobid.
     * @param ncode The notification code the callback to unregister refers to.
     * @param dg_jobig A string containing the job id the notification refers to.
     * @param callback A pointer to a callback function. 
     */
    bool unregisterCallback(int ncode, const std::string& dg_jobid, callback_t = NULL) = 0;
    
    /**
     * Callback de-registration: remove all previously registered entries for ajob.
     * @param dg_jobid A string containing the job id the notification refers to.
     * @return whether the remote callback function(s) has(have) been successfully un-registered.
    */ 
    bool unregisterCallback( const std::string& dg_jobid );


    /** 
     * Registry lookup. 
     * Collects any registered callbacks matching a given notification code and job id.
     * @param id the data grid job identifier
     * @param n the notification code
     * @param l a reference to callback list which will be filled with callbacks matching search parameters.
     * @return whether at least one matching call back has been found, or not.
     */
    bool lookupCallbackRegistry( const string& id, int n, list<callback_t>& l );
    
    /** 
     * Execute a callback. 
     * @param callback teh callback to be executed
     * @param ad the notification ad to pass as argument
     * @return the thread id on success 0 otherwise
     */
    pthread_t executeCallback(callback_t callback, ClassAd* ad);
    
    /** 
     * Release a callback. 
     * Update the reference count to running thread performing call-back call.
     */
    void releaseCallback();
    
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
    bool composeNotifications( int &code, const std::string &jobid, notification_type_t nt );
    
  private:
    /** The GSI notification listener. */
    socket_pp::GSISocketServer* notification_listener;
    /** The listener port. */
    size_t           listener_port;
    
    /** Defines the call-backs registry entry type.**/
    typedef std::pair<int, callback_t> registry_entry_t;
    /** The call-back registry entry list. */
    typedef std::list<registry_entry_t> entry_list_t;
    /** The call back entry. */
    typedef std::map<std::string,entry_list_t> callback_registry_t;
    /** Defines the call-back wrapper argument. */
    typedef triple<NSClientCallback*,callback_t,ClassAd*> callback_wrapper_arg_t;
  
    /** The number of processed requests at this moment. */
    size_t n_servicing;
    /** The max number of acceptable requests. */
    size_t n_max_servicing;
    
    /** The call-back registry. */
    callback_registry_t callback_registry;
    /** The notification listener thread id. */
    pthread_t listener_thread;
    /** The notification dispatcher thread id. */
    pthread_t dispatcher_thread;
    /**
     * The notification mutex. 
     * Ensures exclusive access to the notifications queue. 
     */
    pthread_mutex_t notifications_mutex;
    /**
     * The registry mutex. 
     * Ensures exclusive access to the call-back registry.
     */
    pthread_mutex_t callback_registry_mutex;
    /** 
     * Defines mutex, cond and predicate for enabling/disabling the listener 
     * whether there is any registered notification to listen at, or not.
     */ 
    bool                is_callback_registry_empty;
    /** The thread listener condition. */
    pthread_cond_t      enable_listener_cond;
    /** The thread listener mutex. */
    pthread_mutex_t     enable_listener_mutex;
    
    /** 
     * Defines mutex, cond and predicate for enabling/disabling the dispatcher 
     * whether there is any NotifiedAd to dispatch to, or not.
     */ 
    bool                is_notified_queue_empty;
    /** The thread dispatcher condition. */
    pthread_cond_t      enable_dispatcher_cond;
    /** The thread dispatcher mutex. */
    pthread_mutex_t     enable_dispatcher_mutex;
    
    /** 
     * Defines mutex, cond and predicate for enabling/disabling the dispatcher 
     * whether there are too many running threads servicing notifications.
     */ 
    bool                is_dispatcher_busy;
  /** The thread dispatcher busy condition. */
    pthread_cond_t      dispatcher_busy_cond;
    /** The thread dispatcher busy mutex. */
    pthread_mutex_t     dispatcher_busy_mutex;
    
    /** The queue of the received notifications. */
    std::queue<ClassAd*> notifiedAds;
  };

} // namespace client
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif
