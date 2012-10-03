/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
#ifndef GLITE_WMS_ISM_PURCHASER_ISM_FILE_PURCHASER_H
#define GLITE_WMS_ISM_PURCHASER_ISM_FILE_PURCHASER_H

#include <string>
#include <vector>
#include "glite/wms/ism/purchaser/common.h"
#include "glite/wms/ism/purchaser/ism-purchaser.h"
#include "glite/wms/ism/purchaser/ism-ii-purchaser.h"
#include "glite/wms/ism/purchaser/ism-cemon-purchaser.h"
#include "glite/wms/ism/purchaser/ism-rgma-purchaser.h"

namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

class ism_file_purchaser : public ism_purchaser
{
public:
                
  ism_file_purchaser(
    std::string const& file,
    exec_mode_t mode = once,
    size_t interval = 30,
    exit_predicate_type exit_predicate = exit_predicate_type(),
    skip_predicate_type skip_predicate = skip_predicate_type()
  );

  void operator()();

private:                
  std::string              m_filename;
};

namespace file {
// the types of the class factories
typedef ism_file_purchaser* create_t(
    std::string const& file,
    exec_mode_t mode = once,
    size_t interval = 30,
    exit_predicate_type exit_predicate = exit_predicate_type(),
    skip_predicate_type skip_predicate = skip_predicate_type()
);

typedef void destroy_t(ism_file_purchaser*);

// the type of the set entry update factory function
typedef void set_purchaser_entry_update_fns_t(
  ii::create_entry_update_fn_t*,
  cemon::create_entry_update_fn_t*,
  rgma::create_entry_update_fn_t*
);
}

} // namespace purchaser
} // namespace ism
} // namespace wms
} // namespace glite

#endif
