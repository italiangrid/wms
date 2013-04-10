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
#ifndef GLITE_WMS_ICE_GET_STARTEDJOBS_H
#define GLITE_WMS_ICE_GET_STARTEDJOBS_H

#include "AbsDbOperation.h"
#include <utility>
#include <string>
#include <vector>

#include <boost/tuple/tuple.hpp>

namespace glite {
  namespace wms {
    namespace ice {
      namespace db {
	
	/**
	 *
	 */
	class GetStartedJobs : public AbsDbOperation {
	protected:
	  
	  std::vector< boost::tuple< time_t, std::string, std::string, std::string > > *m_target;
	  time_t                                   m_datefrom;
	  time_t                                   m_dateto;
	  std::string				   m_creamurl;
	  bool					   m_order;

	public:
	  GetStartedJobs( std::vector< boost::tuple< time_t, std::string, std::string, std::string > >& target, 
	  	          const time_t        datefrom,
		          const time_t        dateto,
		          const std::string&  creamurl,
			  const bool          order,
		          const std::string&  caller ) 
            : AbsDbOperation( caller ), 
	      m_target(&target), 
	      m_datefrom( datefrom ), 
	      m_dateto( dateto ), 
	      m_creamurl( creamurl ),
	      m_order( order ) {}
	  
	  virtual void execute( sqlite3* db ) throw( DbOperationException& );
	  
	};
	
      } // namespace db
    } // namespace ice
  } // namespace wms
} // namespace glite

#endif
