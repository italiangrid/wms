#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITADAPTER_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITADAPTER_H

#include <string>
#include <memory>

#include "../jobcontrol_namespace.h"

namespace classad {
  class ClassAd;
};

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class SubmitAd;

class SubmitAdapter {
public:
  SubmitAdapter( const classad::ClassAd &inad );
  ~SubmitAdapter( void );

  inline const SubmitAd *operator->( void ) const { return this->sa_sad.get(); }

  classad::ClassAd *adapt_for_submission( const std::string &seqcode = "" );

private:
  void adapt( void );

  bool                      sa_good;
  std::auto_ptr<SubmitAd>   sa_sad;
  std::string               sa_seqcode;
};

inline classad::ClassAd *adapt_for_submission( const classad::ClassAd &inad )
{
  SubmitAdapter    adapter( inad );

  return adapter.adapt_for_submission();
}

}; // Namespace controlelr

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITADAPTER_H */

// Local Variables:
// mode: c++
// End:
