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
  inline bool is_dag( void ) const { return this->sa_isDag; }
  inline bool is_subjob( void ) const { return this->sa_hasDagId; }
  inline operator const classad::ClassAd &( void ) const { return *this->sa_ad; }
  inline const std::string &reason( void ) const { return this->sa_reason; }
  inline const std::string &job_id( void ) const { return this->sa_jobid; }
  inline const std::string &dag_id( void ) const { return this->sa_dagid; }
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

  bool                                sa_good, sa_last, sa_hasDagId, sa_isDag;
  int                                 sa_jobperlog;
  time_t                              sa_lastEpoch;
  std::auto_ptr<classad::ClassAd>     sa_ad;
  std::string                         sa_jobid, sa_dagid, sa_jobtype;
  std::string                         sa_submitfile, sa_submitad, sa_reason, sa_seqcode, sa_classadfile, sa_logfile;
};

}; // Namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITAD_H */

// Local Variables:
// mode: c++
// End:
