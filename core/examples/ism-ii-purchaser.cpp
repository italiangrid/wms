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
#include <vector>
#include <string>
#include <exception>

#include "glite/wms/ism/purchaser/ism-ii-purchaser.h"
#include "glite/wms/ism/purchaser/ism-file-purchaser.h"
#include "glite/wms/ism/ism.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"

#include "glite/wms/common/logger/edglog.h"

namespace purchaser = glite::wms::ism::purchaser;
namespace ism       = glite::wms::ism;
namespace logger    = glite::wms::common::logger::threadsafe;
namespace configuration = glite::wms::common::configuration;

std::string opt_conf_file("ism_wms.conf");

int main(void)
{
  logger::edglog.open(std::clog, glite::wms::common::logger::debug);

  configuration::Configuration config(opt_conf_file,
                                      configuration::ModuleType::workload_manager);

  configuration::WMConfiguration const* const wm_config(config.wm());
  configuration::NSConfiguration const* const ns_config(config.ns()); 
while (true) {
  purchaser::ism_ii_purchaser icp(ns_config->ii_contact(), 
				  ns_config->ii_port(),
				  ns_config->ii_dn(),
				  ns_config->ii_timeout(),
				  purchaser::once);

  try { 
    icp();

    sleep(1);
    ism::call_update_ism_entries()();
    ism::call_dump_ism_entries()();

    std::string filename(ism::get_ism_dump());
    purchaser::ism_file_purchaser ifp(filename);

  } catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

  return 0;	
}
