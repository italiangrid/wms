// File: matchmakerGlueISM.h
// Author: Salvatore Monforte
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#ifndef GLITE_WMS_MATCHMAKING_MATCHMAKERISMIMPL_H
#define GLITE_WMS_MATCHMAKING_MATCHMAKERISMIMPL_H

#include "glite/wms/matchmaking/matchmaker.h"

namespace classad {
class ClassAd;
}

namespace glite {
namespace wms {
namespace matchmaking { 

struct matchmakerISMImpl
{
  void checkRequirement(
    classad::ClassAd&,
    matchtable&
  );

  void checkRequirement(
    classad::ClassAd&,
    std::set<matchtable::key_type> const&,
    matchtable&
  );

  void checkRank(
    classad::ClassAd&,
    matchtable&
  );
};

}}}

#endif
