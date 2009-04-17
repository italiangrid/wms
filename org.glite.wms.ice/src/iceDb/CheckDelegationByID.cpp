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
 * Get all user proxies
 *
 * Authors: Alvise Dorigo <alvise.dorigo@pd.infn.it>
 *          Moreno Marzolla <moreno.marzolla@pd.infn.it>
 */

#include "CheckDelegationByID.h"
#include <sstream>
#include <vector>
#include <iostream>

using namespace glite::wms::ice::db;
using namespace std;

namespace { // begin local namespace

    // Local helper function: callback for sqlite
    static int fetch_field_callback(void *param, int argc, char **argv, char **azColName) {
    
      string* result = (string*) param;
      
      if ( argv && argv[0] ) {
        (*result) = argv[0];
      }
	  
      return 0;
      
    }

} // end local namespace

void CheckDelegationByID::execute( sqlite3* db ) throw ( DbOperationException& )
{
  ostringstream sqlcmd;
 
  sqlcmd << "SELECT delegationid FROM delegation WHERE delegationid=\'";
  sqlcmd << m_delegid << "\';";

  string tmp;

  if(::getenv("GLITE_WMS_ICE_PRINT_QUERY") )
    cout << "Executing query ["<<sqlcmd.str()<<"]"<<endl;

  do_query( db, sqlcmd.str(), fetch_field_callback, &tmp );
  
  if( !tmp.empty() ) {
    m_found = true;
  }
}
