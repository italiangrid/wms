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

#ifndef GLITE_WMS_ICE_ICEDB_CREATE_JOB_H
#define GLITE_WMS_ICE_ICEDB_CREATE_JOB_H

#include "AbsDbOperation.h"
#include "iceUtils/CreamJob.h"

namespace glite {
namespace wms {
namespace ice {
namespace db {

    /**
     * This operation creates a new job into the database. It does not
     * update an existing job with the same gridjobid.
     */
    class CreateJob : public AbsDbOperation { 
    public:
        CreateJob( const glite::wms::ice::util::CreamJob& j, 
		   const std::string& caller ) 
	  : AbsDbOperation( caller ), 
	  m_theJob ( j ) {}
//	  m_JDL( j.jdl() ) {}

        virtual void execute( sqlite3* db ) throw( DbOperationException& );
    protected:
        //std::string                       m_serialized_job;
	glite::wms::ice::util::CreamJob   m_theJob;
	//std::string                       m_JDL;
    };

} // namespace db
} // namespace ice
} // namespace wms
} // namespace glite

#endif
