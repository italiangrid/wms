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

#ifndef GLITE_WMS_ICE_GET_JOBS_TO_POLL_H
#define GLITE_WMS_ICE_GET_JOBS_TO_POLL_H

#include "AbsDbOperation.h"
#include "utils/CreamJob.h"
#include <list>
#include <string>
//#include <boost/tuple/tuple.hpp>

//                   creamjobid,  complete cid gridjobid,   userdn,      creamurl     last seen   last empty
//typedef boost::tuple<std::string, std::string, std::string, std::string, std::string, time_t,     time_t     > JobToPoll;

namespace glite {
namespace wms {
namespace ice {
namespace db {

    /**
     *
     */
    class GetJobsToPoll : public AbsDbOperation {
    protected:
        bool m_poll_all_jobs;
//        std::list< boost::tuple<std::string, std::string, std::string> > m_result;
	std::list< glite::wms::ice::util::CreamJob > *m_result;
	const int                                     m_limit;
	const std::string                             m_userdn;
	const std::string                             m_creamurl;

    public:
        GetJobsToPoll( std::list< glite::wms::ice::util::CreamJob > *result,
		       const std::string& userdn, 
		       const std::string& creamurl, 
		       const bool poll_all_jobs, 
		       const std::string& caller , 
		       const int limit = 0 ) 
	: AbsDbOperation( caller ),
	  m_poll_all_jobs( poll_all_jobs ),
	  m_result( result ),
	  m_limit ( limit ),
	  m_userdn( userdn ),
	  m_creamurl( creamurl ) {}

        virtual void execute( sqlite3* db ) throw( DbOperationException& );

        /**
         * Return the list of jobs to poll
         */ 
/*         std::list< glite::wms::ice::util::CreamJob > get_jobs( void ) const { */
/*             return m_result; */
/*         }; */

    };

} // namespace db
} // namespace ice
} // namespace wms
} // namespace glite

#endif
