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

#ifndef ICE_CE_BLACKLIST_H
#define ICE_CE_BLACKLIST_H

#include <string>
#include <ctime>
#include <map>
#include "boost/thread/recursive_mutex.hpp"

namespace log4cpp {
    class Category;
}

namespace glite {
namespace wms {
namespace ice {
namespace util {

    /**
     * A CEBlackList is a data structure which contains a set of
     * "blacklisted" endpoints. No interaction with blacklisted
     * endpoint should be attempted. An endpoint stays in the
     * blacklist for a fixed maximum amount of time (at the moment
     * this amount is given by the max_blacklist_time constant in this
     * data structure, in the future it will be configurable). After
     * the blacklist time expires, an endpoint is removed from the
     * blacklist and operations on it can be attempted again.
     */
    class CEBlackList {
    
      
    
    protected:
        CEBlackList( );

        /**
         * Iterates over the set of blacklisted endpoints, and removes
         * all expired entries.
         *
         * @param force if true, then all expired elements are removed
         * by this call. If false, then expired elements MAY NOT be
         * removed if the number of operations is less than
         * m_operation_count.
         *
         * This method is _NOT_ thread-safe.
         */
        void cleanup_blacklist( bool force = false );
        
        static CEBlackList* s_instance; ///< Singleton instance of this blacklist
        static boost::recursive_mutex m_mutex; ///< Mutex

        log4cpp::Category* m_log_dev;
        std::size_t m_operation_count; ///< number of operations on the blacklist
        std::size_t m_operation_count_max; ///< max number of operations before the blacklist is purged from stale entries
        std::size_t m_max_blacklist_time; ///< max time (in seconds) an entry is kept in the blacklist
	//bool m_fail_jobs_to_BLCE; // if true, a job destined to a Blacklisted CE will be immediately failed

        std::map< std::string, std::time_t > m_blacklist; ///< map (endpoint) -> expiration time

    public:
        ~CEBlackList( ) { };

        /**
         * Returns the singleton instance of this class. This method
         * is thread-safe.
         *
         * @return The singleton instance of this class. The caller
         * DOES NOT OWN the returned pointer, and must NOT be
         * deallocated.
         */
        static CEBlackList* instance( );

        /**
         * Adds a new ce into the blacklist. The entry will expire
         * at time() + max_blacklist_time. This method is thread-safe.
         *
         * @param endpoint The endpoint to blacklist. If the given
         * endpoint is already in the blacklist, this operation does nothing.
         */
        void blacklist_endpoint( const std::string& endpoint );

        /**
         * Returns true iff a CE is blacklisted. This method is thread-safe.
         *
         * @param endpoint The endpoint to look for.
         *
         * @return true iff the endpoint is blacklisted.
         */
        bool is_blacklisted( const std::string& endpoint );
    };

} // namespace util
} // namespace ice
} // namespace wms
} // namespace glite

#endif
