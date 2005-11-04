#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLER_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLER_H

// File: JobController.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//         Rosario Peluso <Rosario.Peluso@pd.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <cstdio>

#include "../jobcontrol_namespace.h"
#include <glite/wmsutils/jobid/JobId.h>

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
  bool cancel( const glite::wmsutils::jobid::JobId &id, const char *logfile = NULL, bool force = false );
  bool cancel( int condorid, const char *logfile = NULL, bool force = true );
  size_t queue_size( void );

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
