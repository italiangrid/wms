// File: match_utils.h
// Author: Francesco Giacomini
// Author: Salvatore Monforte

// $Id$

#ifndef GLITE_WMS_BROKER_MATCH_UTILS_H
#define GLITE_WMS_BROKER_MATCH_UTILS_H

#include "glite/wms/broker/match.h"

namespace glite {
namespace wms {
namespace broker {

std::vector<double> make_distribution(MatchTable const& matches, double f);

struct rank_less_than_comparator
{
  bool operator()(MatchInfo const& lhs, MatchInfo const& rhs) const
  {
    return lhs.rank < rhs.rank;
  }
};

class rank_equal_to
{
  double m_rank;
public:
  rank_equal_to(double rank)
    : m_rank(rank)
  {
  }
  bool operator()(MatchInfo const& match) const
  {
    return match.rank == m_rank;
  }
};

}}}

#endif
