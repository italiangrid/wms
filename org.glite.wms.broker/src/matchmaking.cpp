// $Id:

#include <algorithm>
#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/timer.hpp>
#include "matchmaking.h"
#include "glite/wms/common/utilities/scope_guard.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/common/utilities/wm_commands.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/wms/ism/ism.h"
#include "exceptions.h"

namespace jdl = glite::jdl;

namespace glite {
namespace classad_utils = wmsutils::classads;
namespace utils = wms::common::utilities;
namespace wms {
namespace broker {

namespace {

bool
match_and_rank(
  classad::ClassAd* lhs, 
  classad::ClassAd* rhs,
  double& rank_result
)
{
  classad::MatchClassAd match_ad(lhs, rhs);

  bool match_result = false;
  match_ad.EvaluateAttrBool("symmetricMatch", match_result);
  utils::scope_guard remove_left_ad(
    boost::bind(&classad::MatchClassAd::RemoveLeftAd, boost::ref(match_ad))
  );
  utils::scope_guard remove_right_ad(
    boost::bind(&classad::MatchClassAd::RemoveRightAd, boost::ref(match_ad))
  );

  if (match_result)
  try {
    rank_result =
      classad_utils::evaluate_attribute(match_ad, "rightRankValue");
  } catch (classad_utils::InvalidValue&) {
    throw classad_utils::UndefinedRank();
  }
  return match_result;
}

struct match_slice_content : std::binary_function<
  matchtable*, ism::MutexSlicePtr, matchtable*
>
{
classad::ClassAd* jdl_ptr;
  match_slice_content(classad::ClassAd& j)
  : jdl_ptr(&j) 
  {
  }
  matchtable*
  operator()(
    matchtable* suitableCEs,
    ism::MutexSlicePtr mt_slice
  )
  {
    bool has_jobid = false;
    std::string const job_id(jdl::get_edg_jobid(*jdl_ptr, has_jobid));
    boost::timer t;
    ism::Mutex::scoped_lock l(mt_slice->mutex);
    double t_lock = t.elapsed();
    
    ism::Slice::const_iterator it = mt_slice->slice->begin();
    ism::Slice::const_iterator const e = mt_slice->slice->end();

    for( ; it != e; ++it) {

      if (ism::is_entry_void(*it)) continue;

      boost::shared_ptr<classad::ClassAd> ce_ad_ptr =
        boost::tuples::get<ism::Ad>(*it);

      classad::ClassAd& ce_ad(*ce_ad_ptr);

      std::string const& ism_id(boost::tuples::get<ism::Id>(*it));

      try {
        double rank;
        if (match_and_rank(&ce_ad, jdl_ptr, rank)) {
          suitableCEs->push_back(
            boost::tuples::make_tuple( ism_id, rank, ce_ad_ptr )
          );
        }
      } catch (classad_utils::UndefinedRank&) {
        Error("Unexpected result while ranking " << ism_id);
      }
    }
    l.unlock();

    if (has_jobid) {
      Info("MM for job: " << job_id << " (" << t_lock << ", " << t.elapsed() << ')');
    } else {
      Info("MM listmatch (" << t_lock << ", " << t.elapsed() << ')');
    }

    return suitableCEs;
  }
};

struct match_slice_content_if : std::binary_function<
  matchtable*, ism::MutexSlicePtr, matchtable*
>
{
classad::ClassAd* jdl_ptr;
boost::function< bool(ism::SliceEntry const&) > pred;
  match_slice_content_if(
    classad::ClassAd& j, 
    boost::function< bool(ism::SliceEntry const&)> p
  )
  : jdl_ptr(&j), pred(p)
  {
  }
  matchtable*
  operator()(
    matchtable* suitableCEs,
    ism::MutexSlicePtr mt_slice
  )
  {
    ism::Mutex::scoped_lock l(mt_slice->mutex);
    ism::Slice::const_iterator it = mt_slice->slice->begin();
    ism::Slice::const_iterator const e = mt_slice->slice->end();

    for( ; it != e; ++it) {

      if (ism::is_entry_void(*it)) continue;
      if (!pred(*it)) continue;

      boost::shared_ptr<classad::ClassAd> ce_ad_ptr =
        boost::tuples::get<ism::Ad>(*it);

      classad::ClassAd ce_ad(*ce_ad_ptr);

      std::string const& ism_id(boost::tuples::get<ism::Id>(*it));

      try {
        double rank;
        if (match_and_rank(&ce_ad, jdl_ptr, rank)) {
          Info(ism_id << ": ok!");
          suitableCEs->push_back(
            boost::tuples::make_tuple( ism_id, rank, ce_ad_ptr )
          );
        }
      } catch (classad_utils::UndefinedRank&) {
        Error("Unexpected result while ranking " << ism_id);
      }
    }
    return suitableCEs;
  }
};

typedef std::vector<std::string> previous_matches_type;

struct in 
{
previous_matches_type::iterator m_begin;
previous_matches_type::iterator const m_end;

  in(previous_matches_type& pm)
   : m_begin(pm.begin()), m_end(pm.end()) 
  {
  }
  bool operator()(matchinfo const& i)
  {
    std::string const& s = boost::tuples::get<0>(i);
    return std::find(m_begin, m_end, s) != m_end;
  }
};

} // anonymous namespace

void
match(
  classad::ClassAd& jdl,
  matchtable& suitableCEs
) 
{
  ism::Ism const& the_ism = *ism::get_ism();

  std::accumulate(
    the_ism.computing.begin(),
    the_ism.computing.end(),
    &suitableCEs,
    match_slice_content(jdl)
  );

  previous_matches_type previous_matches;
  
  bool previous_matches_exists = false;
  jdl::get_edg_previous_matches(
    jdl, 
    previous_matches, 
    previous_matches_exists
  );
  
  if (previous_matches_exists) {
    matchtable::iterator _(      
    std::remove_if(
        suitableCEs.begin(), suitableCEs.end(), in(previous_matches)
      )
    );
    suitableCEs.erase(_, suitableCEs.end());
  }
}

void
match(
  classad::ClassAd& jdl,
  std::set<std::string> const& ce_ids,
  matchtable& suitableCEs
)
{
  std::set<std::string>::const_iterator ce_ids_it = ce_ids.begin();
  std::set<std::string>::const_iterator const ce_ids_end(
    ce_ids.end()
  );

  ism::Ism const& the_ism = *ism::get_ism();
   
  for( ; ce_ids_it != ce_ids_end ; ++ce_ids_it ) {
  
    accumulate(
      the_ism.computing.begin(),
      the_ism.computing.end(),
      &suitableCEs,
      match_slice_content_if(jdl, ism::KeyStartsWith(*ce_ids_it))
    );
  }

  previous_matches_type previous_matches;
  
  bool previous_matches_exists = false;
  jdl::get_edg_previous_matches(
    jdl, 
    previous_matches, 
    previous_matches_exists
  );
  
  if (previous_matches_exists) {
    matchtable::iterator _(
    std::remove_if(
        suitableCEs.begin(), suitableCEs.end(), in(previous_matches)
      )
    );
    suitableCEs.erase(_, suitableCEs.end());
  }
}

}}}
