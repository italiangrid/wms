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

#ifndef GLITE_WMS_ICE_ICECOMMANDSTATUSPOLLER_H
#define GLITE_WMS_ICE_ICECOMMANDSTATUSPOLLER_H

#undef soapStub_H

#include "IceAbstractCommand.h"
#include "utils/CreamJob.h"
#include "glite/ce/cream-client-api-c/JobInfoWrapper.h"
#include "db/GetJobsToPoll.h" // needed for definition of type 'JobToPoll'

#include <boost/scoped_ptr.hpp>

#include<vector>
#include<string>
#include<list>


namespace glite {
  namespace wms {
    namespace ice {

      class Main;

      namespace util {
        
        class IceLBLogger;
	//	class jobCache;
	class IceConfManager;

	class IceCommandStatusPoller : public IceAbstractCommand {

          //log4cpp::Category                           *m_log_dev;
          glite::wms::ice::util::IceLBLogger          *m_lb_logger;
          Main					      *m_iceManager;
	  time_t                                       m_threshold;
          const unsigned int                           m_max_chunk_size; ///< maximum number of jobs which will be used in a cumulative request to CREAM
          time_t                                       m_empty_threshold; ///< Threshold for empty notifications
	  bool                                         m_poll_all_jobs;
	  glite::wms::ice::util::IceConfManager       *m_conf;
	  bool 					       m_stopped;
	  const std::pair< std::string, std::string >  m_dnce;
	  
	  

	  //void purgeJobs( const std::vector< std::string >& );

	  /**
           * Gets the list of jobs to poll. The list contains all jobs
           * in the cache whose "oldness" is greater than the threshold
           * defined in ICE configuration file.
           *
           * @return the list of Cream Job IDs for jobs to poll.
           */ 
	  void get_jobs_to_poll( std::list<CreamJob>& /* std::list< glite::wms::ice::util::CreamJob >& */,const std::string&, const std::string& ) throw();
	  //void deleteJobsByDN( const std::string& dn ) throw( );
          
	  /**
           * Updates the status informations for all jobs in the list
           * l.
           *
           * @param l the list of job status informations (this list is
           * typically the resout of the scanJobs() method call).
           */
          void updateJobCache( const std::list< glite::ce::cream_client_api::soap_proxy::JobInfoWrapper >& ) throw();

	  /**
           * Updates the cache with the job status changes (for a single job)
           * contained in s
           *
           * @param s the StatusInfo object from which job informations are updated
           */
          void update_single_job( const glite::ce::cream_client_api::soap_proxy::JobInfoWrapper& ) throw();

	  //void remove_unknown_jobs_from_cache(std::vector< const glite::ce::cream_client_api::soap_proxy::JobInfo >&) throw();

	  /**
           * Prevents copying
           */
          //IceCommandStatusPoller( const IceCommandStatusPoller& ) : m_max_chunk_size( 0 ) { } // keep the compiler happy

          /**
           *
           */
	  std::list< glite::ce::cream_client_api::soap_proxy::JobInfoWrapper > 
	    check_multiple_jobs( const std::string& proxy, 
				 const std::string& user_dn, 
				 const std::string& cream_url, 
				 //const std::vector< glite::wms::ice::util::CreamJob >& cream_job_ids ) throw();
				 const std::list< CreamJob >& cream_job_ids ) throw();



          /**
           * This method removes jobs which have been reported as not found by
           * CREAM from the job cache. 
           *
           * @param all_jobs the vector of all CREAM job IDs which
           * were passed to the cumulative CREAM call. Upon succesful
           * completion (if no job is missing), we expect that
           * jobs_found contains exactly JobInfo structures for the
           * exact same job IDs in all_jobs.
           *
           * @param jobs_found the vector of JobInfo structures
           * returned by the cumulative CREAM call. This vector
           * contains *at most* one element for each CREAM job ID
           * in the all_jobs vector.
           */
	  void 
	    remove_unknown_jobs_from_cache(const std::vector<std::string>& all_jobs, 
					   const std::list< glite::ce::cream_client_api::soap_proxy::JobInfoWrapper >& jobs_found ) throw();

	  //void check_user_jobs( const std::string&, const std::string& ) throw();

	public:
	  
          //! EventStatusPoller constructor
          /*!
            Creates a EventStatusPoller object
            \param iceManager is the ICE main object (see the ice class) that creates the poller thread. Ownership of this pointer is not trans
	    ferred
            \param D is the delay (default 10 seconds) between two polls
            \throw EventStatusPollerException& if the creation of the internal cream communication client failed
            \sa ice
          */
          IceCommandStatusPoller( Main*, 
				  const std::pair<std::string, std::string>&,
				  bool poll_all_jobs = false ); //throw(glite::wms::ice::util::EventStatusPollerException&, glite::wms::ice::util::ConfigurationManagerException&);
          
          virtual ~IceCommandStatusPoller() throw() { } 

	  void execute( const std::string& ) throw();
	  
	  std::string get_grid_job_id() const { return std::string(); }

	  void stop() { m_stopped = true; }
	};
      }
    }
  }
}

#endif
