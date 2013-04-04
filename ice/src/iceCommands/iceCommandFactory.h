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

#ifndef GLITE_WMS_ICE_ICECOMMANDFACTORY_H
#define GLITE_WMS_ICE_ICECOMMANDFACTORY_H

#include "iceUtils/ClassadSyntax_ex.h"
#include "iceUtils/JobRequest_ex.h"
#include "iceUtils/CreamJob.h"
#include <string>

namespace glite {
  namespace wms {
    namespace ice {
      
      namespace util {
	
        class Request;
      }
	
      class IceAbstractCommand;
      
      /**
       * This class is used to build an IceAbstractCommand object
       * corresponding to the command serialized in the request
       * classad. This class implements the classic "factory"
       * design pattern.
       */
      class iceCommandFactory {
      public:
	virtual ~iceCommandFactory( ) { };
	
	/**
	 * Returns an object instance of IceAbstractCommand class;
	 * the actual object class depends on the content of
	 * the "request" parameter. The caller owns the
	 * returned object, and is responsible for
	 * deallocating it.
	 *
	 * @param request the classad containing the
	 * serialized version of the command to be built.
	 *
	 * @return a dynamically allocated IceAbstractCommand object.
	 */ 
	static IceAbstractCommand* mkCommand( util::Request* request,
					 const util::CreamJob&,
					 const std::string& cmdtype) 
	  throw( util::ClassadSyntax_ex&, util::JobRequest_ex&);
	
      protected:
	iceCommandFactory( ) { };
	
      };
      
      
    } // namespace ice
  } // namespace wms
} // namespace glite

#endif
