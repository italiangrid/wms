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
 *
 *
 * Authors: Alvise Dorigo <alvise.dorigo@pd.infn.it>
 *          Moreno Marzolla <moreno.marzolla@pd.infn.it>
 */

#include "GetJobByCid.h"

//#include "boost/algorithm/string.hpp"

#include <sstream>
#include <iostream>

using namespace glite::wms::ice::db;
using namespace glite::wms::ice::util;
using namespace std;
namespace cream_api = glite::ce::cream_client_api;

GetJobByCid::GetJobByCid( const string& cid ) :
  AbsDbOperation(),
  m_complete_creamjobid( cid ),
  m_theJob(),
  m_found( false )
  //    m_serialized_job( "" )
{
  
}

// Local helper function: callback for sqlite
static int fetch_job_callback(void *param, int argc, char **argv, char **azColName){
  
  vector<string> *fields = (vector<string>*)param;
  if ( argv && argv[0] ) 
    {
      for(int i = 0; i<=24; i++) {// a database record for a CreamJob has 26 fields, as you can see in Transaction.cpp, but we excluded the complete_cream_jobid from the query
	if( argv[i] )
	  fields->push_back( argv[i] );
	else
	  fields->push_back( "" );
      }
    }
  return 0;
}

void GetJobByCid::execute( sqlite3* db ) throw ( DbOperationException& )
{

    ostringstream sqlcmd("");
    sqlcmd << "SELECT " << CreamJob::get_query_fields() << " FROM jobs WHERE complete_cream_jobid=\'" << m_complete_creamjobid << "\';";
    
    vector<string> field_list;
    
  if(::getenv("GLITE_WMS_ICE_PRINT_QUERY") )
    cout << "Executing query ["<<sqlcmd.str()<<"]"<<endl;

    do_query( db, sqlcmd.str(), fetch_job_callback, &field_list );

    if( !field_list.empty() ) {
      m_found = true;

      string gridjobid                = field_list.at(0);	    
      string creamjobid               = field_list.at(1);	    
      string jdl                      = field_list.at(2);    
      string userproxy                = field_list.at(3);
      string ceid                     = field_list.at(4);
      string endpoint                 = field_list.at(5);
      string creamurl                 = field_list.at(6);        
      string creamdelegurl            = field_list.at(7);   
      string userdn                   = field_list.at(8);          
      string myproxyurl               = field_list.at(9);      
      string proxy_renewable          = field_list.at(10); 
      string failure_reason           = field_list.at(11);  
      string sequence_code            = field_list.at(12);   
      string wn_sequence_code         = field_list.at(13);		
      string prev_status              = field_list.at(14);		
      string status                   = field_list.at(15);			
      string num_logged_status_changes= field_list.at(16);
      string leaseid                  = field_list.at(17);   
      string status_poller_retry_count= field_list.at(18);	
      string exit_code                = field_list.at(19);			
      string worker_node              = field_list.at(20);		
      string is_killed_byice          = field_list.at(21);
      string delegationid             = field_list.at(22);
      string last_empty_notification  = field_list.at(23);
      string last_seen                = field_list.at(24);
      
      m_theJob = CreamJob (
      			field_list.at(0),	    
                        field_list.at(1),    
                        field_list.at(2),   
                        field_list.at(3),
                        field_list.at(4),
                        field_list.at(5),
                        field_list.at(6) ,       
                        field_list.at(7)  , 
                        field_list.at(8)   ,       
                        field_list.at(9)    ,  
                        field_list.at(10) ,
                        field_list.at(11)  ,
                        field_list.at(12)   ,
                	field_list.at(13)	,	
                     	field_list.at(14)		,
                        field_list.at(15)			,
       			field_list.at(16),
                        field_list.at(17) ,  
       			field_list.at(18)	,
                        field_list.at(19)		,	
                        field_list.at(20)		,
             		field_list.at(21),
                    	field_list.at(22),
         		field_list.at(23),
                       	field_list.at(24)
			);
			
    }
}
