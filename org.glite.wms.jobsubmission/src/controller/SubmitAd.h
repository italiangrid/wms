#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITAD_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITAD_H

#include <memory>
#include <string>

#include <ctime>

#include <boost/shared_ptr.hpp>

#include "jobcontrol_namespace.h"

namespace classad { class ClassAd; }

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class SubmitAd {
public:
  SubmitAd(classad::ClassAd * ad);

  bool good() const { return this->sa_good; }
  bool is_dag() const { return this->sa_isDag; }
  bool is_subjob() const { return this->sa_hasDagId; }
  classad::ClassAd* classad_ptr() const { return this->sa_ad; }
  operator const classad::ClassAd&() const { return *this->sa_ad; }
  const std::string &reason() const { return this->sa_reason; }
  const std::string &job_id() const { return this->sa_jobid; }
  const std::string &dag_id() const { return this->sa_dagid; }
  const std::string &submit_file() const { return this->sa_submitfile; }
  const std::string &classad_file() const { return this->sa_classadfile; }
  const std::string &log_file() const { return this->sa_logfile; }
  const classad::ClassAd& classad() const { return *this->sa_ad; }
  void createFromAd(classad::ClassAd *ad);

  inline SubmitAd &create(classad::ClassAd *ad) { this->createFromAd( ad ); return *this; }

  SubmitAd &set_sequence_code( const std::string &code );

private:
  void loadStatus();
  void saveStatus();

  bool sa_good, sa_last, sa_hasDagId, sa_isDag;
  int sa_jobperlog;
  time_t sa_lastEpoch;
  classad::ClassAd *sa_ad;
  std::string sa_jobid, sa_dagid, sa_jobtype;
  std::string sa_submitfile, sa_submitad, sa_reason, sa_seqcode, sa_classadfile, sa_logfile;
};

}; // Namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITAD_H */

// Local Variables:
// mode: c++
// End:
