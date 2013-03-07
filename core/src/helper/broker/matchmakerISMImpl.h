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

// File: matchmakerGlueISM.h
// Author: Salvatore Monforte
// Copyright (c) 2002 EU DataGrid.

// $Id: matchmakerISMImpl.h,v 1.2.8.1.2.2 2010/04/08 13:57:07 mcecchi Exp $

#ifndef GLITE_WMS_MATCHMAKING_MATCHMAKERISMIMPL_H
#define GLITE_WMS_MATCHMAKING_MATCHMAKERISMIMPL_H

#include "matchmaker.h"

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
