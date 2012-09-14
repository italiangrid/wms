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

#include "Url.h"
#include <cstdlib>

using namespace std;

glite::wms::ice::util::Url::Url( const string& url ) 
 : m_protocol(""), m_hostname(""), m_port(0), m_path(""), m_url(url), m_valid( false ), m_error(""), m_endpoint("")
{
  string::size_type protosep = m_url.find( "://", 0 );

  if( protosep == string::npos ) {
    m_path = m_url;
    m_valid = true;
    return;
  }
  
  /**
   * if we're here the proto IS NOT empty
   */
  m_protocol = m_url.substr( 0, protosep );

  if(m_protocol == "file" ) {
    m_path = m_url.substr( protosep+3, m_url.length()-3-protosep );
    m_valid = true;
    return;
  }
  
  string _address = m_url.substr(protosep+3, m_url.length()-protosep-3);
  
  string::size_type slashpos = _address.find( "/", 0 );
  if( slashpos != string::npos ) {
    m_path = _address.substr( slashpos+1, _address.length() - slashpos );
    m_endpoint = _address.substr( 0, slashpos );
  } else {
    m_endpoint = _address;
  }


  string::size_type tcpport_pos = m_endpoint.find( ":", 0 );
  string restport;
  
  if( tcpport_pos != string::npos ) {
    restport = m_endpoint.substr( tcpport_pos+1, m_endpoint.length() - 1 - tcpport_pos );
    m_hostname = m_endpoint.substr( 0, tcpport_pos );
    
    if( !restport.empty() ) {
      m_port = ::atoi( restport.c_str() );
      
      if (m_port <=0) {
        m_error = "Specified an zero or negative TCP port in the address [";
        m_error += m_url + "]. It must be in the range [1-65535]";     
        return;
      }
      
    } else {
        m_error = "Specified an empty TCP port in the address [";
        m_error += m_url + "]. Format must be schema://hostname[:tcpport] and ";
        m_error += "tcpport must be full numeric in the range [2000-65535]";     
        return;
    }
  }

  m_valid = true;
}
