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
#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITADAPTER_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITADAPTER_H

#include <string>
#include <memory>

namespace classad {
  class ClassAd;
};

namespace glite { namespace wms { namespace jobsubmission {

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

} // Namespace controlelr

}}} // Namespace jobsubmission wms glite

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITADAPTER_H */

// Local Variables:
// mode: c++
// End:
