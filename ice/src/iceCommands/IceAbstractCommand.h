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


#ifndef GLITE_WMS_ICE_ICEABSCOMMAND_H
#define GLITE_WMS_ICE_ICEABSCOMMAND_H

#include "iceUtils/ClassadSyntax_ex.h"
#include "classad_distribution.h"
#include "iceUtils/JobRequest_ex.h"
#include "iceCommandFatal_ex.h"
#include "iceCommandTransient_ex.h"

// forward declaration
namespace glite {
    namespace ce {
        namespace cream_client_api {
            namespace soap_proxy {
                class CreamProxy;
            }
        }
    }
}

namespace glite {
    namespace wms {
        namespace ice {

            class Ice;

            class IceAbstractCommand {
	
            public:

                virtual ~IceAbstractCommand() { };

                /**
                 * Executes the command. 
                 *
                 * @throw an iceCommandFatal_ex if the command is to
                 * be considered permanently failed;
                 * @throw an iceCommandTransient_ex if the command failed
                 * but could be tried again and succeed.
                 */
                virtual void execute( const std::string& ) throw( iceCommandFatal_ex&, iceCommandTransient_ex& ) = 0;

                /**
                 * Returns the Grid jobID for the job this command
                 * refers to.
                 */
                virtual std::string get_grid_job_id( void ) const = 0;
          
                /**
                 * Return the name of this command
                 */
                std::string name( void ) { return m_name; };
            protected:

                IceAbstractCommand( const std::string& name, const std::string& tid ) throw(glite::wms::ice::util::ClassadSyntax_ex&, glite::wms::ice::util::JobRequest_ex&) : m_name( name ) {};
                std::string m_name; ///< Name of this command, default empty
		
		std::string m_thread_id;

		std::string getThreadID( void ) const { return m_thread_id; }
            };
        }
    }
}

#endif
