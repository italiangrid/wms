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
 * File: maxRankSelector.cpp
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 */

// $Id: maxRankSelector.cpp,v 1.1.2.1 2012/09/11 10:19:42 mcecchi Exp $

#include "maxRankSelector.h"
#include <vector>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_smallint.hpp>
#include <boost/random/variate_generator.hpp>
#include <ctime>

namespace glite {
namespace wms {
namespace broker {

namespace {

boost::minstd_rand f_rnd;
boost::uniform_smallint<size_t> distrib(0, 666);
boost::variate_generator<
  boost::minstd_rand,
  boost::uniform_smallint<size_t>
> unirand(f_rnd, distrib);

struct clustered_rank_less_than_comparator :
    public std::unary_function<maxRankSelector::rank_to_match_container_map_type::value_type&, bool>
{
  bool operator()(
    maxRankSelector::rank_to_match_container_map_type::value_type const& a,
    maxRankSelector::rank_to_match_container_map_type::value_type const& b
  )
  {
    return a.first < b.first;
  }
};

struct clustered_rank_greater_than_comparator :
    public std::unary_function<maxRankSelector::rank_to_match_container_map_type::value_type&, bool>
{
  bool operator()(
    maxRankSelector::rank_to_match_container_map_type::value_type const& a,
    maxRankSelector::rank_to_match_container_map_type::value_type const& b
  )
  {
    return a.first > b.first;
  }
};

}

maxRankSelector::maxRankSelector()
{
  f_rnd.seed(std::time(0));
}

maxRankSelector::~maxRankSelector()
{
}

mm::matchtable::const_iterator
maxRankSelector::selectBestCE(mm::matchtable const& match_table)
{
  rank_to_match_container_map_type clustered_rank_match_table;

  mm::matchtable::const_iterator it(match_table.begin());
  mm::matchtable::const_iterator const e(match_table.end());
  for ( ; it != e; ++it) {
    double r = mm::getRank(it->second);
    clustered_rank_match_table[r].push_back(it);
  }
  rank_to_match_container_map_type::const_iterator max_cluster =
    std::max_element(
      clustered_rank_match_table.begin(),
      clustered_rank_match_table.end(),
      clustered_rank_less_than_comparator()
    );
  // The range is invalid
  if (max_cluster == clustered_rank_match_table.end()) {
    return e;
  }

  size_t n = max_cluster->second.size();

  if (n == 1) {
    return max_cluster->second.front();
  }
  return max_cluster->second[unirand()%n];
}

}}} // glite::wms::broker
