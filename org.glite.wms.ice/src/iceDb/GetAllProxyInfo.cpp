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

#include "GetAllProxyInfo.h"
#include <sstream>
#include <list>

using namespace glite::wms::ice::db;
using namespace std;

GetAllProxyInfo::GetAllProxyInfo( ) :    
  AbsDbOperation()
{
}

namespace { // begin local namespace

    // Local helper function: callback for sqlite
    static int fetch_fields_callback(void *param, int argc, char **argv, char **azColName) {
    
      map<string, boost::tuple< string, time_t, long long> >* result = (map<string, boost::tuple< string, time_t, long long> >*) param;
      
      if ( argv && argv[0] ) {
        (*result)[ argv[0] ] = boost::make_tuple(argv[1], (time_t)atoi(argv[2]), atoll(argv[3]) );
      }
	  
      return 0;
      
    }

} // end local namespace

void GetAllProxyInfo::execute( sqlite3* db ) throw ( DbOperationException& )
{
 
  string sqlcmd = "SELECT * FROM proxy;";
    
  do_query( db, sqlcmd, fetch_fields_callback, &m_result );
  

}
