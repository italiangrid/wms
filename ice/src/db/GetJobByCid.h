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

#ifndef GLITE_WMS_ICE_ICEDB_GETJOBBYCID_H
#define GLITE_WMS_ICE_ICEDB_GETJOBBYCID_H

#include "AbsDbOperation.h"
#include "utils/CreamJob.h"
#include <string>

namespace glite {
namespace wms {
namespace ice {
namespace db {

    /**
     *
     */
    class GetJobByCid : public AbsDbOperation { 

    protected:
        const std::string                m_complete_creamjobid;
        glite::wms::ice::util::CreamJob  m_theJob;
        bool                             m_found;

    public:
        GetJobByCid( const std::string& cid, const std::string& caller ) : AbsDbOperation( caller ), m_complete_creamjobid( cid ), m_theJob(), m_found(false) {}

        virtual void execute( sqlite3* db ) throw( DbOperationException& );

        /**
         * Return the job which has been found. Returns a dummy
         * job if none has been found
         */
        glite::wms::ice::util::CreamJob get_job( void ) const { return m_theJob; };

        /**
         * True iff a job has been found
         */
        bool found( void ) const { return m_found; };
    };

} // namespace db
} // namespace ice
} // namespace wms
} // namespace glite

#endif
