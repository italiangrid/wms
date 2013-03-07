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

#ifndef GLITE_WMS_ICE_GET_PROXYFIELDS_MYPRX_H
#define GLITE_WMS_ICE_GET_PROXYFIELDS_MYPRX_H

#include "AbsDbOperation.h"
#include <list>
#include <string>

#include <boost/tuple/tuple.hpp>

namespace glite {
  namespace wms {
    namespace ice {
      namespace db {
	
	/**
	 *
	 */
	class GetProxyInfoByDN_MYProxy : public AbsDbOperation {
	protected:
	  boost::tuple<std::string, time_t, long long int> m_result;
	  //const std::list<std::string> m_fields_to_retrieve;
	  //const std::list<std::pair<std::string, std::string> > m_clause;
	  const std::string m_userdn;
	  const std::string m_myproxy;
	  bool m_found;

	public:
	  GetProxyInfoByDN_MYProxy( const std::string& userdn, 
				    const std::string& myproxy,
			    	    const std::string& caller )
	    :  AbsDbOperation( caller ),
	    m_userdn( userdn ),
	    m_myproxy( myproxy ),
	    m_found( false ) {}
	  
	  virtual void execute( sqlite3* db ) throw( DbOperationException& );
	  
	  /**
	   * Return the list of jobs to poll
	   */ 
	  boost::tuple<std::string, time_t, long long int> get_info( void ) const {
            return m_result;
	  }
	  
	  bool found( void ) const { return m_found; }
	  
	};
	
      } // namespace db
    } // namespace ice
  } // namespace wms
} // namespace glite

#endif
