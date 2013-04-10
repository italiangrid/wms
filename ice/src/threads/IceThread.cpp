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
#include "IceThread.h"
#include "utils/IceUtils.h"
#include "glite/ce/cream-client-api-c/creamApiLogger.h"

#include <boost/lexical_cast.hpp>

using namespace glite::wms::ice;
namespace apiLogger = glite::ce::cream_client_api::util;

threads::IceThread::IceThread( const std::string& name ) :
    m_name( name ),
    m_running( false ),
    m_stopped( false )
{
  m_thread_id = boost::lexical_cast<std::string>( (long long)this );//os.str();
}

threads::IceThread::IceThread( ) :
    m_name( ),
    m_running( false ),
    m_stopped( false )
{
  m_thread_id = boost::lexical_cast<std::string>( (long long) this ) ;
}

void threads::IceThread::operator()()
{
    m_running = true;

    body( );

    m_running = false;
}

void threads::IceThread::stop() 
{
  m_stopped = true;
  CREAM_SAFE_LOG(apiLogger::creamApiLogger::instance()->getLogger()->debugStream()
		 << "IceThread::stop() - Thread [" << getName() << "] Called STOP."
		 );
}
