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

#ifndef GLITE_WMS_ICE_ICEDB_CREATE_PROXYFIELD_H
#define GLITE_WMS_ICE_ICEDB_CREATE_PROXYFIELD_H

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
    class CreateProxyField : public AbsDbOperation { 
    public:
        CreateProxyField( const std::string& userdn,
			  const std::string& myproxy,
			  const std::string& proxyfile,
			  const time_t exptime,
			  const long long counter, 
			  const std::string& caller )
	  : AbsDbOperation( caller ),
	  m_userdn( userdn),
	  m_myproxy( myproxy ),
	  m_proxyfile( proxyfile ),
	  m_exptime( exptime ),
	  m_counter( counter ) {}
			  
        virtual void execute( sqlite3* db ) throw( DbOperationException& );
    protected:
        //std::string                       m_serialized_job;
	const std::string m_userdn;
	const std::string m_myproxy;
	const std::string m_proxyfile;
	const time_t      m_exptime;
	const long long   m_counter;
    };

} // namespace db
} // namespace ice
} // namespace wms
} // namespace glite

#endif
