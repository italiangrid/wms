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

#include "GetAllDelegation.h"
#include "main/Main.h"

#include <string>
#include <vector>

using namespace glite::wms::ice;
using namespace std;

namespace { // begin local namespace

    // Local helper function: callback for sqlite
    static int fetch_fields_callback(void *param, int argc, char **argv, char **azColName) {
    
      list<vector<string> >* result = (list<vector<string> >*) param;
      
      vector<string> tmp;
      if ( argv && argv[0] ) {
        tmp.push_back(argv[0]);
	tmp.push_back(argv[1]);
	tmp.push_back(argv[2]);
	tmp.push_back(argv[3]);
	tmp.push_back(argv[4]);
	tmp.push_back(argv[5]);
	tmp.push_back(argv[6]);
	tmp.push_back(argv[7]);
	result->push_back( tmp );
      }
	  
      return 0;
      
    }

} // end local namespace

void db::GetAllDelegation::execute( sqlite3* db ) throw ( DbOperationException& )
{
  string sqlcmd;
  if( m_only_renewable) {
    sqlcmd += "SELECT * FROM delegation WHERE renewable=";
    sqlcmd += glite::wms::ice::util::IceUtils::withSQLDelimiters( "1" );
    sqlcmd += ";";
  }
  else
    sqlcmd = "SELECT * FROM delegation;";

  list<vector<string> > tmp;

  do_query( db, sqlcmd, fetch_fields_callback, &tmp );
  
  if( tmp.size() ) {
    for(list<vector<string> >::const_iterator it = tmp.begin();
        it != tmp.end();
	++it)
	{
	  glite::wms::ice::util::Delegation_manager::table_entry tb( it->at(0), 
								     it->at(1), 
								     (time_t)atoi(it->at(2).c_str()), 
								     atoi(it->at(3).c_str()), 
								     it->at(4),
								     it->at(5),
								     atoi(it->at(6).c_str()),
								     it->at(7)
								     );
	  m_result.push_back( tb );
	}
  }
}
