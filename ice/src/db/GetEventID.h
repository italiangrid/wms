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


#ifndef GLITE_WMS_ICE_GET_EVENTID
#define GLITE_WMS_ICE_GET_EVENTID

#include "AbsDbOperation.h"

namespace glite {
namespace wms {
namespace ice {
namespace db {

    /**
     *
     */
    class GetEventID : public AbsDbOperation {
    protected:

	const std::string  m_userdn;
	const std::string  m_creamurl;
	long long          m_result;
	bool               m_found;

    public:
        GetEventID( const std::string& userdn, 
		    const std::string& ceurl, 
		    const std::string& caller )
	 : AbsDbOperation( caller ),
    m_userdn( userdn ),    m_creamurl( ceurl ),    m_result( -1 ),    m_found( false ) {}


	long long getEventID( void ) const { return m_result; }
	bool      found( void ) const { return m_found; }
        virtual void execute( sqlite3* db ) throw( DbOperationException& );

    };

} // namespace db
} // namespace ice
} // namespace wms
} // namespace glite

#endif
