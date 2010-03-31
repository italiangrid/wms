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
 * Get all user DNs
 *
 * Authors: Alvise Dorigo <alvise.dorigo@pd.infn.it>
 *          Moreno Marzolla <moreno.marzolla@pd.infn.it>
 */

#include "GetCidByCreamURLUserDN.h"
#include <sstream>
#include <iostream>
#include "boost/algorithm/string.hpp"
#include "boost/format.hpp"
using namespace glite::wms::ice::db;
using namespace std;

namespace { // begin local namespace

    // Local helper function: callback for sqlite
    static int fetch_cids_callback(void *param, int argc, char **argv, char **azColName){
        list< string > *result = (list< string >*)param;
	if ( argv && argv[0] ) {
	    result->push_back( argv[0] );
        }
        return 0;
    }

} // end local namespace

void GetCidByCreamURLUserDN::execute( sqlite3* db ) throw ( DbOperationException& )
{
  ostringstream sqlcmd("");

  string dn( m_creamurl_userdn.second );

  boost::replace_all( dn, "'", "''" );

  sqlcmd << "SELECT complete_cream_jobid FROM jobs WHERE creamurl=\'" 
         << m_creamurl_userdn.first 
	 << "\' AND userdn=\'"
	 << dn
	 << "\';";
  
  do_query( db, sqlcmd.str(), fetch_cids_callback, &m_result );
}
