#include <ctime>
#include <cstdio>
#include <cstring>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <classad_distribution.h>
#include <user_log.c++.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "jobcontrol_namespace.h"
#include "EventAd.h"

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

char *local_strdup( const string &s )
{
  char   *pc = new char[ s.length() + 1 ];

  strcpy( pc, s.c_str() );

  return pc;
}

typedef  struct rusage   Rusage;

classad::ClassAd *rusage_to_classad( const Rusage &ru )
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

Rusage classad_to_rusage( const classad::ClassAd *ad )
{
  double    double_value;
  Rusage    ru;

  ad->EvaluateAttrNumber( "ru_maxrss", double_value );   ru.ru_maxrss = static_cast<long int>( double_value );
  ad->EvaluateAttrNumber( "ru_ixrss", double_value );    ru.ru_ixrss = static_cast<long int>( double_value );
  ad->EvaluateAttrNumber( "ru_idrss", double_value );    ru.ru_idrss = static_cast<long int>( double_value );
  ad->EvaluateAttrNumber( "ru_isrss", double_value );    ru.ru_isrss = static_cast<long int>( double_value );
  ad->EvaluateAttrNumber( "ru_minflt", double_value );   ru.ru_minflt = static_cast<long int>( double_value );
  ad->EvaluateAttrNumber( "ru_majflt", double_value );   ru.ru_majflt = static_cast<long int>( double_value );
  ad->EvaluateAttrNumber( "ru_nswap", double_value );    ru.ru_nswap = static_cast<long int>( double_value );
  ad->EvaluateAttrNumber( "ru_inblock", double_value );  ru.ru_inblock = static_cast<long int>( double_value );
  ad->EvaluateAttrNumber( "ru_oublock", double_value );  ru.ru_oublock = static_cast<long int>( double_value );
  ad->EvaluateAttrNumber( "ru_msgsnd", double_value );   ru.ru_msgsnd = static_cast<long int>( double_value );
  ad->EvaluateAttrNumber( "ru_msgrcv", double_value );   ru.ru_msgrcv = static_cast<long int>( double_value );
  ad->EvaluateAttrNumber( "ru_nsignals", double_value ); ru.ru_nsignals = static_cast<long int>( double_value );
  ad->EvaluateAttrNumber( "ru_nvcsw", double_value );    ru.ru_nvcsw = static_cast<long int>( double_value );
  ad->EvaluateAttrNumber( "ru_nivcsw", double_value );   ru.ru_nivcsw = static_cast<long int>( double_value );
  ad->EvaluateAttrNumber( "ru_utime", double_value );
  ru.ru_utime.tv_sec = static_cast<long int>( double_value ); ru.ru_utime.tv_usec = 0;
  ad->EvaluateAttrNumber( "ru_stime", double_value );
  ru.ru_stime.tv_sec = static_cast<long int>( double_value ); ru.ru_stime.tv_usec = 0;

  return ru;
}

}; // Anonymous namespace

const char   *EventAd::ea_s_EventTime = "EventTime", *EventAd::ea_s_EventNumber = "EventNumber";
const char   *EventAd::ea_s_Cluster = "Cluster", *EventAd::ea_s_Proc = "Proc", *EventAd::ea_s_SubProc = "SubProc";
const char   *EventAd::ea_s_SubmitHost = "SubmitHost", *EventAd::ea_s_LogNotes = "LogNotes";
const char   *EventAd::ea_s_ExecuteHost = "ExecuteHost";
const char   *EventAd::ea_s_ExecErrorType = "ExecErrorType", *EventAd::ea_s_Node = "Node";
const char   *EventAd::ea_s_RunLocalRusage = "RunLocalRusage", *EventAd::ea_s_RunRemoteRusage = "RunRemoteRusage";
const char   *EventAd::ea_s_TotalLocalRusage = "TotalLocalRusage", *EventAd::ea_s_TotalRemoteRusage = "TotalRemoteRusage";
const char   *EventAd::ea_s_CheckPointed = "CheckPointed", *EventAd::ea_s_SentBytes = "SentBytes";
const char   *EventAd::ea_s_RecvdBytes = "RecvdBytes", *EventAd::ea_s_Terminate = "Terminate";
const char   *EventAd::ea_s_Normal = "Normal", *EventAd::ea_s_ReturnValue = "ReturnValue";
const char   *EventAd::ea_s_SignalNumber = "SignalNumber", *EventAd::ea_s_Reason = "Reason";
const char   *EventAd::ea_s_CoreFile = "CoreFile", *EventAd::ea_s_TotalSentBytes = "TotalSentBytes";
const char   *EventAd::ea_s_TotalRecvdBytes = "TotalRecvdBytes", *EventAd::ea_s_Size = "Size";
const char   *EventAd::ea_s_Message = "Message", *EventAd::ea_s_Info = "Info", *EventAd::ea_s_NumPids = "NumPids";
const char   *EventAd::ea_s_RestartableJM = "RestartableJM", *EventAd::ea_s_RmContact = "RmContact";
const char   *EventAd::ea_s_JmContact = "JmContact"; 

#if CONDORG_AT_LEAST(6,5,3)
const char   *EventAd::ea_s_DaemonName = "DaemonName", *EventAd::ea_s_ErrorStr = "ErrorStr";
const char   *EventAd::ea_s_CriticalError = "CriticalError";
const char   *EventAd::ea_s_ReasonCode = "ReasonCode", *EventAd::ea_s_ReasonSubCode = "ReasonSubCode";
const char   *EventAd::ea_s_UserNotes = "UserNotes";
#endif

#if CONDORG_AT_LEAST(6,7,14)
const char   *EventAd::ea_s_ResourceName= "GridResource", *EventAd::ea_s_JobId = "GridJobId";
#endif

EventAd::EventAd( void ) : ea_ad()
{}

EventAd::EventAd( const classad::ClassAd *ad )
  : ea_ad(*static_cast<classad::ClassAd*>(ad->Copy()))
{}

EventAd::EventAd( const ULogEvent *event ) : ea_ad()
{
  this->from_event( event );
}

EventAd::~EventAd( void )
{}

EventAd &EventAd::from_event( const ULogEvent *const_event )
{
  time_t       epoch = mktime( const_cast<tm *>(&const_event->eventTime) );
  ULogEvent   *event = const_cast<ULogEvent *>( const_event );

  this->ea_ad.Clear();

  this->ea_ad.InsertAttr( ea_s_EventNumber, static_cast<int>(event->eventNumber) );
  this->ea_ad.InsertAttr( ea_s_EventTime, boost::lexical_cast<string>(epoch) );
  this->ea_ad.InsertAttr( ea_s_Cluster, event->cluster );
  this->ea_ad.InsertAttr( ea_s_Proc, static_cast<int>(event->proc) );
  this->ea_ad.InsertAttr( ea_s_SubProc, static_cast<int>(event->subproc) );

  switch( event->eventNumber ) {
  case ULOG_SUBMIT: {
    SubmitEvent    *sev = dynamic_cast<SubmitEvent *>( event );
    NullString      host( sev->submitHost );

    this->ea_ad.InsertAttr( ea_s_SubmitHost, host );

    if( sev->submitEventLogNotes ) {
      NullString    lnotes( sev->submitEventLogNotes );

      this->ea_ad.InsertAttr( ea_s_LogNotes, lnotes );
    }

#if CONDORG_AT_LEAST(6,5,3)
    if( sev->submitEventUserNotes ) {
      NullString    unotes( sev->submitEventUserNotes );
	
      this->ea_ad.InsertAttr( ea_s_UserNotes, unotes );
    }
#endif

    break;
  }
  case ULOG_EXECUTE: {
    ExecuteEvent   *eev = dynamic_cast<ExecuteEvent *>( event );
    NullString      host( eev->executeHost );

    this->ea_ad.InsertAttr( ea_s_ExecuteHost, host );

    break;
  }
  case ULOG_EXECUTABLE_ERROR: {
    ExecutableErrorEvent   *eeev = dynamic_cast<ExecutableErrorEvent *>( event );

    this->ea_ad.InsertAttr( ea_s_ExecErrorType, static_cast<int>(eeev->errType) );

    break;
  }
  case ULOG_CHECKPOINTED: {
    CheckpointedEvent   *chev = dynamic_cast<CheckpointedEvent *>( event );

    this->ea_ad.Insert( ea_s_RunLocalRusage, rusage_to_classad(chev->run_local_rusage) );
    this->ea_ad.Insert( ea_s_RunRemoteRusage, rusage_to_classad(chev->run_remote_rusage) );

    break;
  }
  case ULOG_JOB_EVICTED: {
    JobEvictedEvent   *jev = dynamic_cast<JobEvictedEvent *>( event );
    NullString         reason( jev->getReason() ), corefile( jev->getCoreFile() );

    this->ea_ad.InsertAttr( ea_s_CheckPointed, jev->checkpointed );
    this->ea_ad.InsertAttr( ea_s_SentBytes, jev->sent_bytes );
    this->ea_ad.InsertAttr( ea_s_RecvdBytes, jev->recvd_bytes );
    this->ea_ad.InsertAttr( ea_s_Terminate, jev->terminate_and_requeued );
    this->ea_ad.InsertAttr( ea_s_Normal, jev->normal );
    this->ea_ad.InsertAttr( ea_s_ReturnValue, jev->return_value );
    this->ea_ad.InsertAttr( ea_s_SignalNumber, jev->signal_number );
    this->ea_ad.InsertAttr( ea_s_Reason, reason );
    this->ea_ad.InsertAttr( ea_s_CoreFile, corefile );
    this->ea_ad.Insert( ea_s_RunLocalRusage, rusage_to_classad(jev->run_local_rusage) );
    this->ea_ad.Insert( ea_s_RunRemoteRusage, rusage_to_classad(jev->run_remote_rusage) );

    break;
  }
  case ULOG_JOB_TERMINATED: {
    JobTerminatedEvent   *te = dynamic_cast<JobTerminatedEvent *>( event );
    NullString            corefile( te->getCoreFile() );

    this->ea_ad.InsertAttr( ea_s_Normal, te->normal );
    this->ea_ad.InsertAttr( ea_s_ReturnValue, te->returnValue );
    this->ea_ad.InsertAttr( ea_s_SignalNumber, te->signalNumber );
    this->ea_ad.InsertAttr( ea_s_SentBytes, te->sent_bytes );
    this->ea_ad.InsertAttr( ea_s_RecvdBytes, te->recvd_bytes );
    this->ea_ad.InsertAttr( ea_s_TotalSentBytes, te->total_sent_bytes );
    this->ea_ad.InsertAttr( ea_s_TotalRecvdBytes, te->total_recvd_bytes );
    this->ea_ad.InsertAttr( ea_s_CoreFile, corefile );
    this->ea_ad.Insert( ea_s_RunLocalRusage, rusage_to_classad(te->run_local_rusage) );
    this->ea_ad.Insert( ea_s_RunRemoteRusage, rusage_to_classad(te->run_remote_rusage) );
    this->ea_ad.Insert( ea_s_TotalLocalRusage, rusage_to_classad(te->total_local_rusage) );
    this->ea_ad.Insert( ea_s_TotalRemoteRusage, rusage_to_classad(te->total_remote_rusage) );

    break;
  }
  case ULOG_IMAGE_SIZE: {
    JobImageSizeEvent   *jisev = dynamic_cast<JobImageSizeEvent *>( event );

    this->ea_ad.InsertAttr( ea_s_Size, jisev->size );

    break;
  }
  case ULOG_SHADOW_EXCEPTION: {
    ShadowExceptionEvent   *sev = dynamic_cast<ShadowExceptionEvent *>( event );
    NullString              message( sev->message );

    this->ea_ad.InsertAttr( ea_s_Message, message );
    this->ea_ad.InsertAttr( ea_s_SentBytes, sev->sent_bytes );
    this->ea_ad.InsertAttr( ea_s_RecvdBytes, sev->recvd_bytes );

    break;
  }
  case ULOG_GENERIC: {
    GenericEvent  *gev = dynamic_cast<GenericEvent *>( event );
    NullString     info( gev->info );

    this->ea_ad.InsertAttr( ea_s_Info, info );

    break;
  }
  case ULOG_JOB_ABORTED: {
    JobAbortedEvent  *jaev = dynamic_cast<JobAbortedEvent *>( event );
    NullString        reason( jaev->getReason() );

    this->ea_ad.InsertAttr( ea_s_Reason, reason );

    break;
  }
  case ULOG_JOB_SUSPENDED: {
    JobSuspendedEvent   *jsev = dynamic_cast<JobSuspendedEvent *>( event );

    this->ea_ad.InsertAttr( ea_s_NumPids, jsev->num_pids );

    break;
  }
  case ULOG_JOB_UNSUSPENDED: break; // Nothing seems to be done
  case ULOG_JOB_HELD: {
    JobHeldEvent   *jhev = dynamic_cast<JobHeldEvent *>( event );
    NullString      reason( jhev->getReason() );

    this->ea_ad.InsertAttr( ea_s_Reason, reason );
#if CONDORG_AT_LEAST(6,5,3)
    this->ea_ad.InsertAttr( ea_s_ReasonCode, jhev->getReasonCode() );
    this->ea_ad.InsertAttr( ea_s_ReasonSubCode, jhev->getReasonSubCode() );
#endif

    break;
  }
  case ULOG_JOB_RELEASED: {
    JobReleasedEvent  *jrev = dynamic_cast<JobReleasedEvent *>( event );
    NullString         reason( jrev->getReason() );

    this->ea_ad.InsertAttr( ea_s_Reason, reason );

    break;
  }
  default: // Nothing to do, by now...
    break;
  }

  return *this;
}

ULogEvent *EventAd::create_event( void )
{
  bool                  boolean_value;
  int                   integer_value;
  time_t                epoch;
  ULogEventNumber       eventN;
  double                double_value;
  classad::ClassAd     *temporary_ad;
  auto_ptr<ULogEvent>   event;
  string                string_value;

  this->ea_ad.EvaluateAttrInt( ea_s_EventNumber, integer_value );
  eventN = static_cast<ULogEventNumber>( integer_value ); 
  event.reset( instantiateEvent(eventN) );

  if( event.get() ) {
    if( this->ea_ad.EvaluateAttrString(ea_s_EventTime, string_value) ) {
      epoch = boost::lexical_cast<time_t>( string_value );
      localtime_r( &epoch, &event->eventTime );
    }

    this->ea_ad.EvaluateAttrNumber( ea_s_Cluster, event->cluster );
    this->ea_ad.EvaluateAttrNumber( ea_s_Proc, event->proc );
    this->ea_ad.EvaluateAttrNumber( ea_s_SubProc, event->subproc );

    switch( eventN ) {
    case ULOG_SUBMIT: {
      SubmitEvent   *sev = dynamic_cast<SubmitEvent *>( event.get() );

      this->ea_ad.EvaluateAttrString( ea_s_SubmitHost, string_value );
      strncpy( sev->submitHost, string_value.c_str(), 128 );

      if( this->ea_ad.EvaluateAttrString(ea_s_LogNotes, string_value) )
	sev->submitEventLogNotes = local_strdup( string_value );

#if CONDORG_AT_LEAST(6,5,3)
      if( this->ea_ad.EvaluateAttrString(ea_s_UserNotes, string_value) )
	sev->submitEventUserNotes = local_strdup( string_value );
#endif

      break;
    }
    case ULOG_EXECUTE: {
      ExecuteEvent    *eev = dynamic_cast<ExecuteEvent *>( event.get() );

      this->ea_ad.EvaluateAttrString( ea_s_ExecuteHost, string_value );
      strncpy( eev->executeHost, string_value.c_str(), 128 );

      break;
    }
    case ULOG_EXECUTABLE_ERROR: {
      ExecutableErrorEvent  *eeev = dynamic_cast<ExecutableErrorEvent *>( event.get() );

      this->ea_ad.EvaluateAttrNumber( ea_s_ExecErrorType, integer_value );
      eeev->errType = static_cast<ExecErrorType>( integer_value );

      break;
    }
    case ULOG_CHECKPOINTED: {
      CheckpointedEvent   *chev = dynamic_cast<CheckpointedEvent *>( event.get() );

      temporary_ad = dynamic_cast<classad::ClassAd *>( this->ea_ad.Lookup(ea_s_RunLocalRusage) );
      if( temporary_ad )
	chev->run_local_rusage = classad_to_rusage( temporary_ad );

      temporary_ad = dynamic_cast<classad::ClassAd *>( this->ea_ad.Lookup(ea_s_RunRemoteRusage) );
      if( temporary_ad )
	chev->run_remote_rusage = classad_to_rusage( temporary_ad );

      break;
    }
    case ULOG_JOB_EVICTED: {
      JobEvictedEvent   *jeev = dynamic_cast<JobEvictedEvent *>( event.get() );

      this->ea_ad.EvaluateAttrBool( ea_s_CheckPointed, jeev->checkpointed );
      this->ea_ad.EvaluateAttrNumber( ea_s_SentBytes, double_value ); jeev->sent_bytes = static_cast<float>( double_value );
      this->ea_ad.EvaluateAttrNumber( ea_s_RecvdBytes, double_value ); jeev->recvd_bytes = static_cast<float>( double_value );
      this->ea_ad.EvaluateAttrBool( ea_s_Terminate, jeev->terminate_and_requeued );
      this->ea_ad.EvaluateAttrBool( ea_s_Normal, jeev->normal );
      this->ea_ad.EvaluateAttrNumber( ea_s_ReturnValue, jeev->return_value );
      this->ea_ad.EvaluateAttrNumber( ea_s_SignalNumber, jeev->signal_number );
      this->ea_ad.EvaluateAttrString( ea_s_Reason, string_value ); jeev->setReason( local_strdup(string_value) );
      this->ea_ad.EvaluateAttrString( ea_s_CoreFile, string_value ); jeev->setCoreFile( local_strdup(string_value) );

      temporary_ad = dynamic_cast<classad::ClassAd *>( this->ea_ad.Lookup(ea_s_RunLocalRusage) );
      if( temporary_ad )
	jeev->run_local_rusage = classad_to_rusage( temporary_ad );

      temporary_ad = dynamic_cast<classad::ClassAd *>( this->ea_ad.Lookup(ea_s_RunRemoteRusage) );
      if( temporary_ad )
	jeev->run_remote_rusage = classad_to_rusage( temporary_ad );

      break;
    }
    case ULOG_JOB_TERMINATED: {
      JobTerminatedEvent    *jtev = dynamic_cast<JobTerminatedEvent *>( event.get() );

      this->ea_ad.EvaluateAttrBool( ea_s_Normal, jtev->normal );
      this->ea_ad.EvaluateAttrNumber( ea_s_ReturnValue, jtev->returnValue );
      this->ea_ad.EvaluateAttrNumber( ea_s_SignalNumber, jtev->signalNumber );
      this->ea_ad.EvaluateAttrNumber( ea_s_SentBytes, double_value ); jtev->sent_bytes = static_cast<float>( double_value );
      this->ea_ad.EvaluateAttrNumber( ea_s_RecvdBytes, double_value ); jtev->recvd_bytes = static_cast<float>( double_value );
      this->ea_ad.EvaluateAttrNumber( ea_s_TotalSentBytes, double_value ); jtev->total_sent_bytes = static_cast<float>( double_value );
      this->ea_ad.EvaluateAttrNumber( ea_s_TotalRecvdBytes, double_value ); jtev->total_recvd_bytes = static_cast<float>( double_value );
      this->ea_ad.EvaluateAttrString( ea_s_CoreFile, string_value ); jtev->setCoreFile( local_strdup(string_value) );

      temporary_ad = dynamic_cast<classad::ClassAd *>( this->ea_ad.Lookup(ea_s_RunLocalRusage) );
      if( temporary_ad )
	jtev->run_local_rusage = classad_to_rusage( temporary_ad );

      temporary_ad = dynamic_cast<classad::ClassAd *>( this->ea_ad.Lookup(ea_s_RunRemoteRusage) );
      if( temporary_ad )
	jtev->run_remote_rusage = classad_to_rusage( temporary_ad );

      temporary_ad = dynamic_cast<classad::ClassAd *>( this->ea_ad.Lookup(ea_s_TotalLocalRusage) );
      if( temporary_ad )
	jtev->total_local_rusage = classad_to_rusage( temporary_ad );

      temporary_ad = dynamic_cast<classad::ClassAd *>( this->ea_ad.Lookup(ea_s_TotalRemoteRusage) );
      if( temporary_ad )
	jtev->total_remote_rusage = classad_to_rusage( temporary_ad );

      break;
    }
    case ULOG_IMAGE_SIZE: {
      JobImageSizeEvent   *jisev = dynamic_cast<JobImageSizeEvent *>( event.get() );

      this->ea_ad.EvaluateAttrNumber( ea_s_Size, jisev->size );

      break;
    }
    case ULOG_SHADOW_EXCEPTION: {
      ShadowExceptionEvent  *seev = dynamic_cast<ShadowExceptionEvent *>( event.get() );

      this->ea_ad.EvaluateAttrString( ea_s_Message, string_value );
      strncpy( seev->message, string_value.c_str(), BUFSIZ );

      this->ea_ad.EvaluateAttrNumber( ea_s_SentBytes, double_value ); seev->sent_bytes = static_cast<float>( double_value );
      this->ea_ad.EvaluateAttrNumber( ea_s_RecvdBytes, double_value ); seev->recvd_bytes = static_cast<float>( double_value );

      break;
    }
    case ULOG_GENERIC: {
      GenericEvent   *gev = dynamic_cast<GenericEvent *>( event.get() );

      this->ea_ad.EvaluateAttrString( ea_s_Info, string_value );
      strncpy( gev->info, string_value.c_str(), 128 );

      break;
    }
    case ULOG_JOB_ABORTED: {
      JobAbortedEvent  *jaev = dynamic_cast<JobAbortedEvent *>( event.get() );

      this->ea_ad.EvaluateAttrString( ea_s_Reason, string_value );
      jaev->setReason( local_strdup(string_value) );

      break;
    }
    case ULOG_JOB_SUSPENDED: {
      JobSuspendedEvent  *jseev = dynamic_cast<JobSuspendedEvent *>( event.get() );

      this->ea_ad.EvaluateAttrNumber( ea_s_NumPids, jseev->num_pids );

      break;
    }
    case ULOG_JOB_UNSUSPENDED: break; // Nothing seems to be done
    case ULOG_JOB_HELD: {
      JobHeldEvent   *jhev = dynamic_cast<JobHeldEvent *>( event.get() );

      this->ea_ad.EvaluateAttrString( ea_s_Reason, string_value );
      jhev->setReason( local_strdup(string_value) );

#if CONDORG_AT_LEAST(6,5,3)
      this->ea_ad.EvaluateAttrNumber( ea_s_ReasonCode, integer_value );
      jhev->setReasonCode( integer_value );

      this->ea_ad.EvaluateAttrNumber( ea_s_ReasonSubCode, integer_value );
      jhev->setReasonSubCode( integer_value );
#endif

      break;
    }
    case ULOG_JOB_RELEASED: {
      JobReleasedEvent   *jrev = dynamic_cast<JobReleasedEvent *>( event.get() );

      this->ea_ad.EvaluateAttrString( ea_s_Reason, string_value );
      jrev->setReason( local_strdup(string_value) );

      break;
    }
    case ULOG_NODE_EXECUTE: {
      NodeExecuteEvent   *neev = dynamic_cast<NodeExecuteEvent *>( event.get() );

      this->ea_ad.EvaluateAttrNumber( ea_s_Node, neev->node );

      this->ea_ad.EvaluateAttrString( ea_s_ExecuteHost, string_value );
      strncpy( neev->executeHost, string_value.c_str(), 128 );

      break;
    }
    case ULOG_NODE_TERMINATED: {
      NodeTerminatedEvent *ntev = dynamic_cast<NodeTerminatedEvent *>( event.get() );

      this->ea_ad.EvaluateAttrBool( ea_s_Normal, ntev->normal );
      this->ea_ad.EvaluateAttrNumber( ea_s_ReturnValue, ntev->returnValue );
      this->ea_ad.EvaluateAttrNumber( ea_s_SignalNumber, ntev->signalNumber );
      this->ea_ad.EvaluateAttrNumber( ea_s_Node, ntev->node );
      this->ea_ad.EvaluateAttrNumber( ea_s_SentBytes, double_value ); ntev->sent_bytes = static_cast<float>( double_value );
      this->ea_ad.EvaluateAttrNumber( ea_s_RecvdBytes, double_value ); ntev->recvd_bytes = static_cast<float>( double_value );
      this->ea_ad.EvaluateAttrNumber( ea_s_TotalSentBytes, double_value ); ntev->total_sent_bytes = static_cast<float>( double_value );
      this->ea_ad.EvaluateAttrNumber( ea_s_TotalRecvdBytes, double_value ); ntev->total_recvd_bytes = static_cast<float>( double_value );
      this->ea_ad.EvaluateAttrString( ea_s_CoreFile, string_value ); ntev->setCoreFile( local_strdup(string_value) );

      temporary_ad = dynamic_cast<classad::ClassAd *>( this->ea_ad.Lookup(ea_s_RunLocalRusage) );
      if( temporary_ad )
	ntev->run_local_rusage = classad_to_rusage( temporary_ad );

      temporary_ad = dynamic_cast<classad::ClassAd *>( this->ea_ad.Lookup(ea_s_RunRemoteRusage) );
      if( temporary_ad )
	ntev->run_remote_rusage = classad_to_rusage( temporary_ad );

      temporary_ad = dynamic_cast<classad::ClassAd *>( this->ea_ad.Lookup(ea_s_TotalLocalRusage) );
      if( temporary_ad )
	ntev->total_local_rusage = classad_to_rusage( temporary_ad );

      temporary_ad = dynamic_cast<classad::ClassAd *>( this->ea_ad.Lookup(ea_s_TotalRemoteRusage) );
      if( temporary_ad )
	ntev->total_remote_rusage = classad_to_rusage( temporary_ad );

      break;
    }
    case ULOG_POST_SCRIPT_TERMINATED: {
      PostScriptTerminatedEvent   *pstev = dynamic_cast<PostScriptTerminatedEvent *>( event.get() );

      this->ea_ad.EvaluateAttrBool( ea_s_Normal, pstev->normal );
      this->ea_ad.EvaluateAttrNumber( ea_s_ReturnValue, pstev->returnValue );
      this->ea_ad.EvaluateAttrNumber( ea_s_SignalNumber, pstev->signalNumber );

      break;
    }
    case ULOG_GLOBUS_SUBMIT: {
      GlobusSubmitEvent    *gsev = dynamic_cast<GlobusSubmitEvent *>( event.get() );

      this->ea_ad.EvaluateAttrBool( ea_s_RestartableJM, gsev->restartableJM );

      this->ea_ad.EvaluateAttrString( ea_s_RmContact, string_value );
      gsev->rmContact = local_strdup( string_value );

      this->ea_ad.EvaluateAttrString( ea_s_JmContact, string_value );
      gsev->jmContact = local_strdup( string_value );

      break;
    }
    case ULOG_GLOBUS_SUBMIT_FAILED: {
      GlobusSubmitFailedEvent   *gsfev = dynamic_cast<GlobusSubmitFailedEvent *>( event.get() );

      this->ea_ad.EvaluateAttrString( ea_s_Reason, string_value );
      gsfev->reason = local_strdup( string_value );

      break;
    }
    case ULOG_GLOBUS_RESOURCE_UP: {
      GlobusResourceUpEvent   *gruev = dynamic_cast<GlobusResourceUpEvent *>( event.get() );

      this->ea_ad.EvaluateAttrString( ea_s_RmContact, string_value );
      gruev->rmContact = local_strdup( string_value );

      break;
    }
    case ULOG_GLOBUS_RESOURCE_DOWN: {
      GlobusResourceDownEvent  *grdev = dynamic_cast<GlobusResourceDownEvent *>( event.get() );

      this->ea_ad.EvaluateAttrString( ea_s_RmContact, string_value );
      grdev->rmContact = local_strdup( string_value );

      break;
    }
#if CONDORG_AT_LEAST(6,5,3)
    case ULOG_REMOTE_ERROR: {
      RemoteErrorEvent        *reev = dynamic_cast<RemoteErrorEvent *>( event.get() );
      
      this->ea_ad.EvaluateAttrString( ea_s_ExecuteHost, string_value );
      reev->setExecuteHost( local_strdup(string_value) );

      this->ea_ad.EvaluateAttrString( ea_s_DaemonName, string_value );
      reev->setDaemonName( local_strdup(string_value) );

      this->ea_ad.EvaluateAttrString( ea_s_ErrorStr, string_value );
      reev->setErrorText( local_strdup(string_value) );

      this->ea_ad.EvaluateAttrBool( ea_s_CriticalError, boolean_value );
      reev->setCriticalError( boolean_value );
     
      break;
    }
#endif
#if CONDORG_AT_LEAST(6,7,14)
    case ULOG_GRID_SUBMIT: {
      GridSubmitEvent    *gsev = dynamic_cast<GridSubmitEvent *>( event.get() );

      this->ea_ad.EvaluateAttrString( ea_s_ResourceName, string_value );
      gsev->resourceName = local_strdup( string_value );

      this->ea_ad.EvaluateAttrString( ea_s_JobId, string_value );
      gsev->jobId = local_strdup( string_value );

      break;
    }
    case ULOG_GRID_RESOURCE_UP: {
      GridResourceUpEvent   *gruev = dynamic_cast<GridResourceUpEvent *>( event.get() );

      this->ea_ad.EvaluateAttrString( ea_s_ResourceName, string_value );
      gruev->resourceName = local_strdup( string_value );

      break;
    }
    case ULOG_GRID_RESOURCE_DOWN: {
      GridResourceDownEvent   *grdev = dynamic_cast<GridResourceDownEvent *>( event.get() );

      this->ea_ad.EvaluateAttrString( ea_s_ResourceName, string_value );
      grdev->resourceName = local_strdup( string_value );

      break;
    }
#endif // 6.7.14 and beyond
    default: // Nothing to do, by now..
      break;
    }
  }

  return event.release();
}

EventAd &EventAd::set_time( time_t epoch )
{
  this->ea_ad.InsertAttr( ea_s_EventTime, boost::lexical_cast<string>(epoch) );

  return *this;
}

}; // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END;
