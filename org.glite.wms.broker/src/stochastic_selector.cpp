/*
 * File: stochasticRankSelector.cpp
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$

#include <algorithm>
#include <numeric>
#include <iterator>
#include <vector>
#include <cmath>

#include <boost/random/variate_generator.hpp>
#include <boost/random/linear_congruential.hpp>

#include "stochastic_selector.h"

namespace glite {
namespace wms {
namespace broker {

namespace 
{
  boost::minstd_rand f_rnd(time(0));
  boost::uniform_01<boost::minstd_rand> f_unirand01(f_rnd);

  std::vector<double> 
  make_distribution(matchtable const& matches, double f)
  {
    std::vector<double> ranks;
    ranks.reserve(matches.size());
    
    double rank_sum = 0.;
    
    matchtable::const_iterator b = matches.begin();
    matchtable::const_iterator const e = matches.end();
    
    for (; b != e; ++b) {
      double r = b->get<Rank>();
      ranks.push_back(r);
      rank_sum += r;
    }

    double rank_mean = rank_sum / ranks.size();
    static const double PI_2 = std::atan(1.) * 2.;

    for (size_t r = 0; r < ranks.size(); ++r) {
      ranks[r] = atan( f * (ranks[r] - rank_mean) ) + PI_2;
    }
    std::partial_sum(ranks.begin(), ranks.end(), ranks.begin());
    return ranks;
  }

} // anonymous namespace
	
stochastic_selector::stochastic_selector(double fuzzy_factor) 
  : m_fuzzy_factor(fuzzy_factor)
{
}
 
matchtable::iterator 
stochastic_selector::operator()(
 matchtable& matches
)
{
  if( matches.empty() ) return matches.end();

  std::vector<double> const distribution(
    make_distribution(matches, m_fuzzy_factor)
  );

  double const p = f_unirand01() * distribution.back();
  std::vector<double>::const_iterator const i(
    std::lower_bound(distribution.begin(), distribution.end(), p)
  );
  matchtable::iterator result = matches.begin();
  std::advance(result, std::distance(distribution.begin(), i));

  return result;
}	

}; // namespace broker
}; // namespace wms
}; // namespace glite
