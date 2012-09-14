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
#ifndef GLITE_WMS_ICE_GET_STATS_H
#define GLITE_WMS_ICE_GET_STATS_H

#include "AbsDbOperation.h"
#include <utility>
#include <string>
#include <vector>

namespace glite {
  namespace wms {
    namespace ice {
      namespace db {
	
	/**
	 *
	 */
	class GetStats : public AbsDbOperation {
	protected:
	  
	  std::vector< std::pair< time_t, int > > *m_target;
	  time_t                                   m_datefrom;
	  time_t                                   m_dateto;

	public:
	  GetStats( std::vector< std::pair< time_t, int > >& target, 
	  	    const time_t datefrom,
		    const time_t dateto,
		    const std::string& caller ) : AbsDbOperation( caller ), m_target(&target), m_datefrom( datefrom ), m_dateto( dateto ) {}
	  
	  virtual void execute( sqlite3* db ) throw( DbOperationException& );
	  
	};
	
      } // namespace db
    } // namespace ice
  } // namespace wms
} // namespace glite

#endif
