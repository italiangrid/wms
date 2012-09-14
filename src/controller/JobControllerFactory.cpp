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

#include <string>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"

#include "JobControllerFactory.h"
#include "JobControllerProxy.h"
#include "JobControllerReal.h"
#include "JobControllerFake.h"
#include "JobControllerClientJD.h"
#include "JobControllerExceptions.h"

USING_COMMON_NAMESPACE;

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

JobControllerFactory *JobControllerFactory::jcf_s_instance = NULL;

void JobControllerFactory::createQueue()
{
  const configuration::JCConfiguration *jc_config
    = configuration::Configuration::instance()->jc();

  try {
   boost::filesystem::path base(jc_config->input(), boost::filesystem::native);
   this->jcf_jobdir.reset(new utilities::JobDir(base));
  } catch(utilities::JobDirError &error) {
    throw CannotCreate(error.what());
  }
}

JobControllerFactory::JobControllerFactory()
{
  const configuration::Configuration *configure
    = configuration::Configuration::instance();

  if(configure->get_module() != configuration::ModuleType::job_controller) {
    this->createQueue();
  }
}

JobControllerFactory *JobControllerFactory::instance( void )
{
  if( jcf_s_instance == NULL ) jcf_s_instance = new JobControllerFactory;

  return jcf_s_instance;
}

JobControllerImpl *JobControllerFactory::create_server( edg_wll_Context *cont )
{
  const configuration::Configuration      *configure = configuration::Configuration::instance();
  JobControllerImpl                       *result = NULL;

  if( configure->get_module() == configuration::ModuleType::job_controller ) {
    if( configure->jc()->use_fake_for_real() ) result = new JobControllerFake;
    else result = new JobControllerReal( cont );
  }
  else {
    if( configure->jc()->use_fake_for_proxy() ) result = new JobControllerFake;
    else result = new JobControllerProxy(this->jcf_jobdir, cont);
  }

  return result;
}

JobControllerClientImpl *JobControllerFactory::create_client( void )
{
  const configuration::Configuration      *configure = configuration::Configuration::instance();
  JobControllerClientImpl                 *result = NULL;

  if( configure->get_module() == configuration::ModuleType::job_controller ) {
      result = new JobControllerClientJD();
  } else {
    result = new JobControllerClientUnknown();
  }

  return result;
}

} // namespace controller

} JOBCONTROL_NAMESPACE_END
