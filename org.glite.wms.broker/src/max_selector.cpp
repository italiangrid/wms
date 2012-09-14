// File: max_selector.cpp
// Author: Francesco Giacomini
// Author: Salvatore Monforte

// $Id$

#include "glite/wms/broker/match.h"

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_smallint.hpp>
#include <boost/random/variate_generator.hpp>

#include "match_utils.h"

namespace glite {
namespace wms {
namespace broker {

namespace {

boost::minstd_rand f_rnd(std::time(0));

MatchTable::iterator max_partition(MatchTable& matches)
{
  MatchTable::const_iterator const max(
    std::max_element(
      matches.begin(), matches.end(), rank_less_than_comparator()
    )
  );
  return std::partition(
    matches.begin(),
    matches.end(),
    rank_equal_to(max->rank)
  );
}

}

MatchTable::iterator
MaxRankSelector::operator()(MatchTable& matches) const
{
  MatchTable::iterator const max_partition_end(
    max_partition(matches)
  );
  size_t const n = std::distance(matches.begin(), max_partition_end);

  boost::uniform_smallint<size_t> distrib(0, n-1);

  boost::variate_generator<
    boost::minstd_rand,
    boost::uniform_smallint<size_t>
  > unirand(f_rnd, distrib);

  MatchTable::iterator result = matches.begin();
  std::advance(result, unirand());
  return result;
}

}}}
