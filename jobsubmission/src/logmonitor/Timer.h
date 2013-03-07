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
#ifndef EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_TIMER_H
#define EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_TIMER_H

#include <iostream>

#include <ctime>

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>

#include <classad_distribution.h>

#include "common/filelist.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

class ULogEvent;

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor {

class Timer;

class TimeoutEvent {
  friend class Timer;

private:
  typedef glite::wms::jobsubmission::jccommon::FileList<classad::ClassAd> FileContainer;

public:
  ~TimeoutEvent( void );

  inline std::time_t timeout( void ) { if( !this->te_event ) this->to_event(); return this->te_epoch; }
  inline FileContainer::iterator &pointer( void ) { return this->te_pointer; }

  inline TimeoutEvent &pointer( FileContainer::iterator &ptr ) { this->te_pointer = ptr; return *this; }

  bool expired( void );
  classad::ClassAd *to_classad( void );
  ULogEvent *to_event( void );

private:
  TimeoutEvent( std::time_t epoch, ULogEvent *event );
  TimeoutEvent( const classad::ClassAd &ad );

  TimeoutEvent( const TimeoutEvent &ev ); // Not implemented
  TimeoutEvent &operator=( const TimeoutEvent &ev ); // Not implemented

  std::time_t                          te_epoch;
  boost::shared_ptr<ULogEvent>         te_event;
  boost::shared_ptr<classad::ClassAd>  te_classad;
  FileContainer::iterator              te_pointer;

  static const char     *te_s_Timeout, *te_s_EventNumber, *te_s_EventTime, *te_s_Cluster, *te_s_Proc, *te_s_SubProc;
  static const char     *te_s_LogNotes;
  static const char     *te_s_ExecuteHost, *te_s_Node;
  static const char     *te_s_ExecErrorType;
  static const char     *te_s_RunLocalRusage, *te_s_RunRemoteRusage, *te_s_TotalLocalRusage, *te_s_TotalRemoteRusage;
  static const char     *te_s_SentBytes, *te_s_RecvdBytes, *te_s_Terminate, *te_s_Normal, *te_s_ReturnValue, *te_s_SignalNumber;
  static const char     *te_s_Reason, *te_s_CoreFile, *te_s_TotalSentBytes, *te_s_TotalRecvdBytes;
  static const char     *te_s_Size, *te_s_Message, *te_s_Info, *te_s_NumPids;
  static const char     *te_s_RmContact, *te_s_JmContact, *te_s_RestartableJM;
  static const char     *te_s_CheckPointed;
  static const char     *te_s_DaemonName, *te_s_ErrorStr, *te_s_CriticalError;
  static const char     *te_s_ReasonCode, *te_s_ReasonSubCode, *te_s_UserNotes;
  static const char      *te_s_StartdAddr, *te_s_StartdName, *te_s_StarterAddr;
  static const char      *te_s_DisconnReason, *te_s_NoReconnReason, *te_s_CanReconn;
  static const char     *te_s_ResourceName, *te_s_JobId; 
};

class Timer {
public:
  typedef  boost::shared_ptr<TimeoutEvent>   EventPointer;

private:
  typedef std::multimap<std::time_t, EventPointer> EventMap;
  typedef glite::wms::jobsubmission::jccommon::FileList<classad::ClassAd> FileContainer;

public:
  typedef  EventMap::iterator                EventIterator;

  Timer( const std::string &backup );
  ~Timer( void );

  Timer &start_timer( std::time_t expire, ULogEvent *ptr );
  Timer &remove_all_timeouts( int condorid );
  // Looking for a timeout attached to the event eventcode for the job condorid
  Timer &remove_timeout( int condorid, int eventcode );
  Timer &remove_timeout( EventIterator &evIt );

  inline const std::string &filename( void ) const { return this->t_filename; }
  inline EventIterator begin( void ) { return this->t_events.begin(); }
  inline EventIterator end( void ) { return this->t_events.end(); }

private:
  void restoreFromFile( void );

  std::string    t_filename;
  EventMap       t_events;
  FileContainer  t_backup;
};

} // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_LOGMONITOR_TIMER_H */

// Local Variables:
// mode: c++
// End:
