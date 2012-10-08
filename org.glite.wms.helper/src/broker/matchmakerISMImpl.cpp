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

// File: matchmakerISMImpl.cpp
// Authors: Salvatore Monforte
//          Marco Cecchi
// Copyright (c) 2002 EU DataGrid.

// $Id: matchmakerISMImpl.cpp,v 1.15.2.11.2.6.2.3.2.3.2.3 2012/06/05 15:07:47 mcecchi Exp $

#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <algorithm>
#include <ctime>
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/wms/ism/ism.h"
#include "matchmakerISMImpl.h"
#include "mm_exceptions.h"

namespace jdlc = glite::jdl;

namespace {

std::string const gangmatch_storage_ad_str(
  "[CloseSEs = retrieveCloseSEsInfo(.VirtualOrganisation);]"
);

// regular expression to extract attribute names referenced with 'other.'
boost::regex pattern(
  "(.*?)other\\.(.*?)[+[\\-*/ ):?;,}&|\\^=!\\n<>%](.*)",
  boost::regex::perl | boost::regex::icase
);

void add_referenced_attributes(
  classad::ExprTree* expr_tree,
  std::set<std::string>& attributes)
{
  std::string expr_str;
  classad::ClassAdUnParser unp;
  unp.Unparse(expr_str, expr_tree);
  expr_str += ' '; // for an easier regexp

  // let's find all the 'other' attributes referenced in the requirements
  std::string::const_iterator start = expr_str.begin();
  std::string::const_iterator end = expr_str.end();
  boost::match_results<std::string::const_iterator> what;
  boost::match_flag_type flags = boost::match_default;
  while (regex_search(start, end, what, pattern, flags)) {
    attributes.insert(std::string(what[2].first, what[2].second));
    start = what[3].first;
    flags |= boost::match_prev_avail;
  }
}

} // anonymous

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
  bool has_jobid = false;
  std::string const job_id(jdl::get_edg_jobid(jdl, has_jobid));
  std::pair<boost::shared_ptr<void>, int> ret = ism::match_on_active_side();
  // ++matching threads on the currently active ISM side, as ret.first increases
  // by one the reference counting on that ISM side;
  // this will not prevent the ISM from
  // switching over with pending matches
  // insisting onto the target slice, but before
  // starting yet another purchasing (in background),
  // no pending matching_threads must exist
  ism::ism_type const& taken_ism = ism::get_ism(ism::ce, ret.second /* side */);

  // it doesn't matter if the active side becomes inactive from now on
  // the important is that we use taken_ism consistently
  std::time_t const t_start = std::time(0);
  int valid_entries = 0;
  ism::ism_type::const_iterator ism_it = taken_ism.begin();
  ism::ism_type::const_iterator const ism_end = taken_ism.end();

  std::set<std::string> attributes;

  // required by helper modules, so they must always be present in the 'enriched' classad
  attributes.insert("DataAccessProtocol");
  attributes.insert("GlueCEUniqueID");
  attributes.insert("GlobusResourceContactString");
  attributes.insert("QueueName");
  attributes.insert("LRMSType");
  attributes.insert("CEid");
  attributes.insert("GlueCEInfoHostName");
  attributes.insert("GlueCEInfoApplicationDir");

  // for match-making, we'll only need to retrieve those attributes
  // referenced in the requirements and rank expressions, not the whole ce ad
  for (classad::ClassAd::iterator it(jdl.begin()); it != jdl.end(); ++it) {
    add_referenced_attributes(it->second, attributes);
  }
  // this would be faster but not totally correct
  //add_referenced_attributes(jdl.Lookup("requirements"), attributes);
  //add_referenced_attributes(jdl.Lookup("rank"), attributes);

  bool match;
  classad::ClassAdParser parser;
  for ( ; ism_it != ism_end; ++ism_it) {

    boost::unordered_map<
      boost::flyweight<std::string>,
      boost::flyweight<std::string>,
      ism::flyweight_hash
    > ce_map(
      boost::tuples::get<ism::keyvalue_info_entry>(ism_it->second)
    );
    std::string ce_ad_str("[");
    boost::unordered_map<
      boost::flyweight<std::string>,
      boost::flyweight<std::string>,
      ism::flyweight_hash
    >::iterator const ce_end = ce_map.end();
    for (
      boost::unordered_map<
        boost::flyweight<std::string>,
        boost::flyweight<std::string>,
        ism::flyweight_hash
      >::iterator ce_it = ce_map.begin();
      ce_it != ce_end;
      ++ce_it
    ) {
      if (attributes.find(ce_it->first) != attributes.end()) {
          ce_ad_str += std::string(ce_it->first) + '=' + std::string(ce_it->second) + ';';
      }
    }
    ce_ad_str += "];";

    std::auto_ptr<classad::ClassAd> ce_ad_ptr(utils::parse_classad(ce_ad_str));
    std::auto_ptr<classad::ExprTree> gangmatch_storage_ad(
      parser.ParseExpression(gangmatch_storage_ad_str)
    );
    ce_ad_ptr->Insert("storage", gangmatch_storage_ad.get());
    gangmatch_storage_ad.release();

    // Insert here causes to delete the previous 'other', if it exists
    jdl.Insert("other", static_cast<classad::ExprTree*>(ce_ad_ptr.get()));
    ce_ad_ptr.release();
    // this per-entry lock is needed to synchronize with the updater thread
    // that updates values on the live side (without invalidating iterators)
    boost::mutex::scoped_lock l(*boost::tuples::get<ism::mutex_entry>(ism_it->second));
    if (-1 != boost::tuples::get<ism::expiry_time_entry>(ism_it->second))
    { // not set as expired by the ISM updater thread
        if (jdl.EvaluateAttrBool("requirements", match) && match) {

          boost::shared_ptr<classad::ClassAd> ce_ad_ptr_copy(
            static_cast<classad::ClassAd*>(jdl.Lookup("other")->Copy())
          );
          // TODO: optimization here, we don't need the whole ad. also, not
          // using ce_ad_ptr_copy will not need passing a Copy()
          suitableCEs[ism_it->first] = boost::tuples::make_tuple(
            std::make_pair(false, 0.0), ce_ad_ptr_copy
          );
        }
      ++valid_entries;
    }
    l.unlock();
  }
  jdl.Delete("other");

  std::time_t const t_mm = std::time(0) - t_start;
  if (has_jobid) {
    Info(
      "MM for job: " << job_id << " (" << suitableCEs.size() <<
      '/' << valid_entries << " [" << t_mm << "] )"
    );
  } else {
    Info(
      "MM for listmatch (" << suitableCEs.size() <<
      '/' << valid_entries << " [" << t_mm << "] )"
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

    boost::shared_ptr<matchtable> save_suitableCEs(
      new matchtable(suitableCEs)
    );

    previous_matches_type::iterator const previous_matches_begin(
      previous_matches.begin()
    );
    previous_matches_type::iterator const previous_matches_end(
      previous_matches.end()
    );

    matchtable::iterator ces_it = suitableCEs.begin();
    matchtable::iterator const ces_end = suitableCEs.end();
    while (ces_it != ces_end) {
      std::string ce_info_hostname;
      getAd(ces_it->second)->EvaluateAttrString(
        "GlueCEInfoHostName", ce_info_hostname);
      if (find(
            previous_matches_begin,
            previous_matches_end,
            ce_info_hostname
          ) != previous_matches_end) {
        suitableCEs.erase(ces_it++);
      } else {
        ++ces_it;
      }
    }

    if (suitableCEs.empty()) {
      suitableCEs.swap(*save_suitableCEs);
    }
  }
}

namespace {
struct starts_with
{
 std::string m_pattern;
 starts_with(const std::string& p) : m_pattern(p) {}
 bool operator()(ism::ism_type::value_type const& v)
 {
   return boost::starts_with(std::string(v.first), m_pattern);
 }
};
}

void
matchmakerISMImpl::checkRequirement(
  classad::ClassAd& jdl,
  std::set<matchtable::key_type> const& ce_ids,
  matchtable& suitableCEs
)
{  

  bool has_jobid = false;
  std::string const job_id(jdl::get_edg_jobid(jdl, has_jobid));
  std::time_t const t_start = std::time(0);
  int valid_entries = 0;
 
  std::set<std::string> attributes;
  // for match-making, we'll only need to retrieve those attributes
  // referenced in the requirements and rank expressions, not the whole ce ad
  add_referenced_attributes(jdl.Lookup("requirements"), attributes);
  add_referenced_attributes(jdl.Lookup("rank"), attributes);

  bool match;
  classad::ClassAdParser parser;

  std::set<matchtable::key_type>::const_iterator ce_ids_it = ce_ids.begin();
  std::set<matchtable::key_type>::const_iterator const ce_ids_end = ce_ids.end();

  std::pair<boost::shared_ptr<void>, int> ret = ism::match_on_active_side();
  // ++matching threads on the currently active ISM side, as ret.first increases
  // by one the reference counting on that ISM side;
  // this will not prevent the ISM from
  // switching over with pending matches
  // insisting onto the target slice, but before
  // starting yet another purchasing in background,
  // no pending matching_threads must exist
  ism::ism_type& taken_ism = ism::get_ism(ism::ce, ret.second /* side */);
  ism::ism_type::const_iterator const ism_end = taken_ism.end();
  for( ; ce_ids_it != ce_ids_end ; ++ce_ids_it ) {

    // Previous match handling
    ism::ism_type::iterator ism_it(
      std::find_if(
        taken_ism.begin(),
        taken_ism.end(),
        starts_with(*ce_ids_it)
      )
    );

    if (ism_it == ism_end) {
      continue;
    }
 
    do {
      boost::unordered_map<
        boost::flyweight<std::string>,
        boost::flyweight<std::string>,
        ism::flyweight_hash
      > ce_map(
        boost::tuples::get<ism::keyvalue_info_entry>(ism_it->second)
      );
      std::string ce_ad_str("[");
      boost::unordered_map<
        boost::flyweight<std::string>,
        boost::flyweight<std::string>,
        ism::flyweight_hash
      >::iterator const ce_end = ce_map.end();
      for (
        boost::unordered_map<
          boost::flyweight<std::string>,
          boost::flyweight<std::string>,
          ism::flyweight_hash
        >::iterator ce_it = ce_map.begin();
        ce_it != ce_end;
        ++ce_it
      ) {
        if (attributes.find(ce_it->first) != attributes.end()) {
          ce_ad_str += std::string(ce_it->first) + '=' + std::string(ce_it->second) + ';';
        }
      }
      ce_ad_str += "];";

      std::auto_ptr<classad::ClassAd> ce_ad_ptr(utils::parse_classad(ce_ad_str));
      std::auto_ptr<classad::ExprTree> gangmatch_storage_ad(
        parser.ParseExpression(gangmatch_storage_ad_str)
      );
      ce_ad_ptr->Insert("storage", gangmatch_storage_ad.get());
      gangmatch_storage_ad.release();

      // Insert here causes to delete the previous 'other', if it exists
      jdl.Insert("other", static_cast<classad::ExprTree*>(ce_ad_ptr->Copy()));
      ce_ad_ptr.release();

      boost::mutex::scoped_lock l(*boost::tuples::get<ism::mutex_entry>(ism_it->second));
      if (-1 != boost::tuples::get<ism::expiry_time_entry>(ism_it->second)) {
        if (jdl.EvaluateAttrBool("requirements", match) && match) {

          boost::shared_ptr<classad::ClassAd> ce_ad_ptr_copy(
            static_cast<classad::ClassAd*>(jdl.Lookup("other")->Copy())
          );
          // TODO II refactoring: optimization here
          suitableCEs[ism_it->first] = boost::tuples::make_tuple(
            std::make_pair(false, 0.0), ce_ad_ptr_copy
          );
        }
        ++valid_entries;
      }
      l.unlock();

      if (++ism_it == ism_end) {
        break;
      }

      ism_it = std::find_if(
        ism_it,
        taken_ism.end(),
        starts_with(*ce_ids_it)
      );
    } while (ism_it != ism_end);
    jdl.Delete("other");
  }

  std::time_t const t_mm = std::time(0) - t_start;
  if (has_jobid) {
    Info(
      "MM for job: " << job_id << " (" << suitableCEs.size() <<
      '/' << valid_entries << " [" << t_mm << "] )"
    );
  } else {
    Info(
      "MM for listmatch (" << suitableCEs.size() <<
      '/' << valid_entries << " [" << t_mm << "] )"
    );
  }
}

void
matchmakerISMImpl::checkRank(
  classad::ClassAd& jdl,
  matchtable& suitableCEs
)
{
  if (suitableCEs.empty()) return;

  bool unable_to_rank_all = true;
  
  matchtable::iterator ces_it = suitableCEs.begin();
  matchtable::iterator const ces_end = suitableCEs.end();
  for ( ; ces_it != ces_end; ++ces_it) {

    std::string const ce_id = ces_it->first;
    classad::ClassAd ce_ad(*getAd(ces_it->second));

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
