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

#include <cstdlib>

#include "GetDbID.h"
#include "utils/IceUtils.h"

using namespace std;
using namespace glite::wms::ice;

namespace {
  
  static int fetch_jobs_callback(void *param, int argc, char **argv, char **azColName){
    
    if( argv && argv[0] ) {
   
      long long* result = (long long*)param;
      *result = atoll(argv[0]);

    }

    return 0;
  }
  
} // end local namespace

//______________________________________________________________________________
void db::GetDbID::execute( sqlite3* db ) throw ( DbOperationException& )
{
  string sqlcmd( "SELECT db_id FROM ce_dbid WHERE ceurl=" );
  sqlcmd += util::IceUtils::withSQLDelimiters( m_creamurl );
  sqlcmd += ";";

  do_query( db, sqlcmd, fetch_jobs_callback, &m_result );

  if(m_result>-1)
    m_found = true;
}
