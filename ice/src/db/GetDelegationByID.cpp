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

#include "GetDelegationByID.h"
#include "utils/IceUtils.h"

#include <sstream>
#include <vector>

using namespace glite::wms::ice;
using namespace std;

namespace { // begin local namespace

    // Local helper function: callback for sqlite
    static int fetch_fields_callback(void *param, int argc, char **argv, char **azColName) {
    
      vector<string>* result = (vector<string>*) param;
      
      if ( argv && argv[0] ) {
        result->push_back(argv[0]);
	result->push_back(argv[1]);
	result->push_back(argv[2]);
	result->push_back(argv[3]);
	result->push_back(argv[4]);
	result->push_back(argv[5]);
	result->push_back(argv[6]);
	result->push_back(argv[7]);
      }
	  
      return 0;
      
    }

} // end local namespace

//______________________________________________________________________________
void db::GetDelegationByID::execute( sqlite3* db ) throw ( DbOperationException& )
{
  string sqlcmd( "SELECT * FROM delegation WHERE delegationid=");
  sqlcmd += util::IceUtils::withSQLDelimiters( m_id );
  sqlcmd += ";";

  vector<string> tmp;

  do_query( db, sqlcmd, fetch_fields_callback, &tmp );
  
  if( tmp.size() ) {
    m_found = true;
    m_result = glite::wms::ice::util::DelegationManager::table_entry( 
								      tmp.at(0), 
								      tmp.at(1),
								      (time_t)atoi(tmp.at(2).c_str()), 
								      atoi(tmp.at(3).c_str()),
								      tmp.at(4),
								      tmp.at(5),
								      ( atoi(tmp.at(6).c_str()) == 0 ? false : true ),
								      tmp.at(7)
								      );
  }
}
