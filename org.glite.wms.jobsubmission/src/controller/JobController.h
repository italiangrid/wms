#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLER_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLER_H

// File: JobController.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//         Rosario Peluso <Rosario.Peluso@pd.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <vector>
#include <cstdio>

#include <boost/shared_ptr.hpp>

#include "jobcontrol_namespace.h"
#include <glite/wmsutils/jobid/JobId.h>

namespace classad { class ClassAd; }

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon { class EventLogger; }

namespace controller {

class JobControllerImpl;

class JobController
{
public:
  JobController(boost::shared_ptr<jccommon::EventLogger> ctx);
  ~JobController();

  int msubmit(std::vector<classad::ClassAd*>);
  int submit(classad::ClassAd*);
  bool cancel( const glite::wmsutils::jobid::JobId &id, const char *logfile = NULL );
  bool cancel( int condorid, const char *logfile = NULL );

private:
  JobController( const JobController &rhs ); // Not implemented
  JobController &operator=( const JobController& rhs ); // Not implemented

  JobControllerImpl    *jc_impl;
};

} // namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLER_H */

// Local Variables:
// mode: c++
// End:
