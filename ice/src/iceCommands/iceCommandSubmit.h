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

#ifndef GLITE_WMS_ICE_ICECOMMANDSUBMIT_H
#define GLITE_WMS_ICE_ICECOMMANDSUBMIT_H

#include "iceAbsCommand.h"
#include "iceCommandFatal_ex.h"
#include "iceCommandTransient_ex.h"
#include "iceUtils/BlackListFailJob_ex.h"
#include "iceUtils/CreamJob.h"

#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/AbsCreamProxy.h"
#include "glite/ce/cream-client-api-c/VOMSWrapper.h"

#include "iceUtils/ClassadSyntax_ex.h"
#include "classad_distribution.h"

#include <boost/scoped_ptr.hpp>

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
    class iceLBLogger;
    class Request;
}
     
  
 class iceCommandSubmit : public iceAbsCommand {
     
 private:
     void  doSubscription( const glite::wms::ice::util::CreamJob& );
     
     IceCore *m_theIce;
     static boost::recursive_mutex s_localMutexForSubscriptions;
     static boost::recursive_mutex s_localMutexForDelegations;

 public:
     iceCommandSubmit( util::Request* request, 
		       const util::CreamJob& );
       //       throw(util::ClassadSyntax_ex&, util::JobRequest_ex&);
     
     virtual ~iceCommandSubmit() { }
     
     /**
      * This method is invoked to execute this command.
      */
     virtual void execute( const std::string& ) throw( iceCommandFatal_ex&, iceCommandTransient_ex& );
     
     std::string get_grid_job_id( void ) const { return m_theJob.grid_jobid(); };

 

 protected:

     /**
      * This method is called only by the execute() method. This
      * method does the real work of submitting the job. If something
      * fails, this method raises an exception. The caller ( execute()
      * ) must take care of the post-failure operations, that is
      * logging the appropriate events to LB and try to resubmit.
      */

     void try_to_submit( const bool only_start ) throw( BlackListFailJob_ex&, iceCommandFatal_ex&, iceCommandTransient_ex& );
     //bool try_to_register( void ) throw( iceCommandFatal_ex&, iceCommandTransient_ex& );
     //     bool try_to_start( void ) throw( iceCommandFatal_ex&, iceCommandTransient_ex& );

     void process_lease( const bool force_lease, 
			 const std::string& jobdesc, 
			 const std::string&, 
			 std::string& lease_id ) throw( iceCommandFatal_ex&, iceCommandTransient_ex& );

     void handle_delegation( std::string& delegation, 
			     bool&,
			     const std::string& jobdesc,
			     const std::string& _gid,
			     const std::string& ceurl) throw( iceCommandTransient_ex& );

     bool register_job( const bool is_lease_enabled, 
			const std::string& jobdesc,
			const std::string& _gid,
			const std::string& delegation,
			const std::string& lease_id,
			const std::string& modified_jdl,
			bool& force_delegation,
			bool& force_lease,
			glite::ce::cream_client_api::soap_proxy::AbsCreamProxy::RegisterArrayResult& res) 
       throw( BlackListFailJob_ex&, iceCommandTransient_ex&, iceCommandFatal_ex& );
     
     void process_result( bool& retry, 
			  bool& force_delegation, 
			  bool& force_lease,
			  const bool,
			  const std::string& _gid,
			  const glite::ce::cream_client_api::soap_proxy::AbsCreamProxy::RegisterArrayResult& res )
       throw( iceCommandTransient_ex& );

     
     
     /**
      * This method is used to transform a "standard" jdl into the
      * format expected by CREAM. In particular: the mandatory (for
      * CREAM) attributes QueueName and BatchSystem are
      * added. Moreover, the Input and Output sandbox attributes are
      * modified.
      *
      * @param oldJdl the original jdl
      * @retyrn the CREAM-compliand jdl
      */
     //std::string creamJdlHelper( const std::string& oldJdl ) throw( glite::wms::ice::util::ClassadSyntax_ex& );
     
     /**
      * This function updates the "InputSandbox" attribute value on
      * the jdl passed as parameter.
      *
      * @param jdl the original jdl, which will be modified
      * by this function
      */
     //void updateIsbList( classad::ClassAd* jdl );
     
     /**
      * This function updates the "OutputSandbox"-related
      * attribute value on the jdl passed as parameter.
      *
      * @param jdl the original jdl, which will be modified
      * by this function
      */
     //void updateOsbList( classad::ClassAd* jdl );
     
     std::string m_myname;	
     std::string m_jdl;
     util::CreamJob m_theJob;
     log4cpp::Category* m_log_dev;
     glite::wms::common::configuration::Configuration* m_configuration;
     std::string m_myname_url;
     util::iceLBLogger *m_lb_logger;
     util::Request* m_request;
};

} // namespace ice
} // namespace wms
} // namespace glite

#endif
