// File: fuzzy_selector.cpp
// Author: Monforte Salvatore
// Author: Francesco Giacomini

// $Id$

#include "glite/wms/broker/match.h"
#include <algorithm>
#include <cmath>
#include <boost/random/variate_generator.hpp>
#include <boost/random/linear_congruential.hpp>
#include "match_utils.h"

namespace glite {
namespace wms {
namespace broker {

namespace {

boost::minstd_rand f_rnd(time(0));
boost::uniform_01<boost::minstd_rand> f_unirand01(f_rnd);

}
	
FuzzySelector::FuzzySelector(double fuzzy_factor)
  : m_factor(fuzzy_factor)
{
}

MatchTable::iterator
FuzzySelector::operator()(MatchTable& matches) const
{
  if (matches.empty()) {
    return matches.end();
  }

  std::vector<double> const distribution(
    make_distribution(matches, m_factor)
  );

  double const p = f_unirand01() * distribution.back();
  std::vector<double>::const_iterator const i(
    std::lower_bound(distribution.begin(), distribution.end(), p)
  );
  MatchTable::iterator result = matches.begin();
  std::advance(result, std::distance(distribution.begin(), i));

  return result;
}

}}}
