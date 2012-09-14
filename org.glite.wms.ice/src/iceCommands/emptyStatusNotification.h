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
#ifndef ICE_EMPTY_STATUS_NOTIFICATION_FACTORY_H
#define ICE_EMPTY_STATUS_NOTIFICATION_FACTORY_H

#include "absStatusNotification.h"
#include <string>

namespace glite {
namespace wms {
namespace ice {
namespace util {
    
    class emptyStatusNotification : public absStatusNotification {
    public:
        /**
         * Builds a new emptyStatusNotification object. These objects
         * will apply all actions requested by an empty CEMon status
         * notification to a given job. Currently this means setting
         * the timestamp of the last received empty status
         * notification to the current time.
         *
         * @param cream_job_id the CREAM job id for the job whose
         * timestamp should be updated.
         */
        emptyStatusNotification( const std::string& cream_job_id );

        virtual ~emptyStatusNotification( ) { };

        /**
         * Applies the empty status change notification to the job
         * specified in the constructor
         */ 
        void apply( void ); // can throw anything
    protected:
        std::string m_cream_job_id;
    };

} // namespace util    
} // namespace ice
} // namespace wms
} // namespace glite

#endif
