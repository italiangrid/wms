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
#include <string>

#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/security/proxyrenewal/renewal.h"
#include "jobcontrol_namespace.h"

#include "ProxyUnregistrar.h"

USING_COMMON_NAMESPACE;
using namespace std;

namespace ts = glite::wms::common::logger::threadsafe;

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

ProxyUnregistrar::ProxyUnregistrar( const string &id ) : pu_id( id ) {}

ProxyUnregistrar::~ProxyUnregistrar( void ) {}

void ProxyUnregistrar::unregister( void )
{
  int      err = 0;
  logger::StatePusher      pusher( ts::edglog, "ProxyUnregistrar::unregister()" );

  ts::edglog << logger::setlevel( logger::verylow )
	     << "Unregistering user proxy..." << endl;

  err = glite_renewal_UnregisterProxy( this->pu_id.c_str(), NULL );

  if( err && (err != EDG_WLPR_PROXY_NOT_REGISTERED) )
    ts::edglog << logger::setlevel( logger::null ) << "I cannot unregister the job proxy." << endl
	       << "Reason: \"" << edg_wlpr_GetErrorText(err) << "\"." << endl;
  else if( err == EDG_WLPR_PROXY_NOT_REGISTERED )
    ts::edglog << logger::setlevel( logger::null ) << "Job proxy not registered. Going ahead." << endl;

  return;
}

} // Namespace jccommon

} JOBCONTROL_NAMESPACE_END
