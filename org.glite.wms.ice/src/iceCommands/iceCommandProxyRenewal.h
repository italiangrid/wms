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

#ifndef GLITE_WMS_ICE_ICECOMMANDPROXYRENEWAL_H
#define GLITE_WMS_ICE_ICECOMMANDPROXYRENEWAL_H

#include "iceAbsCommand.h"
#include "creamJob.h"
#include "boost/scoped_ptr.hpp"

#include<string>
#include<list>

namespace log4cpp {
  class Category;
};

/* namespace glite { */
/*     namespace ce { */
/*         namespace cream_client_api { */
/*             namespace soap_proxy { */
                
/*                 class CreamProxy; */
                
/*             } */
/*         } */
/*     } */
/* }; */

namespace glite {
  namespace wms {
    namespace ice {
      namespace util {
	
	//class jobCache;
	
	class iceCommandProxyRenewal : public iceAbsCommand {
	  log4cpp::Category* m_log_dev;
	  //boost::scoped_ptr< glite::ce::cream_client_api::soap_proxy::CreamProxy > m_theProxy;
	  //jobCache* m_cache;
	  
	  iceCommandProxyRenewal( const iceCommandProxyRenewal& );
	  
	  //bool iceCommandProxyRenewal::renewProxy( const std::list<CreamJob>& jobs) throw();
	  //	  bool iceCommandProxyRenewal::renewProxy( const std::pair<std::string, std::string>&, const std::string&) throw();
	  void renewAllDelegations( void ) throw();

	  //	  void getAllPhysicalNewProxies( std::set<std::string>& ) throw();

	public:
	  iceCommandProxyRenewal( );
	  virtual ~iceCommandProxyRenewal( ) {}
	  void execute( const std::string& ) throw();
	  
	  std::string get_grid_job_id() const { return ""; }
	};
	
      } // namespace util
    } // namespace ice
  } // namespace wms
} // namespace glite

#endif
