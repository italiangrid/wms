/* LICENSE:
Copyright (c) Members of the EGEE Collaboration. 2010. 
See http://www.eu-egee.org/partners/ for details on the copyright
holders.  

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

   http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. 
See the License for the specific language governing permissions and 
limitations under the License.

END LICENSE */
#include "IceThreadPool.h"
#include "IceThreadPoolState.h"
#include "commands/IceAbstractCommand.h"
#include "main/Main.h"
#include "commands/IceCommandFatalException.h"
#include "commands/IceCommandTransientException.h"
#include "utils/logging.h"

//#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/soap_runtime_ex.h"

#include "common/src/configuration/ICEConfiguration.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <csignal>

namespace conf_ns=glite::wms::ice::util;
using namespace glite::wms::ice;
using namespace std;
//namespace api_util = glite::ce::cream_client_api::util;

//______________________________________________________________________________
/**
 * Definition of the inner worker thread class
 */
threads::IceThreadPool::IceThreadPoolWorker::IceThreadPoolWorker( IceThreadPoolState* st, int id ) :
    IceThread( boost::str( boost::format( "IceThreadPoolWorker(pool=%1%, id=%2%)" ) % st->m_name % id ) ),
    m_state( st ),
    m_threadNum( id )
    //m_log_dev( glite::ce::cream_client_api::util::creamApiLogger::instance()->getLogger() )
{
  edglog_fn("IceThreadPool::IceThreadPoolWorker::IceThreadPoolWorker");
  sigset_t set;
  ::sigemptyset(&set);
  ::sigaddset(&set, SIGCHLD);
  if(::pthread_sigmask( SIG_BLOCK, &set, 0 ) < 0 ) 
    //CREAM_SAFE_LOG( m_log_dev->fatalStream() 
    edglog(fatal)<< "IceThreadPoolWorker::CTOR"
  	                                     << "pthread_sigmask failed. This could compromise correct working"
                	                     << " of ICE's threads..."<<endl;// );
                
}

//______________________________________________________________________________
threads::IceThreadPool::IceThreadPoolWorker::~IceThreadPoolWorker( )
{

}

//______________________________________________________________________________
void threads::IceThreadPool::IceThreadPoolWorker::body( )
{
    static const char* method_name = "IceThreadPoolWorker::body() - ";
    edglog_fn("IceThreadPool::IceThreadPoolWorker::body");
    while( !isStopped() ) {

        boost::scoped_ptr< IceAbstractCommand > cmd;
        {
            boost::recursive_mutex::scoped_lock L( m_state->m_mutex );
            
            while ( m_state->m_requests_queue.end() == get_first_request() ) {
                try {
                    --m_state->m_num_running;
		    
		    m_state->m_no_requests_available.wait( L );

		    if( isStopped() ) return;

                    ++m_state->m_num_running;
                } catch( boost::lock_error& err ) {
                    //CREAM_SAFE_LOG( m_log_dev->fatalStream() << method_name
                                  edglog(fatal)  << "Worker Thread " 
                                    << m_state->m_name << "/" << m_threadNum 
                                    << " raised the following lock_error "
                                    << "exception while waiting on the "
                                    << "command queue: " << err.what()
                                    << ". Giving up."<<endl;
                                    //);
                    abort();
                }
            } 
            // Remove one request from the queue
            list< IceAbstractCommand* >::iterator req_it = get_first_request( );
            assert( req_it != m_state->m_requests_queue.end() );
            IceAbstractCommand* cmd_ptr = *req_it;
            cmd.reset( cmd_ptr );
            m_state->m_requests_queue.erase( req_it );
            m_state->m_pending_jobs.insert( cmd->get_grid_job_id() );

            //CREAM_SAFE_LOG(
                           //m_log_dev->debugStream() << method_name
                           edglog(debug)<< "Worker Thread "
                           << m_state->m_name << "/" << m_threadNum 
                           << " started processing new request"
                           << " (Currently " << m_state->m_num_running
                           << " threads are running, "
                           << m_state->m_requests_queue.size()
                           << " commands in the queue)"<<endl;
                           //);

        } // releases lock

        try {
            string label = boost::str( boost::format( "%1% TIMER %2% cmd=%3% threadid=%4%" ) % method_name % cmd->name() % m_state->m_name % m_threadNum );

/*	    CREAM_SAFE_LOG(
                           m_log_dev->debugStream() << method_name
                           << "Thread [" 
			   <<  m_state->m_name << "] is executing an AbsCommand [" 
			   << cmd->name() << "]"
                           );
*/
            cmd->execute( getThreadID() );

/*	    CREAM_SAFE_LOG(
                           m_log_dev->debugStream() << method_name
                           << "Thread [" 
                           <<  m_state->m_name << "] has finished execution of command ["
                           << cmd->name() << "]"
                           );
*/
        } catch ( glite::wms::ice::IceCommandFatalException& ex ) {
           // CREAM_SAFE_LOG( 
                          // m_log_dev->errorStream() << method_name
                          edglog(error) << "Command execution got FATAL exception: "
                           << ex.what()<<endl;
//                           );
        } catch ( glite::wms::ice::IceCommandTransientException& ex ) {
          //  CREAM_SAFE_LOG(
                          // m_log_dev->errorStream() << method_name
                          edglog(error) << "Command execution got TRANSIENT exception: "
                           << ex.what() << endl;
//                           );
        } catch( glite::ce::cream_client_api::soap_proxy::soap_runtime_ex& ex ) {

//	  CREAM_SAFE_LOG(
			// m_log_dev->fatalStream() << method_name
			edglog(fatal) << "A VERY SEVERE error occurred: "
			 << ex.what() << ". Shutting down !!" << endl;
			 //);
	  exit(2);

	} catch( exception& ex ) {
            //CREAM_SAFE_LOG(
                          // m_log_dev->errorStream() << method_name
                         edglog(error)  << "Command execution got exception: "
                           << ex.what()<< endl;
                          // );
        } catch( ... ) {
            //CREAM_SAFE_LOG(
                          // m_log_dev->errorStream() << method_name
                          edglog(error) << "Command execution got unknown exception" << endl;
                          // );
        }

        // Now, wake up another worker thread (just in case someone is
        // waiting for us to complede)
        boost::recursive_mutex::scoped_lock L( m_state->m_mutex );
        m_state->m_pending_jobs.erase( cmd->get_grid_job_id() );
        m_state->m_no_requests_available.notify_all();

    }
    //CREAM_SAFE_LOG(
		   //m_log_dev->debugStream()
		   edglog(debug)<< "IceThreadPool::IceThreadPoolWorker::body() - Thread ["
		   << getName() << "] ENDING ..."<<endl;
//		   );
}

//______________________________________________________________________________
list< glite::wms::ice::IceAbstractCommand* >::iterator
threads::IceThreadPool::IceThreadPoolWorker::get_first_request( void )
{
    list< glite::wms::ice::IceAbstractCommand* >::iterator it = m_state->m_requests_queue.begin();
    while ( ( it != m_state->m_requests_queue.end() ) &&
            ( m_state->m_pending_jobs.end() != 
              m_state->m_pending_jobs.find( (*it)->get_grid_job_id() ) ) ) {
        ++it;
    }
    return it;
}


//______________________________________________________________________________
/**
 *
 * Implementation of the IceThreadPool class
 *
 */
threads::IceThreadPool::IceThreadPool( const std::string& name, int s ) :
    m_state( new IceThreadPoolState(name, s) ),
    m_all_threads( )
   // m_log_dev( glite::ce::cream_client_api::util::creamApiLogger::instance()->getLogger() )
{
    edglog_fn("IceThreadPool::IceThreadPool");
    int n_threads = m_state->m_num_running;
    //CREAM_SAFE_LOG( m_log_dev->debugStream()
                   edglog(debug) << "IceThreadPool::IceThreadPool("
                    << m_state->m_name << ") - "
                    << "Creating " << m_state->m_num_running 
                    << " worker threads" << endl;
                    
                   // );            
    for ( int i=0; i<n_threads; i++ ) {
        boost::shared_ptr< threads::IceThread > ptr_thread( new IceThreadPoolWorker( m_state.get(), i ) );
        boost::thread* thr;
        try {
            thr = new boost::thread(boost::bind(&threads::IceThread::operator(), ptr_thread ) );
        } catch( boost::thread_resource_error& ex ) {
            //CREAM_SAFE_LOG( m_log_dev->fatalStream()
                           edglog(fatal) << "IceThreadPool::IceThreadPool("
                            << m_state->m_name << ") -"
                            << "Unable to create worker thread. Giving up."<<endl;
//                            
  //                          );            
            abort();
        }
        m_all_threads.add_thread( thr );

	m_thread_list.push_back( ptr_thread.get() );

    }
}

//______________________________________________________________________________
threads::IceThreadPool::~IceThreadPool( )
{

}

//______________________________________________________________________________
void threads::IceThreadPool::stopAllThreads( void ) throw()
{
  edglog_fn("IceThreadPool::stopAllThreads");
  for(list<IceThread*>::iterator thisThread = m_thread_list.begin();
      thisThread != m_thread_list.end();
      ++thisThread) { 
     /// CREAM_SAFE_LOG( m_log_dev->debugStream()
		     edglog(debug) << "IceThreadPool::stopAllThreads() - "
		      << "Calling ::stop() on thread ["
		      << (*thisThread)->getName() << "]" << endl;		      
		     // );
      (*thisThread)->stop(); 
    }
 
  m_state->m_no_requests_available.notify_all();
 
  //CREAM_SAFE_LOG( m_log_dev->debugStream()
		 edglog(debug) << "IceThreadPool::stopAllThreads() - "
		  << "Waiting for all pool-thread termination ..." << endl;  
                  //);  
  
  m_all_threads.join_all();
  
  //CREAM_SAFE_LOG( m_log_dev->fatalStream()
		 edglog(fatal) << "IceThreadPool::stopAllThreads() - "
		  << "All pool-threads TERMINATED !" << endl;
                 // );

}

//______________________________________________________________________________
void threads::IceThreadPool::add_request( glite::wms::ice::IceAbstractCommand* req )
{
    boost::recursive_mutex::scoped_lock L( m_state->m_mutex );
    m_state->m_requests_queue.push_back( req );
    m_state->m_no_requests_available.notify_all(); // wake up one worker thread
}

//______________________________________________________________________________
int threads::IceThreadPool::get_command_count( void ) const
{
    boost::recursive_mutex::scoped_lock L( m_state->m_mutex );
    return m_state->m_requests_queue.size();
}
