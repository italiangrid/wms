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

#ifndef GLITE_WMS_ICE_UTIL_REQPARSER_HH
#define GLITE_WMS_ICE_UTIL_REQPARSER_HH

#include "ClassadSyntax_ex.h"
#include "JobRequest_ex.h"
#include "CreamJob.h"

namespace glite {
  namespace wms {
    namespace ice {
      namespace util {
      
        class Request;
        
	class RequestParser {
	  
	  std::string  m_command;
	  std::string  m_protocol;
	  CreamJob     m_job;
	  Request     *m_request;
	 
	 public:
	  RequestParser( Request* req ) : m_request( req ) { }
	  void        unparse_request( void ) throw(ClassadSyntax_ex&, JobRequest_ex&);
	  CreamJob    get_job( void ) const { return m_job; }
	  std::string get_command( void ) const { return m_command; }
	  
	  
	  
	};
      
      }
    }
  }
}

#endif
