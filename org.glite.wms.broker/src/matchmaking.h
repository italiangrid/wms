// $Id$

#ifndef GLITE_WMS_MATCHMAKING_MATCHMAKER_H
#define GLITE_WMS_MATCHMAKING_MATCHMAKER_H

#include <boost/utility.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <deque>
#include <set>

namespace classad
{
  class ClassAd;
}

namespace glite {
namespace wms {
namespace broker {

typedef boost::tuple<
  std::string,                        // ceid
  double,                             // the rank information 
  boost::shared_ptr<classad::ClassAd> // the ce classad representation
> matchinfo;

enum { Id, Rank, Ad }; 

typedef std::deque<matchinfo> matchtable;

void
match(
  classad::ClassAd& requestAd,
  matchtable& suitableCEs
);

void
match(
  classad::ClassAd& requestAd,
  std::set<std::string> const& CEids,
  matchtable& suitableCEs
);

struct rank_less_than_comparator :
  std::binary_function< matchinfo&, matchinfo&, bool >
{
  bool operator()(
    matchinfo& a,
    matchinfo& b)
  {
    return a.get<Rank>() < b.get<Rank>();
  }
};

struct rank_greater_than_comparator :
  std::binary_function< matchinfo&, matchinfo&, bool >
{
  bool operator()(
    const matchinfo& a,
    const matchinfo& b)
  {
    return a.get<Rank>() > b.get<Rank>();
  }
};

class rank_greater_than
  : public std::unary_function<matchinfo&, bool>
{
  double m_rank;
public:
  rank_greater_than(double rank)
    : m_rank(rank)
  {
  }
  bool operator()(matchinfo const& match)
  {
    return match.get<Rank>() > m_rank;
  }
};

class rank_equal_to
  : public std::unary_function<matchinfo&, bool>
{
  double m_rank;
public:
  rank_equal_to(double rank)
    : m_rank(rank)
  {
  }
  bool operator()(matchinfo const& match)
  {
    return match.get<Rank>() == m_rank;
  }
};


}}}

#endif
