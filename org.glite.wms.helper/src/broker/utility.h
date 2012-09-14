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

/*
 * File: utility.h
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 */

// $Id$

#ifndef _GLITE_WMS_MATCHMAKING_UTILITY_H_
#define _GLITE_WMS_MATCHMAKING_UTILITY_H_

#include <vector>
#include <numeric>
#include <algorithm>
#include "glite/wms/matchmaking/matchmaker.h"

using namespace std;

namespace glite {
namespace wms {
namespace matchmaking {

typedef std::vector<
  std::pair<
    matchtable::key_type,
    matchtable::mapped_type
  >
> matchvector;

struct rank_less_than_comparator :
  binary_function<
    matchtable::value_type&,
    matchtable::value_type&,
    bool
  >
{
  bool operator()(
    const matchtable::value_type& a,
    const matchtable::value_type& b)
  {
    return getRank(a.second) < getRank(b.second);
  }
};

struct rank_greater_than_comparator :
  binary_function<
    matchtable::value_type&,
    matchtable::value_type&,
    bool
  >
{
  bool operator()(
    const matchtable::value_type& a,
    const matchtable::value_type& b)
  {
    return getRank(a.second) > getRank(b.second);
  }
};

} // namespace matchmaking
} // namespace wms
} // namespace glite


#endif
