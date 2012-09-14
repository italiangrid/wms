// File: match_utils.cpp
// Author: Francesco Giacomini
// Author: Salvatore Monforte

// $Id$

#include "match_utils.h"
#include <numeric>
#include <cmath>

namespace glite {
namespace wms {
namespace broker {

std::vector<double> make_distribution(MatchTable const& matches, double f)
{
  std::vector<double> ranks;
  ranks.reserve(matches.size());

  double rank_sum = 0.;

  MatchTable::const_iterator b = matches.begin();
  MatchTable::const_iterator const e = matches.end();

  for (; b != e; ++b) {
    double r = b->rank;
    ranks.push_back(r);
    rank_sum += r;
  }

  double rank_mean = rank_sum / ranks.size();
  static const double PI_2 = std::atan(1.) * 2.;

  for (size_t r = 0; r < ranks.size(); ++r) {
    ranks[r] = std::atan( f * (ranks[r] - rank_mean) ) + PI_2;
  }
  std::partial_sum(ranks.begin(), ranks.end(), ranks.begin());
  return ranks;
}

}}}

