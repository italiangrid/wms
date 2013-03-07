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

//
// This file is heavily based on org.glite.wms.jobsubmission/src/common/EventLogger.cpp
//
// See also:
// org.glite.lb.client-interface/build/producer.h
// org.glite.lb.client-interface/doc/C/latex/refman.pdf

#include "IceLBEvent.h"
#include "IceLBContext.h"
#include "boost/format.hpp"
#include "IceConfManager.h"
//#include "glite/wms/common/configuration/ICEConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"

using namespace glite::wms::ice::util;

//////////////////////////////////////////////////////////////////////////////
//
// IceLBEvent
//
//////////////////////////////////////////////////////////////////////////////
IceLBEvent::IceLBEvent( const CreamJob& j, edg_wll_Source src, const std::string& dsc ) :
    m_job( j ),
    m_src( src ),
    m_description( dsc )
{

}

//////////////////////////////////////////////////////////////////////////////
//
// Cream Transfer Start Event
//
//////////////////////////////////////////////////////////////////////////////
cream_transfer_start_event::cream_transfer_start_event( const CreamJob& j ) :
    IceLBEvent( j, EDG_WLL_SOURCE_LOG_MONITOR, "Cream Transfer Start Event" )
{

}

int cream_transfer_start_event::execute( IceLBContext* ctx )
{

if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
	return edg_wll_LogTransferSTARTProxy( *(ctx->el_context), 
                                          EDG_WLL_SOURCE_LRMS, 
                                          m_job.cream_address( ).c_str(),
                                          ctx->el_s_unavailable,
                                          m_job.jdl().c_str(),  
                                          ctx->el_s_unavailable,
                                          ctx->el_s_unavailable );
else
    return edg_wll_LogTransferSTART( *(ctx->el_context), 
                                     EDG_WLL_SOURCE_LRMS, 
                                     m_job.cream_address( ).c_str(),
                                     ctx->el_s_unavailable,
                                     m_job.jdl().c_str(),  
                                     ctx->el_s_unavailable,
                                     ctx->el_s_unavailable );

}

//////////////////////////////////////////////////////////////////////////////
//
// Cream Transfer OK event
//
//////////////////////////////////////////////////////////////////////////////
cream_transfer_ok_event::cream_transfer_ok_event( const CreamJob& j ) :
    IceLBEvent( j, EDG_WLL_SOURCE_LOG_MONITOR, "Cream Transfer OK Event" )
{

}

int cream_transfer_ok_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogTransferOKProxy( *(ctx->el_context), 
                                       EDG_WLL_SOURCE_LRMS, 
                                       m_job.cream_address( ).c_str(),
                                       ctx->el_s_unavailable,
                                       m_job.jdl().c_str(),  
                                       ctx->el_s_unavailable,
                                       m_job.complete_cream_jobid().c_str() );    
else
    return edg_wll_LogTransferOK( *(ctx->el_context), 
                                  EDG_WLL_SOURCE_LRMS, 
                                  m_job.cream_address( ).c_str(),
                                  ctx->el_s_unavailable,
                                  m_job.jdl().c_str(),  
                                  ctx->el_s_unavailable,
                                  m_job.complete_cream_jobid().c_str() );    
}

//////////////////////////////////////////////////////////////////////////////
//
// CREAM transfer FAIL event
//
//////////////////////////////////////////////////////////////////////////////
cream_transfer_fail_event::cream_transfer_fail_event( const CreamJob& j, const std::string& reason ) :
    IceLBEvent( j, EDG_WLL_SOURCE_LOG_MONITOR, boost::str( boost::format( "Cream Transfer Fail Event, reason=[%1%]") % reason ) ),
    m_reason( reason )
{

}

int cream_transfer_fail_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogTransferFAILProxy( *(ctx->el_context), 
                                         EDG_WLL_SOURCE_LRMS, 
                                         m_job.cream_address( ).c_str(),
                                         ctx->el_s_unavailable,
                                         m_job.jdl().c_str(),  
                                         m_reason.c_str(), 
                                         ctx->el_s_unavailable );
else
    return edg_wll_LogTransferFAIL( *(ctx->el_context), 
                                    EDG_WLL_SOURCE_LRMS, 
                                    m_job.cream_address( ).c_str(),
                                    ctx->el_s_unavailable,
                                    m_job.jdl().c_str(),  
                                    m_reason.c_str(), 
                                    ctx->el_s_unavailable );
}


//////////////////////////////////////////////////////////////////////////////
//
// cream refused event
//
//////////////////////////////////////////////////////////////////////////////
cream_refused_event::cream_refused_event( const CreamJob& j, const std::string& reason ) :
    IceLBEvent( j, EDG_WLL_SOURCE_LOG_MONITOR, boost::str( boost::format( "Cream Refused Event, reason=[%1%]") % reason ) ),
    m_reason( reason )
{

}

int cream_refused_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogRefusedProxy( *(ctx->el_context), 
                                    EDG_WLL_SOURCE_JOB_SUBMISSION,
                                    ctx->el_s_unavailable,
                                    ctx->el_s_unavailable,
                                    m_reason.c_str() );
else
    return edg_wll_LogRefused( *(ctx->el_context), 
                               EDG_WLL_SOURCE_JOB_SUBMISSION,
                               ctx->el_s_unavailable,
                               ctx->el_s_unavailable,
                               m_reason.c_str() );
}

//////////////////////////////////////////////////////////////////////////////
//
// cream cancel request event
//
//////////////////////////////////////////////////////////////////////////////
cream_cancel_request_event::cream_cancel_request_event( const CreamJob& j, const std::string& reason ) :
    IceLBEvent( j, EDG_WLL_SOURCE_JOB_SUBMISSION, boost::str( boost::format( "Cream Cancel Request Event, reason=[%1%]" ) % reason ) ),
    m_reason( reason )
{

}

int cream_cancel_request_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogCancelREQProxy( *(ctx->el_context), 
                                      m_reason.c_str()
                                      );
else
    return edg_wll_LogCancelREQ( *(ctx->el_context), 
                                 m_reason.c_str()
                                 );
}



//////////////////////////////////////////////////////////////////////////////
//
// cancel refuse event
//
//////////////////////////////////////////////////////////////////////////////
cream_cancel_refuse_event::cream_cancel_refuse_event( const CreamJob& j, const std::string& reason ) :
    IceLBEvent( j, EDG_WLL_SOURCE_JOB_SUBMISSION, boost::str( boost::format("Cream Cancel Refuse Event, reason=[%1%]" ) % reason ) ),
    m_reason( reason )
{

}

int cream_cancel_refuse_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogCancelREFUSEProxy( *(ctx->el_context), 
                                         m_reason.c_str()
                                         );
else
    return edg_wll_LogCancelREFUSE( *(ctx->el_context), 
                                    m_reason.c_str()
                                    );
}


//////////////////////////////////////////////////////////////////////////////
//
// job running event
//
//////////////////////////////////////////////////////////////////////////////
job_running_event::job_running_event( const CreamJob& j ) :
    IceLBEvent( j, EDG_WLL_SOURCE_LOG_MONITOR, boost::str( boost::format("Job Running Event, worker_node=[%1%]" ) % j.worker_node() ) )
{

}

int job_running_event::execute( IceLBContext* ctx )
{
    std::string worker_node( m_job.worker_node() );
    if ( worker_node.empty() ) {
        worker_node = "N/A"; // LB requires a nonempty worker_node to
                             // log the Running event.
    }
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogRunningProxy( *(ctx->el_context), m_job.worker_node().c_str() );
else
    return edg_wll_LogRunning( *(ctx->el_context), m_job.worker_node().c_str() );
}

//////////////////////////////////////////////////////////////////////////////
//
// job really running event
//
//////////////////////////////////////////////////////////////////////////////
job_really_running_event::job_really_running_event( const CreamJob& j ) :
    IceLBEvent( j, EDG_WLL_SOURCE_LOG_MONITOR, "Job Really Running Event" )
{

}

int job_really_running_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogReallyRunningProxy( *(ctx->el_context), m_job.wn_sequence_code().c_str() );
else
    return edg_wll_LogReallyRunning( *(ctx->el_context), m_job.wn_sequence_code().c_str() );
}


//////////////////////////////////////////////////////////////////////////////
//
// job cancelled event
//
//////////////////////////////////////////////////////////////////////////////
job_cancelled_event::job_cancelled_event( const CreamJob& j ) :
    IceLBEvent( j, EDG_WLL_SOURCE_LOG_MONITOR, boost::str( boost::format( "Job Cancelled Event, reason=[%1%]" ) % j.failure_reason() ) )
{

}

int job_cancelled_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy()) {

    edg_wll_LogCancelDONEProxy( *(ctx->el_context), 
                                m_job.failure_reason().c_str() );

    return edg_wll_LogDoneCANCELLEDProxy( *(ctx->el_context), 
                                          m_job.failure_reason().c_str(),
                                          m_job.exit_code() );
} else {

    edg_wll_LogCancelDONE( *(ctx->el_context), 
                           m_job.failure_reason().c_str() );

    return edg_wll_LogDoneCANCELLED( *(ctx->el_context), 
                                     m_job.failure_reason().c_str(),
                                     m_job.exit_code() );
}
}


//////////////////////////////////////////////////////////////////////////////
//
// job cancelled event
//
//////////////////////////////////////////////////////////////////////////////
job_aborted_event::job_aborted_event( const CreamJob& j ) :
    IceLBEvent( j, EDG_WLL_SOURCE_LOG_MONITOR, boost::str( boost::format( "Job Aborted Event, reason=[%1%]" ) % j.failure_reason() ) )
{

}

int job_aborted_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogAbortProxy( *(ctx->el_context), 
                                  m_job.failure_reason().c_str() );
else
    return edg_wll_LogAbort( *(ctx->el_context), 
                             m_job.failure_reason().c_str() );
}


//////////////////////////////////////////////////////////////////////////////
//
// job done ok event
//
//////////////////////////////////////////////////////////////////////////////
job_done_ok_event::job_done_ok_event( const CreamJob& j ) :
    IceLBEvent( j, EDG_WLL_SOURCE_LOG_MONITOR, boost::str( boost::format("Job Done Ok Event, ExitCode=[%1%]" ) % j.exit_code() ) )
{

}

int job_done_ok_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogDoneOKProxy( *(ctx->el_context), 
                                   ctx->el_s_succesfully,
                                   m_job.exit_code() );
else
    return edg_wll_LogDoneOK( *(ctx->el_context), 
                              ctx->el_s_succesfully,
                              m_job.exit_code() );
}

//////////////////////////////////////////////////////////////////////////////
// 
// job done failed event
//
//////////////////////////////////////////////////////////////////////////////
job_done_failed_event::job_done_failed_event( const CreamJob& j ) :
    IceLBEvent( j, EDG_WLL_SOURCE_LOG_MONITOR, boost::str( boost::format("Job Done Failed Event, ExitCode=[%1%], FailureReason=[%2%]") % j.exit_code() % j.failure_reason() ) )
{

}

int job_done_failed_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogDoneFAILEDProxy( *(ctx->el_context), 
                                       m_job.failure_reason().c_str(), 
                                       m_job.exit_code() );
else
    return edg_wll_LogDoneFAILED( *(ctx->el_context), 
                                  m_job.failure_reason().c_str(), 
                                  m_job.exit_code() );
}


//////////////////////////////////////////////////////////////////////////////
//
// ns enqueued start event
//
//////////////////////////////////////////////////////////////////////////////
ns_enqueued_start_event::ns_enqueued_start_event( const CreamJob& j, const std::string& qname ) :
    IceLBEvent( j, EDG_WLL_SOURCE_LOG_MONITOR, boost::str( boost::format( "NS Enqueued Start Event, qname=[%1%]" ) % qname ) ),
    m_qname( qname )
{

}

int ns_enqueued_start_event::execute( IceLBContext* ctx ) 
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogEnQueuedSTARTProxy( *(ctx->el_context), 
                                          m_qname.c_str(),
                                          m_job.complete_cream_jobid().c_str(),
                                          ctx->el_s_unavailable
                                          );
else
    return edg_wll_LogEnQueuedSTART( *(ctx->el_context), 
                                     m_qname.c_str(),
                                     m_job.complete_cream_jobid().c_str(),
                                     ctx->el_s_unavailable
                                     );
}


//////////////////////////////////////////////////////////////////////////////
//
// ns enqueued fail event
//
//////////////////////////////////////////////////////////////////////////////
ns_enqueued_fail_event::ns_enqueued_fail_event( const CreamJob& j, const std::string& qname, const std::string& reason ) :
    IceLBEvent( j, EDG_WLL_SOURCE_LOG_MONITOR, boost::str( boost::format("NS Enqueued Fail Event, queue=[%1%], reason=[%2%]") % qname % reason ) ),
    m_qname( qname ),
    m_reason( reason )
{

}

int ns_enqueued_fail_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogEnQueuedFAILProxy( *(ctx->el_context), 
                                         m_qname.c_str(),
                                         m_job.complete_cream_jobid().c_str(),
                                         m_reason.c_str()
                                         );
else
    return edg_wll_LogEnQueuedFAIL( *(ctx->el_context), 
                                    m_qname.c_str(),
                                    m_job.complete_cream_jobid().c_str(),
                                    m_reason.c_str()
                                    );
}

//////////////////////////////////////////////////////////////////////////////
//
// ns enqueued ok event
//
//////////////////////////////////////////////////////////////////////////////
ns_enqueued_ok_event::ns_enqueued_ok_event( const CreamJob& j, const std::string& qname ) :
    IceLBEvent( j, EDG_WLL_SOURCE_LOG_MONITOR, boost::str( boost::format( "NS Enqueued OK Event, qname=[%1%]" ) % qname ) ),
    m_qname( qname )
{

}

int ns_enqueued_ok_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogEnQueuedOKProxy( *(ctx->el_context), 
                                       m_qname.c_str(),
                                       m_job.complete_cream_jobid().c_str(),
                                       ctx->el_s_unavailable
                                       );
else
    return edg_wll_LogEnQueuedOK( *(ctx->el_context), 
                                  m_qname.c_str(),
                                  m_job.complete_cream_jobid().c_str(),
                                  ctx->el_s_unavailable
                                  );
}

//////////////////////////////////////////////////////////////////////////////
//
// ice resubmission event
//
//////////////////////////////////////////////////////////////////////////////
ice_resubmission_event::ice_resubmission_event( const CreamJob& j, const std::string& reason ) :
    IceLBEvent( j, EDG_WLL_SOURCE_LOG_MONITOR, boost::str( boost::format("ICE Resubmission Event, reason=[%1%]") % reason ) ),
    m_reason( reason )
{

}

int ice_resubmission_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogResubmissionWILLRESUBProxy( *(ctx->el_context), 
                                                  m_reason.c_str(),
                                                  ctx->el_s_unavailable
                                                  );
else
    return edg_wll_LogResubmissionWILLRESUB( *(ctx->el_context), 
                                             m_reason.c_str(),
                                             ctx->el_s_unavailable
                                             );
}


//////////////////////////////////////////////////////////////////////////////
//
// wms dequeued event
//
//////////////////////////////////////////////////////////////////////////////
wms_dequeued_event::wms_dequeued_event( const CreamJob& j, const std::string& qname ) :
    IceLBEvent( j, EDG_WLL_SOURCE_JOB_SUBMISSION, boost::str( boost::format( "WMS Dequeue Event, qname=[%1%]") % qname ) ),
    m_qname( qname )
{

}

int wms_dequeued_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogDeQueuedProxy( *(ctx->el_context), 
                                     m_qname.c_str(),
                                     m_job.grid_jobid().c_str()
                                     );
else
    return edg_wll_LogDeQueued( *(ctx->el_context), 
                                m_qname.c_str(),
                                m_job.grid_jobid().c_str()
                                );
}

//////////////////////////////////////////////////////////////////////////////
//
// job suspended event
//
//////////////////////////////////////////////////////////////////////////////
job_suspended_event::job_suspended_event( const CreamJob& j ) :
    IceLBEvent( j, EDG_WLL_SOURCE_JOB_SUBMISSION, "Job Suspended Event" ),
    m_reason( "Job suspended by the batch system" )
{

}

int job_suspended_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogSuspendProxy( *(ctx->el_context), 
                                    m_reason.c_str()
                                    );
else
    return edg_wll_LogSuspend( *(ctx->el_context), 
                               m_reason.c_str()
                               );
}

//////////////////////////////////////////////////////////////////////////////
//
// job resumed event
//
//////////////////////////////////////////////////////////////////////////////
job_resumed_event::job_resumed_event( const CreamJob& j ) :
    IceLBEvent( j, EDG_WLL_SOURCE_JOB_SUBMISSION, "Job Resumed Event" ),
    m_reason( "Job resumed by the batch system" )
{

}

int job_resumed_event::execute( IceLBContext* ctx )
{
if(IceConfManager::instance()->getConfiguration()->common()->lbproxy())
    return edg_wll_LogResumeProxy( *(ctx->el_context), 
                                   m_reason.c_str()
                                   );
else
    return edg_wll_LogResume( *(ctx->el_context), 
                              m_reason.c_str()
                              );
}
