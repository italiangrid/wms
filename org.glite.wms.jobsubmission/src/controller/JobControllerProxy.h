#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERPROXY_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERPROXY_H

// File: JobControllerProxy.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//         Rosario Peluso <Rosario.Peluso@pd.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <vector>
#include <classad_distribution.h>

#include "glite/wms/common/utilities/FileList.h"
#include "glite/wms/common/utilities/FileListLock.h"
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

  boost::shared_ptr<utils::FileListMutex>               jcp_mutex;
  boost::shared_ptr<utils::FileList<classad::ClassAd> > jcp_queue;
  boost::shared_ptr<utils::JobDir>                      jcp_jobdir;

  boost::shared_ptr<jccommon::EventLogger> jcp_logger;
public:
  JobControllerProxy(
    boost::shared_ptr<utils::FileList<classad::ClassAd> >q,
    boost::shared_ptr<utils::FileListMutex> m,
    boost::shared_ptr<utils::JobDir> jcp_jd,
    boost::shared_ptr<jccommon::EventLogger> ctx
  );

  virtual int msubmit(std::vector<classad::ClassAd*>&);
  virtual int submit(classad::ClassAd *ad);
  virtual bool cancel(const glite::wmsutils::jobid::JobId &id, const char *logfile);
  virtual bool cancel(int condorid, const char *logfile);
};

}; // namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERPROXY_H */

// Local Variables:
// mode: c++
// End:
