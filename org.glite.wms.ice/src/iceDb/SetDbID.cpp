#include <sstream>
#include <cstdlib>
#include <iostream>

#include "SetDbID.h"

using namespace std;
using namespace glite::wms;

//______________________________________________________________________________
void ice::db::SetDbID::execute( sqlite3* db ) throw ( DbOperationException& )
{
  ostringstream sqlcmd;
  sqlcmd << "INSERT OR REPLACE INTO ce_dbid (ceurl,db_id) VALUES (\'" 
	 << m_creamurl << "\', \'" 
	 << m_new_dbid <<"\');";

  do_query( db, sqlcmd.str() );
}
