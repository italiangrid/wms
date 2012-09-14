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
#include "normalStatusNotification.h"

// ICE stuff
#include "iceLBLogger.h"
#include "iceLBEventFactory.h"
#include "iceUtils.h"
#include "ice-core.h"
#include "iceConfManager.h"
#include "ice-core.h"
#include "DNProxyManager.h"

#include "iceDb/GetJobByCid.h"
#include "iceDb/UpdateJobByGid.h"
#include "iceDb/RemoveJobByGid.h"
#include "iceDb/Transaction.h"

// CREAM and WMS stuff
#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"

// other gLite stuff
#include "classad_distribution.h"

// boost includes
#include "boost/format.hpp"
#include "boost/algorithm/string.hpp"

// System includes
#include <iostream>
#include <string>

namespace api = glite::ce::cream_client_api;
using namespace glite::wms::ice::util;
using namespace std;

//////////////////////////////////////////////////////////////////////////////
//
// Helper class StatusChange

/**
 * This class represents status change notifications as sent by
 * CEMON to ICE. A Job status change notification is a classad
 * with several fields indicating che cream job id of the job
 * whose status changed, the new status, and the timestamp when
 * the change occurred.
 */
class StatusChange {
public:
    /**
     * Builds a StatusChange object from a classad.
     *
     * @param ad the string representing a classad to build
     * this object from.
     */
    StatusChange( const string& ad ) throw( ClassadSyntax_ex& );
    virtual ~StatusChange( ) { };

    /**
     * Returns the CREAM job ID for this notification
     */
    string get_complete_cream_job_id( void ) const { 

      string completeURL = m_cream_url;
      boost::replace_all( completeURL, 
			  iceConfManager::getInstance()->getConfiguration()->ice()->cream_url_postfix(), "" );

      completeURL += "/" + m_cream_job_id;

      //      return m_cream_job_id; 
      
      return completeURL;
      
    }; // FIXME: Take into account the CREAM URL also!!!

    /**
     * Returns the status for this notification
     */
    api::job_statuses::job_status  get_status( void ) const { return m_job_status; };

    /**
     * Returns the exit code
     */ 
    int get_exit_code( void ) const { return m_exit_code; };


    /**
     * Returns the worker node
     */
    const string& get_worker_node( void ) {return m_worker_node; };

    /**
     * Apply this status change notification to all relevant jobs in
     * cache.  This means that the status of all jobs referenced in
     * this notification (and possibly their exit codes) are changed
     * according to this notification.
     */
    void apply_to_job( CreamJob& j ) const;

    const string& get_failure_reason( void ) const { return m_failure_reason; };
    const string& get_description( void ) const { return m_description; };

protected:

    string m_cream_job_id;
    string m_cream_url;
    api::job_statuses::job_status m_job_status;
    bool m_has_exit_code;
    bool m_has_failure_reason;
    bool m_has_description;
    int m_exit_code;
    string m_failure_reason;
    string m_worker_node;
    string m_description;
};

//
//
//______________________________________________________________________________
StatusChange::StatusChange( const string& ad_string ) throw( ClassadSyntax_ex& ) :
    m_has_exit_code( false ),
    m_has_failure_reason( false ),
    m_has_description( false ),
    m_exit_code( 0 ) // default
{

  { // Classad-mutex protected region
    boost::recursive_mutex::scoped_lock M_classad( glite::wms::ice::util::CreamJob::s_classad_mutex );
    
    classad::ClassAdParser parser;
    classad::ClassAd *ad = parser.ParseClassAd( ad_string );	
    
    if (!ad)
      throw ClassadSyntax_ex( boost::str( boost::format("StatusChange::CTOR(): Got an error while parsing notification classad: %1%" ) % ad_string ) );
    
    boost::scoped_ptr< classad::ClassAd > classad_safe_ptr( ad );
    
    if ( !classad_safe_ptr->EvaluateAttrString( "CREAM_JOB_ID", m_cream_job_id ) )
      throw ClassadSyntax_ex( boost::str( boost::format( "StatusChange::CTOR(): CREAM_JOB_ID attribute not found, or is not a string, in classad: %1%") % ad_string ) );
    boost::trim_if( m_cream_job_id, boost::is_any_of("\"" ) );
    
    if ( !classad_safe_ptr->EvaluateAttrString( "CREAM_URL", m_cream_url ) )
      throw ClassadSyntax_ex( boost::str( boost::format( "StatusChange::CTOR(): CREAM_URL attribute not found, or is not a string, in classad: %1%") % ad_string ) );
    boost::trim_if( m_cream_url, boost::is_any_of("\"" ) );
    
    string job_status_str;
    if ( !classad_safe_ptr->EvaluateAttrString( "JOB_STATUS", job_status_str ) )
      throw ClassadSyntax_ex( boost::str( boost::format( "StatusChange::CTOR(): JOB_STATUS attribute not found, or is not a string, in classad: %1%") % ad_string ) );
    boost::trim_if( job_status_str, boost::is_any_of("\"" ) );
    m_job_status = api::job_statuses::getStatusNum( job_status_str );
    
    if ( classad_safe_ptr->EvaluateAttrInt( "EXIT_CODE", m_exit_code ) ) {
      m_has_exit_code = true;
    }
    
    if ( classad_safe_ptr->EvaluateAttrString( "FAILURE_REASON", m_failure_reason ) ) {
      m_has_failure_reason = true;
      boost::trim_if( m_failure_reason, boost::is_any_of("\"") );
    }
    
    if ( classad_safe_ptr->EvaluateAttrString( "DESCRIPTION", m_description ) ) {
      m_has_description = true;
      boost::trim_if( m_description, boost::is_any_of("\"") );
    }
    
    if ( classad_safe_ptr->EvaluateAttrString( "WORKER_NODE", m_worker_node ) ) {
      boost::trim_if( m_failure_reason, boost::is_any_of("\"") );
    }
  
    // "Pretty print" the classad
    string pp_classad;
    classad::ClassAdUnParser unparser;
    unparser.Unparse( pp_classad, classad_safe_ptr.get() );
    if (!getenv("NO_LISTENER_MESS")) {
      CREAM_SAFE_LOG(api::util::creamApiLogger::instance()->getLogger()->debugStream()
		     << "StatusChange::CTOR() - "
		     << "Parsed status change notification "
		     << pp_classad
		     );
    }
  } // end of Classad-mutex protected region
};

//
//
//______________________________________________________________________________
void StatusChange::apply_to_job( CreamJob& j ) const
{
    j.set_status( get_status() );
    j.set_worker_node( m_worker_node );
    if ( m_has_exit_code ) {
        j.set_exit_code( m_exit_code );
    }    
    // 
    // Discussion with Gi, 2008/02/11: The failure reason field should
    // be taken only for jobs in aborted/done_failed state.  If the
    // job is in cancelled state, the cancel reason is located into
    // the description field. The same is done in the
    // iceCommandStatusPoller
    //
    if ( get_status() == api::job_statuses::CANCELLED ) {
        j.set_failure_reason( m_description );
    } else {
        j.set_failure_reason( m_failure_reason );
    }

    {
      //      list<pair<string ,string> > params;
      //      params.push_back( make_pair("status", utilities::to_string((long int)get_status())));
      //      params.push_back( make_pair("worker_node", m_worker_node));
      //if( m_has_exit_code ) 
      //  params.push_back( make_pair("exit_code", utilities::to_string((long int)m_exit_code)));

      //      params.push_back( make_pair("failure_reason", j.get_failure_reason() ));

      //glite::wms::ice::db::UpdateJobByGid updater( j.get_grid_jobid(), params, "StatusChange::apply_to_job" );
      glite::wms::ice::db::UpdateJob updater( j, "StatusChange::apply_to_job" );
      glite::wms::ice::db::Transaction tnx(false, false);
      //tnx.begin_exclusive( );
      tnx.execute( &updater );
    }
}


//////////////////////////////////////////////////////////////////////////////
//
// normalStatusNotification class
normalStatusNotification::normalStatusNotification( const monitortypes__Event& ev, const string& cemondn ) : // FIXME can throw anything!!
    absStatusNotification( ),
    m_ev( ev ),
    m_cemondn( cemondn )
{
    log4cpp::Category *m_log_dev( api::util::creamApiLogger::instance()->getLogger() );
    static const char *method_name = "normalStatusNotification::CTOR() - ";
    
    if( m_ev.Message.empty() ) {        
        CREAM_SAFE_LOG( m_log_dev->debugStream()
                        << method_name
                        << "Got a CEMon notification with no messages. Skipping"
                        );
        throw runtime_error( "got empty notification" );
    }
    
    CREAM_SAFE_LOG( m_log_dev->debugStream()
                    << method_name
                    << "Processing normal status change notification"
                    );
    
    //string cream_job_id;
    
    // First, we need to get the jobID for which this notification 
    // refers. In order to do so, we need to parse at least the first
    // notification in the event.
    try {

        StatusChange first_notification( *(m_ev.Message.begin()) );
        m_complete_cream_jobid = first_notification.get_complete_cream_job_id();

    } catch( ClassadSyntax_ex& ex ) {
        CREAM_SAFE_LOG( m_log_dev->errorStream()
                        << method_name
                        << "Cannot parse the first notification "
                        << *(m_ev.Message.begin())
                        << " due to error: "
                        << ex.what() << ". "
                        << "Skipping the whole monitor event and hoping for the best..." 
                        );
        throw; // FIXME!!!
    }

}

//
//
//______________________________________________________________________________
void normalStatusNotification::apply( void ) // can throw anything
{    
    log4cpp::Category *m_log_dev( api::util::creamApiLogger::instance()->getLogger() );
    static const char* method_name = "normalStatusNotification::apply() - ";

    // the costructor ensures that the notification is non-empty
    CREAM_SAFE_LOG( m_log_dev->debugStream()
                    << method_name
                    << "Processing notification"
                    );

    iceLBLogger *m_lb_logger( iceLBLogger::instance() );
    glite::wms::ice::Ice* m_ice_manager( glite::wms::ice::Ice::instance() );


    // First, we need to get the jobID for which this notification 
    // refers. In order to do so, we need to parse at least the first
    // notification in the event.


    // Now that we (hopefully) have the jobid, we lock the cache
    // and find the job
    
    //    boost::recursive_mutex::scoped_lock jc_M( CreamJob::globalICEMutex );

    CreamJob theJob;
    {
      db::GetJobByCid getter( m_complete_cream_jobid, "normalStatusNotification::apply" );
      db::Transaction tnx(false, false);
      //tnx.begin( );
      tnx.execute( &getter );
      if( !getter.found() )
	{
	  if(!getenv("NO_LISTENER_MESS"))
	    CREAM_SAFE_LOG(m_log_dev->warnStream()
                           << method_name
                           << "creamjobid ["
                           << m_complete_cream_jobid
                           << "] was not found in the database. "
                           << "Ignoring the whole notification..."
                           );
	  return;
	} else {
	theJob = getter.get_job();
      }
    }

    // No job found in cache. This is fine, we may be receiving "old"
    // notifications, for jobs which have been already purged.
/*
    string _cemon_url;
    string proxy = DNProxyManager::getInstance()->getAnyBetterProxyByDN( theJob.get_user_dn() ).get<0>();
    subscriptionManager::getInstance()->getCEMonURL(proxy, theJob.get_cream_address(), _cemon_url);
    string _cemon_dn;
    subscriptionManager::getInstance()->getCEMonDN( theJob.get_user_dn(), _cemon_url, _cemon_dn );

    if( m_cemondn.compare( _cemon_dn ) ) {
	CREAM_SAFE_LOG(m_log_dev->warnStream()
		       << method_name
		       << "the CEMon ["
		       << m_cemondn << "] that sent this notification "
		       << "apparently didn't receive the submission of current job "
		       << theJob.describe()
		       << ". Ignoring the whole notification..."
		       );
	return;
    }
*/
    //
    // Now, for each status change notification, check if it has to be logged
    // vector<StatusNotification>::const_iterator it;
    int count;
    vector<string>::const_iterator msg_it;
    for ( msg_it = m_ev.Message.begin(), count = 1;
          msg_it != m_ev.Message.end(); 
          ++msg_it, ++count ) {

        // setLastSeen must be called ONLY if the job IS NOT in a
        // TERMINAL state (that means that more states are coming...),
        // like DONE-OK; otherwise the eventStatusPoller will never
        // purge it...
        if( !api::job_statuses::isFinished( theJob.get_status() ) ) {
            theJob.set_last_seen( time(0) );
            theJob.set_last_empty_notification_time( time(0) );
	    
	    {
	      //list< pair<string, string> > params;
	      //params.push_back( make_pair("last_seen", utilities::to_string(time(0))));
	      //params.push_back( make_pair("last_empty_notification", utilities::to_string(time(0))));
	      
	      //db::UpdateJobByGid updater( theJob.get_grid_jobid(), params, "normalStatusNotification::apply");
	      db::UpdateJob( theJob, "normalStatusNotification::apply");
	      db::Transaction tnx(false, false);
	      //tnx.begin_exclusive( );
	      tnx.execute( &updater );
	    }
	    
        }
        
        if ( count <= theJob.get_num_logged_status_changes() ) {
            if (!getenv("NO_LISTENER_MESS")) {
                CREAM_SAFE_LOG(m_log_dev->debugStream()
                               << method_name
                               << "Skipping current notification because contains old states"
                               );
            }
            continue; // skip to the next job
        }

        boost::scoped_ptr< StatusChange > notif_ptr;
        try {
            StatusChange* n = new StatusChange( *msg_it );
            notif_ptr.reset( n );
        } catch( ClassadSyntax_ex ex ) {
            if (!getenv("NO_LISTENER_MESS"))
                CREAM_SAFE_LOG(m_log_dev->errorStream()
                               << method_name
                               << "Received a notification "
                               << *msg_it << " which could not be understood; error is: "
                               << ex.what() << ". "
                               << "Skipping this notification and hoping for the best..."
                               );
            continue;
        }

        // If the status is "PURGED", remove the job from cache        
        if( notif_ptr->get_status() == api::job_statuses::PURGED ) {
            if (!getenv("NO_LISTENER_MESS"))
                CREAM_SAFE_LOG(m_log_dev->warnStream()
                               << method_name
                               << theJob.describe()
                               << " is reported as PURGED. Removing from database"
                               ); 
	    
	    {
	      DNProxyManager::getInstance()->decrementUserProxyCounter( theJob.get_user_dn(), theJob.get_myproxy_address() );
	      db::RemoveJobByGid remover( theJob.get_grid_jobid(), "normalStatusNotification::apply" );
	      db::Transaction tnx(false, false);
	      tnx.execute( &remover );
	    }
            return;
        }
        
        if (!getenv("NO_LISTENER_MESS"))
            CREAM_SAFE_LOG(m_log_dev->debugStream() 
                           << method_name
                           << "Checking job [" << notif_ptr->get_complete_cream_job_id()
                           << "] with status [" 
                           << api::job_statuses::job_status_str[ notif_ptr->get_status() ] << "]"
                           << " This is CEMON notification's count=" << count
                           << " num already logged=" 
                           << theJob.get_num_logged_status_changes()
                           );
        
	notif_ptr->apply_to_job( theJob );// this method internally updates the database

	theJob.set_num_logged_status_changes( count );
	{
	  //list< pair<string, string> > params;
	  //params.push_back( make_pair("num_logged_status_changes", utilities::to_string((long int)count)));
	  
	  //db::UpdateJobByGid updater( theJob.get_grid_jobid(), params, "normalStatusNotification::apply" );
	  db::UpdateJob updater( theJob, "normalStatusNotification::apply" );
	  db::Transaction tnx(false, false);
	  //tnx.begin_exclusive( );
	  tnx.execute( &updater );
	}
        iceLBEvent* ev = iceLBEventFactory::mkEvent( theJob /*tmp_job*/ );
        if ( ev ) {
	  //tmp_job = m_lb_logger->logEvent( ev );
	  theJob = m_lb_logger->logEvent( ev );
        }
	
	// For NOW we can trust on logEvent that put the entire job into the database
	// but a more elegant solution must be found later.

	  m_ice_manager->resubmit_or_purge_job( &theJob );
	  
    }
}
