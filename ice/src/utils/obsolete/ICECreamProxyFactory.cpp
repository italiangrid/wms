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


#include "ICECreamProxyFactory.h"
#include "glite/ce/cream-client-api-c/CreamProxyFactory.h"
//#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"

#include <boost/algorithm/string.hpp>

namespace cream_api=glite::ce::cream_client_api;

using namespace glite::ce::cream_client_api::soap_proxy;

using namespace glite::wms::ice::util;
using namespace std;

string ICECreamProxyFactory::hostdn = string();
boost::recursive_mutex ICECreamProxyFactory::mutex;

// void ICECreamProxyFactory::setHostDN( const string& hdn )
// {
//     hostdn = hdn;
//     boost::trim_if(hostdn, boost::is_any_of("/"));
//     boost::replace_all( hostdn, "/", "_" );
//     boost::replace_all( hostdn, "=", "_" );
// }

//______________________________________________________________________________
AbsCreamProxy* 
ICECreamProxyFactory::makeCreamProxyRegister(const AbsCreamProxy::RegisterArrayRequest* param1,
					     AbsCreamProxy::RegisterArrayResult* param2 )
{

    boost::recursive_mutex::scoped_lock M( ICECreamProxyFactory::mutex );

    return CreamProxyFactory::make_CreamProxyRegister( param1, param2 );
}

//______________________________________________________________________________
AbsCreamProxy* 
ICECreamProxyFactory::makeCreamProxyStart(JobFilterWrapper* param1,
					  ResultWrapper* param2,
					  )
{

    boost::recursive_mutex::scoped_lock M( ICECreamProxyFactory::mutex );

    return CreamProxyFactory::make_CreamProxyStart( param1, param2 );
}

//______________________________________________________________________________
AbsCreamProxy* 
ICECreamProxyFactory::makeCreamProxyStatus(JobFilterWrapper* param1,
					   StatusArrayResult* param2
					   )
{

    boost::recursive_mutex::scoped_lock M( ICECreamProxyFactory::mutex );

    return CreamProxyFactory::make_CreamProxyStatus( param1, param2 );
}

//______________________________________________________________________________
AbsCreamProxy* 
ICECreamProxyFactory::makeCreamProxyLease(const std::pair<std::string, time_t>& param1, 
					  std::pair<std::string, time_t>* param2
					  )
{

    boost::recursive_mutex::scoped_lock M( ICECreamProxyFactory::mutex );

    return CreamProxyFactory::make_CreamProxyLease( param1, param2 );
}

//______________________________________________________________________________
AbsCreamProxy* 
ICECreamProxyFactory::makeCreamProxyCancel(JobFilterWrapper* param1,
					   ResultWrapper* param2
					   )
{

    boost::recursive_mutex::scoped_lock M( ICECreamProxyFactory::mutex );

    return CreamProxyFactory::make_CreamProxyCancel( param1, param2 );
}

//______________________________________________________________________________
AbsCreamProxy* 
ICECreamProxyFactory::makeCreamProxyPurge(JobFilterWrapper* param1,
					  ResultWrapper* param2
					  )
{

    boost::recursive_mutex::scoped_lock M( ICECreamProxyFactory::mutex );

    return CreamProxyFactory::make_CreamProxyPurge( param1, param2 );
}

//______________________________________________________________________________
AbsCreamProxy* 
ICECreamProxyFactory::makeCreamProxyDelegate(JobFilterWrapper* param1,
					  ResultWrapper* param2
					  )
{

    boost::recursive_mutex::scoped_lock M( ICECreamProxyFactory::mutex );

    return CreamProxyFactory::make_CreamProxyDelegate( param1, param2 );
}

//______________________________________________________________________________
AbsCreamProxy* 
ICECreamProxyFactory::makeCreamProxyDelegateRenew(JobFilterWrapper* param1,
					  ResultWrapper* param2
					  )
{

    boost::recursive_mutex::scoped_lock M( ICECreamProxyFactory::mutex );

    return CreamProxyFactory::make_CreamProxyDelegateRenew( param1, param2 );
}

//______________________________________________________________________________
AbsCreamProxy*
ICECreamProxyFactory::makeCreamProxyServiceInfo( ServiceInfoWrapper* param1 )
{
  boost::recursive_mutex::scoped_lock M( ICECreamProxyFactory::mutex );

  return CreamProxyFactory::make_CreamProxyGetServiceInfo( param1 );
}
