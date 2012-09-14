#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERFACTORY_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERFACTORY_H

// File: JobControllerFactory.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <memory>

#include <boost/shared_ptr.hpp>

#include <classad_distribution.h>

#include "glite/wms/common/utilities/FileList.h"
#include "glite/wms/common/utilities/FileListLock.h"
#include "glite/wms/common/utilities/jobdir.h"
#include "jobcontrol_namespace.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon { class EventLogger; }

namespace controller {

class JobControllerImpl;
class JobControllerClientImpl;

class Empty { };

class JobControllerFactory {
  friend class Empty;

public:
  JobControllerImpl *create_server(boost::shared_ptr<jccommon::EventLogger> ctx);
  JobControllerClientImpl *create_client();
  static JobControllerFactory *instance();

private:
  typedef glite::wms::common::utilities::FileList<classad::ClassAd>    queue_type;
  typedef glite::wms::common::utilities::FileListMutex                 mutex_type;

  JobControllerFactory(const JobControllerFactory &rhs); // Not implemented
  JobControllerFactory &operator=( const JobControllerFactory &rhs ); // Not implemented

  JobControllerFactory( void );

  void createQueue( void );

  boost::shared_ptr<mutex_type> jcf_mutex;
  boost::shared_ptr<queue_type> jcf_queue;
  boost::shared_ptr<glite::wms::common::utilities::JobDir> jcf_jobdir;

  static JobControllerFactory *jcf_s_instance;
};

} // namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERFACTORY_H */

// Local Variables:
// mode: c++
// End:
