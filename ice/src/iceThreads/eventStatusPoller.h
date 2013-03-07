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
#ifndef GLITE_WMS_ICE_UTIL_EVENTSTATUSPOLLER_H
#define GLITE_WMS_ICE_UTIL_EVENTSTATUSPOLLER_H

#undef soapStub_H
#include "iceThread.h"
#include "iceCommands/iceCommandStatusPoller.h"

namespace log4cpp {
    class Category;
};

namespace glite {
namespace wms {
namespace ice {
    
    class IceCore;

    namespace util {
        
        class iceThreadPool;

	//! A job status poller
	/*! \class eventStatusPoller 
	  This class is conceived to run as a boost::thread (this is the
	  motivation of the implementation of the operator()() ).
	  Its main purpose is to get all status of all jobs ICE has submitted (and that are not finished yet) and whose status notification has not been received since long by the eventStatusListener.
	  When a job is finished the poller purges that job on the remote cream host.
	  The poller also resubmit failed or aborted job by calling the call back ice::doOnJobFailure
	  \sa ice
	*/
	class eventStatusPoller : public iceThread {

	  int m_delay;
	  IceCore* m_iceManager;
	  log4cpp::Category* m_log_dev;
          iceThreadPool* m_threadPool;

          /**
           * Prevents copying
           */
	  eventStatusPoller( const eventStatusPoller& )  { };

	public:

	  //static boost::recursive_mutex s_proxymutex;

	  //! eventStatusPoller constructor
	  /*!
	    Creates a eventStatusPoller object
	    \param iceManager is the ICE main object (see the ice class) that creates the poller thread. Ownership of this pointer is not transferred
	    \param D is the delay (default 10 seconds) between two polls
	    \throw eventStatusPoller_ex& if the creation of the internal cream communication client failed
	    \sa ice
	  */
	  eventStatusPoller( IceCore* iceManager, int d=10 );
	  
	  virtual ~eventStatusPoller();

	  //! Main poller loop (inherited from iceThread)
          virtual void body( void );

	  virtual void stop( void ) { 
	    iceThread::stop();
	//    m_real_poller.stop();
	  }
 
	};

      }
    }
  }
}

#endif
