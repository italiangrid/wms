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

#ifndef GLITE_WMS_ICE_ICECOMMANDUPDATESTATUS_H
#define GLITE_WMS_ICE_ICECOMMANDUPDATESTATUS_H

#include <vector>
#include "iceAbsCommand.h"
#include "glite/ce/monitor-client-api-c/CEConsumer.h"

// Forward declaration
namespace log4cpp {
    class Category;
};

namespace glite {
namespace wms {
namespace ice {
namespace util {

    class iceCommandUpdateStatus : public iceAbsCommand {
    protected:
        log4cpp::Category* m_log_dev;
        std::vector<monitortypes__Event> m_ev;
	std::string m_cemondn;

    public:
        iceCommandUpdateStatus( const std::vector<monitortypes__Event>& ev, const std::string& cemondn );
        virtual ~iceCommandUpdateStatus( ) { };
        virtual void execute( const std::string& ) throw( );
        std::string get_grid_job_id( void ) const { return std::string(); };
    };

} // namespace util
} // namespace ice
} // namespace wms
} // namespace glite

#endif
