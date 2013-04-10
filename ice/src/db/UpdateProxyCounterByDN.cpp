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

#include "UpdateProxyCounterByDN.h"
#include "utils/IceUtils.h"
#include <boost/lexical_cast.hpp>

using namespace glite::wms::ice;
using namespace std;

void db::UpdateProxyCounterByDN::execute( sqlite3* db ) throw ( DbOperationException& )
{
  string sqlcmd("UPDATE proxy SET counter=" );
  sqlcmd += util::IceUtils::withSQLDelimiters( boost::lexical_cast<std::string>( m_counter ) );
  sqlcmd += " WHERE userdn=";
  sqlcmd += util::IceUtils::withSQLDelimiters( m_dn );
  sqlcmd += " AND myproxyurl=";
  sqlcmd += util::IceUtils::withSQLDelimiters( m_myproxy );
  sqlcmd += ";";
  
  do_query( db, sqlcmd );
}
