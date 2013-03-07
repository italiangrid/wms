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

// File: matchmaker.cpp
// Author: Monforte Salvatore

// $Id: matchmaker.cpp,v 1.1.2.2.2.2 2010/04/08 13:57:07 mcecchi Exp $

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
