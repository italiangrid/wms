/*
 * Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://www.eu-egee.org/partners/ for details on the copyright
 * holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ICE thread pool class
 *
 * Authors: Alvise Dorigo <alvise.dorigo@pd.infn.it>
 *          Moreno Marzolla <moreno.marzolla@pd.infn.it>
 */

#include "iceThreadPool.h"
#include "iceThreadPoolState.h"
#include "iceAbsCommand.h"
#include "ice-core.h"
#include "iceConfManager.h"
#include "iceCommandFatal_ex.h"
#include "iceCommandTransient_ex.h"
//#include "CreamProxyFactory.h"

//#include "glite/ce/cream-client-api-c/CreamProxy.h"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/soap_runtime_ex.h"

#include "glite/wms/common/configuration/ICEConfiguration.h"

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

namespace conf_ns=glite::wms::ice::util;
using namespace glite::wms::ice::util;
using namespace std;

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

}

//______________________________________________________________________________
iceThreadPool::iceThreadPoolWorker::~iceThreadPoolWorker( )
{

}

//______________________________________________________________________________
void iceThreadPool::iceThreadPoolWorker::body( )
{
    while( !isStopped() ) {
        boost::scoped_ptr< iceAbsCommand > cmd;
        {
            boost::recursive_mutex::scoped_lock L( m_state->m_mutex );
            
            while ( m_state->m_requests_queue.end() == get_first_request() ) {
                try {
                    --m_state->m_num_running;
                    m_state->m_no_requests_available.wait( L );
                    ++m_state->m_num_running;
                } catch( boost::lock_error& err ) {
                    CREAM_SAFE_LOG( m_log_dev->fatalStream()
                                    << "iceThreadPoolWorker::body() - "
                                    << "Worker Thread " 
                                    << m_state->m_name << "/" << m_threadNum 
                                    << " raised the following lock_error "
                                    << "exception while waiting on the "
                                    << "command queue: " << err.what()
                                    << ". Giving up."
                                    << log4cpp::CategoryStream::ENDLINE
                                    );
                    abort();
                }
            } 
            CREAM_SAFE_LOG(
                           m_log_dev->debugStream()
                           << "iceThreadPoolWorker::body() - "
                           << "Worker Thread "
                           << m_state->m_name << "/" << m_threadNum 
                           << " started processing new request"
                           << " (Currently " << m_state->m_num_running
                           << " threads are running)" 
                           << log4cpp::CategoryStream::ENDLINE
                           );
            // Remove one request from the queue
            list< iceAbsCommand* >::iterator req_it = get_first_request( );
            assert( req_it != m_state->m_requests_queue.end() );
            iceAbsCommand* cmd_ptr = *req_it;
            cmd.reset( cmd_ptr );
            m_state->m_requests_queue.erase( req_it );
            m_state->m_pending_jobs.insert( cmd->get_grid_job_id() );
        } // releases lock

        try {
            cmd->execute( );
        } catch ( glite::wms::ice::iceCommandFatal_ex& ex ) {
            CREAM_SAFE_LOG( 
                           m_log_dev->errorStream()
                           << "iceThreadPool::iceThreadPoolWorker::body() - Command execution got FATAL exception: "
                           << ex.what()
                           << log4cpp::CategoryStream::ENDLINE
                           );
        } catch ( glite::wms::ice::iceCommandTransient_ex& ex ) {
            CREAM_SAFE_LOG(
                           m_log_dev->errorStream()
                           << "iceThreadPool::iceThreadPoolWorker::body() - Command execution got TRANSIENT exception: "
                           << ex.what()
                           << log4cpp::CategoryStream::ENDLINE
                           );
        } catch( glite::ce::cream_client_api::soap_proxy::soap_runtime_ex& ex ) {

	  CREAM_SAFE_LOG(
			 m_log_dev->fatalStream()
			 << "ceThreadPool::iceThreadPoolWorker::body()  - "
			 << "A VERY SEVERE error occurred: "
			 << ex.what() << ". Shutting down !!"
			 << log4cpp::CategoryStream::ENDLINE
			 );
	  exit(2);

	} catch( exception& ex ) {
            CREAM_SAFE_LOG(
                           m_log_dev->errorStream()
                           << "ceThreadPool::iceThreadPoolWorker::body() - Command execution got exception: "
                           << ex.what()
                           << log4cpp::CategoryStream::ENDLINE
                           );
        } catch( ... ) {
            CREAM_SAFE_LOG(
                           m_log_dev->errorStream()
                           << "iceThreadPool::iceThreadPoolWorker::body() - Command execution got unknown exception"
                           << log4cpp::CategoryStream::ENDLINE
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
		   << "iceThreadPool::iceThreadPoolWorker::body() - I'm ENDING ..."
		   << log4cpp::CategoryStream::ENDLINE
		   );
}

//______________________________________________________________________________
list< glite::wms::ice::iceAbsCommand* >::iterator iceThreadPool::iceThreadPoolWorker::get_first_request( void )
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
                    << log4cpp::CategoryStream::ENDLINE
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
                            << log4cpp::CategoryStream::ENDLINE
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
      ++thisThread) { (*thisThread)->stop(); }
  
  CREAM_SAFE_LOG( m_log_dev->fatalStream()
		  << "iceThreadPool::stopAllThreads() - "
		  << "Waiting for all pool-thread termination ..."
		  << log4cpp::CategoryStream::ENDLINE
		  );  

  m_all_threads.join_all();
  
  CREAM_SAFE_LOG( m_log_dev->fatalStream()
		  << "iceThreadPool::stopAllThreads() - "
		  << "All pool-threads TERMINATED !"
		  << log4cpp::CategoryStream::ENDLINE
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
