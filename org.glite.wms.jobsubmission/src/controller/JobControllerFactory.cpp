// File: JobControllerFactory.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <string>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"

#include "JobControllerFactory.h"
#include "JobControllerProxy.h"
#include "JobControllerReal.h"
#include "JobControllerFake.h"
#include "JobControllerClientReal.h"
#include "JobControllerClientJD.h"
#include "JobControllerExceptions.h"

USING_COMMON_NAMESPACE;

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

JobControllerFactory *JobControllerFactory::jcf_s_instance = NULL;

void JobControllerFactory::createQueue()
{
  const configuration::JCConfiguration 
    *config = configuration::Configuration::instance()->jc();

  if (config->input_type() == "filelist") {
    try {
      this->jcf_queue.reset(new queue_type(config->input()));
      this->jcf_mutex.reset(new mutex_type(*this->jcf_queue));
    } catch (utilities::FileContainerError const& e) {
      throw CannotCreate(e.string_error());
    }  
  } else {
    try {
     this->jcf_jobdir.reset(
        new utilities::JobDir(
        boost::filesystem::path(config->input(), boost::filesystem::native)
      )
    );
    } catch(utilities::JobDirError const& e) {
      throw CannotCreate(e.what());
    }
  }
}

JobControllerFactory::JobControllerFactory()
{
  configuration::Configuration const* const configure
    = configuration::Configuration::instance();

  if (configure->get_module() != configuration::ModuleType::job_controller) {
    this->createQueue();
  }
}

JobControllerFactory *JobControllerFactory::instance()
{
  if (!jcf_s_instance) {
    jcf_s_instance = new JobControllerFactory;
  }

  return jcf_s_instance;
}

JobControllerImpl *JobControllerFactory::create_server(edg_wll_Context *cont)
{
  const configuration::Configuration *configure = configuration::Configuration::instance();
  JobControllerImpl *result = NULL;

  if (configure->get_module() == configuration::ModuleType::job_controller) {
    if (configure->jc()->use_fake_for_real()) {
      result = new JobControllerFake;
    } else {
      result = new JobControllerReal(cont);
    }
  } else {
    if (configure->jc()->use_fake_for_proxy()) {
      result = new JobControllerFake;
    } else {
      result = new JobControllerProxy(this->jcf_queue, this->jcf_mutex, this->jcf_jobdir, cont);
    }
  }

  return result;
}

JobControllerClientImpl *JobControllerFactory::create_client()
{
  const configuration::Configuration      *configure = configuration::Configuration::instance();
  JobControllerClientImpl                 *result = 0;

  if (configure->get_module() == configuration::ModuleType::job_controller) {
    if (configure->jc()->input_type() == "filelist") {
      result = new JobControllerClientReal();
    } else {
      result = new JobControllerClientJD();
    }
  }

  return result;
}

}; // namespace controller

} JOBCONTROL_NAMESPACE_END;
