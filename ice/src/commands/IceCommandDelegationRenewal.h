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

#ifndef GLITE_WMS_ICE_ICECOMMANDDELEGRENEWAL_H
#define GLITE_WMS_ICE_ICECOMMANDDELEGRENEWAL_H

#include "IceAbstractCommand.h"
#include "iceUtils/CreamJob.h"
#include <boost/scoped_ptr.hpp>

#include "glite/security/proxyrenewal/renewal_core.h"

#include<string>
#include<list>

namespace log4cpp {
  class Category;
};

namespace glite {
  namespace wms {
    namespace ice {
      namespace util {
	
	class IceCommandDelegationRenewal : public IceAbstractCommand {
	  log4cpp::Category* m_log_dev;
	  glite_renewal_core_context m_ctx;

	  IceCommandDelegationRenewal( const IceCommandDelegationRenewal& ) : IceAbstractCommand("", "") {}
	  
	  void renewAllDelegations( void ) throw();

	public:

	  //const int DELEGATION_EXPIRATION_THRESHOLD_TIME;

	  IceCommandDelegationRenewal( );
	  virtual ~IceCommandDelegationRenewal( );
	  void execute( const std::string& ) throw();
	  
	  std::string get_grid_job_id() const;// { return ""; }
	};
	
      } // namespace util
    } // namespace ice
  } // namespace wms
} // namespace glite

#endif
