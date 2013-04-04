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

#ifndef GLITE_WMS_ICETHREADPOOLSTATE
#define GLITE_WMS_ICETHREADPOOLSTATE

#include <boost/thread/condition.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <list>
#include <set>
#include <string>

namespace glite {
namespace wms {
namespace ice {

    class IceAbstractCommand;

namespace util {

    class iceThreadPoolState {
    public:
        /**
         * @param name the name of this thread pool
         *
         * @param n the number of threads in the pool
         */
        iceThreadPoolState( const std::string& name, int n ) :
            m_num_running( ( n < 1 ? 1 : n ) ),
            m_name( name )
            { }
        
        int m_num_running; ///< Number of running threads
        const std::string m_name; ///< name of the pool
        boost::recursive_mutex m_mutex; ///< Mutex to protect the shared state object
        boost::condition m_no_requests_available; ///< Condition triggered when there is a new request in the queue
        std::list< IceAbstractCommand* > m_requests_queue; ///< The queue of requests (commands to be issued to CREAM)
        std::set< std::string > m_pending_jobs; ///< Grei Job IDs of pending jobs (jobs being processed)
    };

} // namespace util
} // namespace ice
} // namespace wms
} // namespace glite

#endif
