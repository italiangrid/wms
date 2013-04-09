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

#ifndef GLITE_WMS_ICE_ICECOMMANDSUBUPD_H
#define GLITE_WMS_ICE_ICECOMMANDSUBUPD_H

#include "iceAbsCommand.h"
#include "glite/ce/monitor-client-api-c/CESubscriptionMgr.h"

#include <boost/scoped_ptr.hpp>

namespace cemon_api = glite::ce::monitor_client_api::soap_proxy;

namespace glite {
  namespace wms {
    namespace ice {
      namespace util {
        class subscriptionManager;
      }
      }
      }
      };

namespace log4cpp {
  class Category;
};

namespace glite {
namespace wms {

namespace common {
namespace configuration {
    class Configuration;
} // namespace configuration
} // namespace common

namespace ice {
     
// Forward declarations
namespace util {                
    class IceConfManager;
    class iceLBLogger;

     
  
 class iceCommandSubUpdater : public iceAbsCommand {
   std::string m_proxyfile;
   subscriptionManager *m_subMgr;
   
   log4cpp::Category *m_log_dev;
   
   IceConfManager *m_conf;

   void retrieveCEURLs( std::set<std::string>& );
   void renewSubscriptions( std::vector<cemon_api::Subscription>& vec );

  public:
   iceCommandSubUpdater( ) throw( );
   
   virtual void execute( const std::string& ) throw( );
   
   virtual ~iceCommandSubUpdater() { }
   
   std::string get_grid_job_id( void ) const { return ""; }
 };

} // namespace util
} // namespace ice
} // namespace wms
} // namespace glite


#endif
