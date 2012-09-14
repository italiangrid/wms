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

// File: MatchMaker.h
// Author: Monforte Salvatore

// $Id$

#ifndef GLITE_WMS_MATCHMAKING_MATCHMAKER_H
#define GLITE_WMS_MATCHMAKING_MATCHMAKER_H

#include <boost/utility.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <utility>
#include <string>
#include <map>
#include <set>

namespace classad
{
  class ClassAd;
}

namespace glite {
namespace wms {
namespace matchmaking {

typedef boost::tuple<
  std::pair<bool, double>,            // the rank information 
  boost::shared_ptr<classad::ClassAd> // the ce classad representation
> matchinfo;

typedef std::map<
  std::string, // the ceid
  matchinfo
> matchtable;

class MatchMaker : boost::noncopyable
{
 class Impl;
 boost::shared_ptr<Impl> m_impl;

 public:

   MatchMaker();

    void checkRequirement(
      classad::ClassAd& requestAd,
      matchtable& suitableCEs
    );

    void checkRequirement(
      classad::ClassAd& requestAd,
      std::set<matchtable::key_type> const& CEids,
      matchtable& suitableCEs
    );

    void checkRank(
      classad::ClassAd& requestAd,
      matchtable& suitableCEs
    );

};

bool isRankUndefined(matchinfo const&);
double getRank(matchinfo const&);
void setRank(matchinfo&, double);
boost::shared_ptr<classad::ClassAd> getAd(matchinfo const&);

}}}

#endif
