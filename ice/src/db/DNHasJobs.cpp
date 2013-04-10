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

#include "DNHasJobs.h"
#include "utils/CreamJob.h"

#include <cstdlib>

using namespace glite::wms::ice;
using namespace std;

namespace { // begin local namespace

  // Local helper function: callback for sqlite
  static int fetch_jobs_callback(void *param, int argc, char **argv, char **azColName){
    
    bool* found = (bool*)param;
    
    if( argv && argv[0] ) {
    
      (*found) = true;
    
    }

    return 0;
  }
  
} // end local namespace

void db::DNHasJobs::execute( sqlite3* db ) throw ( DbOperationException& )
{

  string sqlcmd = "SELECT ";
  sqlcmd += util::CreamJob::grid_jobid_field( );
  sqlcmd += " FROM jobs WHERE " ;
  sqlcmd += util::CreamJob::user_dn_field( );
  sqlcmd += "=";
 
  sqlcmd += glite::wms::ice::util::IceUtils::withSQLDelimiters( m_dn );
  sqlcmd += " AND ";
  sqlcmd += util::CreamJob::cream_address_field( ) ;
  sqlcmd += "=";
  sqlcmd += glite::wms::ice::util::IceUtils::withSQLDelimiters( m_ce );
  sqlcmd += " LIMIT 1";

  do_query( db, sqlcmd, fetch_jobs_callback, &m_found );
}
