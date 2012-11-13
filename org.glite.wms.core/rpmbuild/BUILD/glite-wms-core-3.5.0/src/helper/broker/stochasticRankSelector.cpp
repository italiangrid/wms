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
 * File: stochasticRankSelector.cpp
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 */

// $Id: stochasticRankSelector.cpp,v 1.1.2.1 2012/09/11 10:19:43 mcecchi Exp $

#include <algorithm>
#include <vector>
#include <cmath>
#include "stochasticRankSelector.h"

using namespace std;

namespace glite {
namespace wms {
namespace broker {

namespace 
{
  boost::minstd_rand f_rnd(time(0));
  boost::uniform_01<boost::minstd_rand> f_unirand01(f_rnd);

  template<typename Container, typename T>
  T variance(Container const& c, T mean)
  {
    T v = T();
    size_t n = 0;
    typename Container::const_iterator first = c.begin();
    typename Container::const_iterator const last = c.end();
    for ( ; first != last; ++first, ++n) {
      T t = *first - mean;
      v += t * t;
    }

    return n ? v / n : v;
  }
}
	
stochasticRankSelector::stochasticRankSelector()
{
}
 
stochasticRankSelector::~stochasticRankSelector()
{
}	  

mm::matchtable::const_iterator 
stochasticRankSelector::selectBestCE(
 mm::matchtable const& match_table
)
{
  if( match_table.empty() ) return match_table.end();

  vector<double> rank;
  rank.reserve(match_table.size());

  double rank_sum = 0.0;
  mm::matchtable::const_iterator it = match_table.begin();
  mm::matchtable::const_iterator const e = match_table.end();

  for( ; it != e; ++it) {

    double r = mm::getRank(it -> second);
    rank.push_back(r);
    rank_sum += r;
  }
  static const double PI_2 = std::atan(1.0) * 2.;
  double rank_mean      =  rank_sum / (double)(rank.size());

  rank_sum = 0.0;

  for(size_t r=0; r < rank.size(); r++) {
    double x = rank[r];
    rank[r] = atan(  (x - rank_mean) * RBSelectionSchema::FuzzyFactor ) + PI_2;
    rank_sum += rank[r];
  }

  double prob_sum   = 0.0;
  double p = f_unirand01() * rank_sum;
  size_t i = 0;
  mm::matchtable::const_iterator retval;
  mm::matchtable::const_iterator best = match_table.begin();
  do {
    retval = best;
    prob_sum += rank[i++];
    if ( p <= prob_sum ) break;
  } while( ++best != e );

  return retval;
}	

} // namespace broker
} // namespace wms
} // namespace glite
