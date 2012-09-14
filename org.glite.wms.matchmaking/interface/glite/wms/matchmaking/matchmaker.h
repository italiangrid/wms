// File: MatchMaker.h
// Author: Monforte Salvatore
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#ifndef GLITE_WMS_MATCHMAKING_MATCHMAKER_H
#define GLITE_WMS_MATCHMAKING_MATCHMAKER_H

#include <boost/utility.hpp>
#include <boost/tuple/tuple.hpp>
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
