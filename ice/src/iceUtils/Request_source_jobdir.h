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

#ifndef ICE_REQUEST_SOURCE_JOBDIR_H
#define ICE_REQUEST_SOURCE_JOBDIR_H

#include "glite/wms/common/utilities/jobdir.h"
#include "Request_source.h"

namespace glite {
namespace wms {
namespace ice {
namespace util {

    class Request_source_jobdir : public Request_source {
    public:
        Request_source_jobdir( const std::string& jdir_name, bool create );

        virtual ~Request_source_jobdir( );

        /**
         *
         */
        std::list<Request*> get_requests( size_t max_size );
	//Request* get_single_request( );

        /**
         *
         */
        void remove_request( Request* req );
        
        /**
         *
         */
        void put_request( const std::string& ad );

        /**
         *
         */
        std::size_t get_size( void );

    protected:
        glite::wms::common::utilities::JobDir* m_jobdir;
    };

} // namespace util
} // namespace ice
} // namespace wms
} // namespace glite

#endif
