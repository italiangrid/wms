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

// File: matchmakerGlueImpl.h
// Author: Salvatore Monforte
// Copyright (c) 2002 EU DataGrid.

// $Id$

#ifndef _GLITE_WMS_MATCHMAKING_MATCHMAKERGLUEIMPL_H
#define _GLITE_WMS_MATCHMAKING_MATCHMAKERGLUEIMPL_H

#include "glite/wms/matchmaking/matchmaker.h"
#include "glite/wms/common/ldif2classad/LDIFObject.h"

namespace ldif2classad = glite::wms::common::ldif2classad;

namespace glite {
namespace wms {
namespace matchmaking { 

class matchmakerGlueImpl : public MatchMakerImpl
{
 public:
  matchmakerGlueImpl();
  ~matchmakerGlueImpl();
  void prefetchCEInfo  (const classad::ClassAd* requestAd, match_table_t& suitableCEs);
  void checkRequirement(classad::ClassAd& requestAd, match_table_t& suitableCEs, bool use_prefetched_ces=false);
  void checkRank       (classad::ClassAd& requestAd, match_table_t& suitableCEs, bool use_prefetched_ces=false);
 private:
    // The following members have been added by Francesco Prelz...
    // TODO: check if it is really necessary having this as static member
    // and try to change the implementation...i don't like this design
    // but there is no time for changes now...		
    static boost::scoped_ptr< classad::ClassAd > gang_match_storageAd;
    std::vector<ldif2classad::LDIFObject> m_CE_info_cache;
    bool m_CE_info_prefetched;
};

} // namespace matchmaking
} // namespace wms
} // namespace glite

#endif
