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

#ifndef __GLITE_WMS_ICE_UTIL_PROXYNOTFOUND_H__
#define __GLITE_WMS_ICE_UTIL_PROXYNOTFOUND_H__

#include <exception>
#include <string>

namespace glite {
  namespace wms {
    namespace ice {
      namespace util {
        
        //! An exception for all errors that relate to BDB operation
        class CopyProxyException : public std::exception {
          
          std::string cause;
          
        public:
          //! SourceProxyNotFoundException constructor
          /**!
            Creates a SourceProxyNotFoundException object
            \param _cause the cause of the error
          */
          CopyProxyException(const std::string& _cause) throw() : cause(_cause) {};
          virtual ~CopyProxyException() throw() {}
          //! Gets the cause of the error
          const char* what() const throw() { return cause.c_str(); }
          
        };
      }
    }
  }
}

#endif
