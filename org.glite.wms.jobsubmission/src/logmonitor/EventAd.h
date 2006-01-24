#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_EVENTAD_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_EVENTAD_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor {

class EventAd {
public:
  EventAd( void );
  EventAd( const classad::ClassAd *ad );
  EventAd( const ULogEvent *event );
  ~EventAd( void );

  inline EventAd &from_classad( const classad::ClassAd *ad )
  {
    this->ea_ad.Update(*static_cast<classad::ClassAd*>(ad->Copy()));
    return *this;
  }
  inline const classad::ClassAd &get_classad( void ) { return this->ea_ad; }

  EventAd &set_time( time_t epoch );
  EventAd &from_event( const ULogEvent *event );
  ULogEvent *create_event( void );

private:
  classad::ClassAd      ea_ad;

  static const char   *ea_s_EventTime, *ea_s_EventNumber, *ea_s_Cluster;
  static const char   *ea_s_Proc, *ea_s_SubProc, *ea_s_SubmitHost, *ea_s_LogNotes;
  static const char   *ea_s_ExecuteHost, *ea_s_ExecErrorType, *ea_s_RunLocalRusage, *ea_s_RunRemoteRusage;
  static const char   *ea_s_TotalLocalRusage, *ea_s_TotalRemoteRusage, *ea_s_CheckPointed, *ea_s_SentBytes, *ea_s_RecvdBytes;
  static const char   *ea_s_Terminate, *ea_s_Normal, *ea_s_ReturnValue, *ea_s_SignalNumber, *ea_s_Reason;
  static const char   *ea_s_CoreFile, *ea_s_TotalSentBytes, *ea_s_TotalRecvdBytes, *ea_s_Size, *ea_s_Message;
  static const char   *ea_s_Info, *ea_s_NumPids, *ea_s_Node;
  static const char   *ea_s_RestartableJM, *ea_s_RmContact, *ea_s_JmContact;
#if CONDORG_AT_LEAST(6,5,3)
  static const char   *ea_s_DaemonName, *ea_s_ErrorStr, *ea_s_CriticalError;
  static const char   *ea_s_ReasonCode, *ea_s_ReasonSubCode, *ea_s_UserNotes;
#endif
#if CONDORG_AT_LEAST(6,7,14)
  static const char   *ea_s_JobId, *ea_s_ResourceName;
#endif
};

}; // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_EVENTAD_H */

// Local Variables:
// mode: c++
// End:
