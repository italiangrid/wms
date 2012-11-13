// File: ism_utils.cpp
// Author: Francesco Giacomini
// Copyright (c) 2004 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <boost/bind.hpp>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wmsutils/classads/classad_utils.h"

#include "ism_utils.h"
#include "signal_handling.h"
#include "dynamic_library.h"

#include "glite/wms/ism/ism.h"
#include "glite/wms/ism/purchaser.h"

#include "glite/wms/ism/ii-purchaser.h"
#include "glite/wms/ism/cemon-purchaser.h"
#include "glite/wms/ism/cemon-asynch-purchaser.h"
#include "glite/wms/ism/file-purchaser.h"
#include "glite/wms/ism/rgma-purchaser.h"


namespace configuration = glite::wms::common::configuration;
namespace ca = glite::wmsutils::classads;
namespace ism = glite::wms::ism;

namespace glite {
namespace wms {
namespace manager {

namespace main {

namespace {

typedef boost::shared_ptr<server::DynamicLibrary> DynamicLibraryPtr;
typedef boost::shared_ptr<ism::purchaser> PurchaserPtr;

DynamicLibraryPtr make_dll(std::string const& lib_name)
{
  return DynamicLibraryPtr(new server::DynamicLibrary(lib_name));
}

void load_dlls_and_create_purchasers(
  std::vector<DynamicLibraryPtr>& dlls,
  std::vector<PurchaserPtr>& purchasers,
  ism::Ism& the_ism,
  ism::MutexSlicePtr& bdii_ce_mutex_slice,
  ism::MutexSlicePtr& bdii_se_mutex_slice,
  ism::MutexSlicePtr& rgma_ce_mutex_slice,
  ism::MutexSlicePtr& rgma_se_mutex_slice,
  ism::MutexSlicePtr& cemon_ce_mutex_slice,
  ism::MutexSlicePtr& cemon_asynch_ce_mutex_slice
){
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );

  ism::SkipPredicate const the_skip_predicate(
    ism::InBlackList(config.wm()->ism_black_list())
  );

  bool const purchasing_from_rgma_is_enabled(
    config.wm()->enable_purchasing_from_rgma()
  );

  std::vector<std::string> const cemon_services(
    config.wm()->ce_monitor_services()
  );
  bool const purchasing_from_cemon_is_enabled(!cemon_services.empty());

  bool const purchasing_from_cemon_asynch_is_enabled(
    config.wm()->ce_monitor_asynch_port() > 0
  );


  {
    Info("loading II purchaser");

    std::string const dll_name("libglite_wms_ism_ii_purchaser.so");
    DynamicLibraryPtr dll(make_dll(dll_name));

    std::string const create_function_name("create_ii_purchaser");
    ism::ii::create_t* create_function;
    dll->lookup(create_function_name, create_function);

    std::string const destroy_function_name("destroy_ii_purchaser");
    ism::ii::destroy_t* destroy_function;
    dll->lookup(destroy_function_name, destroy_function);

    bdii_ce_mutex_slice.reset( new ism::MutexSlice );
    bdii_se_mutex_slice.reset( new ism::MutexSlice );

    bdii_ce_mutex_slice->slice.reset(new ism::Slice);
    bdii_se_mutex_slice->slice.reset(new ism::Slice);

    the_ism.computing.push_back(bdii_ce_mutex_slice);
    the_ism.storage.push_back(bdii_se_mutex_slice);

    PurchaserPtr purchaser(
      create_function(
        bdii_ce_mutex_slice,
        bdii_se_mutex_slice, 
        config.ns()->ii_contact(),
        config.ns()->ii_port(),
        config.ns()->ii_dn(),
        config.ns()->ii_timeout()
      ),
      destroy_function
    );
    purchaser->set_loop_mode(true, config.wm()->ism_ii_purchasing_rate());
    purchaser->skip_predicate(the_skip_predicate);

    dlls.push_back(dll);
    purchasers.push_back(purchaser);

  }

  if (purchasing_from_rgma_is_enabled) {

    Info("loading rgma purchaser");

    std::string const dll_name("libglite_wms_ism_rgma_purchaser.so");
    DynamicLibraryPtr dll(make_dll(dll_name));

    std::string const create_function_name("create_rgma_purchaser");
    ism::rgma::create_t* create_function;
    dll->lookup(create_function_name, create_function);

    std::string const destroy_function_name("destroy_rgma_purchaser");
    ism::rgma::destroy_t* destroy_function;
    dll->lookup(destroy_function_name, destroy_function);

    rgma_ce_mutex_slice.reset( new ism::MutexSlice );
    rgma_se_mutex_slice.reset( new ism::MutexSlice );

    rgma_ce_mutex_slice->slice.reset(new ism::Slice);
    rgma_se_mutex_slice->slice.reset(new ism::Slice);

    the_ism.computing.push_back(rgma_ce_mutex_slice);
    the_ism.storage.push_back(rgma_se_mutex_slice);

    PurchaserPtr purchaser(
      create_function(
        rgma_ce_mutex_slice,
        rgma_se_mutex_slice,
        config.wm()->rgma_query_timeout(),
        config.wm()->rgma_consumer_ttl()
      ),
      destroy_function
    );
    purchaser->set_loop_mode(true, config.wm()->ism_rgma_purchasing_rate());
    purchaser->skip_predicate(the_skip_predicate);

    dlls.push_back(dll);
    purchasers.push_back(purchaser);

  }

  if (purchasing_from_cemon_is_enabled) {

    Info("loading cemon purchaser");

    std::string const dll_name("libglite_wms_ism_cemon_purchaser.so");
    DynamicLibraryPtr dll(make_dll(dll_name));

    std::string const create_function_name("create_cemon_purchaser");
    ism::cemon::create_t* create_function;
    dll->lookup(create_function_name, create_function);

    std::string const destroy_function_name("destroy_cemon_purchaser");
    ism::cemon::destroy_t* destroy_function;
    dll->lookup(destroy_function_name, destroy_function);

    char const* certificate_path = ::getenv("GLITE_CERT_DIR");
    if (!certificate_path) {
      certificate_path = "/etc/grid-security/certificates";
    }

    cemon_ce_mutex_slice.reset( new ism::MutexSlice );
    cemon_ce_mutex_slice->slice.reset( new ism::Slice );

    the_ism.computing.push_back(cemon_ce_mutex_slice);

    PurchaserPtr purchaser(
      create_function(
        cemon_ce_mutex_slice,
        config.common()->host_proxy_file(),
        certificate_path,
        cemon_services,
        "CE_MONITOR"
      ),
      destroy_function
    );
    purchaser->set_loop_mode(true, config.wm()->ism_cemon_purchasing_rate());
    purchaser->skip_predicate(the_skip_predicate);

    dlls.push_back(dll);
    purchasers.push_back(purchaser);

  }

  if (purchasing_from_cemon_asynch_is_enabled) {

    Info("loading asynch cemon purchaser");

    std::string const dll_name("libglite_wms_ism_cemon_asynch_purchaser.so");
    DynamicLibraryPtr dll(make_dll(dll_name));

    std::string const create_function_name("create_cemon_asynch_purchaser");
    ism::cemon_asynch::create_t* create_function;
    dll->lookup(create_function_name, create_function);

    std::string const destroy_function_name("destroy_cemon_asynch_purchaser");
    ism::cemon_asynch::destroy_t* destroy_function;
    dll->lookup(destroy_function_name, destroy_function);

    cemon_asynch_ce_mutex_slice.reset( new ism::MutexSlice );
    cemon_asynch_ce_mutex_slice->slice.reset( new ism::Slice );

    the_ism.computing.push_back(cemon_asynch_ce_mutex_slice);


    PurchaserPtr purchaser(
      create_function(
        cemon_asynch_ce_mutex_slice,       
        "CE_MONITOR",
        config.wm()->ce_monitor_asynch_port()
      ),
      destroy_function
    );
    purchaser->set_loop_mode(true, config.wm()->ism_cemon_asynch_purchasing_rate());
    purchaser->skip_predicate(the_skip_predicate);

    dlls.push_back(dll);
    purchasers.push_back(purchaser);

  }


}

void
start_purchasing_threads(
  std::vector<PurchaserPtr> const& purchasers,
  boost::thread_group& purchasing_threads
)
{
  Info("starting purchaser threads");

  std::vector<PurchaserPtr>::const_iterator it = purchasers.begin();
  std::vector<PurchaserPtr>::const_iterator const end = purchasers.end();

  for ( ; it != end; ++it) {
    PurchaserPtr purchaser_ptr = *it;
    purchasing_threads.create_thread(
      boost::bind(
        &ism::purchaser::operator(),
        purchaser_ptr
      )
    );
  }
}

void periodic_quit_check(const std::vector<PurchaserPtr>& m_purchasers){

  while (!server::received_quit_signal()) {
    ::sleep(5);
  }
  std::vector<PurchaserPtr>::const_iterator it = m_purchasers.begin();
  std::vector<PurchaserPtr>::const_iterator const end = m_purchasers.end();

  for ( ; it != end; ++it) (*it)->stop();
}  

void start_periodic_quit_check(
  const std::vector<PurchaserPtr>& purchasers,
  boost::thread_group& purchasing_threads
){
  Info("starting periodic quit check thread");
  purchasing_threads.create_thread(
    boost::bind(
      &periodic_quit_check,
      purchasers
    )
  );

}


}

struct ISM_Manager::Impl
{
  ism::Ism m_ism;
  ism::MutexSlicePtr m_bdii_ce_mt_slice;
  ism::MutexSlicePtr m_bdii_se_mt_slice;
  ism::MutexSlicePtr m_rgma_ce_mt_slice;
  ism::MutexSlicePtr m_rgma_se_mt_slice;
  ism::MutexSlicePtr m_cemon_ce_mt_slice;
  ism::MutexSlicePtr m_cemon_asynch_ce_mt_slice;

  std::vector<DynamicLibraryPtr> m_dlls;
  std::vector<PurchaserPtr> m_purchasers;
  boost::thread_group m_purchasing_threads;
};

ISM_Manager::ISM_Manager()
  : m_impl(new Impl)
{
  Info("creating the ISM");
  load_dlls_and_create_purchasers(m_impl->m_dlls,
                                  m_impl->m_purchasers,
                                  m_impl->m_ism,
                                  m_impl->m_bdii_ce_mt_slice,
                                  m_impl->m_bdii_se_mt_slice,
                                  m_impl->m_rgma_ce_mt_slice,
                                  m_impl->m_rgma_se_mt_slice,
                                  m_impl->m_cemon_ce_mt_slice,
                                  m_impl->m_cemon_asynch_ce_mt_slice
  );

  ism::set_ism( & m_impl->m_ism );

  start_purchasing_threads(m_impl->m_purchasers, m_impl->m_purchasing_threads);
  start_periodic_quit_check(m_impl->m_purchasers, m_impl->m_purchasing_threads);
}

ISM_Manager::~ISM_Manager()
{
  m_impl->m_purchasing_threads.join_all();
  m_impl->m_purchasers.clear();
  m_impl->m_dlls.clear();
  m_impl->m_ism.computing.clear();
  m_impl->m_ism.storage.clear();
}

}}}} // glite::wms::manager::main
