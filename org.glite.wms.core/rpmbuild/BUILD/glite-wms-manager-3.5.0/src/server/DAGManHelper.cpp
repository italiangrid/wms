// File: DAGManHelper.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2003 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include "DAGManHelper.h"
#include "match_utils.h"
#include "dagman_utils.h"
#include <memory>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include "glite/wms/common/utilities/boost_fs_add.h"
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
#include <classad_distribution.h>
#include "glite/wms/helper/HelperFactory.h"
#include "glite/wms/helper/Helper.h"
#include "glite/wms/helper/exceptions.h"
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"
#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/PrivateAttributes.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/DAGAd.h"
#include "glite/jdl/DAGAdManipulation.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/common/utilities/scope_guard.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/logger/logger_utils.h"

namespace fs = boost::filesystem;
namespace utilities = glite::wms::common::utilities;
namespace configuration = glite::wms::common::configuration;
namespace jobid = glite::wmsutils::jobid;
namespace ca = glite::wmsutils::classads;
namespace jdl = glite::jdl;
namespace helper = glite::wms::helper;

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {

std::string helper_id("DAGManHelper");

helper::HelperImpl* create_helper()
{
  return new DAGManHelper;
}

struct Register
{
  Register()
  {
    helper::HelperFactory::instance()->register_helper(helper_id, create_helper);
  }
  ~Register()
  {
    helper::HelperFactory::instance()->unregister_helper(helper_id);
  }
};

Register r;

std::string f_output_file_suffix(".dmh");

classad::ClassAd const*
get_description(classad::ExprTree const* node)
try {
  classad::ClassAd const* result = 0;

  if (ca::is_classad(node)) {
    result = ca::evaluate_attribute(
      *dynamic_cast<classad::ClassAd const*>(node),
      "description"
    );
  }

  return result;

} catch (ca::InvalidValue&) {
  return 0;
}

fs::path
jc_submit_file_dir()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  std::string const path_ = config.jc()->submit_file_dir();

  return fs::path(path_, fs::native);
}

fs::path
jc_output_file_dir()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  std::string const path_ = config.jc()->output_file_dir();

  return fs::path(path_, fs::native);
}

fs::path
lm_condor_log_dir()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  std::string const path_ = config.lm()->condor_log_dir();

  return fs::path(path_, fs::native);
}

fs::path
sandbox_dir()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  std::string path_ = config.ns()->sandbox_staging_path();

  return fs::path(path_, fs::native);
}

std::string
get_condor_dagman()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.jc()->condor_dagman();
}

int
get_dagman_log_level()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.jc()->dagman_log_level();
}

int
get_dagman_max_pre()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.jc()->dagman_max_pre();
}

int
get_dagman_log_rotate()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.jc()->dagman_log_rotate();
}

std::string const dag_description_file("dag_description.con");

typedef std::map<std::string, std::string> Id2MatchFile;

std::string get_match_file(std::string const& unique)
{
  return "cluster." + unique;
}

class Paths
{
  jobid::JobId m_dag_id;
  fs::path m_base_submit_dir;
  fs::path m_base_output_dir;
  fs::path m_dag_log;
  fs::path m_sandbox_dir;

public:
  Paths(jobid::JobId const& dag_id)
    : m_dag_id(dag_id),
      m_base_submit_dir(jc_submit_file_dir()),
      m_base_output_dir(jc_output_file_dir()),
      m_dag_log(lm_condor_log_dir()),
      m_sandbox_dir(sandbox_dir())
  {
    m_base_submit_dir /= fs::path(jobid::get_reduced_part(dag_id), fs::native);
    m_base_submit_dir /= fs::path(
      "dag." + jobid::to_filename(dag_id),
      fs::native
    );
    m_base_output_dir /= fs::path(jobid::get_reduced_part(dag_id), fs::native);
    m_base_output_dir /= jobid::to_filename(dag_id);
    m_dag_log /= fs::path(
      "dag." + jobid::to_filename(dag_id) + ".log",
      fs::native
    );
  }

  jobid::JobId dag_id() const
  {
    return m_dag_id;
  }
  fs::path base_submit_dir() const
  {
    return m_base_submit_dir;
  }
  fs::path base_output_dir() const
  {
    return m_base_output_dir;
  }
  fs::path dag_description() const
  {
    fs::path result(m_base_submit_dir);
    result /= fs::path(dag_description_file, fs::native);

    return result;
  }
  fs::path lock_file() const
  {
    fs::path const result(dag_description_file + ".lock", fs::native);

    return result;
  }
  fs::path lib_log() const
  {
    fs::path result(m_base_output_dir);
    result /= fs::path("dag.lib.out", fs::native);

    return result;
  }
  fs::path debug_log() const
  {
    fs::path result(m_base_output_dir);
    result /= fs::path("dag.dagman.out", fs::native);

    return result;
  }
  fs::path dag_log() const
  {
    return m_dag_log;
  }
  fs::path rescue() const
  {
    fs::path result(m_base_submit_dir);
    result /= fs::path(dag_description_file + ".rescue", fs::native);

    return result;
  }
  fs::path ad(jobid::JobId const& node_id) const
  {
    fs::path result(m_base_submit_dir);
    result /= fs::path("ad." + jobid::to_filename(node_id), fs::native);

    return result;
  }
  fs::path submit(jobid::JobId const& node_id) const
  {
    fs::path result(m_base_submit_dir);
    result /= fs::path(
      "Condor." + jobid::to_filename(node_id) + ".submit",
      fs::native
    );

    return result;
  }
  fs::path log(jobid::JobId const& node_id) const
  {
    fs::path result(m_base_submit_dir);
    result /= fs::path("log." + jobid::to_filename(node_id), fs::native);

    return result;
  }
  fs::path match_file(jobid::JobId const& node_id, Id2MatchFile const& match_files) const
  {
    fs::path result(m_base_submit_dir);
    Id2MatchFile::const_iterator it = match_files.find(node_id.getUnique());
    assert(it != match_files.end());
    result /= fs::path(
      get_match_file(it->second),
      fs::native
    );
    return result;
  }
  fs::path pre(jobid::JobId const& node_id) const
  {
    char const* getenv_GLITE_WMS_LOCATION = std::getenv("GLITE_WMS_LOCATION");
    assert(getenv_GLITE_WMS_LOCATION);
    std::string const glite_wms_location(getenv_GLITE_WMS_LOCATION);

    fs::path result(glite_wms_location, fs::native);
    result /= fs::path("libexec/glite-wms-planner", fs::native);

    return result;
  }
  fs::path post(jobid::JobId const& node_id) const
  {
    char const* getenv_GLITE_WMS_LOCATION = std::getenv("GLITE_WMS_LOCATION");
    assert(getenv_GLITE_WMS_LOCATION);
    std::string const glite_wms_location(getenv_GLITE_WMS_LOCATION);

    fs::path result(glite_wms_location, fs::native);
    result /= fs::path("libexec/glite-wms-dag-post.sh", fs::native);

    return result;
  }
  fs::path standard_output(jobid::JobId const& node_id) const
  {
    fs::path result(m_base_output_dir);
    result /= fs::path(jobid::to_filename(node_id), fs::native);
    result /= fs::path("StandardOutput", fs::native);

    return result;
  }
  fs::path maradona(jobid::JobId const& node_id) const
  {
    fs::path result(m_sandbox_dir);
    result /= fs::path(jobid::get_reduced_part(node_id), fs::native);
    result /= fs::path(jobid::to_filename(node_id), fs::native);
    result /= fs::path("Maradona.output", fs::native);

    return result;
  }

};

int get_deep_retry_count(classad::ClassAd const& jdl, int default_count)
{
  bool is_in_jdl = false;
  int result = jdl::get_retry_count(jdl, is_in_jdl);
  if (!is_in_jdl || result < 0) {
    result = default_count;
  }
  return result;
}

int get_sys_max_deep_retry_count()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  int const result = config.wm()->max_retry_count();

  return result >= 0 ? result : 0;
}

int get_shallow_retry_count(classad::ClassAd const& jdl, int default_count)
{
  bool is_in_jdl = false;
  int result = jdl::get_shallow_retry_count(jdl, is_in_jdl);
  if (!is_in_jdl || result < 0 && result != -1) {
    result = default_count;
  }
  return result;
}

int get_sys_max_shallow_retry_count()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  int const result = config.wm()->max_shallow_retry_count();

  return result >= 0 ? result : 0;
}

void
fix_retry_count(
  jdl::DAGNodeInfo& node_info,
  classad::ClassAd const& jdl,
  int default_retry_count,
  int default_shallow_retry_count
)
{
  int const job_deep_retry_count(
    get_deep_retry_count(jdl, default_retry_count)
  );
  int const sys_deep_retry_count(get_sys_max_deep_retry_count());
  int const job_shallow_retry_count(
    get_shallow_retry_count(jdl, default_shallow_retry_count)
  );
  int const sys_shallow_retry_count(get_sys_max_shallow_retry_count());

  int const deep_retry_count(
    std::min(job_deep_retry_count, sys_deep_retry_count)
  );
  int const shallow_retry_count(
    std::min(job_shallow_retry_count, sys_shallow_retry_count)
  );

  int retry_count = deep_retry_count + 1;
  if (shallow_retry_count != -1) {
    retry_count *= shallow_retry_count + 1;
  }

  // upper limit on the number of retries; the planner will check the actual
  // limits
  node_info.retry_count(retry_count);
}

class DescriptionAdToSubmitFile
{
  jdl::DAGAd* m_dagad;
  Paths const* m_paths;
  int m_default_retry_count;
  int m_default_shallow_retry_count;

public:
  DescriptionAdToSubmitFile(jdl::DAGAd* dagad, Paths const* paths)
    : m_dagad(dagad),
      m_paths(paths),
      m_default_retry_count(0),
      m_default_shallow_retry_count(0)
  {
    classad::ClassAd const& ad(m_dagad->ad());
    try {
      m_default_retry_count
        = ca::evaluate_attribute(ad, "DefaultNodeRetryCount");
    } catch (ca::InvalidValue&) {
    }
    try {
      m_default_shallow_retry_count
        = ca::evaluate_attribute(ad, "DefaultNodeShallowRetryCount");
    } catch (ca::InvalidValue&) {
    }
  }
  void operator()(jdl::DAGAd::node_value_type const& node) const
  {
    std::string const& name      = node.first;
    jdl::DAGNodeInfo const& node_info = node.second;

    classad::ClassAd const* ad = node_info.description_ad();
    assert(ad);

    jobid::JobId node_id = jdl::get_edg_jobid(*ad);

    fs::path submit_file = m_paths->submit(node_id);
    fs::ofstream os(submit_file);
    assert(os);
    os << "log = " << m_paths->dag_log().native_file_string() << '\n';

    jdl::DAGNodeInfo new_node_info = node_info;
    new_node_info.description_file_for_ad(submit_file.native_file_string());

    fix_retry_count(
      new_node_info,
      *ad,
      m_default_retry_count,
      m_default_shallow_retry_count
    );

    // TODO should change node type too
    m_dagad->replace_node(name, new_node_info);
  }
};

// <node>.<file>
boost::regex node_file_regex("([a-zA-Z0-9_]+)\\.(.*)");

std::pair<std::string,std::string>
get_node_file(classad::ExprTree const* et)
{
  std::string s(ca::unparse(et));
  boost::smatch m;
  std::string node;
  std::string file;
  if (boost::regex_match(s, m, node_file_regex)) {
    node.assign(m[1].first, m[1].second);
    file.assign(m[2].first, m[2].second);
  }

  return std::make_pair(node, file);
}

boost::shared_ptr<classad::ExprTree>
make_requirements(std::string const& ce_id)
{
  boost::shared_ptr<classad::ExprTree> result;

  if (!ce_id.empty()) {
    classad::ExprTree* expr = 0;
    std::string expr_string("other.GlueCEUniqueID == ");
    expr_string += ce_id;
    classad::ClassAdParser parser;
    if (parser.ParseExpression(expr_string, expr)) {
      result.reset(expr);
    }
  }

  return result;
}

// consider the ISB; replace references to files in other node OSBs with actual
// paths
class NodeAdTransformation
{
  jdl::DAGAd const* m_dagad;
  boost::shared_ptr<classad::ExprTree> m_ce_id_requirements;
  int m_default_retry_count;
  int m_default_shallow_retry_count;

public:
  NodeAdTransformation(jdl::DAGAd const& dagad, std::string const& ce_id)
    : m_dagad(&dagad),
      m_ce_id_requirements(make_requirements(ce_id)),
      m_default_retry_count(0),
      m_default_shallow_retry_count(0)
  {
    classad::ClassAd const& ad(m_dagad->ad());
    try {
      m_default_retry_count
        = ca::evaluate_attribute(ad, "DefaultNodeRetryCount");
    } catch (ca::InvalidValue&) {
    }
    try {
      m_default_shallow_retry_count
        = ca::evaluate_attribute(ad, "DefaultNodeShallowRetryCount");
    } catch (ca::InvalidValue&) {
    }
  }
  classad::ClassAd* operator()(classad::ClassAd const& ad) const
  {
    std::auto_ptr<classad::ClassAd> result(
      static_cast<classad::ClassAd*>(ad.Copy())
    );

    jdl::set_x509_user_proxy(*result, jdl::get_x509_user_proxy(*m_dagad));
    jdl::set_certificate_subject(
      *result,
      jdl::get_certificate_subject(*m_dagad)
    );
    jdl::set_edg_dagid(*result, jdl::get_edg_jobid(*m_dagad));
    bool is_in_jdl = false;
    jdl::get_retry_count(*result, is_in_jdl);
    if (!is_in_jdl) {
      jdl::set_retry_count(*result, m_default_retry_count);
    }
    is_in_jdl = false;
    jdl::get_shallow_retry_count(*result, is_in_jdl);
    if (!is_in_jdl) {
      jdl::set_shallow_retry_count(*result, m_default_shallow_retry_count);
    }
    if (m_ce_id_requirements) {
      result->Insert("Requirements", m_ce_id_requirements->Copy());
    }

    // manage isb
    boost::scoped_ptr<classad::ExprTree const> et(
      result->Remove(jdl::JDL::INPUTSB)
    );
    if (!et) {
      return result.release();
    }

    // can it be a single value or is it always a list?
    // assume the latter for the moment
    classad::ExprList const* el(
      dynamic_cast<classad::ExprList const*>(et.get())
    );

    std::auto_ptr<classad::ExprList> new_isb(new classad::ExprList);

    classad::ExprList::const_iterator it = el->begin();
    classad::ExprList::const_iterator const end = el->end();
    for ( ; it != end; ++it) {

      if (ca::is_attribute_reference(*it)) {
        std::string node;
        std::string file;
        boost::tie(node, file) = get_node_file(*it);
        if (!node.empty() && !file.empty()) {
          jdl::DAGAd::node_iterator node_it = m_dagad->find(node);
          if (node_it != m_dagad->nodes().second) {
            classad::Value v;
            jdl::DAGNodeInfo const& node_info = node_it->second;
            classad::ClassAd const* ad = node_info.description_ad();
            assert(ad);

            bool exists = false;
            std::string osb_path(jdl::get_output_sandbox_path(*ad, exists));
            if (exists && !osb_path.empty()) {
              v.SetStringValue(osb_path + "/" + file);
              new_isb->push_back(classad::Literal::MakeLiteral(v));
            }
          }
        }
      } else {
        new_isb->push_back((*it)->Copy());
      }

    }

    result->Insert(jdl::JDL::INPUTSB, new_isb.release());

    return result.release();
  }

};

class DumpAdToFile
{
  typedef boost::function<classad::ClassAd*(classad::ClassAd)> transform_function;

  transform_function m_transform;
  Paths const* m_paths;

public:
  DumpAdToFile(transform_function transform, Paths const* paths)
    : m_transform(transform), m_paths(paths)
  {
  }
  void operator()(jdl::DAGAd::node_value_type const& node) const
  {
    jdl::DAGNodeInfo const& node_info = node.second;

    classad::ClassAd const* ad = node_info.description_ad();
    assert(ad);

    fs::path ad_file = m_paths->ad(jdl::get_edg_jobid(*ad));
    fs::ofstream os(ad_file);
    assert(os);

    boost::scoped_ptr<classad::ClassAd> transformed_ad(m_transform(*ad));
    assert(transformed_ad);
    os << ca::unparse_classad(*transformed_ad) << '\n';
  }
};

int get_match_retry_period()
{
  return configuration::Configuration::instance()->wm()->match_retry_period();
}

int get_max_planners()
{
  return static_cast<const int>(
    configuration::Configuration::instance()->wm()->worker_threads() * 1.6 + 0.5
  );
}

class GeneratePrePost
{
  jdl::DAGAd* m_dagad;
  Id2MatchFile const* m_match_files;
  Paths const* m_paths;

public:
  GeneratePrePost(
    jdl::DAGAd& dagad,
    Id2MatchFile const& match_files,
    Paths const& paths
  )
    : m_dagad(&dagad), m_match_files(&match_files), m_paths(&paths)
  {
  }
  void operator()(jdl::DAGAd::node_value_type const& node) const
  {
    std::string const& name      = node.first;
    jdl::DAGNodeInfo const& node_info = node.second;

    jdl::DAGNodeInfo new_node_info = node_info;
    classad::ClassAd const* ad = node_info.description_ad();
    assert(ad);
    jobid::JobId node_id = jdl::get_edg_jobid(*ad);

    std::string const pre_file = m_paths->pre(node_id).native_file_string();
    std::string const output_file(
      m_paths->submit(node_id).native_file_string()
    );
    std::string const input_file(
      m_paths->ad(node_id).native_file_string()
    );
    std::string const log_file(
      m_paths->log(node_id).native_file_string()
    );
    std::ostringstream pre_args;

    pre_args << "-max " << get_max_planners()
             << " -rwait " << get_match_retry_period()
             << " -rcode " << boost::lexical_cast<std::string>(EXIT_RETRY_NODE)
             << " -- ";

    pre_args << "--output " << output_file
             << " --input " << input_file
             << " --log " << log_file;

    if (!m_match_files->empty()) {
      std::string const match_file(
        m_paths->match_file(node_id, *m_match_files).native_file_string()
      );
      if (!match_file.empty()) {
        pre_args << " --matches " << match_file;
      }
    }

    new_node_info.pre(pre_file, pre_args.str());

    std::string post_file = m_paths->post(node_id).native_file_string();
    std::string post_args = m_paths->standard_output(node_id).native_file_string() + ' '
      + m_paths->maradona(node_id).native_file_string();
    new_node_info.post(post_file, post_args);

    m_dagad->replace_node(name, new_node_info);
  }
};

struct InsertJobInSubmitFile
{
  std::ostream* operator()(std::ostream* osp, const jdl::DAGAd::node_value_type& node) const
  {
    std::ostream& os = *osp;

    std::string const& node_name = node.first;
    jdl::DAGNodeInfo const& node_info = node.second;

    os << "JOB " << node_name << ' ' << node_info.description_file() << '\n';

    std::string pre;
    std::string pre_args;
    boost::tie(pre, pre_args) = node_info.pre();
    if (!pre.empty()) {
      os << "SCRIPT PRE " << node_name << ' ' << pre;
      if (!pre_args.empty()) {
        os << ' ' << pre_args;
      }
      os << '\n';
    }

    std::string post;
    std::string post_args;
    boost::tie(post, post_args) = node_info.post();
    if (!post.empty()) {
      os << "SCRIPT POST " << node_name << ' ' << post;
      if (!post_args.empty()) {
        os << ' ' << post_args;
      }
      os << '\n';
    }

    int retry_count = node_info.retry_count();
    if (retry_count > 0) {
      os << "RETRY " << node_name << ' '
         << node_info.retry_count()
         << " UNLESS-EXIT " << EXIT_ABORT_NODE
         << '\n';
    }

    return osp;
  }
};

struct InsertDependencyInSubmitFile
{
  std::ostream* operator()(
    std::ostream* osp,
    const jdl::DAGAd::dependency_value_type& dependency
  ) const
  {
    std::ostream& os = *osp;

    os << "PARENT " << dependency.first->first
       << " CHILD " << dependency.second->first
       << '\n';

    return osp;
  }
};

std::ostream&
to_dag_description(std::ostream& os, const jdl::DAGAd& ad)
{
  jdl::DAGAd::node_iterator node_b;
  jdl::DAGAd::node_iterator node_e;
  boost::tie(node_b, node_e) = ad.nodes();
  std::accumulate(node_b, node_e, &os, InsertJobInSubmitFile());

  std::accumulate(ad.dependencies().first, ad.dependencies().second, &os,
                  InsertDependencyInSubmitFile());

  return os;
}

void
create_dagman_job_ad(
  classad::ClassAd& result,
  Paths const& paths,
  int max_running_nodes
)
{
  jdl::set_type(result, "dag");
  jdl::set_universe(result, "scheduler");
  std::string condor_dagman(get_condor_dagman());
  jdl::set_executable(result, condor_dagman);
  jdl::set_getenv(result, "true");

  jdl::set_output(result, paths.lib_log().native_file_string());
  jdl::set_error_(result, paths.lib_log().native_file_string());
  jdl::set_remove_kill_sig(result, "SIGUSR1");

  int dagman_log_level = get_dagman_log_level();
  if (dagman_log_level < 0) {
    dagman_log_level = 0;
  } else if (dagman_log_level > 5) {
    dagman_log_level = 5;
  }

  int dagman_max_pre = get_dagman_max_pre();
  if (dagman_max_pre < 1) {
    dagman_max_pre = 1;
  }
  std::ostringstream arguments;
  arguments << "-f"
            << " -l " << paths.base_submit_dir().native_file_string()
            << " -NoEventChecks"
            << " -Debug " << dagman_log_level
            << " -Lockfile " << paths.lock_file().native_file_string()
            << " -Dag " << paths.dag_description().native_file_string()
            << " -Rescue " << paths.rescue().native_file_string()
            << " -maxpre " << dagman_max_pre
            << " -Condorlog " << paths.dag_log().native_file_string();
  jdl::set_arguments(result, arguments.str());
  jdl::set_ce_id(result, "dagman");

  int dagman_log_rotate = get_dagman_log_rotate();
  if (dagman_log_rotate < 0) {
    dagman_log_rotate = 0;
  }
  std::ostringstream environment;
  environment << "_CONDOR_DAGMAN_LOG="
              << paths.debug_log().native_file_string()
              << ";_CONDOR_MAX_DAGMAN_LOG=" << dagman_log_rotate;
  result.InsertAttr("environment", environment.str());
}

// return ce id GlueCEUniqueID
std::string
nodes_collocation_match(jdl::DAGAd const& dag)
{
  std::string result;

  classad::ExprTree const* reqs(dag.get_generic(jdl::JDL::REQUIREMENTS));
  classad::ExprTree const* rank(dag.get_generic(jdl::JDL::RANK));
  classad::ExprTree const* vo(dag.get_generic(jdl::JDL::VIRTUAL_ORGANISATION));
  classad::ExprTree const* x509(dag.get_generic(jdl::JDLPrivate::USERPROXY));

  if (!(reqs && rank && vo && x509)) {
    return result;
  }

  classad::ClassAd jdl;
  jdl.Insert(jdl::JDL::REQUIREMENTS, reqs->Copy());
  jdl.Insert(jdl::JDL::RANK, rank->Copy());
  jdl.Insert(jdl::JDL::VIRTUAL_ORGANISATION, vo->Copy());
  jdl.Insert(jdl::JDLPrivate::USERPROXY, x509->Copy());
  std::auto_ptr<classad::ClassAd> match_result(
    helper::Helper("MatcherHelper").resolve(&jdl)
  );

  matches_type matches;
  try {
    if (fill_matches(*match_result, matches)) {
      bool use_fuzzy_rank = false;
      jdl::get_fuzzy_rank(jdl, use_fuzzy_rank);
      matches_type::const_iterator it(
        select_best_ce(matches, use_fuzzy_rank)
      );
      result = it->get<0>();
    }
  } catch (MatchError&) {
  }

  return result;
}

int get_max_dag_running_nodes()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.jc()->max_dagrunning_nodes();
}

// do bulk mm iff:
//   there are no dependencies
//   bulk mm is enabled at compile time
//   bulk mm is enabled at run time
bool do_bulk_mm(jdl::DAGAd const& dagad)
{
#ifdef GLITE_WMS_HAVE_BULK_MM
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  if (config.wm()->enable_bulk_mm()) {
    jdl::DAGAd::dependency_iterator b, e;
    boost::tie(b, e) = dagad.dependencies();
    return b == e;
  } else {
    return false;
  }
#else
  return false;
#endif
}

typedef std::pair<
  std::string,                  // name
  std::string                   // value
> SignificantAttribute;
typedef std::vector<SignificantAttribute> SignificantAttributes;

struct ClusterKey
{
  SignificantAttributes m_significant_attributes;
};

bool operator<(ClusterKey const& lhs, ClusterKey const& rhs)
{
  return lhs.m_significant_attributes < rhs.m_significant_attributes;
}

ClusterKey make_key(classad::ClassAd const& jdl)
{
  ClusterKey result;
  std::vector<std::string> attributes;
  bool no_throw;
  jdl::get_significant_attributes(jdl, attributes, no_throw);

  classad::ClassAdUnParser unparser;

  std::vector<std::string>::const_iterator first = attributes.begin();
  std::vector<std::string>::const_iterator const last = attributes.end();
  for ( ; first != last; ++first) {
    std::string const& attribute = *first;
    classad::ClassAd const* ignore = 0;
    classad::ExprTree const* expr = jdl.LookupInScope(attribute, ignore);
    std::string value;
    unparser.Unparse(value, expr);
    result.m_significant_attributes.push_back(
      std::make_pair(attribute, value)
    );
  }

  return result;
}

enum {
  jdl_tag,
  ids_tag,
  match_tag
};

typedef boost::tuple<
  classad::ClassAd,             // template jdl
  std::vector<std::string>,     // jobid's in this cluster
  ClassAdPtr                    // match result
> Cluster;

typedef std::map<
  ClusterKey,
  Cluster
> Clusters;

class clusterize
{
  Clusters* m_clusters;
public:
  clusterize(Clusters& clusters)
    : m_clusters(&clusters)
  {
  }
  void operator()(jdl::DAGAd::node_value_type const& v)
  {
    jdl::DAGNodeInfo const& node = v.second;
    classad::ClassAd const& jdl = *node.description_ad();
    std::string const id = jdl::get_edg_jobid(jdl);
    ClusterKey key(make_key(jdl));
    Cluster cluster = (*m_clusters)[key];
    if (cluster.get<ids_tag>().empty()) {
      cluster.get<jdl_tag>() = jdl;
    }
    cluster.get<ids_tag>().push_back(id);
  }
};

void do_clustering(jdl::DAGAd const& dagad, Clusters& clusters)
{
  if (do_bulk_mm(dagad)) {
    jdl::DAGAd::node_iterator node_b;
    jdl::DAGAd::node_iterator node_e;
    boost::tie(node_b, node_e) = dagad.nodes();

    std::for_each(node_b, node_e, clusterize(clusters));
  }
}

void match(Clusters::value_type& v)
{
  Cluster& cluster = v.second;
  classad::ClassAd jdl = cluster.get<jdl_tag>();
  bool const include_brokerinfo = true;
  int const number_of_results = cluster.get<ids_tag>().size(); // needed/useful?
  jdl.InsertAttr("include_brokerinfo", include_brokerinfo);
  jdl.InsertAttr("number_of_results", number_of_results);
  ClassAdPtr match_result(helper::Helper("MatcherHelper").resolve(&jdl));

  cluster.get<match_tag>() = match_result;
}

class dump_match_file
{
  Id2MatchFile* m_match_files;
  fs::path const* m_submit_dir;
public:
  dump_match_file(Id2MatchFile& match_files, fs::path const& submit_dir)
    : m_match_files(&match_files), m_submit_dir(&submit_dir)
  {
  }
  void operator()(Clusters::value_type const& v)
  {
    Cluster const& cluster = v.second;
    ClassAdPtr match_result = cluster.get<match_tag>();
    std::vector<std::string> const& ids = cluster.get<ids_tag>();
    jobid::JobId const template_id(ids.front());
    std::string const template_unique = template_id.getUnique();

    fs::path const cluster_file(get_match_file(template_unique), fs::native);
    fs::ofstream os(*m_submit_dir / cluster_file);
    os << *match_result << '\n';

    std::vector<std::string>::const_iterator it = ids.begin();
    std::vector<std::string>::const_iterator const end = ids.end();
    for ( ; it != end; ++it) {
      jobid::JobId const id(*it);
      bool ok;
      boost::tie(boost::tuples::ignore, ok) = m_match_files->insert(
          std::make_pair(id.getUnique(), template_unique)
      );
      assert(ok && "DAGManHelper_bmm: id already existing in match_files");
    }
  }
};

classad::ClassAd* f_resolve(classad::ClassAd const& input_ad)
try {

  std::auto_ptr<classad::ClassAd> result(new classad::ClassAd);

  jdl::DAGAd dagad(input_ad);

  std::string const dag_id_str = jdl::get_edg_jobid(dagad);
  jobid::JobId const dag_id(dag_id_str);

  jdl::set_edg_jobid(*result, dag_id_str);

  jdl::set_x509_user_proxy(*result, jdl::get_x509_user_proxy(dagad));

  Paths paths(dag_id);

  fs::create_parents(paths.base_submit_dir());
  utilities::scope_guard base_submit_dir_undo(
    boost::bind(fs::remove_all, paths.base_submit_dir())
  );

  // do collocation of nodes, if so requested
  std::string ce_id;
  if (jdl::get_nodes_collocation(dagad)) {
    ce_id = nodes_collocation_match(dagad);
    if (ce_id.empty()) {
      throw DAGManHelperError();
    }
  }

  jdl::DAGAd::node_iterator node_b;
  jdl::DAGAd::node_iterator node_e;

  // in the following assume for the moment that all node infos contain a
  // description ad rather than a description file

  // dump description ads to file, applying a transformation first
  boost::tie(node_b, node_e) = dagad.nodes();
  NodeAdTransformation node_ad_transformation(dagad, ce_id);
  std::for_each(node_b, node_e, DumpAdToFile(node_ad_transformation, &paths));
  
  Clusters clusters;
  do_clustering(dagad, clusters);
  std::for_each(clusters.begin(), clusters.end(), match);
  Id2MatchFile match_files;
  std::for_each(
    clusters.begin(),
    clusters.end(),
    dump_match_file(match_files, paths.base_submit_dir())
  );

  // generate pre and post scripts
  boost::tie(node_b, node_e) = dagad.nodes();
  std::for_each(node_b, node_e, GeneratePrePost(dagad, match_files, paths));

  // replace description ads with (submit) files
  // generate a preliminary submit file containing just the log attribute to
  // make dagman happy. The submit file will be overwritten (better: completed)
  // by the Big Helper
  boost::tie(node_b, node_e) = dagad.nodes(); // node_iterator is an input iterator
  std::for_each(node_b, node_e, DescriptionAdToSubmitFile(&dagad, &paths));

  // TODO: what to do if an operation fails, in particular the generation of a
  // file? undo the previous calls?

  // generate dag description
  fs::ofstream dag_description(paths.dag_description());
  assert(dag_description);
  to_dag_description(dag_description, dagad) << '\n';

  // create the job ad, to be then converted by the JC to a submit file
  int max_running_nodes = dagad.max_running_nodes();
  if (max_running_nodes == 0) {
    max_running_nodes = 1;
  }
  int const sys_max_running_nodes = get_max_dag_running_nodes();
  if (0 < sys_max_running_nodes && sys_max_running_nodes < max_running_nodes) {
    max_running_nodes = sys_max_running_nodes;
  }
  create_dagman_job_ad(*result, paths, max_running_nodes);

  // touch the lock file to force dagman to start in recovery mode. This is
  // needed because currently dagman (in non-recovery mode) removes the log
  // file at start up if it exists. But this causes the dagman submission
  // event to be lost because we use the same log file both for the dagman job
  // and for its subjobs

  fs::ofstream lf(paths.base_submit_dir() / paths.lock_file());
  if (!lf) {
    throw DAGManHelperError();
  }

  base_submit_dir_undo.dismiss();

  return result.release();

} catch (jdl::DAG_NodeError const& e) {
  throw DAGManHelperError();
} catch (jdl::DAG_Error const& e) {
  throw DAGManHelperError();
} catch (fs::filesystem_error const& ex) {
  throw helper::FileSystemError("DAGManHelper", ex);
} catch (ca::ClassAdError& ex) {
  Error(ex.what());
  return 0;
}

} // {anonymous}

std::string
DAGManHelper::id() const
{
  return helper_id;
}

std::string
DAGManHelper::output_file_suffix() const
{
  return f_output_file_suffix;
}

classad::ClassAd*
DAGManHelper::resolve(classad::ClassAd const* input_ad) const
{
  // not yet
  // assert(input_ad != 0);

  return input_ad ? f_resolve(*input_ad) : 0;

  // not yet
  //  assert(result != 0);
}

}}}} // glite::wms::manager::server
