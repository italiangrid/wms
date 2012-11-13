// File: match_utils.cpp
// Author: Francesco Giacomini
// Copyright (c) 2005 INFN

// $Id$

#include "match_utils.h"
#include <functional>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_smallint.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/regex.hpp>
#include <boost/thread/xtime.hpp>
#include <classad_distribution.h>
#include "glite/wmsutils/classads/classad_utils.h"

namespace ca = glite::wmsutils::classads;

namespace glite {
namespace wms {
namespace manager {
namespace server {

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

class rank_less_than
  : public std::unary_function<match_type, bool>
{
  double m_rank;
public:
  rank_less_than(double rank)
    : m_rank(rank)
  {
  }
  bool operator()(match_type const& match)
  {
    return match.get<1>() < m_rank;
  }
};

matches_type::const_iterator
select_best_ce_max_rank(matches_type const& matches)
{
  assert(!matches.empty());
  matches_type::const_iterator const begin(matches.begin());

  double max_rank = begin->get<1>();
  matches_type::const_iterator it(
    find_if(begin, matches.end(), rank_less_than(max_rank))
  );

  boost::xtime current;
  boost::xtime_get(&current, boost::TIME_UTC);
  boost::minstd_rand generator(static_cast<unsigned int>(current.nsec));
  boost::uniform_smallint<size_t> distrib(0, distance(begin, it) - 1);
  boost::variate_generator<
    boost::minstd_rand,
    boost::uniform_smallint<size_t>
    > rand(generator, distrib);

  return begin + rand();
}

double get_p(double sum)
{
  boost::minstd_rand dist(std::time(0));
  boost::uniform_01<boost::minstd_rand> rand(dist);
  return rand() * sum;
}

matches_type::const_iterator
select_best_ce_stochastic(matches_type const& matches)
{
  assert(!matches.empty());

  vector<double> ranks;
  ranks.reserve(matches.size());
  double rank_sum = 0.;
  matches_type::const_iterator b = matches.begin();
  matches_type::const_iterator e = matches.end();
  for (; b != e; ++b) {
    double r = b->get<1>();
    ranks.push_back(r);
    rank_sum += r;
  }
  double rank_mean     = rank_sum / ranks.size();
  double rank_variance = variance(ranks, rank_mean);
  // We smooth rank values according to the following function:
  // f(x) = atan( V * (x - mean ) / dev ) + PI
  // Thanks to Alessio Gianelle for his usefull support and suggestions.
  static const double PI = std::atan(1.) * 4.;
  static const double V = PI;
  // Computing the variance and standard deviation of rank samples...
  double rank_deviation = rank_variance > 0 ? sqrt(rank_variance) : V;

  rank_sum = 0.;
  for (size_t r = 0; r < ranks.size(); ++r) {
    ranks[r] = atan(V * (ranks[r] - rank_mean) / rank_deviation) + PI;
    rank_sum += ranks[r];
  }

  boost::xtime current;
  boost::xtime_get(&current, boost::TIME_UTC);
  boost::minstd_rand generator(static_cast<unsigned int>(current.nsec));
  boost::uniform_01<boost::minstd_rand> rand(generator);
  double const p = rand() * rank_sum;

  double prob_sum = 0.;
  size_t i = 0;
  matches_type::const_iterator best = matches.begin();
  matches_type::const_iterator const matches_end = matches.end();
  do {
    prob_sum += ranks[i++];
    if (p < prob_sum) {
      break;
    }
  } while (++best != matches_end);

  return best;
}

matches_type::const_iterator
select_best_ce(matches_type const& matches, bool use_fuzzy_rank)
{
  assert(!matches.empty());

  if (use_fuzzy_rank) {
    return select_best_ce_stochastic(matches);
  } else {
    return select_best_ce_max_rank(matches);
  }
}

bool
fill_matches(
  classad::ClassAd const& match_response,
  matches_type& matches,
  bool include_brokerinfo
)
try {

  std::string reason(ca::evaluate_attribute(match_response, "reason"));
  if (reason != "ok") {
    throw MatchError(reason);
  }

  classad::ExprList const* const match_result(
    ca::evaluate_attribute(match_response, "match_result")
  );

  classad::ExprList::const_iterator it = match_result->begin();
  classad::ExprList::const_iterator const end = match_result->end();
  for ( ; it != end; ++it) {
    assert(ca::is_classad(*it));
    classad::ClassAd const& match(*static_cast<classad::ClassAd const*>(*it));

    std::string const ce_id(ca::evaluate_attribute(match, "ce_id"));
    {
      // if the ce is cream-based ignore it, because dagman is not able to
      // manage it
      static boost::regex const re(".+/([^-]+)-.+");
      boost::smatch pieces;
      std::string base;
      if (boost::regex_match(ce_id, pieces, re)) {
        base.assign(pieces[1].first, pieces[1].second);
      }
      if (base.empty() || base == "cream") {
        continue;
      }
    }
    double rank(ca::evaluate_attribute(match, "rank"));
    ClassAdPtr ad_ptr;
    if (include_brokerinfo) {
      classad::ClassAd const* brokerinfo(
        ca::evaluate_attribute(match, "brokerinfo")
      );
      ad_ptr.reset(static_cast<classad::ClassAd*>(brokerinfo->Copy()));
    }
    matches.push_back(
      boost::make_tuple(ce_id, rank, ad_ptr)
    );
  }

  return !matches.empty();

} catch (ca::InvalidValue&) {
  throw MatchError(
    "invalid format: " + ca::unparse_classad(match_response)
  );
}

}}}} // glite::wms::manager::server
