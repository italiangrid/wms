#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITADAPTER_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITADAPTER_H

#include <string>
#include <memory>

#include <classad_distribution.h>
#include "SubmitAd.h"

namespace classad {
  class ClassAd;
};

namespace glite {
namespace wms {
namespace jobsubmission {

namespace controller {

class SubmitAd;

class SubmitAdapter {
public:
  SubmitAdapter(classad::ClassAd* inad);

  SubmitAd const* operator->() const { return this->sa_sad; }
  void adapt_for_submission(std::string const& seqcode = "");
  bool good() { return sa_good; }
  classad::ClassAd* classad() { return sa_sad->classad_ptr(); }
  void createFromAd(classad::ClassAd* pad);
private:
  void adapt();

  bool sa_good;
  SubmitAd* sa_sad;
  std::string sa_seqcode;
};

void adapt_for_submission(classad::ClassAd* inad)
{
  SubmitAdapter adapter(inad);
}

}}}}
#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITADAPTER_H */

// Local Variables:
// mode: c++
// End:
