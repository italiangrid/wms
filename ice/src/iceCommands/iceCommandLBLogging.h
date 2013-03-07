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

#ifndef GLITE_WMS_ICE_ICECOMMANDLBLOGGING_H
#define GLITE_WMS_ICE_ICECOMMANDLBLOGGING_H

#include "iceAbsCommand.h"
#include "iceUtils/CreamJob.h"
#include <boost/scoped_ptr.hpp>

#include<string>
#include<list>

namespace log4cpp {
  class Category;
};

namespace glite {
  namespace wms {
    namespace ice {
      namespace util {
	
	class iceLBLogger;
	
	class iceCommandLBLogging : public iceAbsCommand {
	  
	  log4cpp::Category			*m_log_dev;
	  std::list<CreamJob> 			 m_jobs_to_remove;
	  glite::wms::ice::util::iceLBLogger    *m_lb_logger;
	  
	  iceCommandLBLogging( const iceCommandLBLogging& ) : iceAbsCommand("", "") {}
	  
	  //void renewAllDelegations( void ) throw();

	public:

	  iceCommandLBLogging( const std::list<CreamJob>& );
	  virtual ~iceCommandLBLogging( );
	  void execute( const std::string& ) throw();
	  
	  std::string get_grid_job_id() const;// { return ""; }
	};
	
      } // namespace util
    } // namespace ice
  } // namespace wms
} // namespace glite

#endif
