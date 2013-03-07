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
#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITAD_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITAD_H

#include <ctime>

#include <memory>
#include <string>

#include "jobcontrol_namespace.h"

namespace classad { class ClassAd; }

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class SubmitAd {
public:
  SubmitAd( const classad::ClassAd *ad = NULL );
  ~SubmitAd( void );

  inline bool good( void ) const { return this->sa_good; }
  inline operator const classad::ClassAd &( void ) const { return *this->sa_ad; }
  inline const std::string &reason( void ) const { return this->sa_reason; }
  inline const std::string &job_id( void ) const { return this->sa_jobid; }
  inline const std::string &submit_file( void ) const { return this->sa_submitfile; }
  inline const std::string &classad_file( void ) const { return this->sa_classadfile; }
  inline const std::string &log_file( void ) const { return this->sa_logfile; }
  inline const classad::ClassAd &classad( void ) const { return *this->sa_ad; }

  inline SubmitAd &create( const classad::ClassAd *ad ) { this->createFromAd( ad ); return *this; }

  SubmitAd &set_sequence_code( const std::string &code );

private:
  void createFromAd( const classad::ClassAd *ad );
  void loadStatus( void );
  void saveStatus( void );

  bool                                sa_good, sa_last;
  int                                 sa_jobperlog;
  time_t                              sa_lastEpoch;
  std::auto_ptr<classad::ClassAd>     sa_ad;
  std::string                         sa_jobid, sa_jobtype;
  std::string                         sa_submitfile, sa_submitad, sa_reason, sa_seqcode, sa_classadfile, sa_logfile;
};

} // Namespace controller

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITAD_H */

// Local Variables:
// mode: c++
// End:
