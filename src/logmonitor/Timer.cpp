/* Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
#include <cstdio>
#include <ctime>
#include <cstring>

#include <sys/resource.h>

#include <algorithm>
#include <string>
#include <list>

#include <boost/lexical_cast.hpp>

#include <condor/user_log.c++.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "jobcontrol_namespace.h"
#include "Timer.h"

using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor {

namespace {

class NullString {
public:
  NullString( const char *c );
  NullString( const std::string &s );
  ~NullString( void );

  inline operator const string &( void ) const { return this->ns_string; }
  inline operator string &( void ) { return this->ns_string; }

private:
  string   ns_string;
};

NullString::NullString( const char *c ) : ns_string( c ? c : "" ) {}

NullString::NullString( const string &s ) : ns_string( s ) {}

NullString::~NullString( void ) {}

char *myStrdup( const string &s )
{
  char   *pc = new char[ s.length() + 1 ];

  strcpy( pc, s.c_str() );

  return pc;
}

} // Anonymous namespace

const char   *TimeoutEvent::te_s_Timeout = "Timeout", *TimeoutEvent::te_s_EventNumber = "EventNumber";
const char   *TimeoutEvent::te_s_EventTime = "EventTime", *TimeoutEvent::te_s_Cluster = "Cluster";
const char   *TimeoutEvent::te_s_Proc = "Proc", *TimeoutEvent::te_s_SubProc = "SubProc";
const char   *TimeoutEvent::te_s_LogNotes = "LogNotes";
const char   *TimeoutEvent::te_s_ExecuteHost = "ExecuteHost", *TimeoutEvent::te_s_Node = "Node";
const char   *TimeoutEvent::te_s_ExecErrorType = "ExecErrorType";
const char   *TimeoutEvent::te_s_RunLocalRusage = "RunLocalRusage", *TimeoutEvent::te_s_RunRemoteRusage = "RunRemoteRusage";
const char   *TimeoutEvent::te_s_TotalLocalRusage = "TotalLocalRusage", *TimeoutEvent::te_s_TotalRemoteRusage = "TotalRemoteRusage";
const char   *TimeoutEvent::te_s_SentBytes = "SentBytes", *TimeoutEvent::te_s_RecvdBytes = "RecvdBytes";
const char   *TimeoutEvent::te_s_Terminate = "Terminate", *TimeoutEvent::te_s_Normal = "Normal";
const char   *TimeoutEvent::te_s_ReturnValue = "ReturnValue", *TimeoutEvent::te_s_SignalNumber = "SignalNumber";
const char   *TimeoutEvent::te_s_Reason = "Reason", *TimeoutEvent::te_s_CoreFile = "CoreFile";
const char   *TimeoutEvent::te_s_TotalSentBytes = "TotalSentBytes", *TimeoutEvent::te_s_TotalRecvdBytes = "TotalRecvdBytes";
const char   *TimeoutEvent::te_s_Size = "Size", *TimeoutEvent::te_s_NumPids = "NumPids";
const char   *TimeoutEvent::te_s_Message = "Message", *TimeoutEvent::te_s_Info = "Info";
const char   *TimeoutEvent::te_s_RmContact = "RmContact", *TimeoutEvent::te_s_JmContact = "JmContact";
const char   *TimeoutEvent::te_s_RestartableJM = "RestartableJM", *TimeoutEvent::te_s_CheckPointed = "CheckPointed";
  const char   *TimeoutEvent::te_s_DaemonName = "DaemonName", *TimeoutEvent::te_s_ErrorStr = "ErrorText"; 
  const char   *TimeoutEvent::te_s_CriticalError = "CriticalError";
  const char   *TimeoutEvent::te_s_ReasonCode = "ReasonCode", *TimeoutEvent::te_s_ReasonSubCode = "ReasonSubCode";
  const char   *TimeoutEvent::te_s_UserNotes = "UserNotes";
  const char   *TimeoutEvent::te_s_StartdAddr = "StartdAddr", *TimeoutEvent::te_s_StartdName = "StartdName";
  const char   *TimeoutEvent::te_s_StarterAddr = "StartesAddr", *TimeoutEvent::te_s_DisconnReason = "DisconnReason";
  const char   *TimeoutEvent::te_s_NoReconnReason = "NoReconnReason", *TimeoutEvent::te_s_CanReconn = "CanReconn";
  const char   *TimeoutEvent::te_s_ResourceName= "GridResource", *TimeoutEvent::te_s_JobId = "GridJobId";

namespace {

typedef  struct rusage   Rusage;

classad::ClassAd *toClassAd( rusage &ru )
{
  classad::ClassAd    *answer = new classad::ClassAd();

  answer->InsertAttr( "ru_maxrss", boost::lexical_cast<string>(ru.ru_maxrss) );
  answer->InsertAttr( "ru_ixrss", boost::lexical_cast<string>(ru.ru_ixrss) );
  answer->InsertAttr( "ru_idrss", boost::lexical_cast<string>(ru.ru_idrss) );
  answer->InsertAttr( "ru_isrss", boost::lexical_cast<string>(ru.ru_isrss) );
  answer->InsertAttr( "ru_minflt", boost::lexical_cast<string>(ru.ru_minflt) );
  answer->InsertAttr( "ru_majflt", boost::lexical_cast<string>(ru.ru_majflt) );
  answer->InsertAttr( "ru_nswap", boost::lexical_cast<string>(ru.ru_nswap) );
  answer->InsertAttr( "ru_inblock", boost::lexical_cast<string>(ru.ru_inblock) );
  answer->InsertAttr( "ru_oublock", boost::lexical_cast<string>(ru.ru_oublock) );
  answer->InsertAttr( "ru_msgsnd", boost::lexical_cast<string>(ru.ru_msgsnd) );
  answer->InsertAttr( "ru_msgrcv", boost::lexical_cast<string>(ru.ru_msgrcv) );
  answer->InsertAttr( "ru_nsignals", boost::lexical_cast<string>(ru.ru_nsignals) );
  answer->InsertAttr( "ru_nvcsw", boost::lexical_cast<string>(ru.ru_nvcsw) );
  answer->InsertAttr( "ru_nivcsw", boost::lexical_cast<string>(ru.ru_nivcsw) );
  answer->InsertAttr( "ru_utime", boost::lexical_cast<string>(ru.ru_utime.tv_sec) );
  answer->InsertAttr( "ru_stime", boost::lexical_cast<string>(ru.ru_stime.tv_sec) );

  return answer;
}

Rusage toRusage( const classad::ClassAd *ad )
{
  double    val;
  Rusage    ru;

  ad->EvaluateAttrNumber( "ru_maxrss", val );   ru.ru_maxrss = static_cast<long int>( val );
  ad->EvaluateAttrNumber( "ru_ixrss", val );    ru.ru_ixrss = static_cast<long int>( val );
  ad->EvaluateAttrNumber( "ru_idrss", val );    ru.ru_idrss = static_cast<long int>( val );
  ad->EvaluateAttrNumber( "ru_isrss", val );    ru.ru_isrss = static_cast<long int>( val );
  ad->EvaluateAttrNumber( "ru_minflt", val );   ru.ru_minflt = static_cast<long int>( val );
  ad->EvaluateAttrNumber( "ru_majflt", val );   ru.ru_majflt = static_cast<long int>( val );
  ad->EvaluateAttrNumber( "ru_nswap", val );    ru.ru_nswap = static_cast<long int>( val );
  ad->EvaluateAttrNumber( "ru_inblock", val );  ru.ru_inblock = static_cast<long int>( val );
  ad->EvaluateAttrNumber( "ru_oublock", val );  ru.ru_oublock = static_cast<long int>( val );
  ad->EvaluateAttrNumber( "ru_msgsnd", val );   ru.ru_msgsnd = static_cast<long int>( val );
  ad->EvaluateAttrNumber( "ru_msgrcv", val );   ru.ru_msgrcv = static_cast<long int>( val );
  ad->EvaluateAttrNumber( "ru_nsignals", val ); ru.ru_nsignals = static_cast<long int>( val );
  ad->EvaluateAttrNumber( "ru_nvcsw", val );    ru.ru_nvcsw = static_cast<long int>( val );
  ad->EvaluateAttrNumber( "ru_nivcsw", val );   ru.ru_nivcsw = static_cast<long int>( val );
  ad->EvaluateAttrNumber( "ru_utime", val );    ru.ru_utime.tv_sec = static_cast<long int>( val ); ru.ru_utime.tv_usec = 0;
  ad->EvaluateAttrNumber( "ru_stime", val );    ru.ru_stime.tv_sec = static_cast<long int>( val ); ru.ru_stime.tv_usec = 0;

  return ru;
}

} // Anonymous namespace

TimeoutEvent::TimeoutEvent( time_t epoch, ULogEvent *event ) : te_epoch( epoch ), te_event( event ), te_classad(), te_pointer()
{}

TimeoutEvent::TimeoutEvent( const classad::ClassAd &ad ) : te_epoch( 0 ), te_event(), te_classad(static_cast<classad::ClassAd*>(ad.Copy())), te_pointer()
{}

TimeoutEvent::~TimeoutEvent( void ) {}

bool TimeoutEvent::expired( void )
{
  time_t    now = time( NULL );

  if( !this->te_event ) this->to_event();

  return( this->te_epoch <= now );
}

classad::ClassAd *TimeoutEvent::to_classad( void )
{
  time_t     epoch;

  if( !this->te_classad ) {
    epoch = mktime( &this->te_event->eventTime );

    this->te_classad.reset( new classad::ClassAd() );

    this->te_classad->InsertAttr( te_s_Timeout, boost::lexical_cast<string>(this->te_epoch) );
    this->te_classad->InsertAttr( te_s_EventNumber, static_cast<int>(this->te_event->eventNumber) );
    this->te_classad->InsertAttr( te_s_EventTime, boost::lexical_cast<string>(epoch) );
    this->te_classad->InsertAttr( te_s_Cluster, static_cast<int>(this->te_event->cluster) );
    this->te_classad->InsertAttr( te_s_Proc, static_cast<int>(this->te_event->proc) );
    this->te_classad->InsertAttr( te_s_SubProc, static_cast<int>(this->te_event->subproc) );

    switch( this->te_event->eventNumber ) {
    case ULOG_SUBMIT: {
      SubmitEvent    *sev = dynamic_cast<SubmitEvent *>( this->te_event.get() );

      if( sev->submitEventLogNotes ) {
	NullString    lnotes( sev->submitEventLogNotes );

	this->te_classad->InsertAttr( te_s_LogNotes, lnotes );
      }
      if( sev->submitEventUserNotes ) {
	NullString    unotes( sev->submitEventUserNotes );
	
	this->te_classad->InsertAttr( te_s_UserNotes, unotes );
      }
      break;
    }
    case ULOG_EXECUTE: {
      break;
    }
    case ULOG_EXECUTABLE_ERROR: {
      ExecutableErrorEvent   *eeev = dynamic_cast<ExecutableErrorEvent *>( this->te_event.get() );

      this->te_classad->InsertAttr( te_s_ExecErrorType, static_cast<int>(eeev->errType) );

      break;
    }
    case ULOG_CHECKPOINTED: {
      CheckpointedEvent   *chev = dynamic_cast<CheckpointedEvent *>( this->te_event.get() );

      this->te_classad->Insert( te_s_RunLocalRusage, toClassAd(chev->run_local_rusage) );
      this->te_classad->Insert( te_s_RunRemoteRusage, toClassAd(chev->run_remote_rusage) );

      break;
    }
    case ULOG_JOB_EVICTED: {
      JobEvictedEvent   *jev = dynamic_cast<JobEvictedEvent *>( this->te_event.get() );
      NullString         reason( jev->getReason() ), corefile( jev->getCoreFile() );

      this->te_classad->InsertAttr( te_s_CheckPointed, jev->checkpointed );
      this->te_classad->InsertAttr( te_s_SentBytes, jev->sent_bytes );
      this->te_classad->InsertAttr( te_s_RecvdBytes, jev->recvd_bytes );
      this->te_classad->InsertAttr( te_s_Terminate, jev->terminate_and_requeued );
      this->te_classad->InsertAttr( te_s_Normal, jev->normal );
      this->te_classad->InsertAttr( te_s_ReturnValue, jev->return_value );
      this->te_classad->InsertAttr( te_s_SignalNumber, jev->signal_number );
      this->te_classad->InsertAttr( te_s_Reason, reason );
      this->te_classad->InsertAttr( te_s_CoreFile, corefile );
      this->te_classad->Insert( te_s_RunLocalRusage, toClassAd(jev->run_local_rusage) );
      this->te_classad->Insert( te_s_RunRemoteRusage, toClassAd(jev->run_remote_rusage) );

      break;
    }
    case ULOG_JOB_TERMINATED: {
      JobTerminatedEvent   *te = dynamic_cast<JobTerminatedEvent *>( this->te_event.get() );
      NullString            corefile( te->getCoreFile() );

      this->te_classad->InsertAttr( te_s_Normal, te->normal );
      this->te_classad->InsertAttr( te_s_ReturnValue, te->returnValue );
      this->te_classad->InsertAttr( te_s_SignalNumber, te->signalNumber );
      this->te_classad->InsertAttr( te_s_SentBytes, te->sent_bytes );
      this->te_classad->InsertAttr( te_s_RecvdBytes, te->recvd_bytes );
      this->te_classad->InsertAttr( te_s_TotalSentBytes, te->total_sent_bytes );
      this->te_classad->InsertAttr( te_s_TotalRecvdBytes, te->total_recvd_bytes );
      this->te_classad->InsertAttr( te_s_CoreFile, corefile );
      this->te_classad->Insert( te_s_RunLocalRusage, toClassAd(te->run_local_rusage) );
      this->te_classad->Insert( te_s_RunRemoteRusage, toClassAd(te->run_remote_rusage) );
      this->te_classad->Insert( te_s_TotalLocalRusage, toClassAd(te->total_local_rusage) );
      this->te_classad->Insert( te_s_TotalRemoteRusage, toClassAd(te->total_remote_rusage) );

      break;
    }
    case ULOG_IMAGE_SIZE: {
      JobImageSizeEvent   *jisev = dynamic_cast<JobImageSizeEvent *>( this->te_event.get() );
      int size = jisev->image_size_kb;
      this->te_classad->InsertAttr(te_s_Size, size);

      break;
    }
    case ULOG_SHADOW_EXCEPTION: {
      ShadowExceptionEvent   *sev = dynamic_cast<ShadowExceptionEvent *>( this->te_event.get() );
      NullString              message( sev->message );

      this->te_classad->InsertAttr( te_s_Message, message );
      this->te_classad->InsertAttr( te_s_SentBytes, sev->sent_bytes );
      this->te_classad->InsertAttr( te_s_RecvdBytes, sev->recvd_bytes );

      break;
    }
    case ULOG_GENERIC: {
      GenericEvent  *gev = dynamic_cast<GenericEvent *>( this->te_event.get() );
      NullString     info( gev->info );

      this->te_classad->InsertAttr( te_s_Info, info );

      break;
    }
    case ULOG_JOB_ABORTED: {
      JobAbortedEvent  *jaev = dynamic_cast<JobAbortedEvent *>( this->te_event.get() );
      NullString        reason( jaev->getReason() );

      this->te_classad->InsertAttr( te_s_Reason, reason );

      break;
    }
    case ULOG_JOB_SUSPENDED: {
      JobSuspendedEvent   *jsev = dynamic_cast<JobSuspendedEvent *>( this->te_event.get() );

      this->te_classad->InsertAttr( te_s_NumPids, jsev->num_pids );

      break;
    }
    case ULOG_JOB_UNSUSPENDED: break; // Nothing seems to be done
    case ULOG_JOB_HELD: {
      JobHeldEvent   *jhev = dynamic_cast<JobHeldEvent *>( this->te_event.get() );
      NullString      reason( jhev->getReason() );

      this->te_classad->InsertAttr( te_s_Reason, reason );
      this->te_classad->InsertAttr( te_s_ReasonCode, jhev->getReasonCode() );
      this->te_classad->InsertAttr( te_s_ReasonSubCode, jhev->getReasonSubCode() );

      break;
    }
    case ULOG_JOB_RELEASED: {
      JobReleasedEvent  *jrev = dynamic_cast<JobReleasedEvent *>( this->te_event.get() );
      NullString         reason( jrev->getReason() );

      this->te_classad->InsertAttr( te_s_Reason, reason );

      break;
    }
    case ULOG_NODE_EXECUTE: {
      NodeExecuteEvent    *neev = dynamic_cast<NodeExecuteEvent *>( this->te_event.get() );
      this->te_classad->InsertAttr( te_s_Node, neev->node );

      break;
    }
    case ULOG_NODE_TERMINATED: {
      NodeTerminatedEvent   *ntev = dynamic_cast<NodeTerminatedEvent *>( this->te_event.get() );
      NullString             corefile( ntev->getCoreFile() );

      this->te_classad->InsertAttr( te_s_Normal, ntev->normal );
      this->te_classad->InsertAttr( te_s_ReturnValue, ntev->returnValue );
      this->te_classad->InsertAttr( te_s_SignalNumber, ntev->signalNumber );
      this->te_classad->InsertAttr( te_s_Node, ntev->node );
      this->te_classad->InsertAttr( te_s_SentBytes, ntev->sent_bytes );
      this->te_classad->InsertAttr( te_s_RecvdBytes, ntev->recvd_bytes );
      this->te_classad->InsertAttr( te_s_TotalSentBytes, ntev->total_sent_bytes );
      this->te_classad->InsertAttr( te_s_TotalRecvdBytes, ntev->total_recvd_bytes );
      this->te_classad->InsertAttr( te_s_CoreFile, corefile );
      this->te_classad->Insert( te_s_RunLocalRusage, toClassAd(ntev->run_local_rusage) );
      this->te_classad->Insert( te_s_RunRemoteRusage, toClassAd(ntev->run_remote_rusage) );
      this->te_classad->Insert( te_s_TotalLocalRusage, toClassAd(ntev->total_local_rusage) );
      this->te_classad->Insert( te_s_TotalRemoteRusage, toClassAd(ntev->total_remote_rusage) );

      break;
    }
    case ULOG_POST_SCRIPT_TERMINATED: {
      PostScriptTerminatedEvent  *pstev = dynamic_cast<PostScriptTerminatedEvent *>( this->te_event.get() );

      this->te_classad->InsertAttr( te_s_Normal, pstev->normal );
      this->te_classad->InsertAttr( te_s_ReturnValue, pstev->returnValue );
      this->te_classad->InsertAttr( te_s_SignalNumber, pstev->signalNumber );

      break;
    }
    case ULOG_GLOBUS_SUBMIT: {
      GlobusSubmitEvent   *gsev = dynamic_cast<GlobusSubmitEvent *>( this->te_event.get() );
      NullString           rmContact( gsev->rmContact ), jmContact( gsev->jmContact );

      this->te_classad->InsertAttr( te_s_RmContact, rmContact );
      this->te_classad->InsertAttr( te_s_JmContact, jmContact );
      this->te_classad->InsertAttr( te_s_RestartableJM, gsev->restartableJM );

      break;
    }
    case ULOG_GLOBUS_SUBMIT_FAILED: {
      GlobusSubmitFailedEvent   *gsfev = dynamic_cast<GlobusSubmitFailedEvent *>( this->te_event.get() );
      NullString                 reason( gsfev->reason );

      this->te_classad->InsertAttr( te_s_Reason, reason );

      break;
    }
    case ULOG_GLOBUS_RESOURCE_UP: {
      GlobusResourceUpEvent     *gruev = dynamic_cast<GlobusResourceUpEvent *>( this->te_event.get() );
      NullString                 rmContact( gruev->rmContact );

      this->te_classad->InsertAttr( te_s_RmContact, rmContact );

      break;
    }
    case ULOG_GLOBUS_RESOURCE_DOWN: {
      GlobusResourceDownEvent   *grdev = dynamic_cast<GlobusResourceDownEvent *>( this->te_event.get() );
      NullString                 rmContact( grdev->rmContact );

      this->te_classad->InsertAttr( te_s_RmContact, rmContact );

      break;
    }
    case ULOG_REMOTE_ERROR: {
      RemoteErrorEvent          *reev = dynamic_cast<RemoteErrorEvent *>( this->te_event.get() );
      NullString                 exeHost( reev->getExecuteHost() );
      NullString                 daemonName( reev->getDaemonName() );
      NullString                 errorStr( reev->getErrorText() );
      
      this->te_classad->InsertAttr( te_s_ExecuteHost, exeHost );
      this->te_classad->InsertAttr( te_s_DaemonName, daemonName );
      this->te_classad->InsertAttr( te_s_ErrorStr, errorStr );
      this->te_classad->InsertAttr( te_s_CriticalError, reev->isCriticalError() );

      break;
    }
    case ULOG_JOB_DISCONNECTED: {
      JobDisconnectedEvent     *jdcev = dynamic_cast<JobDisconnectedEvent *>( this->te_event.get() );
      NullString                startdAddr( jdcev->getStartdAddr() );
      NullString                startdName( jdcev->getStartdName() );
      NullString                disconnReason( jdcev->getDisconnectReason() );
      NullString                noreconnReason( jdcev->getNoReconnectReason() );

      this->te_classad->InsertAttr( te_s_StartdAddr, startdAddr );
      this->te_classad->InsertAttr( te_s_StartdName, startdName );
      this->te_classad->InsertAttr( te_s_DisconnReason, disconnReason );
      this->te_classad->InsertAttr( te_s_NoReconnReason, noreconnReason );
      this->te_classad->InsertAttr( te_s_CanReconn, jdcev->canReconnect() );

      break;
    }
    case ULOG_JOB_RECONNECTED: {
      JobReconnectedEvent      *jrcev = dynamic_cast<JobReconnectedEvent *>( this->te_event.get() );
      NullString                startdAddr( jrcev->getStartdAddr() );
      NullString                startdName( jrcev->getStartdName() );
      NullString                starterAddr( jrcev->getStarterAddr() );
      
      this->te_classad->InsertAttr( te_s_StartdAddr, startdAddr );
      this->te_classad->InsertAttr( te_s_StartdName, startdName );
      this->te_classad->InsertAttr( te_s_StarterAddr, starterAddr );

      break;
    }
    case ULOG_JOB_RECONNECT_FAILED: {
      JobReconnectFailedEvent *jrcfev = dynamic_cast<JobReconnectFailedEvent *>( this->te_event.get() );
      NullString                startdName( jrcfev->getStartdName() );
      NullString                reason( jrcfev->getReason() );

      this->te_classad->InsertAttr( te_s_StartdName, startdName );
      this->te_classad->InsertAttr( te_s_Reason, reason );

      break;
    }
    case ULOG_GRID_SUBMIT: {
      GridSubmitEvent     *gsev = dynamic_cast<GridSubmitEvent *>( this->te_event.get() );
      NullString           resourceName( gsev->resourceName ), jobId( gsev->jobId );

      this->te_classad->InsertAttr( te_s_ResourceName, resourceName );
      this->te_classad->InsertAttr( te_s_JobId, jobId );

      break;
    }
    case ULOG_GRID_RESOURCE_UP: {
      GridResourceUpEvent       *gruev = dynamic_cast<GridResourceUpEvent *>( this->te_event.get() );
      NullString                 resourceName( gruev->resourceName );

      this->te_classad->InsertAttr( te_s_ResourceName, resourceName );

      break;
    }
    case ULOG_GRID_RESOURCE_DOWN: {
      GridResourceDownEvent     *grdev = dynamic_cast<GridResourceDownEvent *>( this->te_event.get() );
      NullString                 resourceName( grdev->resourceName );

      this->te_classad->InsertAttr( te_s_ResourceName, resourceName );

      break;
    }
    } 
  }
  return this->te_classad.get();
}

ULogEvent *TimeoutEvent::to_event( void )
{
  int     ival;
  time_t  tval;
  double  dval;
  string  sval;

  if( !this->te_event ) {
    this->te_classad->EvaluateAttrNumber( te_s_EventNumber, ival );
    this->te_classad->EvaluateAttrString( te_s_Timeout, sval );

    this->te_epoch = boost::lexical_cast<time_t>( sval );

    this->te_event.reset( instantiateEvent(static_cast<ULogEventNumber>(ival)) );

    this->te_classad->EvaluateAttrString( te_s_EventTime, sval );
    tval = boost::lexical_cast<time_t>( sval ); localtime_r( &tval, &this->te_event->eventTime );

    this->te_classad->EvaluateAttrNumber( te_s_Cluster, this->te_event->cluster );
    this->te_classad->EvaluateAttrNumber( te_s_Proc, this->te_event->proc );
    this->te_classad->EvaluateAttrNumber( te_s_SubProc, this->te_event->subproc );

    switch( this->te_event->eventNumber ) {
    case ULOG_SUBMIT: {
      SubmitEvent   *sev = dynamic_cast<SubmitEvent *>( this->te_event.get() );

      if( this->te_classad->EvaluateAttrString(te_s_LogNotes, sval) )
	sev->submitEventLogNotes = myStrdup( sval );
      if( this->te_classad->EvaluateAttrString(te_s_UserNotes, sval) )
	sev->submitEventUserNotes = myStrdup( sval );

      break;
    }
    case ULOG_EXECUTE: {
      ExecuteEvent   *eev = dynamic_cast<ExecuteEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrString( te_s_ExecuteHost, sval );
      eev->setExecuteHost(myStrdup(sval.substr(0, 128)));

      break;
    }
    case ULOG_EXECUTABLE_ERROR: {
      ExecutableErrorEvent  *eeev = dynamic_cast<ExecutableErrorEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrNumber( te_s_ExecErrorType, ival );
      eeev->errType = static_cast<ExecErrorType>( ival );

      break;
    }
    case ULOG_CHECKPOINTED: {
      CheckpointedEvent   *chev = dynamic_cast<CheckpointedEvent *>( this->te_event.get() );
      classad::ClassAd    *tmp;

      tmp = dynamic_cast<classad::ClassAd *>( this->te_classad->Lookup(te_s_RunLocalRusage) );
      chev->run_local_rusage = toRusage( tmp );

      tmp = dynamic_cast<classad::ClassAd *>( this->te_classad->Lookup(te_s_RunRemoteRusage) );
      chev->run_remote_rusage = toRusage( tmp );

      break;
    }
    case ULOG_JOB_EVICTED: {
      JobEvictedEvent   *jeev = dynamic_cast<JobEvictedEvent *>( this->te_event.get() );
      classad::ClassAd  *tmp;

      this->te_classad->EvaluateAttrBool( te_s_CheckPointed, jeev->checkpointed );
      this->te_classad->EvaluateAttrNumber( te_s_SentBytes, dval ); jeev->sent_bytes = static_cast<float>( dval );
      this->te_classad->EvaluateAttrNumber( te_s_RecvdBytes, dval ); jeev->recvd_bytes = static_cast<float>( dval );
      this->te_classad->EvaluateAttrBool( te_s_Terminate, jeev->terminate_and_requeued );
      this->te_classad->EvaluateAttrBool( te_s_Normal, jeev->normal );
      this->te_classad->EvaluateAttrNumber( te_s_ReturnValue, jeev->return_value );
      this->te_classad->EvaluateAttrNumber( te_s_SignalNumber, jeev->signal_number );
      this->te_classad->EvaluateAttrString( te_s_Reason, sval ); jeev->setReason( myStrdup(sval) );
      this->te_classad->EvaluateAttrString( te_s_CoreFile, sval ); jeev->setCoreFile( myStrdup(sval) );

      tmp = dynamic_cast<classad::ClassAd *>( this->te_classad->Lookup(te_s_RunLocalRusage) );
      jeev->run_local_rusage = toRusage( tmp );

      tmp = dynamic_cast<classad::ClassAd *>( this->te_classad->Lookup(te_s_RunRemoteRusage) );
      jeev->run_remote_rusage = toRusage( tmp );

      break;
    }
    case ULOG_JOB_TERMINATED: {
      JobTerminatedEvent *jtev = dynamic_cast<JobTerminatedEvent *>( this->te_event.get() );
      classad::ClassAd   *tmp;

      this->te_classad->EvaluateAttrBool( te_s_Normal, jtev->normal );
      this->te_classad->EvaluateAttrNumber( te_s_ReturnValue, jtev->returnValue );
      this->te_classad->EvaluateAttrNumber( te_s_SignalNumber, jtev->signalNumber );
      this->te_classad->EvaluateAttrNumber( te_s_SentBytes, dval ); jtev->sent_bytes = static_cast<float>( dval );
      this->te_classad->EvaluateAttrNumber( te_s_RecvdBytes, dval ); jtev->recvd_bytes = static_cast<float>( dval );
      this->te_classad->EvaluateAttrNumber( te_s_TotalSentBytes, dval ); jtev->total_sent_bytes = static_cast<float>( dval );
      this->te_classad->EvaluateAttrNumber( te_s_TotalRecvdBytes, dval ); jtev->total_recvd_bytes = static_cast<float>( dval );
      this->te_classad->EvaluateAttrString( te_s_CoreFile, sval ); jtev->setCoreFile( myStrdup(sval) );

      tmp = dynamic_cast<classad::ClassAd *>( this->te_classad->Lookup(te_s_RunLocalRusage) );
      jtev->run_local_rusage = toRusage( tmp );

      tmp = dynamic_cast<classad::ClassAd *>( this->te_classad->Lookup(te_s_RunRemoteRusage) );
      jtev->run_remote_rusage = toRusage( tmp );

      tmp = dynamic_cast<classad::ClassAd *>( this->te_classad->Lookup(te_s_TotalLocalRusage) );
      jtev->total_local_rusage = toRusage( tmp );

      tmp = dynamic_cast<classad::ClassAd *>( this->te_classad->Lookup(te_s_TotalRemoteRusage) );
      jtev->total_remote_rusage = toRusage( tmp );

      break;
    }
    case ULOG_IMAGE_SIZE: {
      JobImageSizeEvent   *jisev = dynamic_cast<JobImageSizeEvent *>( this->te_event.get() );

      int size = jisev->image_size_kb;
      this->te_classad->EvaluateAttrNumber(te_s_Size, size);

      break;
    }
    case ULOG_SHADOW_EXCEPTION: {
      ShadowExceptionEvent  *seev = dynamic_cast<ShadowExceptionEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrString( te_s_Message, sval );
      strncpy( seev->message, sval.c_str(), BUFSIZ );

      this->te_classad->EvaluateAttrNumber( te_s_SentBytes, dval ); seev->sent_bytes = static_cast<float>( dval );
      this->te_classad->EvaluateAttrNumber( te_s_RecvdBytes, dval ); seev->recvd_bytes = static_cast<float>( dval );

      break;
    }
    case ULOG_GENERIC: {
      GenericEvent   *gev = dynamic_cast<GenericEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrString( te_s_Info, sval );
      strncpy( gev->info, sval.c_str(), 128 );

      break;
    }
    case ULOG_JOB_ABORTED: {
      JobAbortedEvent  *jaev = dynamic_cast<JobAbortedEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrString( te_s_Reason, sval );
      jaev->setReason( myStrdup(sval) );

      break;
    }
    case ULOG_JOB_SUSPENDED: {
      JobSuspendedEvent  *jseev = dynamic_cast<JobSuspendedEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrNumber( te_s_NumPids, jseev->num_pids );

      break;
    }
    case ULOG_JOB_UNSUSPENDED: break; // Nothing seems to be done
    case ULOG_JOB_HELD: {
      JobHeldEvent   *jhev = dynamic_cast<JobHeldEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrString( te_s_Reason, sval );
      jhev->setReason( myStrdup(sval) );
      this->te_classad->EvaluateAttrNumber( te_s_ReasonCode, ival );
      jhev->setReasonCode( ival );
      this->te_classad->EvaluateAttrNumber( te_s_ReasonSubCode, ival );
      jhev->setReasonSubCode( ival );

      break;
    }
    case ULOG_JOB_RELEASED: {
      JobReleasedEvent   *jrev = dynamic_cast<JobReleasedEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrString( te_s_Reason, sval );
      jrev->setReason( myStrdup(sval) );

      break;
    }
    case ULOG_NODE_EXECUTE: {
      NodeExecuteEvent   *neev = dynamic_cast<NodeExecuteEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrNumber( te_s_Node, neev->node );

      this->te_classad->EvaluateAttrString( te_s_ExecuteHost, sval );
      neev->setExecuteHost(myStrdup(sval.substr(0, 128)));

      break;
    }
    case ULOG_NODE_TERMINATED: {
      NodeTerminatedEvent *ntev = dynamic_cast<NodeTerminatedEvent *>( this->te_event.get() );
      classad::ClassAd   *tmp;

      this->te_classad->EvaluateAttrBool( te_s_Normal, ntev->normal );
      this->te_classad->EvaluateAttrNumber( te_s_ReturnValue, ntev->returnValue );
      this->te_classad->EvaluateAttrNumber( te_s_SignalNumber, ntev->signalNumber );
      this->te_classad->EvaluateAttrNumber( te_s_Node, ntev->node );
      this->te_classad->EvaluateAttrNumber( te_s_SentBytes, dval ); ntev->sent_bytes = static_cast<float>( dval );
      this->te_classad->EvaluateAttrNumber( te_s_RecvdBytes, dval ); ntev->recvd_bytes = static_cast<float>( dval );
      this->te_classad->EvaluateAttrNumber( te_s_TotalSentBytes, dval ); ntev->total_sent_bytes = static_cast<float>( dval );
      this->te_classad->EvaluateAttrNumber( te_s_TotalRecvdBytes, dval ); ntev->total_recvd_bytes = static_cast<float>( dval );
      this->te_classad->EvaluateAttrString( te_s_CoreFile, sval ); ntev->setCoreFile( myStrdup(sval) );

      tmp = dynamic_cast<classad::ClassAd *>( this->te_classad->Lookup(te_s_RunLocalRusage) );
      ntev->run_local_rusage = toRusage( tmp );

      tmp = dynamic_cast<classad::ClassAd *>( this->te_classad->Lookup(te_s_RunRemoteRusage) );
      ntev->run_remote_rusage = toRusage( tmp );

      tmp = dynamic_cast<classad::ClassAd *>( this->te_classad->Lookup(te_s_TotalLocalRusage) );
      ntev->total_local_rusage = toRusage( tmp );

      tmp = dynamic_cast<classad::ClassAd *>( this->te_classad->Lookup(te_s_TotalRemoteRusage) );
      ntev->total_remote_rusage = toRusage( tmp );

      break;
    }
    case ULOG_POST_SCRIPT_TERMINATED: {
      PostScriptTerminatedEvent   *pstev = dynamic_cast<PostScriptTerminatedEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrBool( te_s_Normal, pstev->normal );
      this->te_classad->EvaluateAttrNumber( te_s_ReturnValue, pstev->returnValue );
      this->te_classad->EvaluateAttrNumber( te_s_SignalNumber, pstev->signalNumber );

      break;
    }
    case ULOG_GLOBUS_SUBMIT: {
      GlobusSubmitEvent    *gsev = dynamic_cast<GlobusSubmitEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrBool( te_s_RestartableJM, gsev->restartableJM );

      this->te_classad->EvaluateAttrString( te_s_RmContact, sval );
      gsev->rmContact = myStrdup( sval );

      this->te_classad->EvaluateAttrString( te_s_JmContact, sval );
      gsev->jmContact = myStrdup( sval );

      break;
    }
    case ULOG_GLOBUS_SUBMIT_FAILED: {
      GlobusSubmitFailedEvent   *gsfev = dynamic_cast<GlobusSubmitFailedEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrString( te_s_Reason, sval );
      gsfev->reason = myStrdup( sval );

      break;
    }
    case ULOG_GLOBUS_RESOURCE_UP: {
      GlobusResourceUpEvent   *gruev = dynamic_cast<GlobusResourceUpEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrString( te_s_RmContact, sval );
      gruev->rmContact = myStrdup( sval );

      break;
    }
    case ULOG_GLOBUS_RESOURCE_DOWN: {
      GlobusResourceDownEvent  *grdev = dynamic_cast<GlobusResourceDownEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrString( te_s_RmContact, sval );
      grdev->rmContact = myStrdup( sval );

      break;
    }
    case ULOG_REMOTE_ERROR: {
      bool                     bval;
      RemoteErrorEvent        *reev = dynamic_cast<RemoteErrorEvent *>( this->te_event.get() );
      
      this->te_classad->EvaluateAttrString( te_s_ExecuteHost, sval );
      reev->setExecuteHost( myStrdup( sval ) );
      this->te_classad->EvaluateAttrString( te_s_DaemonName, sval );
      reev->setDaemonName( myStrdup ( sval ) );
      this->te_classad->EvaluateAttrString( te_s_ErrorStr, sval );
      reev->setErrorText( myStrdup ( sval ) );
      this->te_classad->EvaluateAttrBool( te_s_CriticalError, bval );
      reev->setCriticalError( bval );
     
      break;
    }
    case ULOG_JOB_DISCONNECTED: {
      JobDisconnectedEvent     *jdcev = dynamic_cast<JobDisconnectedEvent *>( this->te_event.get() );
                                                                                                                       
      this->te_classad->EvaluateAttrString( te_s_StartdAddr, sval );
      jdcev->setStartdAddr( myStrdup( sval ) );
      this->te_classad->EvaluateAttrString( te_s_StartdName, sval );
      jdcev->setStartdName( myStrdup( sval ) );
      this->te_classad->EvaluateAttrString( te_s_DisconnReason, sval );
      jdcev->setDisconnectReason( myStrdup( sval ) );
      this->te_classad->EvaluateAttrString( te_s_NoReconnReason, sval );
      jdcev->setNoReconnectReason( myStrdup( sval ) );

      break;
    }
    case ULOG_JOB_RECONNECTED: {
      JobReconnectedEvent      *jrcev = dynamic_cast<JobReconnectedEvent *>( this->te_event.get() );
      
      this->te_classad->EvaluateAttrString( te_s_StartdAddr, sval );
      jrcev->setStartdAddr( myStrdup( sval ) );
      this->te_classad->EvaluateAttrString( te_s_StartdName, sval );
      jrcev->setStartdName( myStrdup( sval ) );
      this->te_classad->EvaluateAttrString( te_s_StarterAddr, sval );
      jrcev->setStarterAddr( myStrdup( sval ) );
                                                                                                            
      break;
    }
    case ULOG_JOB_RECONNECT_FAILED: {
      JobReconnectFailedEvent *jrcfev = dynamic_cast<JobReconnectFailedEvent *>( this->te_event.get() );  
      
      this->te_classad->EvaluateAttrString( te_s_StartdName, sval );
      jrcfev->setStartdName( myStrdup( sval ) );
      this->te_classad->EvaluateAttrString( te_s_Reason, sval );
      jrcfev->setReason( myStrdup( sval ) );

      break;
    }
    case ULOG_GRID_SUBMIT: {
      GridSubmitEvent    *gsev = dynamic_cast<GridSubmitEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrString( te_s_ResourceName, sval );
      gsev->resourceName = myStrdup( sval );

      this->te_classad->EvaluateAttrString( te_s_JobId, sval );
      gsev->jobId = myStrdup( sval );

      break;
    }
    case ULOG_GRID_RESOURCE_UP: {
      GridResourceUpEvent   *gruev = dynamic_cast<GridResourceUpEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrString( te_s_ResourceName, sval );
      gruev->resourceName = myStrdup( sval );

      break;
    }
    case ULOG_GRID_RESOURCE_DOWN: {
      GridResourceDownEvent  *grdev = dynamic_cast<GridResourceDownEvent *>( this->te_event.get() );

      this->te_classad->EvaluateAttrString( te_s_ResourceName, sval );
      grdev->resourceName = myStrdup( sval );

      break;
    }
    }
  }

  return this->te_event.get();
}

Timer::Timer( const string &backup ) : t_filename( backup ), t_events(), t_backup( backup )
{
  string                   val;
  FileContainer::iterator  fileIt, end = this->t_backup.end();
  EventPointer             ptr;

  if( !this->t_backup.empty() )
    for( fileIt = this->t_backup.begin(); fileIt != end; ++fileIt ) {
      ptr.reset( new TimeoutEvent(*fileIt) );

      ptr->pointer( fileIt );
      this->t_events.insert( EventMap::value_type(ptr->timeout(), ptr) );
    }
  // Let's save an open file descriptor.
  this->t_backup.close();
}

Timer::~Timer( void )
{}

Timer &Timer::start_timer( time_t expire, ULogEvent *ptr )
{
  EventPointer               event( new TimeoutEvent(expire, ptr) );
  FileContainer::iterator    fIt;

  this->t_backup.open(this->t_filename);

  this->t_backup.push_back( *event->to_classad() );
  fIt = this->t_backup.end(); --fIt;

  event->pointer( fIt );
  this->t_events.insert( EventMap::value_type(expire, event) );

  // Let's save an open file descriptor.
  this->t_backup.close();

  return *this;
}

Timer &Timer::remove_timeout( EventIterator &evIt )
{
  FileContainer::iterator     ptr;

  if( evIt != this->t_events.end() ) {
    ptr = evIt->second->pointer();

    this->t_backup.open(this->t_filename);
    this->t_backup.erase( ptr );
    // Let's save an open file descriptor.
    this->t_backup.close();

    this->t_events.erase( evIt );
  }

  return *this;
}

// remove _all_ the timeouts associated with the given eventcode
Timer &Timer::remove_timeout( int condorid, int eventcode )
{
  EventIterator    it, end = this->t_events.end();
  list<EventIterator>            evList;
  list<EventIterator>::iterator  evIt;

  for( it = this->t_events.begin(); it != end; ++it )
    if( (it->second->to_event()->cluster == condorid ) &&
	( it->second->to_event()->eventNumber == static_cast<ULogEventNumber>(eventcode) ) )
      evList.push_back( it );

  if( !evList.empty() )
     for( evIt = evList.begin(); evIt != evList.end(); ++evIt )
       this->remove_timeout( *evIt );

  return *this;
}

Timer &Timer::remove_all_timeouts( int condorid )
{
  EventIterator                  it, end = this->t_events.end();
  list<EventIterator>            evList;
  list<EventIterator>::iterator  evIt;

  for( it = this->t_events.begin(); it != end; ++it )
    if( it->second->to_event()->cluster == condorid )
      evList.push_back( it );

  if( !evList.empty() )
    for( evIt = evList.begin(); evIt != evList.end(); ++evIt )
      this->remove_timeout( *evIt );

  return *this;
}

} // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END
