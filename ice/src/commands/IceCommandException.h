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

#ifndef __GLITE_WMS_ICE_ICECOMMAND_EX_H__
#define __GLITE_WMS_ICE_ICECOMMAND_EX_H__

#include <exception>
#include <string>

#include "utils/logging.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

namespace glite {
    namespace wms {
        namespace ice {

            class IceCommandException : public std::exception {
            public:
                IceCommandException( const std::string& c ) throw() : _cause( c ) { }
                virtual ~IceCommandException() throw() { }
                const char* what() const throw() { return _cause.c_str(); }
            protected:
                std::string _cause;                
            };

        } // namespace ice
    } // namespace wms
} // namespace glite

#endif
