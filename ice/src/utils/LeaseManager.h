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

#ifndef ICE_LEASE_MANAGER_H
#define ICE_LEASE_MANAGER_H

#include "CreamJob.h"

#include <exception>
#include <iostream>
#include <string>
#include <functional>
#include <ctime>
#include "boost/multi_index_container.hpp"
#include "boost/multi_index/ordered_index.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/sequenced_index.hpp"
#include "boost/multi_index/composite_key.hpp"
#include "boost/thread/recursive_mutex.hpp"

namespace log4cpp {
    class Category;
}

namespace glite {
namespace wms {
namespace ice {
namespace util {

    class lease_exception : public std::exception {
    public:
        lease_exception( const char* reason );
        lease_exception( const std::string& reason );
        virtual ~lease_exception() throw() { };
        virtual const char* what( void ) const throw();
    private:
        std::string m_reason;
    };

    class Lease_manager {
    public:

        /**
         * This datatype represents an Entry of the lease cache.
         */
        struct Lease_t {
            std::string m_user_dn; //< DN of the user owning this lease
            std::string m_cream_url; //< Endpoint of the CREAM service where the lease has been defined
            time_t m_expiration_time; //< (Absolute) time of the lease expiration
            std::string m_lease_id; //< ID of the lease

            Lease_t( const std::string& user_dn, const std::string& cream_url, time_t expiration_time, const std::string& lease_id ) :
                m_user_dn( user_dn ),
                m_cream_url( cream_url ),
                m_expiration_time( expiration_time ),
                m_lease_id( lease_id )
            { };
        };

        /**
         * Multi index container 
         */
        struct idx_user_dn_cream_url{}; // used for tagging the index according to the ( m_user_dn, m_cream_url ) pair
        struct idx_lease_id{}; // used for tagging the index according to the m_lese_id string
        struct idx_expiration_time{}; // used for tagging the index according to the m_expiration_time
        struct idx_sequence{}; // used for tagging the index as a sequence of items

//         typedef boost::multi_index_container<
//             Lease_t,
//             boost::multi_index::indexed_by<
//               boost::multi_index::ordered_unique<
//                 boost::multi_index::tag< idx_user_dn_cream_url >,
//             // The index here is the (user_dn, cream_url) pair
//                 boost::multi_index::composite_key<
//                 Lease_t,
//             // The first composite element is the user_dn
//                 boost::multi_index::member<Lease_t,std::string,&Lease_t::m_user_dn>,
//             // The second composite element is the cream_url
//                 boost::multi_index::member<Lease_t,std::string,&Lease_t::m_cream_url>
//               > 
//             >,
//             boost::multi_index::ordered_non_unique<
//               boost::multi_index::tag< idx_expiration_time >,
//               boost::multi_index::member<Lease_t,time_t,&Lease_t::m_expiration_time>
//             >,
//             boost::multi_index::sequenced<
//               boost::multi_index::tag< idx_sequence >
//             >,
//             boost::multi_index::ordered_unique<
//               boost::multi_index::tag< idx_lease_id >,
//               boost::multi_index::member<Lease_t,std::string,&Lease_t::m_lease_id>
//             >
//           >
//         > t_lease_set;

        /**
         * (Sequence const) iterator to the lease cache
         */ 
      //        typedef t_lease_set::index< idx_sequence >::type::iterator const_iterator;

    protected:
        Lease_manager( );

        /**
         * This method is used to initialize the lease manager, by
         * loading all existing (non expired) lease IDs. To do so,
         * this method builds a list of all lease IDs by scanning the
         * job cache. After that, it asks all relevant CREAM servers
         * for the lease information, and fills the lease cache with
         * the non expired leases only.
         */
      //        void init( void );

        /**
         * Iterates over the lease ID set. Removes all lease IDs which
         * are associated to already expired leases. 
         *
         * This method is _NOT_ thread-safe; it is then supposed to be
         * called from another method which is.
         */
        void purge_old_lease_ids( void );

        static Lease_manager* s_instance;
        static boost::recursive_mutex m_mutex;

        log4cpp::Category* m_log_dev;
        unsigned int m_operation_count;
        const unsigned int m_operation_count_max;
        std::string m_host_dn; // the host DN
        int m_lease_delta_time;
        int m_lease_update_frequency;

      //        t_lease_set m_lease_set;

    public:
        ~Lease_manager( ) { };

        /**
         * Returns the singleton instance of this class. This method
         * is thread-safe: a mutex is acquired before checking whether
         * the instance already exists or must be created.
         *
         * @return The singleton instance of this class. The caller
         * DOES NOT OWN the returned pointer, and must NOT be
         * deallocated.
         */
        static Lease_manager* instance( );

        /**
         * Returns a (possibly fresh) lease ID to be used for a given job.
         *
         * This method must be called before submitting a job to the
         * CREAM service.
         *
         * This method first looks for there is a lease ID which:
         *
         * 1) is associated with a lease which has not expired yet;
         * 2) has been created for the same user DN/CREAM URL pair
         * as the job being submitted.
         *
         * If a lease ID satisfying both conditions above is found,
         * this method returns that lease ID. If there exist no lease
         * ID satisfying the conditions, a new lease ID is created and
         * returned to the caller. This method is guaranteed NOT to
         * return a lease whose duration is less than
         * lease_update_frequency (as specified in the configuration
         * file). If a lease which is expiring in less than
         * leasE_update_frequency does exist in the lease cache, thie
         * method renews the lease before returning it to the caller.
         *
         * This method periodically (every N invocations) checks the
         * lease ID cache to see whether there are lease IDs
         * associated with expired leases. Such entries are purged
         * from the cache.
         *
         * This method is thread-safe: internally, it acquires a mutex
         * so that multiple access to the (shared) lease ID cache do
         * not interfere each other.
         *
         * NOTE: This method guarantees that lease IDs are *always*
         * unique; this means that the same user requesting leases to
         * different cream services will be given different lease IDs.
         * This means that all lease IDs in the lease manager uniquely
         * identify a tuple (lease_id, cream_url, user_dn)
         *
         * @param job the Job for which a lease ID should be
         * requested.  The method uses the following attributes of the
         * job parameter: user DN, cream URL. All these information
         * MUST be already present in the job object.
         *
         * @param force if true, then a new lease ID is returned,
         * even if a valid lease exists in the cache. In the case
         * a valid lease ID existed in the cache, the old lease ID
         * is discarded.
         *
         * @return The lease ID to use for job submission. This method
         * throws a fault if some error occurs. 
         */
        std::string make_lease( const CreamJob& job, bool force = false ) throw( lease_exception& );

        /**
         * Renew an existing lease.          
         *
         * NOTE: This method is thread-safe
         *
         * @param lease_id the lease_id to renew. This lease must
         * exist and be valid both in the lease manager and in the
         * CREAM service. If the lease renewal operation is succesful,
         * this method updates the appropriate entry into the lease
         * cache.
         *
         * @return the nex expiration time for the lease. If the lease
         * does not exist, or if the lease cannot be renewed, this
         * method returns 0. In this case, the lease entry in the lease
         * cache is NOT modified.
         */
        time_t renew_lease( const std::string& lease_id ) throw( lease_exception& );

        /**
         * These methods give access to the lease cache through
         * const iterators
         */
      //        const_iterator begin() const;
      //        const_iterator end() const;

        /**
         * Find the lease with the given lease_id; returns end() if no
         * lease with the requested name has been found.
         *
         * @param lease_id The lease id to look for. If lease_id is
         * the empty string, this function returns the value end()
         *
         * NOTE: This method is _NOT_ thread-safe
         */
        //const_iterator find( const std::string& lease_id ) const;
    };

} // namespace util
} // namespace ice
} // namespace wms
} // namespace glite

#endif
