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
#ifndef GLITE_WMS_ICE_UTIL_EVENTSTATUSLISTENER_H
#define GLITE_WMS_ICE_UTIL_EVENTSTATUSLISTENER_H

#include "glite/ce/monitor-client-api-c/CEConsumer.h"
#include "iceThread.h"

namespace cemon_api = glite::ce::monitor_client_api::soap_proxy;

// Forward declaration for the logger
namespace log4cpp {
    class Category;
};

namespace glite {
  namespace wms {
    namespace common {
      namespace configuration {
        class ICEConfiguration;
      }
    }
  }
}

namespace glite {
  namespace wms {
    namespace ice {
        class Ice;

      namespace util {
          
          class IceConfManager;

	//! A class that receives notification from CEMon about job status changes
	/**!
	   \class eventStatusListener
	   This class is conceived to run as a boost::thread (this is the
	   motivation of the implementation of the operator()() ).
	   Its main purpose is to receive notifications from CEMon about all job status chages; the CEMon sending notifications runs on the same host of the CREAM service. In order to receive notifications, the listener must be subscribed to that CEMon service.

	*/
	class eventStatusListener : public cemon_api::CEConsumer, public iceThread {
	  std::string                                            m_myname;
	  const glite::wms::common::configuration::ICEConfiguration*  m_conf;
          log4cpp::Category                                     *m_log_dev;
	  bool                                                   m_isOK;
          glite::wms::ice::Ice*                                  m_ice_manager;

	protected:

	  eventStatusListener(const eventStatusListener&) : 
              CEConsumer(9999)
              {}

	  void createObject();

	public:

	  eventStatusListener(const int& i);
	  
	  eventStatusListener(const int& i, 
			      const std::string& cert,
			      const std::string& key);
			      
	  virtual ~eventStatusListener() {}

	  void acceptJobStatus(void);
	  virtual void body( void );
	  bool isOK() const { return m_isOK; }
	};

      }
    }
  }
}

#endif
