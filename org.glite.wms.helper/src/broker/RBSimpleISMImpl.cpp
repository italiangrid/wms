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

// File: RBSimpleISMImpl.cpp
// Author: Salvatore Monforte
// Copyright (c) 2002 EU DataGrid.

// $Id$

#include <functional>
#include <vector> 

#include <classad_distribution.h>

#include "matchmakerISMImpl.h"
#include "RBSimpleISMImpl.h"

namespace glite {
namespace wms {
namespace broker {

boost::tuple<
  boost::shared_ptr<matchmaking::matchtable>,
  boost::shared_ptr<brokerinfo::FileMapping>,
  boost::shared_ptr<brokerinfo::StorageMapping>
>
RBSimpleISMImpl::findSuitableCEs(
  classad::ClassAd const* requestAd
){

  if (!requestAd) {
    return boost::tuples::make_tuple(
      boost::shared_ptr<matchmaking::matchtable>(),
      boost::shared_ptr<brokerinfo::FileMapping>(),
      boost::shared_ptr<brokerinfo::StorageMapping>()
    );
  }

  classad::ClassAd jdl(*requestAd);

  boost::shared_ptr<matchmaking::matchtable> suitableCEs(
    new matchmaking::matchtable
  );

  matchmaking::MatchMaker MM;
  MM.checkRequirement(jdl, *suitableCEs);
  MM.checkRank       (jdl, *suitableCEs);

  matchmaking::matchtable::iterator it(suitableCEs->begin());
  matchmaking::matchtable::iterator const e(suitableCEs->end());

  for( ; it != e ; ) {
    if(matchmaking::isRankUndefined(it->second)) {
      suitableCEs->erase(it++);
    }
    else {
      ++it;
    }
  }

  return boost::tuples::make_tuple(
    suitableCEs,
    boost::shared_ptr<brokerinfo::FileMapping>(),
    boost::shared_ptr<brokerinfo::StorageMapping>()
  );
}

}}}
