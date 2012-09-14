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

#include "AbsDbOperation.h"
#include "iceUtils/IceUtils.h"
#include "boost/format.hpp"
#include "boost/algorithm/string.hpp"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/scoped_timer.h"

#include <iostream>

using namespace glite::wms::ice;
using namespace std;
namespace api_util   = glite::ce::cream_client_api::util;

void db::AbsDbOperation::do_query( sqlite3* db, const string& _sqlcmd, sqlite_callback_t callback, void* param ) throw( DbOperationException& )
{

  string encoded_sql( _sqlcmd );
  boost::replace_all( encoded_sql, "'", "''" );
  
  boost::replace_all( encoded_sql, util::IceUtils::get_tmp_name(), "'" );

    char* errMsg; 
    string error;                               
    int retry = 0;
    int s = 2;
    
    if(::getenv("GLITE_WMS_ICE_PRINT_QUERY") )
    	CREAM_SAFE_LOG(
		 	api_util::creamApiLogger::instance()->getLogger()->debugStream()
			<< "AbsDbOperation::do_query - "
			<< "CALLER=["
			<< get_caller()
			<< "] is executing query ["
			<< encoded_sql << "]");

    while(1) {
      int rc = sqlite3_exec(db, encoded_sql.c_str(), callback, param, &errMsg);
      switch ( rc ) {
      case SQLITE_OK:
	return;
        break; // normal termination        
      case SQLITE_BUSY:
      case SQLITE_LOCKED:
        error = boost::str( boost::format( "Query [%1%] failed due to error [%2%]" ) % encoded_sql % errMsg );
        sqlite3_free(errMsg);
        throw DbLockedException( error );
      default:
        error = boost::str( boost::format( "Query [%1%] failed due to error [%2%]" ) % encoded_sql % errMsg );
        sqlite3_free(errMsg);
	sleep(s);
	s = s*2;
	++retry;
	
	//cout << "retrying... " << s << endl;
	
	if(retry > 4) {
	  CREAM_SAFE_LOG(
		 	api_util::creamApiLogger::instance()->getLogger()->fatalStream()
			<< "AbsDbOperation::do_query - " << error);
	  exit(1);
          //throw DbOperationException( error );
	}
      }
    }
}
