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

#ifndef GLITE_WMS_ICE_ICEDB_INSERT_STARTEDJOB_H
#define GLITE_WMS_ICE_ICEDB_INSERT_STARTEDJOB_H

#include "AbsDbOperation.h"
#include <ctime>

namespace glite {
namespace wms {
namespace ice {
namespace db {

    /**
     * This operation creates a new job into the database. It does not
     * update an existing job with the same gridjobid.
     */
    class InsertStartedJob : public AbsDbOperation { 

    public:
    InsertStartedJob( const time_t timestamp_now,
		const std::string& ceurl,
		const std::string& gridjobid, 
		const std::string& creamjobid,
	        const std::string& caller ) : AbsDbOperation(caller),
      m_timestamp( timestamp_now ),
      m_ceurl( ceurl ),
      m_gridjobid( gridjobid ),
      m_creamjobid( creamjobid ) {}
      
      virtual void execute( sqlite3* db ) throw( DbOperationException& );
      
  protected:
      const time_t            m_timestamp;
      const std::string       m_ceurl;
      const std::string       m_gridjobid;
      const std::string       m_creamjobid;
      
  };
  
} // namespace db
} // namespace ice
} // namespace wms
} // namespace glite

#endif
