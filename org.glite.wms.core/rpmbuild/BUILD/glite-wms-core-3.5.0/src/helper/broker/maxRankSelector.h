/*
 * File: maxRankSelector.h
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 */
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

 
#ifndef GLITE_WMS_BROKER_SELECTORS_MAXRANKSELECTOR_H_
#define GLITE_WMS_BROKER_SELECTORS_MAXRANKSELECTOR_H_

#include "RBSelectionSchema.h"

namespace mm = glite::wms::matchmaking;

namespace glite {
namespace wms {
namespace broker {

struct maxRankSelector : RBSelectionSchema
{
  typedef std::vector<
    mm::matchtable::const_iterator
  > match_container_type;

  typedef std::map<
    double, 
    match_container_type
  > rank_to_match_container_map_type;
  
  maxRankSelector();
  ~maxRankSelector();	
  mm::matchtable::const_iterator 
  selectBestCE(mm::matchtable const& match_table);
};

} // namespace broker
} // namespace wms
} // namespace glite

#endif
