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

#ifndef __GLITE_WMS_ICE_ICECOMMANDFATAL_EX_H__
#define __GLITE_WMS_ICE_ICECOMMANDFATAL_EX_H__

#include "iceCommand_ex.h"

namespace glite {
    namespace wms{
        namespace ice {

            /**
             * This class represents a fatal exception thrown during
             * the execution of ICE commands (submit or cancel).  A
             * fatal exception means that the command is permanently
             * failed, which means that submitting the command another
             * time will invariably result in another failure.
             */
            class iceCommandFatal_ex : public iceCommand_ex {
            public:
                iceCommandFatal_ex( const std::string& c ) throw() : 
                    iceCommand_ex( c ) { }
                virtual ~iceCommandFatal_ex() throw() { }
            };


        } // namespace ice
    } // namespace wms
} // namespace glite

#endif
