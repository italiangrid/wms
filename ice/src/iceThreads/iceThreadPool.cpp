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
#include "iceThreadPool.h"
#include "iceThreadPoolState.h"
#include "iceCommands/iceAbsCommand.h"
#include "ice/IceCore.h"
#include "iceCommands/iceCommandFatal_ex.h"
#include "iceCommands/iceCommandTransient_ex.h"

//#include "glite/ce/cream-client-api-c/scoped_timer.h"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/soap_runtime_ex.h"

#include "glite/wms/common/configuration/ICEConfiguration.h"

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include <csignal>

namespace conf_ns=glite::wms::ice::util;
using namespace glite::wms::ice::util;
using namespace std;
namespace api_util = glite::ce::cream_client_api::util;

//______________________________________________________________________________
/**
 * Definition of the inner worker thread class
 */
iceThreadPool::iceThreadPoolWorker::iceThreadPoolWorker( iceThreadPoolState* st, int id ) :
    iceThread( boost::str( boost::format( "iceThreadPoolWorker(pool=%1%, id=%2%)" ) % st->m_name % id ) ),
    m_state( st ),
    m_threadNum( id ),
    m_log_dev( glite::ce::cream_client_api::util::creamApiLogger::instance()->getLogger() )
{
  sigset_t set;
  ::sigemptyset(&set);
  ::sigaddset(&set, SIGCHLD);
  if(::pthread_sigmask( SIG_BLOCK, &set, 0 ) < 0 ) 
    CREAM_SAFE_LOG( m_log_dev->fatalStream() << "iceThreadPoolWorker::CTOR"
  	                                     << "pthread_sigmask failed. This could compromise correct working"
                	                     << " of ICE's threads..." );
                
}

//______________________________________________________________________________
iceThreadPool::iceThreadPoolWorker::~iceThreadPoolWorker( )
{

}

//______________________________________________________________________________
void iceThreadPool::iceThreadPoolWorker::body( )
{
    static const char* method_name = "iceThreadPoolWorker::body() - ";

    while( !isStopped() ) {

        boost::scoped_ptr< iceAbsCommand > cmd;
        {
            boost::recursive_mutex::scoped_lock L( m_state->m_mutex );
            
            while ( m_state->m_requests_queue.end() == get_first_request() ) {
                try {
                    --m_state->m_num_running;
		    
		    m_state->m_no_requests_available.wait( L );

		    if( isStopped() ) return;

                    ++m_state->m_num_running;
                } catch( boost::lock_error& err ) {
                    CREAM_SAFE_LOG( m_log_dev->fatalStream() << method_name
                                    << "Worker Thread " 
                                    << m_state->m_name << "/" << m_threadNum 
                                    << " raised the following lock_error "
                                    << "exception while waiting on the "
                                    << "command queue: " << err.what()
                                    << ". Giving up."
                                    );
                    abort();
                }
            } 
            // Remove one request from the queue
            list< iceAbsCommand* >::iterator req_it = get_first_request( );
            assert( req_it != m_state->m_requests_queue.end() );
            iceAbsCommand* cmd_ptr = *req_it;
            cmd.reset( cmd_ptr );
            m_state->m_requests_queue.erase( req_it );
            m_state->m_pending_jobs.insert( cmd->get_grid_job_id() );

            CREAM_SAFE_LOG(
                           m_log_dev->debugStream() << method_name
                           << "Worker Thread "
                           << m_state->m_name << "/" << m_threadNum 
                           << " started processing new request"
                           << " (Currently " << m_state->m_num_running
                           << " threads are running, "
                           << m_state->m_requests_queue.size()
                           << " commands in the queue)"
                           );

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
        } catch ( glite::wms::ice::iceCommandFatal_ex& ex ) {
            CREAM_SAFE_LOG( 
                           m_log_dev->errorStream() << method_name
                           << "Command execution got FATAL exception: "
                           << ex.what()
                           );
        } catch ( glite::wms::ice::iceCommandTransient_ex& ex ) {
            CREAM_SAFE_LOG(
                           m_log_dev->errorStream() << method_name
                           << "Command execution got TRANSIENT exception: "
                           << ex.what()
                           );
        } catch( glite::ce::cream_client_api::soap_proxy::soap_runtime_ex& ex ) {

	  CREAM_SAFE_LOG(
			 m_log_dev->fatalStream() << method_name
			 << "A VERY SEVERE error occurred: "
			 << ex.what() << ". Shutting down !!"
			 );
	  exit(2);

	} catch( exception& ex ) {
            CREAM_SAFE_LOG(
                           m_log_dev->errorStream() << method_name
                           << "Command execution got exception: "
                           << ex.what()
                           );
        } catch( ... ) {
            CREAM_SAFE_LOG(
                           m_log_dev->errorStream() << method_name
                           << "Command execution got unknown exception"
                           );
        }

        // Now, wake up another worker thread (just in case someone is
        // waiting for us to complede)
        boost::recursive_mutex::scoped_lock L( m_state->m_mutex );
        m_state->m_pending_jobs.erase( cmd->get_grid_job_id() );
        m_state->m_no_requests_available.notify_all();

    }
    CREAM_SAFE_LOG(
		   m_log_dev->debugStream()
		   << "iceThreadPool::iceThreadPoolWorker::body() - Thread ["
		   << getName() << "] ENDING ..."
		   );
}

//______________________________________________________________________________
list< glite::wms::ice::iceAbsCommand* >::iterator
iceThreadPool::iceThreadPoolWorker::get_first_request( void )
{
    list< glite::wms::ice::iceAbsCommand* >::iterator it = m_state->m_requests_queue.begin();
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
 * Implementation of the iceThreadPool class
 *
 */
iceThreadPool::iceThreadPool( const std::string& name, int s ) :
    m_state( new iceThreadPoolState(name, s) ),
    m_all_threads( ),
    m_log_dev( glite::ce::cream_client_api::util::creamApiLogger::instance()->getLogger() )
{
    int n_threads = m_state->m_num_running;
    CREAM_SAFE_LOG( m_log_dev->debugStream()
                    << "iceThreadPool::iceThreadPool("
                    << m_state->m_name << ") - "
                    << "Creating " << m_state->m_num_running 
                    << " worker threads"
                    
                    );            
    for ( int i=0; i<n_threads; i++ ) {
        boost::shared_ptr< util::iceThread > ptr_thread( new iceThreadPoolWorker( m_state.get(), i ) );
        boost::thread* thr;
        try {
            thr = new boost::thread(boost::bind(&util::iceThread::operator(), ptr_thread ) );
        } catch( boost::thread_resource_error& ex ) {
            CREAM_SAFE_LOG( m_log_dev->fatalStream()
                            << "iceThreadPool::iceThreadPool("
                            << m_state->m_name << ") -"
                            << "Unable to create worker thread. Giving up."
                            
                            );            
            abort();
        }
        m_all_threads.add_thread( thr );

	m_thread_list.push_back( ptr_thread.get() );

    }
}

//______________________________________________________________________________
iceThreadPool::~iceThreadPool( )
{

}

//______________________________________________________________________________
void iceThreadPool::stopAllThreads( void ) throw()
{

  for(list<iceThread*>::iterator thisThread = m_thread_list.begin();
      thisThread != m_thread_list.end();
      ++thisThread) { 
      CREAM_SAFE_LOG( m_log_dev->debugStream()
		      << "iceThreadPool::stopAllThreads() - "
		      << "Calling ::stop() on thread ["
		      << (*thisThread)->getName() << "]"		      
		      );
      (*thisThread)->stop(); 
    }
 
  m_state->m_no_requests_available.notify_all();
 
  CREAM_SAFE_LOG( m_log_dev->debugStream()
		  << "iceThreadPool::stopAllThreads() - "
		  << "Waiting for all pool-thread termination ..."  
                  );  
  
  m_all_threads.join_all();
  
  CREAM_SAFE_LOG( m_log_dev->fatalStream()
		  << "iceThreadPool::stopAllThreads() - "
		  << "All pool-threads TERMINATED !"
                  );

}

//______________________________________________________________________________
void iceThreadPool::add_request( glite::wms::ice::iceAbsCommand* req )
{
    boost::recursive_mutex::scoped_lock L( m_state->m_mutex );
    m_state->m_requests_queue.push_back( req );
    m_state->m_no_requests_available.notify_all(); // wake up one worker thread
}

//______________________________________________________________________________
int iceThreadPool::get_command_count( void ) const
{
    boost::recursive_mutex::scoped_lock L( m_state->m_mutex );
    return m_state->m_requests_queue.size();
}
