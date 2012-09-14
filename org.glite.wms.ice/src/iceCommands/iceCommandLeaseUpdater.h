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
 
#ifndef GLITE_WMS_ICE_ICECOMMANDLEASEUPD_H
#define GLITE_WMS_ICE_ICECOMMANDLEASEUPD_H

#include "iceAbsCommand.h"
#include "creamJob.h"
//#include "jobCache.h"
#include "iceUtils.h"

#include <ctime>

namespace log4cpp {
  class Category;
};

namespace glite {
namespace wms {
namespace ice {
namespace util {
	
    class iceLBLogger;
    class Lease_manager;

    class iceCommandLeaseUpdater : public iceAbsCommand {
      
        log4cpp::Category *m_log_dev;
        glite::wms::ice::util::iceLBLogger* m_lb_logger;
        time_t m_frequency;
	//        glite::wms::ice::util::jobCache* m_cache;
        bool m_only_update;
        Lease_manager* m_lease_manager;

        /**
         * Returns true iff the CreamJob job can be removed from the
         * job cache because its lease expired.
         */
        bool job_can_be_removed( const CreamJob& job ) const throw();

        /**
         * This method returns true iff the lease associated to CreamJob 
         * job must be renewed.
         */ 
        bool lease_can_be_renewed( const CreamJob& job ) const throw();
      
    public:
      /**
       * Se only_update==true, si aggiornano i lease SENZA rimuovere i
       * job per cui il lease e' scaduto.
       */
        iceCommandLeaseUpdater( bool only_update = false ) throw();
        
        ~iceCommandLeaseUpdater( ) throw() { }
        
        void execute( const std::string& ) throw();
                
        std::string get_grid_job_id( void ) const { return std::string(); } 
        
    };
    
} // namespace util
} // namespace ice
} // namespace wms
} // namespace glite

#endif
