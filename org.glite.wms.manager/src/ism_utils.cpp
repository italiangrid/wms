/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the
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


// $Id: ism_utils.cpp,v 1.1.2.7.2.1.2.2.2.7 2012/06/22 11:51:21 mcecchi Exp $

#include <vector>

#include <boost/bind.hpp>
#include "ism_utils.h"
#include "signal_handling.h"
#include "dynamic_library.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/ism/ism.h"

#include "glite/wms/ism/purchaser/ism-purchaser.h"
#include "glite/wms/ism/purchaser/ism-ii-purchaser.h"

namespace configuration = glite::wms::common::configuration;
namespace ca = glite::wmsutils::classads;
namespace purchaser = glite::wms::ism::purchaser;
namespace ism = glite::wms::ism;

namespace glite {
namespace wms {
namespace manager {
namespace main {

namespace {

typedef boost::shared_ptr<server::DynamicLibrary> DynamicLibraryPtr;

DynamicLibraryPtr make_dll(std::string const& lib_name)
{
  return DynamicLibraryPtr(new server::DynamicLibrary(lib_name));
}

void load_dlls_and_create_purchasers(
  std::vector<DynamicLibraryPtr>& dlls,
  std::vector<
    boost::shared_ptr<purchaser::ism_purchaser>
  >& purchasers
)
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );

  purchaser::exit_predicate_type const exit_predicate(
    server::received_quit_signal
  );
  purchaser::skip_predicate_type const skip_predicate(
    purchaser::is_in_black_list(config.wm()->ism_black_list())
  );

  ism::purchaser::exec_mode_t mode;
  if (config.wm()->ism_threads()) {
    mode = purchaser::loop;
  } else {
    mode = purchaser::once;
  }

  purchaser::ii::create_entry_update_fn_t* ii_update_function = 0;
  {
    Info("loading II purchaser");

    std::string const glue_dll_name(config.wm()->ii_glue_lib());
    DynamicLibraryPtr glue_dll(make_dll(glue_dll_name));

    std::string const create_fun_name("create_ii_purchaser");
    purchaser::ii::create_t* create_fun;
    glue_dll->lookup(create_fun_name, create_fun);

    std::string const destroy_fun_name("destroy_ii_purchaser");
    purchaser::ii::destroy_t* destroy_fun;
    glue_dll->lookup(destroy_fun_name, destroy_fun);

    Info("first synchronous II purchasing in progress...");
    boost::shared_ptr<purchaser::ism_purchaser> first_ii_purchasing(
      create_fun(
        config.ns()->ii_contact(),
        config.ns()->ii_port(),
        config.ns()->ii_dn(),
        config.ns()->ii_timeout(),
        config.wm()->ism_ii_ldapcefilter_ext(),
        config.wm()->ism_ii_g2_ldapcefilter_ext(),
        config.wm()->ism_ii_g2_ldapsefilter_ext(),
        false, // async purchasing has been discarded
        purchaser::once, // always once here
        0,
        exit_predicate,
        skip_predicate
      ),
     destroy_fun
    );
    (*first_ii_purchasing)(); // first time is done synchronously
    boost::shared_ptr<purchaser::ism_purchaser> purchaser(
      create_fun(
        config.ns()->ii_contact(),
        config.ns()->ii_port(),
        config.ns()->ii_dn(),
        config.ns()->ii_timeout(),
        config.wm()->ism_ii_ldapcefilter_ext(),
        config.wm()->ism_ii_g2_ldapcefilter_ext(),
        config.wm()->ism_ii_g2_ldapsefilter_ext(),
        false,
        mode,
        config.wm()->ism_ii_purchasing_rate(),
        exit_predicate,
        skip_predicate
      ),
     destroy_fun
    );
    dlls.push_back(glue_dll);
    purchasers.push_back(purchaser);
    std::string const update_fun_name("create_ii_entry_update_fn");
    glue_dll->lookup(update_fun_name, ii_update_function);
  }
}

}

struct ISM_manager::Impl : boost::noncopyable
{
  ism::ism_type m_ism_1[2];
  ism::ism_type m_ism_2[2];
  ism::ism_mutex_type m_ism_mutex[2];
  std::vector<DynamicLibraryPtr> m_dlls;
  std::vector<boost::shared_ptr<purchaser::ism_purchaser> > m_purchasers;
};

ISM_manager::ISM_manager()
  : m_impl(new Impl)
{
  ism::set_ism(m_impl->m_ism_1, m_impl->m_ism_2, m_impl->m_ism_mutex, ism::ce);
  ism::set_ism(m_impl->m_ism_1, m_impl->m_ism_2, m_impl->m_ism_mutex, ism::se);

  load_dlls_and_create_purchasers(m_impl->m_dlls, m_impl->m_purchasers);
}

std::vector<
  boost::shared_ptr<purchaser::ism_purchaser>
>& ISM_manager::purchasers()
{
  return m_impl->m_purchasers;
}

}}}} // glite::wms::manager::main
