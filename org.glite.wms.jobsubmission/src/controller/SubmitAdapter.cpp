#include <iostream>
#include <fstream>
#include <string>

#include <classad_distribution.h>

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#include "glite/wms/jobsubmission/SubmitAdapter.h"
#include "SubmitAd.h"

USING_COMMON_NAMESPACE;
using namespace std;
RenameLogStreamNS_ts(ts);

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

SubmitAdapter::SubmitAdapter(classad::ClassAd* inad)
  : sa_good(true), sa_sad(new SubmitAd(inad)), sa_seqcode()
{ }

void SubmitAdapter::createFromAd(classad::ClassAd* pad)
{
  sa_sad->createFromAd(pad);
}

void SubmitAdapter::adapt()
{
  if (this->sa_good) {
    try {
      this->sa_sad->set_sequence_code(this->sa_seqcode);
//TODO restore?
//      if ((this->sa_good = this->sa_sad->good())) {
//	      ofstream ofs(this->sa_sad->classad_file().c_str());
//	      if((this->sa_good = ofs.good())) {
//	        ofs << this->sa_sad->classad() << endl;
//	        ts::edglog << logger::setlevel( logger::medium ) << "Classad file created..." << endl;
//	      }
//      }
    } catch (...) { // TODO specialise
      this->sa_good = false;
    }
  }

  return;
}

void SubmitAdapter::adapt_for_submission(const string &seqcode)
{
  this->sa_seqcode = seqcode;
  this->adapt();
}

}; // Namespace controller

} JOBCONTROL_NAMESPACE_END;
