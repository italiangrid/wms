// File: match.cpp
// Author: Francesco Giacomini
// Author: Salvatore Monforte

// $Id$

#include "glite/wms/broker/match.h"

#include <algorithm>
#include <numeric>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/timer.hpp>

#include "storage_utils.h"
#include "glite/wms/common/utilities/scope_guard.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/common/logger/logging.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/wms/ism/ism.h"

#include <boost/algorithm/string/predicate.hpp>

namespace jdl = glite::jdl;

namespace glite {

namespace classad_utils = wmsutils::classads;
namespace utils = wms::common::utilities;

namespace wms {
namespace broker {

namespace {

enum MatchAndRankResult
{
  NO_MATCH,
  MATCH_NO_RANK,
  MATCH_AND_RANK
};

MatchAndRankResult
match_and_rank(
  classad::ClassAd* lhs,
  classad::ClassAd* rhs,
  double& rank_result
)
{
  classad::MatchClassAd match_ad(lhs, rhs);
  utils::scope_guard remove_left_ad(
    boost::bind(&classad::MatchClassAd::RemoveLeftAd, boost::ref(match_ad))
  );
  utils::scope_guard remove_right_ad(
    boost::bind(&classad::MatchClassAd::RemoveRightAd, boost::ref(match_ad))
  );

  MatchAndRankResult result = NO_MATCH;
  bool match_result = false;
  match_ad.EvaluateAttrBool("symmetricMatch", match_result);

  if (match_result) {
    try {
      rank_result =
        classad_utils::evaluate_attribute(match_ad, "rightRankValue");
      result = MATCH_AND_RANK;
    } catch (classad_utils::InvalidValue&) {
      result = MATCH_NO_RANK;
    }
  }

  return result;
}

class match_slice_content
{
  classad::ClassAd* jdl_ptr;

public:
  match_slice_content(classad::ClassAd& j)
  : jdl_ptr(&j)
  {
  }
  MatchTable*
  operator()(MatchTable* matches, ism::MutexSlicePtr mt_slice) const
  {
    boost::timer t;
    ism::Mutex::scoped_lock l(mt_slice->mutex);
    double t_lock = t.elapsed();

    ism::Slice::const_iterator it = mt_slice->slice->begin();
    ism::Slice::const_iterator const e = mt_slice->slice->end();

    size_t const slice_size = mt_slice->slice->size();
    int rank_failures = 0;

    for( ; it != e; ++it) {

      boost::shared_ptr<classad::ClassAd> ce_ad_ptr = it->ad;

      classad::ClassAd& ce_ad(*ce_ad_ptr);

      std::string const& ism_id(it->id);

      double rank;
      switch (match_and_rank(&ce_ad, jdl_ptr, rank)) {
      case NO_MATCH:
        break;
      case MATCH_NO_RANK:
        ++rank_failures;
        break;
      case MATCH_AND_RANK:
        matches->push_back(
          MatchInfo(ism_id, rank, ce_ad_ptr)
        );
        break;
      }
    }

    l.unlock();

    bool has_jobid = false;
    std::string const job_id(jdl::get_edg_jobid(*jdl_ptr, has_jobid));
    if (has_jobid) {
      GLITE_LOG_ACCESS_INFO<<
        "MM for job: " << job_id
        << " (" << matches->size() << '/' << slice_size << '/' << rank_failures
        << " [" << t_lock << ", " << t.elapsed() << "] )"
      ;
    } else {
      GLITE_LOG_ACCESS_INFO<<
        "MM for listmatch ("
        << matches->size() << '/' << slice_size << '/' << rank_failures
        << " [" << t_lock << ", " << t.elapsed() << "] )"
      ;
    }

    return matches;
  }
};

class match_slice_content_if
{
  classad::ClassAd* jdl_ptr;
  boost::function< bool(ism::SliceEntry const&) > pred;

public:
  match_slice_content_if(
    classad::ClassAd& j,
    boost::function< bool(ism::SliceEntry const&)> p
  )
  : jdl_ptr(&j), pred(p)
  {
  }
  MatchTable*
  operator()(MatchTable* matches, ism::MutexSlicePtr mt_slice) const
  {
    ism::Mutex::scoped_lock l(mt_slice->mutex);
    ism::Slice::const_iterator it = mt_slice->slice->begin();
    ism::Slice::const_iterator const e = mt_slice->slice->end();

    int rank_failures = 0;

    for( ; it != e; ++it) {

      if (!pred(*it)) {
        continue;
      }

      boost::shared_ptr<classad::ClassAd> ce_ad_ptr = it->ad;

      classad::ClassAd ce_ad(*ce_ad_ptr);

      std::string const& ism_id(it->id);

      double rank;
      switch (match_and_rank(&ce_ad, jdl_ptr, rank)) {
      case NO_MATCH:
        break;
      case MATCH_NO_RANK:
        ++rank_failures;
        break;
      case MATCH_AND_RANK:
        matches->push_back(
          MatchInfo(ism_id, rank, ce_ad_ptr)
        );
        break;
      }
    }

    return matches;
  }
};

struct pm_equal_to_match{

  previous_match const& m_pm;
  pm_equal_to_match(previous_match const& pm): m_pm(pm){}

  bool operator()(MatchInfo const& ce){
    return boost::starts_with(ce.ce_id, m_pm.id);
  }

};


void remove_previous_matches(
  MatchTable& matches,
  std::vector<previous_match> const& skipping_ces
){

  typedef std::set<
    previous_match,
    previous_match::less_than
  > previous_matches_t;

  if ( skipping_ces.empty() ) return;

  previous_matches_t previous_matches;
  std::copy(
    skipping_ces.begin(),
    skipping_ces.end(),
    std::inserter(previous_matches, previous_matches.begin())
  );

  boost::shared_ptr<MatchTable> save_suitableCEs( new MatchTable(matches) );

  previous_matches_t::const_iterator pm_it = previous_matches.begin();
  previous_matches_t::const_iterator const pm_end = previous_matches.end();

  for( ; pm_it != pm_end ; ++pm_it ){

    matches.erase(
      std::remove_if(
        matches.begin(),
        matches.end(),
        pm_equal_to_match(*pm_it) 
      ),
      matches.end()
    );

  }

  if( matches.empty() && save_suitableCEs )
    matches.swap(*save_suitableCEs);

}

} // anonymous namespace

void
match(
  classad::ClassAd& ad,
  MatchTable& matches,
  std::vector<previous_match> const& skipping_ces
)
{
  ism::Ism const& the_ism = *ism::get_ism();

  std::accumulate(
    the_ism.computing.begin(),
    the_ism.computing.end(),
    &matches,
    match_slice_content(ad)
  );

  remove_previous_matches(matches, skipping_ces); 

}

void
match(
  classad::ClassAd& ad,
  MatchTable& matches,
  std::set<std::string> const& candidate_ces,
  std::vector<previous_match> const& skipping_ces
)
{
  std::set<std::string>::const_iterator ce_it = candidate_ces.begin();
  std::set<std::string>::const_iterator const ce_end(
    candidate_ces.end()
  );

  ism::Ism const& the_ism = *ism::get_ism();

  // TODO: change the loop using a new predicate for
  // the match_slice_content_if which look in the ordered container
  for( ; ce_it != ce_end ; ++ce_it ) {

    accumulate(
      the_ism.computing.begin(),
      the_ism.computing.end(),
      &matches,
      match_slice_content_if(ad, ism::KeyStartsWith(*ce_it))
    );

  }

  remove_previous_matches(matches, skipping_ces);

}

void
match(
  classad::ClassAd& ad,
  MatchTable& matches,
  DataInfo& data_info,
  std::vector<previous_match> const& skipping_ces
)
{
  // Collects SFNs and involved SEs.
  data_info.fm = resolve_filemapping_info(ad);
  data_info.sm = resolve_storagemapping_info(data_info.fm);

  // Selects only comptatible storage
  std::vector<std::string> dap;
  jdl::get_data_access_protocol(ad, dap);
  std::vector<StorageMapping::const_iterator> compatible_storage(
    select_compatible_storage(data_info.sm,dap)
  );

  // std::set required in order to remove duplicates
  std::set<std::string> close_computing_elements_id;
  // selects id of those computing elements bound to
  // compatible storage
  std::accumulate(
    compatible_storage.begin(),
    compatible_storage.end(),
    &close_computing_elements_id,
    extract_computing_elements_id()
  );

  // Filter computing elements on requirements
  match(ad, matches, close_computing_elements_id, skipping_ces);

  MatchTable::iterator ce_it = matches.begin();
  MatchTable::iterator const ce_end = matches.end();

  std::map<
    size_t,
    std::vector<MatchTable::const_iterator>
  > unique_logical_files_per_ce;

  size_t max_files = 0;
  for( ; ce_it != ce_end; ) {

    std::string const& id = ce_it->ce_id;
    std::vector<StorageMapping::const_iterator>::iterator
    storage_part_end(
      std::partition(
        compatible_storage.begin(),
        compatible_storage.end(),
        is_storage_close_to(id)
    ));

    size_t n = count_unique_logical_files(
      compatible_storage.begin(), storage_part_end
    );

    GLITE_LOG_ACCESS_DEBUG<<
      id << " has #" <<
      storage_part_end - compatible_storage.begin() <<
      " close compatible storage element(s) providing #" << n <<
      " accessible file(s)"
    ;

    if ( n > max_files ) max_files = n;

    std::map<
      size_t, std::vector<MatchTable::const_iterator>
    >::iterator it;

    bool inserted = false;
    boost::tie(it,inserted) = unique_logical_files_per_ce.insert(
      std::make_pair(n,std::vector<MatchTable::const_iterator>())
    );

    it->second.push_back(ce_it);
    ++ce_it;
  }
  // return only CEs accessing the max number of logical files
  std::vector<MatchTable::const_iterator> const& max_logical_files_ces(
    unique_logical_files_per_ce[max_files]
  );

  MatchTable::iterator it = matches.begin();
  MatchTable::iterator const e = matches.end();
  while (it != e) {
    if (std::find(
          max_logical_files_ces.begin(),
          max_logical_files_ces.end(),
          it
        ) == max_logical_files_ces.end()) {
      matches.erase(it++);
    } else {
      ++it;
    }
  }
}

}}}
