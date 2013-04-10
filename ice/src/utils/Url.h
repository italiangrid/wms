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

#ifndef ICEUTILURL_H
#define ICEUTILURL_H

#include <string>


namespace glite {
  namespace wms {
    namespace ice {
      namespace util {
      
        class Url {
	
	  std::string    m_protocol;
	  std::string    m_hostname;
	  int            m_port;
	  std::string    m_path;
	  std::string    m_url;
	  bool           m_valid;
	  std::string    m_error;
	  std::string    m_endpoint;
	  
	 public:
	  Url( const std::string& _url );
	  bool is_valid( ) const { return m_valid; }
	  std::string get_error( ) const { return m_error; }
	  std::string get_schema( ) const { return m_protocol; }
	  std::string get_hostname( ) const { return m_hostname; }
	  std::string get_path( ) const { return m_path; }
	  std::string get_endpoint( ) const { return m_endpoint; }
	  int         get_tcpport( ) const { return m_port; }
	  std::string get_url( ) const { return m_url; }
	
	};
 
      
      }
    }
  }
}

#endif
