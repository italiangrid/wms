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
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// Author: Salvatore Monforte
#include <boost/regex.hpp>
#include <fstream>
#include "glite/wms/ism/ism.h"
#include "glite/wms/ism/purchaser/ism-file-purchaser.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wmsutils/classads/classad_utils.h"

using namespace std;
namespace utils = glite::wmsutils::classads;

namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

namespace {
ii::create_entry_update_fn_t      *f_ii_purchaser_entry_update_fn;
cemon::create_entry_update_fn_t   *f_cemon_purchaser_entry_update_fn;
rgma::create_entry_update_fn_t   *f_rgma_purchaser_entry_update_fn;
}

ism_file_purchaser::ism_file_purchaser(
  std::string const& filename,
  exec_mode_t mode,
  size_t interval,
  exit_predicate_type exit_predicate,
  skip_predicate_type skip_predicate
)
  : ism_purchaser(mode, interval, exit_predicate, skip_predicate),
  m_filename(filename)
{
}

void ism_file_purchaser::operator()()
{
  do {
    
    gluece_info_container_type gluece_info_container;
    
    std::ifstream src(m_filename.c_str());
    if (!src.good()) {
	Warning("Unable to load ISM status from dump file: " << m_filename << "\n");
        return;
    }	
    
    Debug("Loading ISM status from dump file: " << m_filename << "\n");
   
    ism_mutex_type::scoped_lock _(get_ism_mutex(ism::ce));

    while(!src.eof()) {
      boost::scoped_ptr<classad::ClassAd> ad;
      string id;
      try {
	ad.reset(utils::parse_classad(src));
        id.assign(utils::evaluate_attribute(*ad,"id"));
      } 
      catch (utils::CannotParseClassAd&) {
	Warning("Error parsing info from ISM dump file\n");
        continue;
      }
      catch (utils::InvalidValue& e) {
        Warning("Error evaluating id for ISM dump info\n");
        continue; 
      }	
      if (m_skip_predicate.empty() || !m_skip_predicate(id)) {

        int    ut(utils::evaluate_attribute(*ad,"update_time"));
	const classad::ClassAd *i=utils::evaluate_attribute(*ad,"info");

        Debug("Loading ISM entry info: " << id << "\n");

	boost::shared_ptr<classad::ClassAd> info(static_cast<classad::ClassAd*>(i->Copy()));
        info->SetParentScope(0);
        
        if(info->Lookup("GlueSEUniqueID")) {
          get_ism(ism::se).insert(
            make_ism_entry(id, ut, info, f_ii_purchaser_entry_update_fn())
          );
        }
        else 
        if(info->Lookup("GlueCEUniqueID")) {
        
          expand_glueceid_info(info);

          // Check the type of puchaser which has generated the info
	  string purchased_by;
          info->EvaluateAttrString("PurchasedBy",purchased_by);
          if (purchased_by==string("ism_ii_purchaser") && 
              f_ii_purchaser_entry_update_fn) {            
            get_ism(ism::ce).insert(make_ism_entry(id, ut, info,
              f_ii_purchaser_entry_update_fn()));
          }
	  else if (purchased_by==string("ism_cemon_purchaser") && 
                   f_cemon_purchaser_entry_update_fn) {
            get_ism(ism::ce).insert(make_ism_entry(id, ut, info,
              f_cemon_purchaser_entry_update_fn()));
          }
          else if (purchased_by==string("ism_rgma_purchaser") &&
                   f_rgma_purchaser_entry_update_fn) {
            get_ism(ism::ce).insert(make_ism_entry(id, ut, info,
              f_rgma_purchaser_entry_update_fn()));
          }
          else if (purchased_by==string("ism_cemon_async_purchaser")) {
            get_ism(ism::ce).insert(make_ism_entry(id, ut, info));
	  }
        }
      }
      if (m_mode == loop) {
        sleep(m_interval);
      }
    }
  } while (m_mode && (m_exit_predicate.empty() || !m_exit_predicate()));
}

// the class factories

extern "C" ism_file_purchaser* create_file_purchaser(
    std::string const& filename,
    exec_mode_t mode,
    size_t interval,
    exit_predicate_type exit_predicate,
    skip_predicate_type skip_predicate
) {
    return new ism_file_purchaser(filename, mode, interval, exit_predicate, skip_predicate);
}

extern "C" void destroy_file_purchaser(ism_file_purchaser* p) {
    delete p;
}

extern "C" void set_purchaser_entry_update_fns(
  ii::create_entry_update_fn_t* ii, 
  cemon::create_entry_update_fn_t* cemon,
  rgma::create_entry_update_fn_t* rgma
)
{
  f_ii_purchaser_entry_update_fn = ii;
  f_cemon_purchaser_entry_update_fn = cemon;
  f_rgma_purchaser_entry_update_fn = rgma;
}

} // namespace purchaser
} // namespace ism
} // namespace wms
} // namespace glite
