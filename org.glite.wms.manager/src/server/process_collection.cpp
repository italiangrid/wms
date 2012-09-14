#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/regex.hpp>

#include "glite/jdl/DAGAd.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/DAGAdManipulation.h"
#include "glite/jdl/JDLAttributes.h"

#include "glite/wms/broker/match.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/common/logger/logger_utils.h"

namespace jdl = glite::jdl;
namespace cu = glite::wmsutils::classads;

namespace classad
{
  class ClassAd;
}

typedef boost::shared_ptr<classad::ClassAd> ClassAdPtr;

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {

struct ScheduleInfo{
  ClassAdPtr  planned_jdl;
  ClassAdPtr  brokerinfo;
  ScheduleInfo(ClassAdPtr const& jdl, ClassAdPtr const& bi)
    : planned_jdl(jdl), brokerinfo(bi) {} 
};

typedef std::vector<
  ScheduleInfo
> ScheduledJobs;

typedef std::set<
  std::string
> PendingJobs;

typedef std::vector<std::string> JobNames;

struct Cluster
{
  classad::ClassAd jdl;
  JobNames names;
  broker::MatchTable matches;
  broker::DataInfo data_info;
  std::vector<broker::MatchTable::iterator> destinations;
};

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

typedef std::map<
  ClusterKey,
  Cluster
> Clusters;

class Clusterize
{
  glite::jdl::DAGAd const* m_dagad;
  Clusters* m_clusters;
public:
  Clusterize(glite::jdl::DAGAd const& dagad, Clusters& clusters)
  : m_dagad(&dagad), m_clusters(&clusters) {}
 
  void operator()(glite::jdl::DAGAd::node_value_type const&);
};

class Schedule
{
  ScheduledJobs* m_scheduled_jobs;
  PendingJobs* m_pending_jobs;
  glite::jdl::DAGAd const* m_dagad;
public:
  Schedule(ScheduledJobs&, PendingJobs&, glite::jdl::DAGAd const&);
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

classad::ClassAd
get_jdl(std::string const& node_name, jdl::DAGAd const& dagad)
{
  jdl::DAGAd::node_iterator it = dagad.find(node_name);
  return it != jdl::DAGAd::node_iterator() ? classad::ClassAd(*it->second.description_ad()) : classad::ClassAd();
}

void Clusterize::operator()(jdl::DAGAd::node_value_type const& v)
{
  std::string const node_name = v.first;
  jdl::DAGNodeInfo const& node = v.second;
  node.description_ad();
  classad::ClassAd const& jdl = *node.description_ad();
  std::string const id = jdl::get_edg_jobid(jdl);
  ClusterKey key(make_key(jdl, m_dagad->ad(), node_name));
  Cluster& cluster = (*m_clusters)[key];
  if (cluster.names.empty()) {
    cluster.jdl = jdl;
  }
  cluster.names.push_back(node_name);
}

void Match(Clusters::value_type& v)
{
  Cluster& cluster = v.second;
  classad::ClassAd& ad = cluster.jdl;

  broker::MatchTable& matches = cluster.matches;
  broker::DataInfo& data_info = cluster.data_info;

  bool const input_data_exists = ad.Lookup(jdl::JDL::INPUTDATA);
  bool const data_requirements_exists = ad.Lookup(jdl::JDL::DATA_REQUIREMENTS);
  bool full_list_ = false;
  bool const full_list(
    ad.EvaluateAttrBool("FullListMatchResult", full_list_) && !full_list_
  );
  if ((input_data_exists || data_requirements_exists) && !full_list) {
    broker::match(ad, matches, data_info);
  } else {
    broker::match(ad, matches);
  } 

  std::vector<broker::MatchTable::iterator>& destinations(
    cluster.destinations
  );
  JobNames const& node_names = cluster.names;
  
  destinations.reserve(node_names.size());
 
  bool stochastic = false;
  if (jdl::get_fuzzy_rank(ad, stochastic) && stochastic)
  {
    double factor = 1;
    try {
      factor = jdl::get_fuzzy_factor(ad);
    }
    catch(...)
    {
    }
    std::transform(
      node_names.begin(),
      node_names.end(),
      std::back_inserter(destinations),
      boost::bind<broker::MatchTable::iterator>(
        broker::FuzzySelector(factor), matches
      )
    );
  } 
  else {
    std::transform(
      node_names.begin(),
      node_names.end(),
      std::back_inserter(destinations),
      boost::bind<broker::MatchTable::iterator>(
        broker::MaxRankSelector(), matches
      )
    );
  }
}

ClassAdPtr
make_planned_jdl(
  classad::ClassAd const& jdl,
  broker::MatchInfo const& the_match
)
{
  ClassAdPtr result(new classad::ClassAd(jdl));

  static boost::regex expression("(.+/[^\\-]+-(.+))-([^/]+)(?:/.+|$)");
  boost::smatch pieces;
  
  std::string const ce_id(
    cu::evaluate_attribute(*the_match.ce_ad, "GlueCEUniqueID")
  );

  if (boost::regex_match(ce_id, pieces, expression)) {

    jdl::set_globus_resource_contact_string(
      *result,
      std::string(pieces[1].first, pieces[1].second)
    );

    jdl::set_lrms_type(
      *result,
      std::string(pieces[2].first, pieces[2].second)
    );

    jdl::set_queue_name(
      *result,
      std::string(pieces[3].first, pieces[3].second)
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

Schedule::Schedule(
  ScheduledJobs& scheduled_jobs, 
  PendingJobs& pending_jobs, 
  jdl::DAGAd const& dagad
) : 
  m_scheduled_jobs(&scheduled_jobs), 
  m_pending_jobs(&pending_jobs), 
  m_dagad(&dagad)
{
}

void Schedule::operator()(Clusters::value_type& v)
{
  Cluster& cluster = v.second;
  JobNames const& node_names = cluster.names;

  broker::DataInfo& data_info = cluster.data_info;
  broker::MatchTable& matches = cluster.matches;
  std::vector<broker::MatchTable::iterator>& destinations(
    cluster.destinations
  );

  for (size_t i = 0; i < node_names.size(); ++i) {
    std::string const node_name = node_names[i];
    if (destinations[i] != matches.end()) {

      broker::MatchInfo const& the_match = *destinations[i];

      classad::ClassAd const& jdl = get_jdl(node_name, *m_dagad);

      ClassAdPtr planned_jdl(make_planned_jdl(jdl, the_match));
      ClassAdPtr brokerinfo(
        create_brokerinfo(jdl, *the_match.ce_ad, data_info)
      );

      add_brokerinfo_to_isb(*planned_jdl);

//      ClassAdPtr adapted_jdl;
//        jobadapter::JobAdapter(planned_jdl.get()).resolve()

      m_scheduled_jobs->push_back(
        ScheduleInfo(planned_jdl, brokerinfo)
      );

      Debug(node_name << "-> " << the_match.ce_id << "\n");

    } else {
      m_pending_jobs->insert( node_name );
      Debug(node_name << "-> pending\n");
    }
  }
}


} // anonymous namespace

void
process_collection(
  classad::ClassAd const& jdl
)
{
  ScheduledJobs sheduled;
  PendingJobs pending;

  jdl::DAGAd::node_iterator node_b;
  jdl::DAGAd::node_iterator node_e;
  
  jdl::DAGAd dagad(jdl);
  boost::tie(node_b, node_e) = dagad.nodes();

  Clusters clusters;
  std::for_each(node_b, node_e, Clusterize(dagad, clusters));
  std::for_each(clusters.begin(), clusters.end(), Match);
  std::for_each(clusters.begin(), clusters.end(), 
    Schedule(sheduled, pending, dagad)
  );
}

}}}} // glite::wms::manager::server
