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

#include "RemoveJobByUserDN.h"
#include "utils/CreamJob.h"


using namespace glite::wms::ice;
using namespace std;

void db::RemoveJobByUserDN::execute( sqlite3* db ) throw ( DbOperationException& )
{
  string sqlcmd("DELETE FROM jobs WHERE ");
  sqlcmd += util::CreamJob::user_dn_field() ;
  sqlcmd += "=";
  sqlcmd += util::IceUtils::withSQLDelimiters( m_dn );
  sqlcmd += ";";
		  
  do_query( db, sqlcmd );
}
