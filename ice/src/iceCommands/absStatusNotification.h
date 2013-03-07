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
#ifndef ICE_ABS_STATUS_NOTIFICATION_H
#define ICE_ABS_STATUS_NOTIFICATION_H

#include <string>

namespace glite {
namespace wms {
namespace ice {
namespace util {

    class absStatusNotification {
    protected:
        absStatusNotification( ) { }; // may throw an exception!
    public:

        /**
         * This method applies all the changes required by this status
         * notification.  This is an abstract method, as each
         * implementation of the specific status notification must
         * handle updates appropriately.
         */
        virtual void apply( ) = 0;
        
        virtual ~absStatusNotification( ) { };

    };
    

} // namespace util
} // namespace ice
} // namespace wms
} // namespace glite

#endif
