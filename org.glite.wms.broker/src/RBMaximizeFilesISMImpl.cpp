// File: RBMaximizeFilesISMImpl.cpp
// Author: Salvatore Monforte
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id: 

#include <algorithm>
#include <functional>
#include <numeric>
#include <vector> 

#include <classad_distribution.h>

#include "glite/jdl/ManipulationExceptions.h"
#include "glite/jdl/JobAdManipulation.h"

#include "matchmaking.h"
#include "RBMaximizeFilesISMImpl.h"

#include "glite/wms/common/logger/logger_utils.h"

using namespace std;

namespace requestad = glite::jdl;

namespace glite {
namespace wms {

namespace logger        = common::logger;

namespace broker {

boost::tuple<
  boost::shared_ptr<matchtable>,
  boost::shared_ptr<filemapping>,
  boost::shared_ptr<storagemapping>
>
RBMaximizeFilesISMImpl::findSuitableCEs(
  const classad::ClassAd* requestAd
) {

  boost::shared_ptr<matchtable> suitableCEs;
  boost::shared_ptr<filemapping> fm;
  boost::shared_ptr<storagemapping> sm;

  if (requestAd) {

    suitableCEs.reset(new matchtable);
    classad::ClassAd jdl(*requestAd);

    // Collects SFNs and involved SEs.
    fm = broker::resolve_filemapping_info(jdl);
    sm = broker::resolve_storagemapping_info(fm);

    // Selects only comptatible storage
    vector<string> dap;
    requestad::get_data_access_protocol(jdl, dap);
    vector<storagemapping::const_iterator> compatible_storage(
     select_compatible_storage(*sm,dap)
    );

    std::set<std::string> close_computing_elements_id;
    // Inserts in suitableCEs id of those computing elements
    // bound to compatible storage 
    std::accumulate(
      compatible_storage.begin(), 
      compatible_storage.end(),
      &close_computing_elements_id,
      extract_computing_elements_id()
    );     
    
    // Filter computing elements on requirements/rank

    match(
      jdl,
      close_computing_elements_id, 
      *suitableCEs);

    matchtable::iterator ce_it = suitableCEs->begin();
    matchtable::iterator const ce_end = suitableCEs->end();
    
    map<
      size_t, 
      vector<matchtable::const_iterator> 
    > unique_logical_files_per_ce;
    
    size_t max_files = 0;
    for( ; ce_it != ce_end; ) {
 
      string const& id = boost::tuples::get<0>(*ce_it);
      vector<storagemapping::const_iterator>::iterator
      storage_part_end(
        std::partition(
          compatible_storage.begin(),
          compatible_storage.end(),
          is_storage_close_to(id)
        )
      );
      
      size_t n =
        count_unique_logical_files(
          compatible_storage.begin(),
          storage_part_end
      );

      Debug(
        id << " has #" << 
        storage_part_end - compatible_storage.begin() << 
        " close compatible storage element(s) providing #" << n <<
        " accessible file(s)"
      );

      if ( n > max_files ) max_files = n;
      
      map<
        size_t, 
        vector<matchtable::const_iterator> 
      >::iterator it;
      bool inserted = false;
      boost::tie(it,inserted) =
        unique_logical_files_per_ce.insert(
          std::make_pair(n,vector<matchtable::const_iterator>())
        );
      it->second.push_back(ce_it);
      ++ce_it;
    }
	
    bool FullListMatchResult = false;
    if (
      !jdl.EvaluateAttrBool("FullListMatchResult", FullListMatchResult) || 
      !FullListMatchResult) {

      vector<matchtable::const_iterator> const& max_logical_files_ces(
         unique_logical_files_per_ce[max_files]
      );
      matchtable::iterator it = suitableCEs->begin();
      matchtable::iterator const e = suitableCEs->end();
      for( ; it != e ; ) {
        if( std::find(
             max_logical_files_ces.begin(),
             max_logical_files_ces.end(),
             it) == max_logical_files_ces.end()) {

          suitableCEs->erase(it++);
        }
        else {
          ++it;
        }
      }
    }  
  }
  return boost::tuples::make_tuple(
    suitableCEs,
    fm,
    sm
  );
}

} // namespace broker
} // namespace wms
} // namespace glite

