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

#include "GetJobByCid.h"

using namespace glite::wms::ice;
using namespace std;
namespace cream_api = glite::ce::cream_client_api;

// Local helper function: callback for sqlite
// static int fetch_job_callback(void *param, int argc, char **argv, char **azColName){
//   
//   vector<string> *fields = (vector<string>*)param;
//   if ( argv && argv[0] ) 
//     {
//       for(int i = 0; i<=util::CreamJob::num_of_members()-1; ++i) {// a database record for a CreamJob has 26 fields, as you can see in Transaction.cpp, but we excluded the complete_cream_jobid from the query
// 	if( argv[i] )
// 	  fields->push_back( argv[i] );
// 	else
// 	  fields->push_back( "" );
//       }
//     }
//   return 0;
// }

void db::GetJobByCid::execute( sqlite3* db ) throw ( db::DbOperationException& )
{

    string sqlcmd = "SELECT ";
    sqlcmd += util::CreamJob::get_query_fields();
    sqlcmd += " FROM jobs WHERE ";
    sqlcmd += util::CreamJob::complete_cream_jobid_field();
    sqlcmd += "=";
    sqlcmd += glite::wms::ice::util::IceUtils::withSQLDelimiters( m_complete_creamjobid );
    sqlcmd += ";";
    
    //vector<string> fields;
    
    list<CreamJob> jobs;
    
    do_query( db, sqlcmd, glite::wms::ice::util::IceUtils::fetch_jobs_callback/*fetch_job_callback*/, &jobs/*&fields*/ );

    if( !jobs.empty() /*!fields.empty()*/ ) {
      m_found = true;

 //    m_theJob = util::CreamJob(fields.at(0),
// 			      fields.at(1),
// 			      fields.at(2),
// 			      fields.at(3),
// 			      fields.at(4),
// 			      fields.at(5),
// 			      fields.at(6),
// 			      fields.at(7),
// 			      fields.at(8),
// 			      fields.at(9),
// 			      fields.at(10),
// 			      fields.at(11),
// 			      fields.at(12),
// 			      (const cream_api::job_statuses::job_status)atoi(fields.at(13).c_str()),
// 			      (const cream_api::job_statuses::job_status)atoi(fields.at(14).c_str()),
// 			      strtoul(fields.at(15).c_str(), 0, 10),
// 			      (time_t)strtoll(fields.at(16).c_str(), 0, 10),
// 			      fields.at(17),
// 			      strtoul(fields.at(18).c_str(), 0, 10),
// 			      strtoul(fields.at(19).c_str(), 0, 10),
// 			      fields.at(20),
// 			      fields.at(21),
// 			      (fields.at(22)=="1" ? true : false),
// 			      (time_t)strtoll(fields.at(23).c_str(), 0, 10),
// 			      (fields.at(24) == "1" ? true : false ),
// 			      fields.at(25),
// 			      (time_t)strtoll(fields.at(26).c_str(), 0, 10),
// 			      fields.at(27),
// 			      (time_t)strtoll(fields.at(28).c_str(), 0, 10),
// 			      strtoull(fields.at(29).c_str(), 0, 10),
// 			      fields.at(30)
// 			      );

	m_theJob = *(jobs.begin());

	// no need to reset, it is already done by glite::wms::ice::util::IceUtils::fetch_jobs_callback
	//m_theJob.reset_change_flags( );
    }
}
