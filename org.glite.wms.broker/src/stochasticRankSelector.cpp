/*
 * File: stochasticRankSelector.cpp
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$

#include <algorithm>
#include <vector>

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

matchtable::const_iterator 
stochasticRankSelector::selectBestCE(
 matchtable const& match_table
)
{
  if( match_table.empty() ) return match_table.end();

  vector<double> rank;
  rank.reserve(match_table.size());

  double rank_sum = 0.0;
  matchtable::const_iterator it = match_table.begin();
  matchtable::const_iterator const e = match_table.end();

  for( ; it != e; ++it) {

    double r = boost::tuples::get<Rank>(*it);
    rank.push_back(r);
    rank_sum += r;
  }

  double rank_mean      =  rank_sum / (double)(rank.size());

  // We smooth rank values according to the following function:
  // f(x) = atan( V * (x - mean ) / dev ) + PI
  // Thanks to Alessio Gianelle for his usefull support and suggestions.

  static const double PI = std::atan(1.0)*4.0;
  static const double V = PI;

  // Computing the variance and standard deviation of rank samples...
  double rank_variance  =  variance(rank, rank_mean);
  double rank_deviation =  rank_variance > 0 ? sqrt(rank_variance) : V;
  
  rank_sum = 0.0;

  for(size_t r=0; r < rank.size(); r++) {
    double x = rank[r];
    rank[r] = atan( V * (x - rank_mean) / rank_deviation ) + PI;
    rank_sum += rank[r];
  }

  double prob_sum   = 0.0;
  double p = f_unirand01() * rank_sum;
  size_t i = 0;
  matchtable::const_iterator retval;
  matchtable::const_iterator best = match_table.begin();
  do {
    retval = best;
    prob_sum += rank[i++];
    if ( p <= prob_sum ) break;
  } while( ++best != e );

  return retval;
}	

}; // namespace broker
}; // namespace wms
}; // namespace glite
