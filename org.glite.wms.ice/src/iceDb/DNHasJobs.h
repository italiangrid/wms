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

#ifndef GLITE_WMS_ICE_DNHAS_JOBS_H
#define GLITE_WMS_ICE_DNHAS_JOBS_H

#include "AbsDbOperation.h"
#include <list>
#include <utility>
#include <string>

namespace glite {
  namespace wms {
    namespace ice {
      namespace db {
	
	/**
	 *
	 */
	class DNHasJobs : public AbsDbOperation {
	protected:
	  
	  std::string  m_dn;
	  std::string  m_ce;
	  bool         m_found;
	  
	public:
	  /*GetJobs( const std::list< std::pair<std::string, std::string>& clauses,
		   std::list< glite::wms::ice::util::CreamJob >& result,
		   const std::string& caller ) : AbsDbOperation( caller ), m_result( &result ), m_clause( clauses ), m_use_or( false ) {}*/
	  DNHasJobs( const std::string& dn, const std::string& ce, const std::string& caller ) : AbsDbOperation( caller ),  m_dn( dn ), m_ce( ce ), m_found( false ) {}
	    
	  virtual void execute( sqlite3* db ) throw( DbOperationException& );
	  bool         found( void ) const { return m_found; }  
	    
	};
	
      } // namespace db
    } // namespace ice
  } // namespace wms
} // namespace glite

#endif
