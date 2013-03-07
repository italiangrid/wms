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

#ifndef GLITE_WMS_ICE_GET_DELEGATIONBYID_H
#define GLITE_WMS_ICE_GET_DELEGATIONBYID_H

#include "AbsDbOperation.h"
#include <string>
#include "iceUtils/DelegationManager.h"

namespace glite {
  namespace wms {
    namespace ice {
      namespace db {
	
	/**
	 *
	 */
	class GetDelegationByID : public AbsDbOperation {
	protected:
	  glite::wms::ice::util::Delegation_manager::table_entry m_result;
	  const std::string m_id;
	  bool              m_found;

	public:
	  GetDelegationByID( const std::string& id, const std::string& caller ) 
	    : AbsDbOperation( caller ), m_id( id ) 
	    { }
	  
	  virtual void execute( sqlite3* db ) throw( DbOperationException& );
	  
	  /**
	   * Return the list of jobs to poll
	   */ 
	  glite::wms::ice::util::Delegation_manager::table_entry
	    get_delegation( void ) const {
              return m_result;
	    }
	  
	  bool found( void ) const { return m_found; }
	  
	};
	
      } // namespace db
    } // namespace ice
  } // namespace wms
} // namespace glite

#endif
