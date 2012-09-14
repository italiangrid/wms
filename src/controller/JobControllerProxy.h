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
#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERPROXY_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERPROXY_H

// File: JobControllerProxy.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//         Rosario Peluso <Rosario.Peluso@pd.infn.it>
// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

// $Id$

#include <classad_distribution.h>

#include "glite/wms/common/utilities/jobdir.h"

#include "common/EventLogger.h"
#include "JobControllerImpl.h"

namespace utils = glite::wms::common::utilities;

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class JobControllerProxy: public JobControllerImpl {
  JobControllerProxy( const JobControllerProxy &rhs ); // Not implemented
  JobControllerProxy &operator=( const JobControllerProxy &rhs ); // Not implemented

  int                      jcp_source;

  boost::shared_ptr<utils::JobDir>                      jcp_jobdir;

  jccommon::EventLogger    jcp_logger;
public:
  JobControllerProxy(
    boost::shared_ptr<utils::JobDir> jcp_jd,
    edg_wll_Context *cont
  );

  virtual int submit( const classad::ClassAd *ad );
  virtual bool cancel( const glite::jobid::JobId &id, const char *logfile );
  virtual bool cancel( int condorid, const char *logfile );
  virtual bool release(int condorid, char const* logfile);
  virtual size_t queue_size( void );
};

} // namespace controller

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERPROXY_H */

// Local Variables:
// mode: c++
// End:
