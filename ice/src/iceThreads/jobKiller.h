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

#ifndef GLITE_WMS_ICE_UTIL_JOBKILLER_H
#define GLITE_WMS_ICE_UTIL_JOBKILLER_H

#include "iceConfManager.h"
#include "iceThread.h"

namespace glite {
namespace wms {
namespace ice {
namespace util {
    
    class jobKiller : public iceThread {
        bool m_valid;
        time_t m_threshold_time;
        time_t m_delay;
    public:
        jobKiller();
        virtual ~jobKiller();
        bool isValid( void ) const { return m_valid; }
        
        virtual void body( void );
        
    };
    
} // namespace util
} // namespace ice
} // namespace wms
} // namespace glite

#endif
