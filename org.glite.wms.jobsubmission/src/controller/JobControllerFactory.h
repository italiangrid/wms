#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERFACTORY_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERFACTORY_H

// File: JobControllerFactory.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <memory>

#include <classad_distribution.h>

#include "jobcontrol_namespace.h"
#include "glite/wms/common/utilities/FileList.h"
#include "glite/wms/common/utilities/FileListLock.h"

typedef  struct _edg_wll_Context  *edg_wll_Context;

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class JobControllerImpl;
class JobControllerClientImpl;

class Empty {};

class JobControllerFactory {
  friend class Empty;

public:
  ~JobControllerFactory( void );

  JobControllerImpl *create_server( edg_wll_Context *cont );
  JobControllerClientImpl *create_client( void );

  static JobControllerFactory *instance( void );

private:
  typedef glite::wms::common::utilities::FileList<classad::ClassAd>    queue_type;
  typedef glite::wms::common::utilities::FileListMutex                 mutex_type;

  JobControllerFactory( const JobControllerFactory &rhs ); // Not implemented
  JobControllerFactory &operator=( const JobControllerFactory &rhs ); // Not implemented

  JobControllerFactory( void );

  void createQueue( void );

  std::auto_ptr<mutex_type>     jcf_mutex;
  std::auto_ptr<queue_type>     jcf_queue;

  static JobControllerFactory *jcf_s_instance;
};

} // namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERFACTORY_H */

// Local Variables:
// mode: c++
// End:
