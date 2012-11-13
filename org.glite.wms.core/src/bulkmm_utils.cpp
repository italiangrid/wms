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

#include <iostream>
#include <algorithm>
#include <iterator>
#include <map>
#include <string>
#include <vector>
#include <cmath>

#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/regex.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/array.hpp>

#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/DAGAd.h"
#include "glite/jdl/DAGAdManipulation.h"
#include "glite/wms/helper/Helper.h"
#include "glite/wms/helper/jobadapter/JobAdapter.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"

#include "match_utils.h"
#include "bulkmm_utils.h"

namespace jdl = glite::jdl;
namespace helper = glite::wms::helper;
namespace jobadapter = glite::wms::helper::jobadapter;
namespace configuration = glite::wms::common::configuration;

namespace helper = glite::wms::helper;

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {

boost::minstd_rand f_rnd(time(0));
boost::uniform_01<boost::minstd_rand> f_unirand01(f_rnd);

typedef std::pair<
  std::string,                  // name
  std::string                   // value
> SignificantAttribute;

typedef std::vector<SignificantAttribute> SignificantAttributes;

struct ClusterKey
{
  SignificantAttributes significant_attributes;
};

bool operator<(ClusterKey const&, ClusterKey const&);

enum {
  jdl_tag,
  names_tag,
  matches_tag
};

typedef std::vector<std::string> JobNames;

typedef boost::tuple<
  classad::ClassAd,         // template jdl
  JobNames,                 // job names in this cluster
  matches_type              // match result
> Cluster;

typedef std::map<
  ClusterKey,
  Cluster
> Clusters;

class Clusterize
{
  glite::jdl::DAGAd const* m_dagad;
  Clusters* m_clusters;
public:
  Clusterize(glite::jdl::DAGAd const&, Clusters&);
  void operator()(glite::jdl::DAGAd::node_value_type const&);
};

class Schedule
{
  ScheduledJobs* m_scheduled_jobs;
  PendingJobs* m_pending_jobs;
  glite::jdl::DAGAd const* m_dagad;
  boost::shared_ptr<std::string> m_jw_template;
public:
  Schedule(
    ScheduledJobs&,
    PendingJobs&,
    glite::jdl::DAGAd const&,
    boost::shared_ptr<std::string> jw_template
  );
  void operator()(Clusters::value_type&);
};

std::vector<std::string>
lookup_significant_attributes(
  classad::ClassAd const& job_jdl,
  classad::ClassAd const& dag_jdl
)
{
  std::vector<std::string> result;

  bool found = false;
  jdl::get_significant_attributes(job_jdl, result, found);
  if (!found) {
    // try in nodes
    classad::ClassAd const* const nodes(
      static_cast<classad::ClassAd const*>(dag_jdl.Lookup("nodes"))
    );
    assert(nodes);
    jdl::get_significant_attributes(*nodes, result, found);
    if (!found) {
      // try in the dag
      jdl::get_significant_attributes(dag_jdl, result, found);
    }
  }

  return result;
}

classad::ExprTree const*
lookup(
  classad::ClassAd const& job_jdl,
  classad::ClassAd const& dag_jdl,
  std::string const& attribute
)
{
  classad::ExprTree const* result = job_jdl.Lookup(attribute);
  if (!result) {
    // try in nodes
    classad::ClassAd const* const nodes(
      static_cast<classad::ClassAd const*>(dag_jdl.Lookup("nodes"))
    );
    assert(nodes);
    result = nodes->Lookup(attribute);
    if (!result) {
      result = dag_jdl.Lookup(attribute);
    }
  }

  return result;
}

ClusterKey
make_key(
  classad::ClassAd const& job_jdl,
  classad::ClassAd const& dag_jdl,
  std::string const& name
)
{
  ClusterKey result;

  std::vector<std::string> const attributes(
    lookup_significant_attributes(job_jdl, dag_jdl)
  );
  classad::ClassAdUnParser unparser;
  std::vector<std::string>::const_iterator first = attributes.begin();
  std::vector<std::string>::const_iterator const last = attributes.end();
  for ( ; first != last; ++first) {
    std::string const& attribute = *first;
    classad::ExprTree const* expr = lookup(job_jdl, dag_jdl, attribute);
    if (expr) {
      std::string value;
      unparser.Unparse(value, expr);
      result.significant_attributes.push_back(
        std::make_pair(attribute, value)
      );
    }
  }

  if (result.significant_attributes.empty()) {
    result.significant_attributes.push_back(
      std::make_pair(name, std::string())
    );
  }

  return result;
}

class rank_greater_than
  : public std::unary_function<match_type, bool>
{
  double m_rank;
public:
  rank_greater_than(double rank)
    : m_rank(rank)
  {
  }
  bool operator()(match_type const& match)
  {
    return match.get<1>() > m_rank;
  }
};

class rank_equal_to
  : public std::unary_function<match_type, bool>
{
  double m_rank;
public:
  rank_equal_to(double rank)
    : m_rank(rank)
  {
  }
  bool operator()(match_type const& match)
  {
    return match.get<1>() == m_rank;
  }
};


bool rank_less_than(match_type a, match_type b)
{
    return a.get<1>() < b.get<1>();
}

template<typename Container, typename T>
T variance(Container const& c, T mean)
{
  T v = T();
  size_t n = 0;
  typename Container::const_iterator first = c.begin();
  typename Container::const_iterator const last = c.end();
  for ( ; first != last; ++first, ++n) {
    T t = *first - mean;
    v += t * t;
  }

  return n ? v / n : v;  
}

matches_type::iterator max_partition(matches_type& matches)
{
  if(matches.empty()) return matches.end();

  matches_type::const_iterator const max(
    std::max_element(
      matches.begin(), matches.end(), 
      rank_less_than
    )
  );
  return std::partition(
    matches.begin(), 
    matches.end(), 
    rank_equal_to(max->get<1>())
  );
}

std::vector<double> make_distribution(matches_type const& matches, double f)
{
  std::vector<double> ranks;
  ranks.reserve(matches.size());
  double rank_sum = 0.;
  matches_type::const_iterator b = matches.begin();
  matches_type::const_iterator const e = matches.end();
  std::string dbg("Ranks: ");
  for (; b != e; ++b) {
    double r = b->get<1>();
    ranks.push_back(r);
    rank_sum += r;
    dbg += boost::lexical_cast<std::string>(r);
    if (b + 1 != e) {
      dbg += ", ";
    }
  }
  Debug(dbg);

  double rank_mean = rank_sum / ranks.size();
  // double const rank_variance = variance(ranks, rank_mean);
  static const double PI_2 = atan(1.) * 2.;
  
  dbg.assign("Normalized ranks: ");
  for (size_t r = 0; r < ranks.size(); ++r) {
    ranks[r] = atan( f * (ranks[r] - rank_mean) ) + PI_2;
    dbg += boost::lexical_cast<std::string>(ranks[r]);
    if (r + 1 < ranks.size()) {
      dbg += ", ";
    }
  }
  std::partial_sum(ranks.begin(), ranks.end(), ranks.begin());
  Debug(dbg);
  return ranks;
}

class SelectDestination
{
  std::vector<double> const* m_distribution;
  matches_type::const_iterator const m_first_match;
  matches_type::const_iterator const m_last_match;

public:
  typedef boost::optional<matches_type::const_iterator> result_type;

  SelectDestination(
    std::vector<double> const& distribution,
    matches_type::const_iterator const& first_match,
    matches_type::const_iterator const& last_match
  )
    : m_distribution(&distribution), 
      m_first_match(first_match), m_last_match(last_match)
  {
  }
  result_type operator()(std::string const& node_name)
  {
    if (m_distribution->empty()) {
      return result_type();
    } else {

      double const p = f_unirand01() * m_distribution->back();
      std::vector<double>::const_iterator const i(
        std::lower_bound(m_distribution->begin(), m_distribution->end(), p)
      );

      matches_type::const_iterator result = m_first_match;
      std::advance(result, std::distance(m_distribution->begin(), i));

      return result;
    }
  }
};

classad::ClassAd
get_jdl(std::string const& node_name, jdl::DAGAd const& dagad)
{
  jdl::DAGAd::node_iterator it = dagad.find(node_name);
  return it != jdl::DAGAd::node_iterator() ? classad::ClassAd(*it->second.description_ad()) : classad::ClassAd();
}

ClassAdPtr
make_planned_jdl(
  classad::ClassAd const& jdl,
  match_type const& the_match
)
{
  ClassAdPtr result(new classad::ClassAd(jdl));

  static boost::regex expression( "(([^\\:]+):[0-9]+/[^\\-]+-([^\\-]+))-(.+)" );
  boost::smatch pieces;
  
  std::string ce_id(the_match.get<0>());

  if (boost::regex_match(ce_id, pieces, expression)) {

    jdl::set_globus_resource_contact_string(
      *result,
      std::string(pieces[1].first, pieces[1].second)
    );

    jdl::set_lrms_type(
      *result,
      std::string(pieces[3].first, pieces[3].second)
    );

    jdl::set_queue_name(
      *result,
      std::string(pieces[4].first, pieces[4].second)
    );
    jdl::set_ceinfo_host_name(
      *result, 
      std::string(pieces[2].first, pieces[2].second)
    );
    jdl::set_ce_id(*result, ce_id);

  }

  return result;
}

void add_brokerinfo_to_isb(classad::ClassAd& jdl)
{
  bool isb_exists = false;
  std::vector<std::string> isb;
  jdl::get_input_sandbox(jdl, isb, isb_exists);
  bool isb_base_uri_exists = false;
  std::string isb_base_uri =
    jdl::get_wmpinput_sandbox_base_uri(jdl, isb_base_uri_exists);
  if (isb_base_uri_exists) {
    isb.push_back(isb_base_uri + "/input/.BrokerInfo");
  } else {
    isb.push_back(".BrokerInfo");
  }
  jdl::set_input_sandbox(jdl, isb);
}

void Match(Clusters::value_type& v)
{
  Cluster& cluster = v.second;
  classad::ClassAd jdl = cluster.get<jdl_tag>();
  bool const include_brokerinfo = true;
  bool const include_cream_resources = true;
  int const number_of_results = 100;//cluster.get<names_tag>().size(); // needed/useful?
  jdl.InsertAttr("include_brokerinfo", include_brokerinfo);
  jdl.InsertAttr("number_of_results", number_of_results);
  
  bool submit_to_exists = false;
  jdl::get_submit_to(jdl, submit_to_exists);

  ClassAdPtr match_result(helper::Helper(
    submit_to_exists ? "BrokerHelper" : "MatcherHelper"
  ).resolve(&jdl));
  
  try {
    fill_matches(
      *match_result, 
      cluster.get<matches_tag>(),
      include_brokerinfo,
      include_cream_resources
    );
  } catch (MatchError const& e) {
    Error( e.what() );
  }
}

bool operator<(ClusterKey const& lhs, ClusterKey const& rhs)
{
  return lhs.significant_attributes < rhs.significant_attributes;
}

Clusterize::Clusterize(jdl::DAGAd const& dagad, Clusters& clusters)
  : m_dagad(&dagad), m_clusters(&clusters)
{
}

void Clusterize::operator()(jdl::DAGAd::node_value_type const& v)
{
  std::string const node_name = v.first;
  jdl::DAGNodeInfo const& node = v.second;
  classad::ClassAd const& jdl = *node.description_ad();
  std::string const id = jdl::get_edg_jobid(jdl);
  ClusterKey key(make_key(jdl, m_dagad->ad(), node_name));
  Cluster& cluster = (*m_clusters)[key];
  if (cluster.get<names_tag>().empty()) {
    cluster.get<jdl_tag>() = jdl;
  }
  cluster.get<names_tag>().push_back(node_name);
}

Schedule::Schedule(
  ScheduledJobs& scheduled_jobs, 
  PendingJobs& pending_jobs, 
  jdl::DAGAd const& dagad,
  boost::shared_ptr<std::string> jw_template
) : 
  m_scheduled_jobs(&scheduled_jobs), 
  m_pending_jobs(&pending_jobs), 
  m_dagad(&dagad),
  m_jw_template(jw_template)
{
}

void Schedule::operator()(Clusters::value_type& v)
{
  Cluster& cluster = v.second;
  classad::ClassAd jdl = cluster.get<jdl_tag>();
  JobNames const& node_names = cluster.get<names_tag>();
  matches_type& matches = cluster.get<matches_tag>();
  

  bool stochastic = false;
  double fuzzy_factor = 1;
  try {
    stochastic = jdl::get_fuzzy_rank(jdl);
    fuzzy_factor = jdl::get_fuzzy_factor(jdl);
  }
  catch(...)
  {
  }

  std::vector<SelectDestination::result_type> destinations;
  destinations.reserve(node_names.size());
 
  if(stochastic) {
    std::vector<double> distribution(
      make_distribution(matches, fuzzy_factor)
    );
    std::transform(
      node_names.begin(),
      node_names.end(),
      std::back_inserter(destinations),
      SelectDestination(
        distribution, matches.begin(), matches.end()
      )
    );
  } 
  else {
    matches_type::iterator const max_partition_end(
      max_partition(matches)
    );

    std::vector<double> ranks(
      std::distance(matches.begin(), max_partition_end), 
      1
    );

    std::partial_sum(ranks.begin(), ranks.end(), ranks.begin());

    std::transform(
      node_names.begin(),
      node_names.end(),
      std::back_inserter(destinations),
      SelectDestination(
        ranks, matches.begin(), max_partition_end
      ) 
    );
  }

  assert(node_names.size() == destinations.size());
  std::string _;
  for (size_t i = 0; i < node_names.size(); ++i) {
    std::string const node_name = node_names[i];
    if (destinations[i]) {
      match_type const& the_match = **destinations[i];

      classad::ClassAd jdl = get_jdl(node_name, *m_dagad);

      ClassAdPtr planned_jdl(make_planned_jdl(jdl, the_match));
      ClassAdPtr brokerinfo(the_match.get<2>());

      classad::ExprTree const* dac = planned_jdl->Lookup("DataAccessProtocol");
      if (dac) {
        brokerinfo->Insert("DataAccessProtocol", dac->Copy());
      }

      add_brokerinfo_to_isb(*planned_jdl);

      ClassAdPtr adapted_jdl(
        jobadapter::JobAdapter(planned_jdl.get(), m_jw_template).resolve()
      );

      m_scheduled_jobs->push_back(
        boost::tuples::make_tuple(planned_jdl, adapted_jdl, brokerinfo)
      );

      _.append("|-").append(node_name).append("-> ").
        append(the_match.get<0>()).append("\n");

    } else {
      m_pending_jobs->insert( node_name );
      _.append("|-").append(node_name).append("-> pending\n");
    }
  }

  Debug(_);
}

} // anonymous namespace

void
do_bulk_mm(
  classad::ClassAd const& jdl, 
  ScheduledJobs& jobs, 
  PendingJobs& pending,
  boost::shared_ptr<std::string> jw_template
)
{
  jdl::DAGAd::node_iterator node_b;
  jdl::DAGAd::node_iterator node_e;
  
  jdl::DAGAd dagad(jdl);
  boost::tie(node_b, node_e) = dagad.nodes();
  Clusters clusters;
  std::for_each(node_b, node_e, Clusterize(dagad, clusters));
  
  Debug( "#clusters:" << clusters.size());

  std::for_each(clusters.begin(), clusters.end(), Match);
  std::for_each(
    clusters.begin(),
    clusters.end(),
    Schedule(jobs, pending, dagad, jw_template)
  );
}

}}}}
