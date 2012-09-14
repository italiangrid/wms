/*
 * NSClientCallback.cpp
 *
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#include "edg/workload/networkserver/client/NSClientCallback.h"
#include "edg/workload/networkserver/client/exceptions.h"
#include "edg/workload/networkserver/client/RBjob.h"
#include "edg/workload/common/socket++/GSISocketServer.h"
#include "edg/workload/common/socket++/GSISocketAgent.h"
#include "edg/workload/common/socket++/GSISocketClient.h"
#include "edg/workload/common/socket++/exceptions.h"
#include "edg/workload/networkserver/client/notification.h"

#include "edg/workload/common/logger/edglog.h"
#include "edg/workload/common/logger/manipulators.h"
 
namespace logger = edg::workload::common::logger;

#include <classad_distribution.h>

using namespace classad;

namespace edg {
namespace workload {
namespace networkserver {
namespace client {
  /**
   * Creates a listener for notifications.
   * @param context a pointer to a context.
   * @throws ConnectionException on connection error.
   * @throws AuthorizationException on authorization error.
   * @throws AuthenticationException on authentication error.
   */
  void listener         (void*);
  /**
   * Creates a dispatcher for notification callback client. 
   * @param context a pointer to a context.
   */
  void dispatcher       (void*);
  /**
   * Creates a wrapper for callback notifications.
   * @param context a pointer to a context.
   */
  void callback_wrapper (void*);

/**
 * Constructor.
 * @param h the host address.
 * @param p the port.
 * @throws Broker::Client::ConnectionException if connection error happens.
 */
NSClientCallback::NSClientCallback(const string& h, int p)
{
  logger::threadsafe::edglog.open( "edglog.log", logger::medium );
  logger::threadsafe:edglog << logger::setfunction( "NSClientCalback::NSClientCallback()" ) << "Creating NS Callback Client." << endl;

  notification_listener = NULL;
  listener_thread   = 0;
  dispatcher_thread = 0;
  n_servicing       = 0;
  n_max_servicing   = 4;
  
  pthread_mutex_init( &notifications_mutex,     NULL );
  pthread_mutex_init( &callback_registry_mutex, NULL );
  pthread_mutex_init( &enable_listener_mutex,   NULL );
  pthread_mutex_init( &enable_dispatcher_mutex, NULL );
  pthread_mutex_init( &dispatcher_busy_mutex,   NULL );
  
  pthread_cond_init( &enable_listener_cond,   NULL);
  pthread_cond_init( &enable_dispatcher_cond, NULL);
  pthread_cond_init( &dispatcher_busy_cond,   NULL);
  
  is_callback_registry_empty = true;
  is_notified_queue_empty    = true;
  is_dispatcher_busy         = false;

  NSClient::NSClient(h, p);
}

/**
 * Destructor. 
 */
NSClientCallback::~NSClientCallback()
{
  if( listener_thread )   pthread_kill( listener_thread,   9 );
  if( dispatcher_thread ) pthread_kill( dispatcher_thread, 9 );
  if( notification_listener != NULL ) delete notification_listener;
}

/**
 * starts the notifications listener thread.
 * @throws Broker::Client::ConnectionException when error occurs.
 */
void NSClientCallback::runListenerThread()
{
  logger::threadsafe:edglog << logger::setfunction( "NSClientCalback::runListenerThread()" ) << "Running Listener Thread..." << endl;

  listener_port = 7999;
  
  for(;;) {      

      try {
	notification_listener = new socket_pp::GSISocketServer( ++listener_port );
	if( !notification_listener -> Open() ) {
	  logger::threadsafe:edglog << "Notification Listener closed." << endl;
	  delete notification_listener;
	}
	else break;
      } 
      catch (socket_pp::IOException& ioe) {
        ConnectionException e(connection -> Host());
        e.setCause("NSClient::AcceptNotification(): " + ioe.getCause());
	e.setSourceInfo(ioe.getSourceInfo()+ ", " + ioe.getMessage());
	logger::threadsafe:edglog << "Exception Caught. Cause:" << ioe.getCause() << " Message: " << ioe.getMessage() << endl;	
	throw e;
      }
  }
  
  if( pthread_create( &listener_thread, NULL, (void* (*)(void*))(listener), (void*) this ) ) {
    delete notification_listener;
  } else {
#ifdef DEBUG
    logger::threadsafe:edglog << "UI listens for notification on port #" << listener_port << endl;
#endif
  }
}

/**
 * Submit a job to broker using notifications.
 * @param s a string representing a job.
 * @return true on successful submission, false otherwise.
 * @throws TimeoutException if timeouts expire.
 * @throws ConnectionException if error occurs while connecting.
 * @throws JDLParsingException if error occurs while parsing jdl string.
 * @throws SandboxIOException when error occurs while transferring sandbox.
 * @throws AuthenticationException for authentication failure.
 */ 
bool NSClientCallback::jobSubmit(const string& s)
{  
  connection -> DelegateCredentials(false);
  
  RBjob theJob(s);
  if( theJob.ad_syntax_error ) {
    JDLParsingException e(string("Error while parsing Jdl string."));
    e.setCause("NSClient::jobSubmit()");
    e.setSourceInfo(s);
    throw e;
  }
  
  int SubmitNotification = 0;
  composeNotifications(SubmitNotification, 
			theJob.id(), 
			SUBMIT_STATUS);
  if( SubmitNotification ) {  
    theJob.InsertAttribute( "SubmitNotification",        SubmitNotification );
    theJob.InsertAttribute( "SubmitNotificationContact", this_ip);
    theJob.InsertAttribute( "SubmitNotificationPort",    listener_port);
  }
  
  if ( connect() ) {
  }
  disconnect();
  return true;
}

/**
 * Lists all jobs matching a jdl string criteria.
 * @param jdl the jdl string.
 * @param l a pointer to the list which will receive all matching CEIds.
 * @return true on success, false otherwise.
 * @throws ConnectionException if error occurs, superclass or send_stub throws it while serializing.
 * @throws JDLParsingException if a parsing error occurs.
 * @throws AuthenticationException for authentication failure.
 * @throws NoSuitableResourceException if no suitable resource is found.
 * @throws MatchMakingException if mathcmaking reports errors.
 */
bool NSClientCallback::listJobMatch(const std::string& jdl, std::list<std::string>* l = NULL)
{
  connection -> DelegateCredentials(false);
  RBjob theJob(jdl);
  
  if( theJob.ad_syntax_error ) {
    JDLParsingException e(string("Error while parsing Jdl string."));
    e.setCause("NSClient::jobListMatch()");
    e.setSourceInfo(jdl);
    throw e;
  }
  
  int ListMatchNotification = 0;
  composeNotifications(ListMatchNotification, 
		       theJob.id(), 
		       LIST_MATCH_STATUS);
  
  if( ListMatchNotification ) {
    theJob.InsertAttribute("ListMatchNotification", ListMatchNotification );
    theJob.InsertAttribute("ListMatchAsynchronously", (l != NULL));
    theJob.InsertAttribute("ListMatchNotificationContact", this_ip);
    theJob.InsertAttribute("ListMatchNotificationPort",    listener_port);
  }
  
  return true;
}

bool NSClientCallback::registerCallback( int ncode, const string& dg_jobid, callback_t callback ) 
{
  bool result = false;
  pthread_mutex_lock( &callback_registry_mutex );
  entry_list_t entry_list = callback_registry[ dg_jobid ];
  registry_entry_t registry_entry(ncode, callback);

  if( entry_list.empty() ||
      find( entry_list.begin(), entry_list.end(), registry_entry ) ==  entry_list.end() ) {
    entry_list.push_back( registry_entry );
    checkEnableListenerCond();
    result = true;
  }
  pthread_mutex_unlock( &callback_registry_mutex );
  if( result && !listener_thread )  runListenerThread();
  return ( result );
}

/**
 * Callback de-registration: remove a previously registered entry for the given 
 * call-back function, notification events and dg_jobid.
 * @param ncode The notification code the callback to unregister refers to.
 * @param dg_jobig A string containing the job id the notification refers to.
 * @param callback A pointer to a callback function.
 */
bool NSClientCallback::unregisterCallback( int ncode, const std::string& dg_jobid, callback_t callback)
{
  bool result = false;
  pthread_mutex_lock( &callback_registry_mutex );
  if( callback != NULL ) {
    entry_list_t entry_list = callback_registry[ dg_jobid ];
    registry_entry_t registry_entry(ncode, callback);
    entry_list_t::iterator it;
    if( !entry_list.empty() ||
	(it = find( entry_list.begin(), entry_list.end(), registry_entry )) !=  entry_list.end() ) {
      
      entry_list.erase( it );
      checkEnableListenerCond();
      result = true;
    }
  }
  else {
    
    callback_registry_t::const_iterator it = callback_registry.find( dg_jobid );
    if( it != callback_registry.end() ) {
      
      entry_list_t entry_list = callback_registry[ dg_jobid ];
      for( entry_list_t::iterator e = entry_list.begin(); e != entry_list.end(); e++ ) {    
	if( (*e).first == ncode  ) entry_list.erase( e );
	result = true;
      }
      if( result ) checkEnableListenerCond();
    }
  }
  pthread_mutex_unlock( &callback_registry_mutex );
  return( result );
}

/**
 * Callback de-registration: remove all previously registered entries for a job. 
 * @param dg_jobid A string containing the job id the notification refers to.
 * @return whether the remote callback function(s) has(have) been successful un-registered.
 */
bool NSClientCallback::unregisterCallback( const std::string& dg_jobid )
{
  bool result = false;
  pthread_mutex_lock( &callback_registry_mutex );
  callback_registry_t::const_iterator it =
     callback_registry.find( dg_jobid );
  if( it != callback_registry.end() ) {    
    callback_registry[ dg_jobid ].clear();
    checkEnableListenerCond();
    result = true;
  }
  pthread_mutex_unlock( &callback_registry_mutex );
  return( result );
}

/** 
 * Checks if the condition needed to enable the listener is met.
 * This function SHOULD be called with callback_registry_mutex locked.
 */
void NSClientCallback::checkEnableListenerCond( void )
{
  pthread_mutex_lock( &enable_listener_mutex );
  if( !(is_callback_registry_empty = callback_registry.empty()) )
    pthread_cond_signal( &enable_listener_cond );
  pthread_mutex_unlock( &enable_listener_mutex );
}

/** 
 * Checks if the condition needed to enable the dispatcehr is met.
 * This function SHOULD be called with notifications_mutex locked.
 */
void NSClientCallback::checkEnableDispatcherCond( void )
{ 
  pthread_mutex_lock( &enable_dispatcher_mutex );
  if( !(is_notified_queue_empty = notifiedAds.empty()) )
    pthread_cond_signal( &enable_dispatcher_cond );
  pthread_mutex_unlock( &enable_dispatcher_mutex );
}

bool NSClientCallback::lookupCallbackRegistry( const string& dg_jobid, int notification_code, list<callback_t>& to_call_list )
{
  bool result = false;
  
  pthread_mutex_lock( &callback_registry_mutex );
  
  callback_registry_t::const_iterator it = callback_registry.find( dg_jobid );
  if( it != callback_registry.end() ) {    
    
    entry_list_t entry_list = callback_registry[ dg_jobid ];
    for( entry_list_t::const_iterator e = entry_list.begin(); e != entry_list.end(); e++ ) {    
      
      if( ((*e).first & notification_code) == notification_code ) { 
	to_call_list.push_back( (*e).second );
	result = true;
      }
    }
  }
  pthread_mutex_unlock( &callback_registry_mutex );
  return result;
}

pthread_t NSClientCallback::executeCallback(callback_t callback, ClassAd* notifiedad)
{
  pthread_t tid = 0;
  pthread_mutex_lock( &dispatcher_busy_mutex );
  if( n_servicing < n_max_servicing ) { 
    
    callback_wrapper_arg_t* callback_wrapper_arg = 
      new callback_wrapper_arg_t(this, callback, notifiedad);
    
    if( pthread_create( &tid, 
			NULL, 
			(void*(*)(void*)) callback_wrapper, 
			callback_wrapper_arg) == 0 ) {
      n_servicing++;
    }
    else delete callback_wrapper_arg;
  }
  else {
    is_dispatcher_busy = true;
    pthread_cond_signal( &dispatcher_busy_cond );
  }
  pthread_mutex_unlock( &dispatcher_busy_mutex );
  return tid;
}

void NSClientCallback::releaseCallback()
{
  pthread_mutex_lock( &dispatcher_busy_mutex );
  n_servicing--;
  
  if( is_dispatcher_busy  &&
      n_servicing < n_max_servicing ) {
    
    is_dispatcher_busy = false;
    pthread_cond_signal( &dispatcher_busy_cond );
  }
  pthread_mutex_unlock( &dispatcher_busy_mutex );
}

/**
 * Finds a callback by notification.
 *
 * @version
 * @date September 16 2002.
 * @author Salvatore Monforte
 */
class find_callback_by_notification_t {
public:
  /**
   * Constructor.
   * @param nt the notification code.
   */
  find_callback_by_notification_t( int nt ) : fc_nt( nt ) {}

  /**
   * Redefines equality operator.
   * @param coppia the registry entry.
   */
  inline bool operator()( NSClientCallback::registry_entry_t &coppia ) { return( (coppia.first & this->fc_nt) == this->fc_nt ); }

private:
  /** the notification code. */
  int   fc_nt;
};


bool NSClientCallback::composeNotifications( int &code, const std::string &jobid, notification_type_t nt )
{
  bool                     answer = true, found = false;
  entry_list_t::iterator   elIt;

  code = 0;

  if( this->callback_registry.count(jobid) != 0 ) {
    entry_list_t      &entry = this->callback_registry[jobid];

    elIt = entry.begin();
    while( (elIt = find_if(elIt, entry.end(), find_callback_by_notification_t((int) nt))) != entry.end() ) {
      code |= elIt->first;
      elIt++; found = true;

      if( elIt == entry.end() )
	break;
    }

    answer = found;
  }
  else answer = false;

  return( answer );
}

void listener(void* context)
{

  socket_pp::GSISocketAgent* agent = NULL;

  try {
    pthread_detach( pthread_self() );

    NSClientCallback *theClient = static_cast<NSClientCallback*>(context);

    for(;;) // forever 
    {    
      // Blocks the listener if there is no call-back registered
      // and thus no notification no listen for.
      pthread_mutex_lock( &theClient -> enable_listener_mutex );
      while( theClient -> is_callback_registry_empty ) 
	{    
	  pthread_cond_wait( &theClient -> enable_listener_cond, 
			     &theClient -> enable_listener_mutex );
	  
	}
      pthread_mutex_unlock( &theClient -> enable_listener_mutex );
      agent = theClient -> notification_listener -> Listen();
      
      string strNotifiedAd;
      if( agent -> Receive( strNotifiedAd ) ) 
	{
	  ClassAdParser parser;
	  ClassAd* NotifiedAd = parser.ParseClassAd( strNotifiedAd );
	  
	  if( NotifiedAd != NULL &&
	      pthread_mutex_lock( &theClient -> notifications_mutex ) == 0 ) 
	    {	      
	      theClient -> notifiedAds.push( NotifiedAd  );
	      theClient -> checkEnableDispatcherCond();
	      pthread_mutex_unlock( &theClient -> notifications_mutex );
	    }
	  else 
	    {
#ifdef DEBUG  
	      cerr << "Error: malformed NotifiedAd..." << endl << flush;
#endif	    
	    }
	} 
      theClient -> notification_listener -> KillAgent(agent);  
    }
  pthread_exit( NULL );

  } catch (socket_pp::AuthenticationException& ae) {
    AuthenticationException e(ae.getMessage());
    e.setCause("uithreads::listener(): " + ae.getCause());
    e.setSourceInfo(ae.getSourceInfo());
    throw e;    
  } catch (socket_pp::AuthorizationException& aue) {
    NotAuthorizedUserException e(aue.getMessage()); 
    e.setCause("uithreads::listener(): " + aue.getCause());
    e.setSourceInfo(aue.getSourceInfo()); 
    throw e; 
  } catch (socket_pp::IOException& ioe) {
    ConnectionException e( agent->HostName() );
    e.setCause("uithreads::listener(): " + ioe.getCause());
    e.setSourceInfo(ioe.getSourceInfo() + ", " + ioe.getMessage());
    throw e;
  }

}

void dispatcher(void* context)
{
  pthread_detach( pthread_self() );
  NSClientCallback *theClient = static_cast<NSClientCallback*>(context);
  
  for(;;) // forever 
    {       
      // Blocks the dispatcher if there is no notifiedad waiting in queue
      // and thus no notification to dispatch for.
      pthread_mutex_lock( &theClient -> enable_dispatcher_mutex );
      while( theClient -> is_notified_queue_empty )
	{
#ifdef DEBUG  
	  cout << "Notification dispatcher idle!" << flush << endl;
#endif  
	  pthread_cond_wait( &theClient -> enable_dispatcher_cond, 
			     &theClient -> enable_dispatcher_mutex );
	}
      pthread_mutex_unlock( &theClient -> enable_dispatcher_mutex );

      
      pthread_mutex_lock( &theClient -> notifications_mutex );
      
      while( !theClient -> notifiedAds.empty() ) {
	
	list<NSClientCallback::callback_t> to_call_list;
	
	// get the first notified-ads waiting in queue
	ClassAd* ad = theClient -> notifiedAds.front();
	
	switch( notifiedad_t(ad).type() ) {
	  
	case SUBMIT_STATUS:
	  theClient -> 
	    lookupCallbackRegistry( notifiedad_t(ad).dg_jobId(), 
				      notifiedad_t(ad).job_status(),
				      to_call_list );
	  break;
	  
	case LIST_MATCH_STATUS:
	  theClient -> 
	    lookupCallbackRegistry( notifiedad_t(ad).dg_jobId(), 
				      notifiedad_t(ad).is_rank_reported() ? LIST_MATCH_STATUS | WITH_RANK :
				      LIST_MATCH_STATUS,
				      to_call_list );
	  break;
	  
	case CANCEL_STATUS:
	  theClient -> 
	    lookupCallbackRegistry( notifiedad_t(ad).dg_jobId(), 
				      CANCEL_STATUS,
				      to_call_list );
	  break;
	  
	case GET_OUTPUT_STATUS:
	  theClient -> 
	    lookupCallbackRegistry( notifiedad_t(ad).dg_jobId(), 
				      notifiedad_t(ad).refers_to_sandboxfile() ? 
				      GET_OUTPUT_STATUS | FOR_EACH_FILE :
				      GET_OUTPUT_STATUS,
				      to_call_list );
	  break;
	}
	
	while( !to_call_list.empty() ) {
	  
	  // Blocks the dispatcher if there are too many running threads
	  // performing user defined call-back code.
	  pthread_mutex_lock( &theClient -> dispatcher_busy_mutex );
	  while( !theClient -> is_dispatcher_busy )
	    {
#ifdef DEBUG  
	      cout << "Notification dispatcher busy!" << flush << endl;
#endif  
	      pthread_cond_wait( &theClient -> dispatcher_busy_cond, 
				 &theClient -> dispatcher_busy_mutex );
	    }
	  pthread_mutex_unlock( &theClient -> enable_dispatcher_mutex );
	  theClient -> executeCallback( to_call_list.front(), ad );
	  to_call_list.pop_front();
	}
      }
    }
}

void callback_wrapper(void *callback_wrapper_arg)
{
  pthread_detach( pthread_self() );
  NSClientCallback* theClient = ((NSClientCallback::callback_wrapper_arg_t*) callback_wrapper_arg) -> first;
  NSClientCallback::callback_t callback = ((NSClientCallback::callback_wrapper_arg_t*) callback_wrapper_arg) -> second;
  ClassAd* ad = ((NSClientCallback::callback_wrapper_arg_t*) callback_wrapper_arg) -> third;

  delete ((NSClientCallback::callback_wrapper_arg_t*) callback_wrapper_arg);

  callback( ad );

  theClient -> releaseCallback();
  pthread_exit( NULL );
}

} // namespace client
} // namespace networkserver
} // namespace workload
} // namespace edg












