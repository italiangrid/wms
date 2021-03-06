/*
 * File: stochasticRankSelector.h
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

// $Id: stochasticRankSelector.h,v 1.1.2.2 2012/09/12 10:02:13 mcecchi Exp $

#ifndef GLITE_WMS_BROKER_SELECTORS_STOCHASTICRANKSELECTOR_H_
#define GLITE_WMS_BROKER_SELECTORS_STOCHASTICRANKSELECTOR_H_

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_01.hpp>

#include "RBSelectionSchema.h"

namespace mm = glite::wms::matchmaking;

namespace glite {
namespace wms {
namespace broker {

class stochasticRankSelector : public RBSelectionSchema
{
 public:
  stochasticRankSelector();
  ~stochasticRankSelector();	
  mm::matchtable::const_iterator 
  selectBestCE(mm::matchtable const& match_table);
};	

} // namespace broker
} // namespace wms
} // namespace glite

#endif
