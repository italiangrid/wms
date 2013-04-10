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
/**
 * This is a wrapper of the wms's configurator. The motivation of this wrapper
 * is that apparently wms configurator doesn't allow to modify conf params at run time
 */

#ifndef GLITE_WMS_ICE_UTIL_ICECONFMANAGER_H
#define GLITE_WMS_ICE_UTIL_ICECONFMANAGER_H

#include <string>
#include <ctime>
#include "ConfigurationManager_ex.h"
#include <boost/scoped_ptr.hpp>
#include "glite/wms/common/configuration/Configuration.h"

// #include "boost/thread/recursive_mutex.hpp"

namespace glite {
  namespace wms {
      namespace common {
          namespace configuration {
              class Configuration;
          }
      }

    namespace ice {
      namespace util {

	class IceConfManager {
	  
	  static IceConfManager* s_instance;
	  static std::string s_conf_file;
	  static bool s_initialized;
          
          boost::scoped_ptr< glite::wms::common::configuration::Configuration > m_configuration;

	protected:
	  IceConfManager()
	    throw (glite::wms::ice::util::ConfigurationManager_ex&);
	  
	public:
	  
	  virtual ~IceConfManager();
          
	  static IceConfManager* instance() 
	    throw (glite::wms::ice::util::ConfigurationManager_ex&);
	  static void init(const std::string&);

          glite::wms::common::configuration::Configuration* getConfiguration() { return m_configuration.get(); }
	};
      }
    }
  }
}

#endif
