/* 
 * Copyright (c) Members of the EGEE Collaboration. 2004. 
 * See http://www.eu-egee.org/partners/ for details on the copyright
 * holders.  
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at 
 *
 *    http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and 
 * limitations under the License.
 *
 * Get jobs to poll
 *
 * Authors: Alvise Dorigo <alvise.dorigo@pd.infn.it>
 *          Moreno Marzolla <moreno.marzolla@pd.infn.it>
 */

#include "GetJobsToPoll.h"
#include "iceUtils/iceConfManager.h"

#include "boost/algorithm/string.hpp"
#include "boost/format.hpp"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"

#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace glite::wms::ice::db;
using namespace glite::wms::ice::util;
using namespace std;
namespace cream_api = glite::ce::cream_client_api;

GetJobsToPoll::GetJobsToPoll(
			     list<CreamJob>* jobs, 
			     const string& userdn, 
			     const string& creamurl, 
			     const bool poll_all_jobs, 
			     const int limit ) :    
    AbsDbOperation(),
    m_result( jobs ),
    m_poll_all_jobs( poll_all_jobs ),
    m_limit( limit ),
    m_userdn( userdn ),
    m_creamurl( creamurl )
{

}

namespace { // begin local namespace

  // Local helper function: callback for sqlite
  static int fetch_jobs_callback(void *param, int argc, char **argv, char **azColName){
    //    string* serialized = (string*)param;
    //list< vector<string> > *jobs = (list<vector<string> >*)param;
    
    list<CreamJob>* jobs = (list<CreamJob>*)param;
    
    if( argv && argv[0] ) {
      vector<string> fields;
      for(int i = 0; i<=24; i++) {// a database record for a CreamJob has 26 fields, as you can see in Transaction.cpp, but we want to exlude the field "complete_creamjobid", as specified in the SELECT sql statement;
	if( argv[i] )
	  fields.push_back( argv[i] );
	else
	  fields.push_back( "" );
      }

      CreamJob tmpJob(fields.at(0),
		      fields.at(1),
		      fields.at(2),
		      fields.at(3),
		      fields.at(4),
		      fields.at(5),
		      fields.at(6),
		      fields.at(7),
		      fields.at(8),
		      fields.at(9),
		      fields.at(10),
		      fields.at(11),
		      fields.at(12),
		      fields.at(13),
		      fields.at(14),
		      fields.at(15),
		      fields.at(16),
		      fields.at(17),
		      fields.at(18),
		      fields.at(19),
		      fields.at(20),
		      fields.at(21),
		      fields.at(22),
		      fields.at(23),
		      fields.at(24)
		      );
      
      jobs->push_back( tmpJob );
    }

    return 0;
  }
  
} // end local namespace

void GetJobsToPoll::execute( sqlite3* db ) throw ( DbOperationException& )
{
    time_t 
        threshold( iceConfManager::getInstance()->getConfiguration()->ice()->poller_status_threshold_time() ),
        empty_threshold( iceConfManager::getInstance()->getConfiguration()->ice()->ice_empty_threshold() );


        //
        // Q: When does a job get polled?
        //
        // A: A job gets polled if one of the following situations are true:
        //
        // 1. ICE starts. When ICE starts, it polls all the jobs it thinks
        // have not been purged yet.
        //
        // 2. ICE received the last non-empty status change
        // notification more than m_threshold seconds ago.
        //
        // 3. ICE received the last empty status change notification
        // more than 10*60 seconds ago (that is, 10 minutes).
        //
	// empty notification are effective to reduce the polling frequency if the m_threshold is much greater than 10x60 seconds

    ostringstream sqlcmd;
    if ( m_poll_all_jobs ) {
      sqlcmd << "SELECT " << CreamJob::get_query_fields() 
	     << " FROM jobs WHERE (creamjobid not null) AND (last_poller_visited not null) " 
	     << " AND creamurl='" 
	     << m_creamurl 
	     << "' AND userdn='" 
	     << m_userdn << "' ORDER BY last_poller_visited ASC";

      if( m_limit ) {
	sqlcmd << " LIMIT " << m_limit << ";";
      } else {
	sqlcmd << ";";
      }
 
    } else {
      time_t t_now( time(NULL) );
      sqlcmd << "SELECT "<< CreamJob::get_query_fields() 
	     << " FROM jobs"					
	     << " WHERE ( creamjobid not null ) AND (last_poller_visited not null)"	
	     << " AND userdn='" << m_userdn << "'"
	     << " AND creamurl='" << m_creamurl << "' AND "
	     << "       ( ( last_seen > 0) AND ( " 
	     <<t_now<<" - last_seen >= "<<threshold<<" ) ) "
	     << "  OR   ( (last_empty_notification > 0) AND ( "
	     <<t_now<<" - last_empty_notification > "<<empty_threshold<<" ) )"
	     << " ORDER BY last_poller_visited ASC";
      if( m_limit ) {
	sqlcmd << " LIMIT " << m_limit << ";";
      } else {
	sqlcmd << ";";
      }
    }

    list< vector<string> > jobs;

  if(::getenv("GLITE_WMS_ICE_PRINT_QUERY") )
    cout << "Executing query ["<<sqlcmd.str()<<"]"<<endl;

  do_query( db, sqlcmd.str(), fetch_jobs_callback, m_result );

//     for( list< vector<string> >::iterator it=jobs.begin();
// 	 it != jobs.end();
// 	 ++it )
//       {
	
// 	string gridjobid                = it->at(0);	    
// 	string creamjobid               = it->at(1);	    
// 	string jdl                      = it->at(2);    
// 	string userproxy                = it->at(3);
// 	string ceid                     = it->at(4);
// 	string endpoint                 = it->at(5);
// 	string creamurl                 = it->at(6);        
// 	string creamdelegurl            = it->at(7);   
// 	string userdn                   = it->at(8);          
// 	string myproxyurl               = it->at(9);      
// 	string proxy_renewable          = it->at(10); 
// 	string failure_reason           = it->at(11);  
// 	string sequence_code            = it->at(12);   
// 	string wn_sequence_code         = it->at(13);		
// 	string prev_status              = it->at(14);		
// 	string status                   = it->at(15);			
// 	string num_logged_status_changes= it->at(16);
// 	string leaseid                  = it->at(17);   
// 	//string proxycert_timestamp      = it->at(18);	
// 	string status_poller_retry_count= it->at(18);	
// 	string exit_code                = it->at(19);			
// 	string worker_node              = it->at(20);		
// 	string is_killed_byice          = it->at(21);
// 	string delegationid             = it->at(22);
// 	string last_empty_notification  = it->at(23);
// 	string last_seen                = it->at(24);
	

// 	CreamJob tmpJob(
// 			gridjobid ,
// 			creamjobid,
// 			jdl,
// 			userproxy,
// 			ceid,
// 			endpoint,
// 			creamurl,
// 			creamdelegurl,
// 			userdn,
// 			myproxyurl,
// 			proxy_renewable,
// 			failure_reason,
// 			sequence_code,
// 			wn_sequence_code,
// 			prev_status,
// 			status,
// 			num_logged_status_changes,
// 			leaseid,
// 			status_poller_retry_count,
// 			exit_code,
// 			worker_node,
// 			is_killed_byice,
// 			delegationid,
// 			last_empty_notification,
// 			last_seen
// 			);
// 	m_result.push_back( tmpJob );
//      }


}
