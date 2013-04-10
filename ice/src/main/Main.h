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

#ifndef GLITE_WMS_ICE_ICE_CORE_H
#define GLITE_WMS_ICE_ICE_CORE_H

#include "threads/IceThread.h"
#include "threads/IceThreadPool.h"
#include "utils/ClassadSyntax_ex.h"

#include <string>
#include <list>
#include <sys/types.h>          // getpid(), getpwnam()
#include <unistd.h>             // getpid(), usleep

#include <boost/shared_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace boost {
  class thread;
};

namespace log4cpp {
  class Category;
};

namespace glite {
namespace wms {
    namespace common {
        namespace configuration {
            class Configuration;
        }
    }

namespace ice {

    class IceInitException : public std::exception {
        
        std::string cause;
        
      public:
        IceInitException(const std::string& _cause) throw() : cause(_cause) {};
        virtual ~IceInitException() throw() {}
        const char* what() const throw() { return cause.c_str(); }
        
    };
    
    namespace util {
        class IceLBLogger;
        class Request_source;
        class Request;
	class CreamJob;
    };
    
    class Main {
        
    public:

      static boost::recursive_mutex s_mutex;

        class IceThreadHelper { 

        public:
            IceThreadHelper( const std::string& name );
            virtual ~IceThreadHelper( );
            
            /**
             * Associates a newly created IceThread object
             * to this thread, and starts it. 
             *
             * @param obj a pointer to the IceThread object from
             * which a thread should be created. The caller
             * transfers ownership of parameter obj.
             */
            void start( threads::IceThread* obj ) throw( IceInitException& );

            /**
             * Returns true iff this thread started (i.e., iff
             * this thread as an associated running IceThread
             * object). False otherwise.
             */
            bool is_started( void ) const;

	    /**
             * Stops this thread. This method has no effect if the
             * start() method was not called first, or if the
             * start() method raised an exception.
             */
	    void stop( void );

        protected:
            /**
             * Stops this thread. This method has no effect if the
             * start() method was not called first, or if the
             * start() method raised an exception.
             */
            //void stop( void );
            
            const std::string m_name; //! The name of this thread (can be any string)
            boost::thread* m_thread; //! The dynamically created thread object. 
            boost::shared_ptr< glite::wms::ice::threads::IceThread > m_ptr_thread; //! Used to instantiate the thread.
            log4cpp::Category* m_log_dev; //! Logging device

        };

        // 
        // ICE threads
        //        
        IceThreadHelper m_poller_thread;
        IceThreadHelper m_proxy_renewer_thread;
        
        // 
        // ICE request sources
        //
        glite::wms::ice::util::Request_source* m_wms_input_queue; ///< Input queue for the WM
        glite::wms::ice::util::Request_source* m_ice_input_queue; ///< Input queue for ICE
	int m_reqnum;

        log4cpp::Category* m_log_dev;
        
        util::IceLBLogger* m_lb_logger;
        glite::wms::common::configuration::Configuration* m_configuration;

        // 
        // ICE thread pools
        //
        glite::wms::ice::threads::IceThreadPool* m_requests_pool; ///< This pool is used to process submission/cancellation requests coming from the input queue
        glite::wms::ice::threads::IceThreadPool* m_ice_commands_pool; ///< This pool is used to process ICE internal commands (proxy renewal, lease updates and so on)
	glite::wms::ice::threads::IceThreadPool* m_ice_lblog_pool; ///< This pool is used to process async log to lb

        static glite::wms::ice::Main* s_instance; ///< Singleton instance of this class
       
        int  	    m_times_too_queue_full;
       
	std::string m_hostdn;
 
	time_t      m_start_time;

        Main( ) throw( IceInitException& );

        // Some utility functions
        void deregister_proxy_renewal( const util::CreamJob* job ) throw();
        void purge_wms_storage( const util::CreamJob* job ) throw();

	//
        // This method is called when ICE starts execution. It should
        // contain general initialization procedures for handling
        // situations when ICE terminated abruptly and should come
        // back as cleanly as possible.
        //
	void init( void );
                
        /**
         * Resubmits a failed job. This method takes care of logging
         * the relevant L&B events. The job is NOT removed from the
         * job cache!
         *
         * @param the_job the job to resubmit.
         *
         * @param reason the reason why this job is being resubmitted
         */
        void resubmit_job( util::CreamJob* the_job, const std::string& reason ) throw();
        
        /**
         * Purge a cancelled/terminated job. This method takes care
         * of logging the relevant L&B events. The job will be
         * removed from the jobCache.
         *
         * @param j an iterator to the job to purge. If j ==
         * jobCache.end(), the not action is done.
         *
         * @param reason the reason why the job is being purged
         *
         * @return an iterator pointing to the next job in cache, if
         * the purge(and subsequent removal) was succesful. jit if
         * not.
         */
        //util::jobCache::iterator 
	void purge_job( /*util::jobCache::iterator */ const util::CreamJob* j, const std::string& reason ) throw();
        
	/**
	 * The main loop: fetch requests from filelist/jobdir and process them
	 */
	int main_loop( void );

    public:
        
        virtual ~Main();
       
	std::string getHostDN( void ) const { return m_hostdn; }
 
        void getNextRequests(std::list< util::Request* >&);

	time_t getStartTime( void ) const { return m_start_time; }

	// util::Request* getNextRequest( void );

        /**
         * Returns the size (numer of requests) of the input queue;
         * this means the input filelist/jobdir which holds requests
         * from the WM to ICE.
         */
	size_t get_input_queue_size( void );

        void removeRequest( util::Request* r );

        // Starter/Stopper methods
        void startPoller( void );
        void startProxyRenewer( void );
        
	void stopAllThreads( void );

        // Query methods
        bool is_poller_started( void ) const;
        bool is_proxy_renewer_started( void ) const;

        // Misc job handling functions
	/**
         * Resubmits or purge a given job.
         *
         * @param it the iterator to the job to purge or
         * resubmit. If it == end(), then nothing is done and this
         * method returns end();
         *
         * @return an iterator to the current job, if it is neither
         * purged nor resubmitted. An interator to the next job, if
         * the job is purged or resubmitted (and hence removed from
         * the job cache).
         */
	bool resubmit_or_purge_job( util::CreamJob* ) throw();

	//void delete_jobs_by_dn( const std::string& ) throw();
	
        
        /**
         * returns the singleton instance of this class.
         */
        static glite::wms::ice::Main* instance( void );

        /**
         * Returns the thread pool responsible for processing
         * submissions and cancellation requests
         */
        glite::wms::ice::threads::IceThreadPool* get_requests_pool( void ) { return m_requests_pool; };

        /**
         * Returns the thread pool responsible for processing internal
         * ICE commands (i.e., proxy renewal, lease updates and so
         * on).
         */
        glite::wms::ice::threads::IceThreadPool* get_ice_commands_pool( void ) { return m_ice_commands_pool; };
        
	glite::wms::ice::threads::IceThreadPool* get_ice_lblog_pool( void ) { return m_ice_lblog_pool; };
	
	//std::string getHostName( void ) const throw() { return m_myname; }

    private:
	long long int check_my_mem(pid_t);

    }; // class ice
    
} // namespace ice
} // namespace ce
} // namespace glite

#endif
