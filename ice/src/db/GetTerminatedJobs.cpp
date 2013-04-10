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

#include "GetTerminatedJobs.h"
#include "utils/IceUtils.h"

#include "glite/ce/cream-client-api-c/job_statuses.h"

#include <cstdlib>

#include <boost/lexical_cast.hpp>

namespace api = glite::ce::cream_client_api;

using namespace glite::wms::ice;
using namespace std;

void db::GetTerminatedJobs::execute( sqlite3* db ) throw ( DbOperationException& )
{
  string sqlcmd("SELECT ");
  sqlcmd += CreamJob::get_query_fields();
  sqlcmd += " FROM jobs WHERE " ;
  sqlcmd += util::CreamJob::status_field();
  sqlcmd += "=";
  sqlcmd += util::IceUtils::withSQLDelimiters( boost::lexical_cast<string>( (unsigned long int)api::job_statuses::CANCELLED ));
  sqlcmd += " OR ";
  sqlcmd += util::CreamJob::status_field();
  sqlcmd += "=";
  sqlcmd += util::IceUtils::withSQLDelimiters( boost::lexical_cast<string>( (unsigned long int)api::job_statuses::DONE_OK ) );
  sqlcmd += " OR ";
  sqlcmd += util::CreamJob::status_field();
  sqlcmd += "=";
  sqlcmd += util::IceUtils::withSQLDelimiters( boost::lexical_cast<string>( (unsigned long int)api::job_statuses::DONE_FAILED ) );
  sqlcmd += " OR ";
  sqlcmd += util::CreamJob::status_field();
  sqlcmd += "=";
  sqlcmd += util::IceUtils::withSQLDelimiters( boost::lexical_cast<string>( (unsigned long int)api::job_statuses::ABORTED ) );
  sqlcmd += ";";

  do_query( db, sqlcmd, glite::wms::ice::util::IceUtils::fetch_jobs_callback, m_result );
}
