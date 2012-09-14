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

#ifndef GLITE_WMS_ICE_ICEDB_CREATE_DELEGATION_H
#define GLITE_WMS_ICE_ICEDB_CREATE_DELEGATION_H

#include "AbsDbOperation.h"
#include <string>
#include <ctime>

namespace glite {
namespace wms {
namespace ice {
namespace db {

    /**
     * This operation creates a new job into the database. It does not
     * update an existing job with the same gridjobid.
     */
    class CreateDelegation : public AbsDbOperation { 

    public:
      CreateDelegation( const std::string& digest,
			const std::string& creamurl,
			const time_t exptime,
			const int duration,
			const std::string& delegationid,
			const std::string& userdn,
			const bool renewable,
			const std::string& myproxyurl,
			const std::string& caller ) : AbsDbOperation( caller ),
							 m_digest( digest ),
							 m_creamurl( creamurl ),
							 m_exptime( exptime ),
							 m_duration( duration ),
							 m_delegid( delegationid ),
							 m_userdn( userdn ),
							 m_renewable( renewable ),
							 m_myproxyurl( myproxyurl ) {}
			  
        virtual void execute( sqlite3* db ) throw( DbOperationException& );

    protected:
      const std::string       m_digest;
      const std::string       m_creamurl;
      const time_t            m_exptime;
      const int               m_duration;
      const std::string       m_delegid;
      const std::string       m_userdn;
      const bool              m_renewable;
      const std::string       m_myproxyurl;
    };

} // namespace db
} // namespace ice
} // namespace wms
} // namespace glite

#endif
