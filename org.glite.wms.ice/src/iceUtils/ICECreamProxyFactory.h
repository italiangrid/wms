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
#ifndef GLITE_WMS_ICE_UTIL_CREAMPROXYFACTORY_H
#define GLITE_WMS_ICE_UTIL_CREAMPROXYFACTORY_H

#include <string>
#include "boost/thread/recursive_mutex.hpp"

namespace glite {
namespace ce {
namespace cream_client_api {
namespace soap_proxy {

    class AbsCreamProxy;
    ServiceInfoWrapper;
  
}
}
}
};

namespace glite {
namespace wms {
namespace ice {
namespace util {
                
    class ICECreamProxyFactory {
        
        static std::string hostdn;
        static boost::recursive_mutex mutex;
        
    public:
        
        static void setHostDN( const std::string& hdn );
        
        /**
         * Creates a new CreamProxy object. Ownership of
         * the returned pointer is transferred to the
         * caller.
         *
         * @param auto_del if true, the returned
         * CreamProxy object will do automatic delegation.
         *
         * @return a new CreamProxy object. The caller is responsible
         * for freeing the returned pointer.
         */
        static glite::ce::cream_client_api::soap_proxy::AbsCreamProxy* 
	  makeCreamProxyRegister( const glite::ce::cream_client_api::soap_proxy::AbsCreamProxy::RegisterArrayRequest* param1,
				  glite::ce::cream_client_api::soap_proxy::AbsCreamProxy::RegisterArrayResult* param2
				  );
	
	static glite::ce::cream_client_api::soap_proxy::AbsCreamProxy* 
	  makeCreamProxyStart( const glite::ce::cream_client_api::soap_proxy::JobFilterWrapper* param1,
			       glite::ce::cream_client_api::soap_proxy::ResultWrapper* param2
			       );
	
	static glite::ce::cream_client_api::soap_proxy::AbsCreamProxy* 
	  makeCreamProxyStatus( const glite::ce::cream_client_api::soap_proxy::JobFilterWrapper* param1,
				glite::ce::cream_client_api::soap_proxy::AbsCreamProxy::StatusArrayResult* param2
				);
	
	static glite::ce::cream_client_api::soap_proxy::AbsCreamProxy* 
	  makeCreamProxyLease( const std::pair<std::string, time_t>& param1, 
			       std::pair<std::string, time_t>* param2
			       );
	
	static glite::ce::cream_client_api::soap_proxy::AbsCreamProxy* 
	  makeCreamProxyCancel( const glite::ce::cream_client_api::soap_proxy::JobFilterWrapper* param1,
				glite::ce::cream_client_api::soap_proxy::ResultWrapper* param2
				);
	
	static glite::ce::cream_client_api::soap_proxy::AbsCreamProxy* 
	  makeCreamProxyPurge( const glite::ce::cream_client_api::soap_proxy::JobFilterWrapper* param1,
			       glite::ce::cream_client_api::soap_proxy::ResultWrapper* param2
			       );
	
	static glite::ce::cream_client_api::soap_proxy::AbsCreamProxy* 
	  makeCreamProxyDelegate( const std::string& param1, std::string* param2 );
	
        static glite::ce::cream_client_api::soap_proxy::AbsCreamProxy* 
	  makeCreamProxyDelegateRenew( const std::string& param1, std::string* param2 );
	
        static glite::ce::cream_client_api::soap_proxy::AbsCreamProxy* 
	  makeCreamProxyServiceInfo( ServiceInfoWrapper* param1 );
        
    };
    
} // namespace util
} // namespace ice
} // namespace wms
}; // namespace glite

#endif
