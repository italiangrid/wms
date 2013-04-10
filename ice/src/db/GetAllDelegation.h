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

#ifndef GLITE_WMS_ICE_GET_ALLDELEGATION_H
#define GLITE_WMS_ICE_GET_ALLDELEGATION_H

#include "AbsDbOperation.h"
#include <list>
#include "utils/DelegationManager.h"

namespace glite {
  namespace wms {
    namespace ice {
      namespace db {
	
	/**
	 *
	 */
	class GetAllDelegation : public AbsDbOperation {
	protected:
	  std::vector<glite::wms::ice::util::Delegation_manager::table_entry> m_result;
	  const bool                                                          m_only_renewable;
	 
	public:
	  GetAllDelegation( const bool only_renewable, const std::string& caller ) : AbsDbOperation( caller ), m_result(), m_only_renewable( only_renewable ) { }
	  
	  virtual void execute( sqlite3* db ) throw( DbOperationException& );
	  
	  /**
	   * Return the list of jobs to poll
	   */ 
	  std::vector<glite::wms::ice::util::Delegation_manager::table_entry>
	    get_delegations( void ) const {
              return m_result;
	    }
	  
	};
	
      } // namespace db
    } // namespace ice
  } // namespace wms
} // namespace glite

#endif
