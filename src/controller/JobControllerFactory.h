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
#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERFACTORY_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERFACTORY_H

// File: JobControllerFactory.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <memory>
#include <classad_distribution.h>
#include <boost/shared_ptr.hpp>

#include "glite/wms/common/utilities/jobdir.h"
#include "jobcontrol_namespace.h"

typedef  struct _edg_wll_Context  *edg_wll_Context;

JOBCONTROL_NAMESPACE_BEGIN {

namespace utilities = glite::wms::common::utilities;

namespace controller {

class JobControllerImpl;
class JobControllerClientImpl;

class Empty {};

class JobControllerFactory {
  friend class Empty;

public:
  JobControllerFactory();

  JobControllerImpl *create_server( edg_wll_Context *cont );
  JobControllerClientImpl *create_client( void );
  static JobControllerFactory *instance( void );
  void createQueue();

private:
  boost::shared_ptr<utilities::JobDir>                      jcf_jobdir;

  static JobControllerFactory *jcf_s_instance;
};

} // namespace controller

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLERFACTORY_H */

// Local Variables:
// mode: c++
// End:
