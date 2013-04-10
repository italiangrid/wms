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
#ifndef GLITE_WMS_ICE_THREADPOOL_H
#define GLITE_WMS_ICE_THREADPOOL_H

#include "iceThread.h"
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <string>

// Forward declaration
namespace log4cpp {
    class Category;
}

namespace glite {
namespace wms {
namespace ice {

    class IceAbstractCommand;
    
namespace threads {

    struct iceThreadPoolState;
    
    class iceThreadPool {
    public:

        /**
         * Creates a new thread pool. 
         *
         * @param name the name of the thread pool
         *
         * @param s the number of worker threads in this pool.  If
         * s<1, then the pool will have exactly one thread in the
         * pool.
         */
        iceThreadPool( const std::string& name, int s );

        virtual ~iceThreadPool( );
                
        /**
         * Adds a request to the thread pool. The request is
         * assigned (and executed) immediately if a thread is
         * available; otherwise, the request is enqueued and
         * will be served in FIFO order.
         *
         * @param req The request (command) to enqueue/execute
         */
        void add_request( glite::wms::ice::IceAbstractCommand* req );

        /**
         * Gets the number of requests currently in the queue.
         *
         * @return the number of requests (commands) currently in the queue
         */        
        int get_command_count( void ) const;

	void stopAllThreads( void ) throw();
  
    protected:       

        std::list<iceThread*> m_thread_list;

        /**
         * The class of worker threads
         */
        class iceThreadPoolWorker : public iceThread {
        public:
            iceThreadPoolWorker( iceThreadPoolState* st, int id );
            virtual ~iceThreadPoolWorker( );
        protected:
            void body( );
            
            /**
             * Gets an iterator to the first request into the request
             * queue which can be processed. This means, gets the
             * first request in the queue which is related to a job
             * whose ID is not in the m_state->m_pending_jobs set.
             *
             * @return an iterator to a command in the queue, or
             * end() if no such command exists.
             */
            std::list< IceAbstractCommand* >::iterator get_first_request( void );
            
            iceThreadPoolState* m_state;
            const int m_threadNum; //!< the id of this thread
            log4cpp::Category* m_log_dev;
        };
        
        boost::scoped_ptr< iceThreadPoolState > m_state;
        boost::thread_group m_all_threads;
        log4cpp::Category* m_log_dev;
    };
    
} // namespace util
} // namespace ice
} // namespace wms
} // namespace glite

#endif
