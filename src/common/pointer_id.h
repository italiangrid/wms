/* Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://www.eu-egee.org/partners/ for details on the copyright
 * holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License. */
#ifndef EDG_WORKLOAD_JOBCONTROL_COMMON_POINTERID_H
#define EDG_WORKLOAD_JOBCONTROL_COMMON_POINTERID_H

#include <list>
#include <string>

#include <boost/shared_ptr.hpp>

#include "common/filelist.h"
#include "jobcontrol_namespace.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

class PointerId {
private:
  typedef glite::wms::jobsubmission::jccommon::FileList<classad::ClassAd> FileList;

public:
  PointerId( void );
  PointerId( FileList::iterator &position );
  PointerId( FileList::iterator &position, const std::string &condorId, const std::string &edgId, const std::string &seqcode );
  PointerId( const std::string &condorId, const std::string &edgId );
  ~PointerId( void );

  inline int condor_status( void ) const { return this->pi_condorStatus; }
  inline int last_status( void ) const { return this->pi_lastStatus; }
  inline int retry_count( void ) const { return this->pi_retryCount; }
  inline const std::string &condor_id( void ) const { return this->pi_condorId; }
  inline const std::string &edg_id( void ) const { return this->pi_edgId; }
  inline const std::string &sequence_code( void ) const { return this->pi_sequenceCode; }
  inline const FileList::iterator &position( void ) const { return this->pi_position; }
  inline PointerId &unset_position( void ) { this->pi_position.reset(); return *this; }

  const classad::ClassAd &job_ad( void );
  std::string proxy_file( void );

  PointerId &reset( FileList::iterator &position );
  PointerId &reset( const std::string &condorId, const std::string &edgId );
  PointerId &reset( FileList::iterator &position, const std::string &seqcode );

  inline static const char *condorId( void ) { return pi_s_CondorId; }
  inline static const char *edgId( void ) { return pi_s_EdgId; }
  inline static const char *sequenceCode( void ) { return pi_s_SequenceCode; }
  inline static const char *condorStatus( void ) { return pi_s_CondorStatus; }
  inline static const char *lastStatus( void ) { return pi_s_LastStatus; }
  inline static const char *retryCount( void ) { return pi_s_RetryCount; }

private:
  int                                  pi_condorStatus, pi_lastStatus, pi_retryCount;
  FileList::iterator                   pi_position;
  std::string                          pi_condorId, pi_edgId, pi_sequenceCode;
  boost::shared_ptr<classad::ClassAd>  pi_jobAd;

  static const char       *pi_s_CondorId, *pi_s_EdgId, *pi_s_SequenceCode, *pi_s_CondorStatus, *pi_s_LastStatus, *pi_s_RetryCount;
};

class EdgId {
public:
  EdgId( std::list<PointerId>::iterator &position );
  EdgId( const EdgId &id );
  ~EdgId( void );

  inline const std::string &edg_id( void ) const { return this->ei_edgId; }
  inline const std::string &condor_id( void ) const { return this->ei_position->condor_id(); }
  inline const std::string &sequence_code( void ) const { return this->ei_position->sequence_code(); }

  inline const std::list<PointerId>::iterator &position( void ) const { return this->ei_position; }

  EdgId &operator=( const EdgId &that );

private:
  std::list<PointerId>::iterator     ei_position;
  std::string                        ei_edgId;
};

class CondorId {
public:
  CondorId( std::list<PointerId>::iterator &position );
  CondorId( const CondorId &id );
  ~CondorId( void );

  inline const std::string &edg_id( void ) const { return this->ci_position->edg_id(); }
  inline const std::string &condor_id( void ) const { return this->ci_condorId; }
  inline const std::string &sequence_code( void ) const { return this->ci_position->sequence_code(); }

  inline const std::list<PointerId>::iterator &position( void ) const { return this->ci_position; }

  CondorId &operator=( const CondorId &id );

private:
  std::list<PointerId>::iterator     ci_position;
  std::string                        ci_condorId;
};

} // namespace common
} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_COMMON_POINTERID_H */
