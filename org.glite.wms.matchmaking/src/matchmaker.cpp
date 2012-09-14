// File: matchmaker.cpp
// Author: Monforte Salvatore

// $Id$

#include "glite/wms/matchmaking/matchmaker.h"
#include "matchmakerISMImpl.h"

namespace glite {
namespace wms {
namespace matchmaking {

struct MatchMaker::Impl
{
  matchmakerISMImpl m_mm;
}; 

MatchMaker::MatchMaker() 
  : m_impl( new Impl ) 
{
} 

void MatchMaker::checkRequirement(
  classad::ClassAd& requestAd, matchtable& suitableCEs
)
{
  matchmakerISMImpl& mm = m_impl->m_mm;
  mm.checkRequirement(requestAd, suitableCEs);
}

void MatchMaker::checkRequirement(
  classad::ClassAd& requestAd, 
  std::set<matchtable::key_type> const& CEids,
  matchtable& suitableCEs
)
{
  matchmakerISMImpl& mm = m_impl->m_mm;
  mm.checkRequirement(requestAd, CEids, suitableCEs);
}

void MatchMaker::checkRank(
  classad::ClassAd& requestAd, matchtable& suitableCEs
)
{
  matchmakerISMImpl& mm = m_impl->m_mm;
  mm.checkRank(requestAd, suitableCEs);
}

bool isRankUndefined(matchinfo const& i)
{
  std::pair<bool,double> const& ri = boost::tuples::get<0>(i);
  return !ri.first;
}

double getRank(matchinfo const& i)
{
  std::pair<bool,double> const& ri = boost::tuples::get<0>(i);
  return ri.second;
}

void setRank(matchinfo& i, double d)
{
  std::pair<bool,double> & ri = boost::tuples::get<0>(i);
  ri.first = true;
  ri.second = d;
}

boost::shared_ptr<classad::ClassAd> 
getAd(matchinfo const& i) 
{
  return boost::tuples::get<1>(i);
}

}}}
