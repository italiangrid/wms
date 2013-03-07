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

#ifndef ICE_NORMAL_STATUS_NOTIFICATION_H
#define ICE_NORMAL_STATUS_NOTIFICATION_H

#include "absStatusNotification.h"
#include "glite/ce/monitor-client-api-c/CEConsumer.h"

#include <string>

namespace glite {
namespace wms {
namespace ice {
namespace util {
    
    class normalStatusNotification : public absStatusNotification {
    protected:
        monitortypes__Event m_ev;
        std::string m_cemondn;
        //std::string m_cream_job_id;
	std::string m_complete_cream_jobid;

    public:
        normalStatusNotification( const monitortypes__Event& ev, const std::string& cemondn ); // FIXME:: can throw anything
        virtual ~normalStatusNotification( ) { };
        std::string get_complete_cream_job_id( void ) const { return m_complete_cream_jobid; };
        void apply( void ); // FIXME:: can throw anything
    };

} // namespace util
} // namespace ice
} // namespace wms
} // namespace glite

#endif
