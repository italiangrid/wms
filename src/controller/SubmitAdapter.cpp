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
RenameLogStreamNS_ts( ts );

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

SubmitAdapter::SubmitAdapter( const classad::ClassAd &inad ) : sa_good( true ), sa_sad( new SubmitAd(&inad) ), sa_seqcode()
{}

SubmitAdapter::~SubmitAdapter( void ) 
{}

void SubmitAdapter::adapt( void )
{
  if( this->sa_good ) {
    try {
      this->sa_sad->set_sequence_code( this->sa_seqcode );

//      if( (this->sa_good = this->sa_sad->good()) ) {
//	ofstream     ofs( this->sa_sad->classad_file().c_str() );

//	if( (this->sa_good = ofs.good()) ) {
//	  ofs << this->sa_sad->classad() << endl;
//	  ofs.close();

//	  ts::edglog << logger::setlevel( logger::medium ) << "Classad file created..." << endl;
//	}
//      }
    }
    catch( ... ) { this->sa_good = false; }
  }

  return;
}

classad::ClassAd *SubmitAdapter::adapt_for_submission( const string &seqcode )
{
  this->sa_seqcode.assign( seqcode );
  this->adapt();

  return this->sa_good ? static_cast<classad::ClassAd*>(this->sa_sad->classad().Copy()) : NULL;
}

} // Namespace controller

} JOBCONTROL_NAMESPACE_END
