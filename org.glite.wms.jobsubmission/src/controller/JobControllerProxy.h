#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERPROXY_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERPROXY_H

// File: JobControllerProxy.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//         Rosario Peluso <Rosario.Peluso@pd.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <classad_distribution.h>

#include "glite/wms/common/utilities/FileList.h"
#include "glite/wms/common/utilities/FileListLock.h"

#include "common/EventLogger.h"
#include "JobControllerImpl.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class JobControllerProxy: public JobControllerImpl {
private:
  typedef glite::wms::common::utilities::FileList<classad::ClassAd>    queue_type;
  typedef glite::wms::common::utilities::FileListMutex                 mutex_type;

public:
  JobControllerProxy( queue_type &q, mutex_type &m, edg_wll_Context *cont );
  ~JobControllerProxy( void );

  virtual int submit( const classad::ClassAd *ad );
  virtual bool cancel( const glite::wmsutils::jobid::JobId &id, const char *logfile );
  virtual bool cancel( int condorid, const char *logfile );
  virtual size_t queue_size( void );

private:
  JobControllerProxy( const JobControllerProxy &rhs ); // Not implemented
  JobControllerProxy &operator=( const JobControllerProxy &rhs ); // Not implemented

  int                      jcp_source;
  mutex_type              &jcp_mutex;
  queue_type              &jcp_queue;
  jccommon::EventLogger    jcp_logger;
};

}; // namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERPROXY_H */

// Local Variables:
// mode: c++
// End:
