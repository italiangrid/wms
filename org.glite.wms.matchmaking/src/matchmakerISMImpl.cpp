// File: matchmakerISMImpl.cpp
// Author: Salvatore Monforte
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <algorithm>
#include <boost/shared_ptr.hpp>

#include "glite/wms/matchmaking/matchmaker.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/wms/ism/ism.h"
#include "matchmakerISMImpl.h"
#include "exceptions.h"

namespace jdlc = glite::jdl;

namespace glite {
namespace utils = wmsutils::classads;
namespace wms {
namespace matchmaking {

void
matchmakerISMImpl::checkRequirement(
  classad::ClassAd& jdl,
  matchtable& suitableCEs
)
{
  ism::ism_type const& the_ism = *ism::get_ism();
  for( size_t i = 0; i <= ism::ism_ce_index_end; ++i ) {

    if( !the_ism[ism::ism_ce_index[i]] ||
        !the_ism[ism::ism_ce_index[i]]->slice) {
      continue;
    }
    
    ism::ism_mutex_type::scoped_lock lock (
      the_ism[ism::ism_ce_index[i]]->mutex
    );

    ism::ism_slice_type::nth_index<1>::type& index(
      the_ism[ism::ism_ce_index[i]]->slice->get<1>()
    );

    ism::ism_slice_type::nth_index<1>::type::iterator ism_it(
      index.begin()
    );
    ism::ism_slice_type::nth_index<1>::type::iterator const ism_end(
      index.end()
    );

    for ( ; ism_it != ism_end; ++ism_it) {

      if (ism::is_void_ism_entry(*ism_it)) {
        continue;
      }

      boost::shared_ptr<classad::ClassAd> ce_ad_ptr =
        boost::tuples::get<ism::ad_ptr_entry>(*ism_it);

      classad::ClassAd ce_ad(*ce_ad_ptr);

      std::string const ism_id(
        boost::tuples::get<ism::id_type_entry>(*ism_it)
      );

      if (utils::symmetric_match(ce_ad, jdl)) {
        Info(ism_id << ": ok!");
        suitableCEs[ism_id] = boost::tuples::make_tuple(
          std::make_pair(false, 0.0), ce_ad_ptr
        );
      }
    }
  }
    
  typedef std::vector<std::string> previous_matches_type;
  previous_matches_type previous_matches;
  
  bool previous_matches_exists = false;
  jdlc::get_edg_previous_matches(
    jdl, 
    previous_matches, 
    previous_matches_exists
  );
  
  if (previous_matches_exists) {
    previous_matches_type::iterator const 
    previous_matches_begin(
      previous_matches.begin()
    );
    previous_matches_type::iterator const 
    previous_matches_end(
      previous_matches.end()
    );
    matchtable::iterator ces_it = suitableCEs.begin();
    matchtable::iterator const ces_end = suitableCEs.end();
    while( ces_it != ces_end ) {
      std::string const& ce_id = ces_it->first;
      if (
        find(
          previous_matches_begin,
          previous_matches_end, 
          ce_id) != previous_matches_end) {
  
        suitableCEs.erase(ces_it++);
      }
      else {
        ++ces_it;
      }
    }
  }
}

void
matchmakerISMImpl::checkRequirement(
  classad::ClassAd& jdl,
  std::set<matchtable::key_type> const& ce_ids,
  matchtable& suitableCEs
)
{
  std::set<matchtable::key_type>::const_iterator ce_ids_it = ce_ids.begin();
  std::set<matchtable::key_type>::const_iterator const ce_ids_end(
    ce_ids.end()
  );

  ism::ism_type const& the_ism = *ism::get_ism();

  for( ; ce_ids_it != ce_ids_end ; ++ce_ids_it ) {

    ism::ism_slice_type::nth_index<1>::type::iterator slice_it;
    ism::ism_slice_type::nth_index<1>::type::iterator slice_end;

    for( size_t i = 0; i <= ism::ism_ce_index_end; ++i ) {

      if( !the_ism[ism::ism_ce_index[i]] ||
        !the_ism[ism::ism_ce_index[i]]->slice) {
        continue;
      }

      ism::ism_mutex_type::scoped_lock lock (
       the_ism[ism::ism_ce_index[i]]->mutex
      );

      ism::ism_slice_type::nth_index<1>::type& index(
        the_ism[ism::ism_ce_index[i]]->slice->get<1>()
      );
  
      slice_it = index.begin();
      slice_end = index.end();
  
      slice_it = std::find_if(
        slice_it,
        slice_end,
        ism::key_starts_with(*ce_ids_it)
      );
      
      if(slice_it == slice_end) {
          continue;
      }
      break;
  
    } 

    if ( slice_it == slice_end ) break;


    do {

      boost::shared_ptr<classad::ClassAd> ce_ad_ptr(
        boost::tuples::get<ism::ad_ptr_entry>(*slice_it)
      );
      classad::ClassAd ce_ad(*ce_ad_ptr);
      if (utils::symmetric_match(ce_ad, jdl)) {
        
        std::string const ism_id(
             boost::tuples::get<ism::id_type_entry>(*slice_it)
        );
        
        Info(ism_id << ": ok!");
        suitableCEs[ism_id] = boost::tuples::make_tuple(
          std::make_pair(false, 0.0),
          ce_ad_ptr
        );
      }
    }
    while( ++slice_it!=slice_end  && 
           ism::key_starts_with(*ce_ids_it)(*(slice_it)) 
    );
  }

  typedef std::vector<std::string> previous_matches_type;
  previous_matches_type previous_matches;
  
  bool previous_matches_exists = false;
  jdlc::get_edg_previous_matches(
    jdl, 
    previous_matches, 
    previous_matches_exists
  );
  
  if (previous_matches_exists) {
    previous_matches_type::iterator const 
    previous_matches_begin(
      previous_matches.begin()
    );
    previous_matches_type::iterator const 
    previous_matches_end(
      previous_matches.end()
    );
    matchtable::iterator ces_it = suitableCEs.begin();
    matchtable::iterator const ces_end = suitableCEs.end();
    while( ces_it != ces_end ) {
      std::string const& ce_id = ces_it->first;
      if (
        find(
          previous_matches_begin,
          previous_matches_end, 
          ce_id) != previous_matches_end) {
  
        suitableCEs.erase(ces_it++);
      }
      else {
        ++ces_it;
      }
    }
  }
}
 
void
matchmakerISMImpl::checkRank(
  classad::ClassAd& jdl,
  matchtable& suitableCEs
)
{
  bool unable_to_rank_all = true;

  matchtable::iterator ces_it = suitableCEs.begin();
  matchtable::iterator const ces_end = suitableCEs.end();
  for ( ; ces_it != ces_end; ++ces_it) {

    std::string const& ce_id = ces_it->first;
    classad::ClassAd ce_ad(*getAd(ces_it->second).get());

    try {
      setRank(ces_it->second,utils::right_rank(ce_ad, jdl));
      unable_to_rank_all = false;
    } catch (utils::UndefinedRank&) {
      Error("Unexpected result while ranking " << ce_id);
    }
  }

  if (unable_to_rank_all) {
    throw matchmaking::RankingError();
  }
}

}}}
