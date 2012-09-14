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

#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERREAL_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERREAL_H

// File: JobControllerReal.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//         Rosario Peluso <Rosario.Peluso@pd.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <memory>

#include "common/EventLogger.h"
#include "JobControllerImpl.h"

typedef  struct _edg_wll_Context  *edg_wll_Context;

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon { class RamContainer; }

namespace controller {

class JobControllerReal: public JobControllerImpl {
public:
  JobControllerReal( edg_wll_Context *cont );
  virtual ~JobControllerReal( void );

  virtual int submit( const classad::ClassAd *ad );
  virtual bool cancel( const glite::jobid::JobId &id, const char *logfile );
  virtual bool cancel( int condorid, const char *logfile );
  virtual bool release(int condorid, char const* logfile);
  virtual size_t queue_size( void );

private:
  JobControllerReal( const JobControllerReal &rhs );
  JobControllerReal &operator=( const JobControllerReal &rhs );
  void readRepository();

  jccommon::EventLogger                    jcr_logger;
  int                                      jcr_threshold;
  std::auto_ptr<jccommon::RamContainer>    jcr_repository;
};

} // namespace controller

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERREAL_H */

// Local Variables:
// mode: c++
// End:
