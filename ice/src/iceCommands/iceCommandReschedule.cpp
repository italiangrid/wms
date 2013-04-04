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

#include "iceCommandReschedule.h"

#include "iceDb/RemoveJobByGid.h"
#include "iceDb/Transaction.h"

#include "iceUtils/DNProxyManager.h"

using namespace glite::wms::ice;

//
//
//____________________________________________________________________________
void iceCommandReschedule::execute( const std::string& tid ) 
  throw( iceCommandFatal_ex&, iceCommandTransient_ex& )
{
  m_thread_id = tid;
  {
  boost::recursive_mutex::scoped_lock M_reschedule( glite::wms::ice::util::CreamJob::s_reschedule_mutex );
  CREAM_SAFE_LOG(
                   m_log_dev->infoStream()
                   << "iceCommandReschedule::execute - TID=[" << getThreadID() << "] "
                   << "This request is a Reschedule for GridJobID ["
		   << m_theJob.grid_jobid( ) << "]. Checking token file ["
		   << m_theJob.token_file( ) << "]"
                   );  

  if( !boost::filesystem::exists( boost::filesystem::path( m_theJob.token_file( ), boost::filesystem::native ) ) ) {
  
    CREAM_SAFE_LOG(
                   m_log_dev->warnStream()
                   << "iceCommandReschedule::execute - TID=[" << getThreadID() << "] "
                   << "Missing token file ["
		   << m_theJob.token_file( ) << "]"
		   << " for GridJobID ["
		   << m_theJob.grid_jobid( ) << "]. Dropping the request."
                   );
  
    return;
    
  }
    
  {
    boost::recursive_mutex::scoped_lock M_reschedule( glite::wms::ice::util::CreamJob::s_reschedule_mutex );
    CREAM_SAFE_LOG(
                   m_log_dev->debugStream()
                   << "iceCommandReschedule::execute -  TID=[" << getThreadID() << "] "
                   << "Ok, token file is there . Removing job from ICE's database and submitting job ["
		   << m_theJob.grid_jobid( ) << "]"
                   );
    db::RemoveJobByGid remover( m_theJob.grid_jobid(), "iceCommandReschedule::execute" );
    db::Transaction tnx( false, false );
    tnx.execute( &remover );
  }
  if( m_theJob.proxy_renewable( ) )
    DNProxyManager::getInstance()->decrementUserProxyCounter( m_theJob.user_dn(), m_theJob.myproxy_address() );
  
  }
  IceCommandSubmit::execute( tid );

}
