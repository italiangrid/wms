// File: JobController.h
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

#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLER_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLER_H


#include <cstdio>

#include "jobcontrol_namespace.h"
#include <glite/jobid/JobId.h>

typedef  struct _edg_wll_Context  *edg_wll_Context;

namespace classad { class ClassAd; }

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class JobControllerImpl;

class JobController
{
public:
  JobController( edg_wll_Context *cont = NULL );
  ~JobController( void );

  int submit( const classad::ClassAd *ad );
  bool cancel( const glite::jobid::JobId &id, const char *logfile = NULL );
  bool cancel( int condorid, const char *logfile = NULL );
  bool release(int condorid, char const* logfile = NULL);
  size_t queue_size( void );

private:
  JobController( const JobController &rhs ); // Not implemented
  JobController &operator=( const JobController& rhs ); // Not implemented

  JobControllerImpl    *jc_impl;
};

} // namespace controller

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLER_H */

// Local Variables:
// mode: c++
// End:
