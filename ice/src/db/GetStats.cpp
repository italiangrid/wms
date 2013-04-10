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
#include "GetStats.h"
#include "utils/IceUtils.h"

#include <vector>
#include <cstdlib>

#include <boost/lexical_cast.hpp>

using namespace glite::wms::ice;
using namespace std;

namespace { // begin local namespace

    // Local helper function: callback for sqlite
    static int fetch_fields_callback(void *param, int argc, char **argv, char **azColName) {
    
      vector< pair<time_t, int> >* result = (vector< pair<time_t, int> >*) param;
      
      if ( argv && argv[0] ) {
        //result->push_back(argv[0]);
	result->push_back( make_pair((time_t)atoi(argv[0]), atoi(argv[1])) );
      }
	  
      return 0;
      
    }

} // end local namespace

void db::GetStats::execute( sqlite3* db ) throw ( DbOperationException& )
{
  string sqlcmd("SELECT timestamp,status FROM stats WHERE timestamp >= ");
  sqlcmd += util::IceUtils::withSQLDelimiters( boost::lexical_cast<std::string>( (unsigned long long int)m_datefrom ) );
  sqlcmd += " AND timestamp <= ";
  sqlcmd += util::IceUtils::withSQLDelimiters( boost::lexical_cast<std::string>( (unsigned long long int)m_dateto  ) );
  sqlcmd += ";";

  do_query( db, sqlcmd, fetch_fields_callback, m_target );
  
}
