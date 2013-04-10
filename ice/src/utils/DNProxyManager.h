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
#ifndef GLITE_WMS_ICE_UTIL_DNPROXYMGR
#define GLITE_WMS_ICE_UTIL_DNPROXYMGR

#include <utility>
#include <string>
#include <map>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/tuple/tuple.hpp>
#include <cstring>
#include <string.h>
#include "CopyProxyException.h"
#include "common/src/configuration/ICEConfiguration.h"
#include "IceConfManager.h"
#include "IceUtils.h"
#include "CreamJob.h"

namespace log4cpp {
  class Category;
};

namespace glite {
namespace wms {
namespace ice {
namespace util {

    class DNProxyManager {
        
        static DNProxyManager               *s_instance;
        /**
         * The m_DNProxyMap[ dn ] is a tuple (proxycert file, expirationTime, duration).
	 * The key 'dn' is the distinguished name (DN+FQAN) of the user
         */
        
	std::map< std::string, std::set< std::string> > m_temp_dnproxy_Map;
        log4cpp::Category *m_log_dev;
        
    protected:
        
        DNProxyManager() throw();
        ~DNProxyManager() throw() {}
        static boost::recursive_mutex  s_mutex;
        
    public:
        
	struct proxy_info {
	  std::string userdn;
	  std::string myproxy;
	  std::string proxyfile;
	  time_t      expiration_time;
	  long long   counter;
	};
	

        static DNProxyManager* getInstance() throw();
        bool setUserProxyIfLonger_Legacy( const std::string& proxy) throw();
        bool setUserProxyIfLonger_Legacy( const std::string& dn, const std::string& proxy) throw();
	bool setUserProxyIfLonger_Legacy( const std::string& dn, const std::string& proxy, const time_t ) throw();

	bool incrementUserProxyCounter( const CreamJob& aJob,
					const time_t proxy_time_end) throw();

	bool decrementUserProxyCounter( const std::string& dn, 
					const std::string& myproxyname ) throw();

	bool setBetterProxy( const std::string& dn,
			     const std::string& proxyfile,
			     const std::string& myproxyname,
			     const time_t,
			     const unsigned long long) throw();

	boost::tuple<std::string, time_t, long long int> getExactBetterProxyByDN( const std::string& dn,
										  const std::string& myproxyname) const throw() ;

	boost::tuple<std::string, time_t, long long int> getAnyBetterProxyByDN( const std::string& dn ) const throw();

	
	bool removeBetterProxy( const std::string& dn, const std::string& myproxyname ) throw();

	bool updateBetterProxy( const std::string& userDN, 
				const std::string& myproxyname,
				const boost::tuple<std::string, 
				time_t, 
				long long int>& newEntry ) throw();

	
	
    private:

	std::string make_betterproxy_path( const std::string& dn, const std::string& myproxy ) throw();
	
        void copyProxy( const std::string& source, const std::string& target ) throw(CopyProxyException&);

	std::string composite( const std::string& userDN, const std::string& myproxy_name) const throw()
	  {
	    return userDN+"_"+myproxy_name;
	  }

    };
    
}
}
}
}

#endif
