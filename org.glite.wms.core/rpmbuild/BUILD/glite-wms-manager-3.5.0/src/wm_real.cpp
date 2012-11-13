// File: WMReal.cpp
// Authors: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//          Marco Cecchi <Marco.Cecchi@cnaf.infn.it>
// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

// $Id: wm_real.cpp,v 1.1.2.4.2.7.2.3.2.2.2.7.4.3 2012/03/29 13:31:42 mcecchi Exp $

#include <syslog.h>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>

#include "glite/lb/producer.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"

#include "glite/wms/common/logger/logger_utils.h"

#include "glite/wms/common/utilities/boost_fs_add.h"
#include "glite/wms/common/utilities/scope_guard.h"

#include "glite/wms/purger/purger.h"
#include "glite/security/proxyrenewal/renewal.h"

#include "glite/jdl/PrivateAttributes.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAttributes.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"
#include "glite/jdl/DAGAd.h"

#include "glite/wms/helper/Request.h"

#include "glite/jobid/JobId.h"
#include "glite/wms/common/utilities/manipulation.h"
#include "glite/wmsutils/classads/classad_utils.h"

#include "glite/security/proxyrenewal/renewal.h"

#include "plan.h"
#include "wm_real.h"
#include "lb_utils.h"
#include "submission_utils.h"

namespace utilities = glite::wms::common::utilities;
namespace configuration = glite::wms::common::configuration;
namespace jobid = glite::jobid;
namespace ca = glite::wmsutils::classads;
namespace jdl = glite::jdl;
namespace fs = boost::filesystem;

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {

bool have_lbproxy()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.common()->lbproxy();
}

std::string get_ice_input()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.ice()->input();
}

std::string get_jc_input()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.jc()->input();
}

fs::path
get_classad_file(std::string const& job_id)
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );

  fs::path f_classad(
    utilities::normalize_path(config.jc()->submit_file_dir()),
    fs::native
  );

  fs::path reduced(
    utilities::get_reduced_part(job_id),
    fs::native
  );
  f_classad /= reduced;

  fs::path cname(
    "ClassAd." + utilities::to_filename(job_id),
    fs::native
  );
  f_classad /= cname;

  return f_classad;
}

std::string get_wm_input()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.wm()->input();
}

} // {anonymous}

struct WMReal::Impl {
  Impl(boost::shared_ptr<utilities::JobDir> to_jc_,
       boost::shared_ptr<utilities::JobDir> to_ice_)
    : ice_output(get_ice_input()), to_ice(to_ice_),
      jc_output(get_jc_input()), to_jc(to_jc_) { }
  std::string ice_output;
  boost::shared_ptr<utilities::JobDir> to_ice;
  std::string jc_output;
  boost::shared_ptr<utilities::JobDir> to_jc;
};

WMReal::WMReal(
  boost::shared_ptr<utilities::JobDir> to_jc,
  boost::shared_ptr<utilities::JobDir> to_ice)
    : m_impl(new Impl(to_jc, to_ice)) { }

namespace {

classad::ClassAd
submit_command_create(std::auto_ptr<classad::ClassAd> job_ad, bool is_replan)
{
  classad::ClassAd result;

  result.InsertAttr("Protocol", std::string("1.0.0"));
  if (is_replan) {
    result.InsertAttr("Command", std::string("Reschedule"));
  } else {
    result.InsertAttr("Command", std::string("Submit"));
  }
  result.InsertAttr("Source", 2);
  std::auto_ptr<classad::ClassAd> args(new classad::ClassAd);
  args->Insert("JobAd", job_ad.get());
  job_ad.release();
  result.Insert("Arguments", args.get());
  args.release();

  return result;
}

}

void
WMReal::submit(std::auto_ptr<classad::ClassAd> planned_ad, ContextPtr context, bool is_replan)
{
  std::string const ce_id = jdl::get_ce_id(*planned_ad);
  log_match(context, ce_id);
  
  std::string ce_info_hostname;
  {
    bool no_throw;
    ce_info_hostname = jdl::get_ceinfo_host_name(*planned_ad, no_throw);
    if (!ce_info_hostname.empty()) {
      log_usertag(context, jdl::JDLPrivate::CE_INFO_HOST, ce_info_hostname);
    }
  }
 
  boost::regex const cream_ce_id(".+/cream-.+");
  bool const is_cream_ce = boost::regex_match(ce_id, cream_ce_id);

  std::string const output(
    is_cream_ce ? m_impl->ice_output : m_impl->jc_output
  );

  log_enqueued_start(context, output);

  std::string const sequence_code_(get_lb_sequence_code(context));
  jdl::set_lb_sequence_code(*planned_ad, sequence_code_);

  classad::ClassAd const cmd(submit_command_create(planned_ad, is_replan));
  std::string const ad_str = ca::unparse_classad(cmd);

  bool success = false;
  std::string error;

  edg_wlc_JobId c_jobid;
  int res = edg_wll_GetLoggingJob(context.get(), &c_jobid);
  if (!res) {
    jobid::JobId jobid(c_jobid);
    edg_wlc_JobIdFree(c_jobid);
    fs::path const cancel_token(get_cancel_token(jobid));
    if (fs::exists(cancel_token)) {
      // right before passing on to the next component in the submission chain
      log_cancelled(context);
      ::unlink(cancel_token.native_file_string().c_str());
      purger::Purger thePurger(have_lbproxy());
      thePurger.force_dag_node_removal();
      thePurger(jobid.toString());
      
      return;
    }

    // Logging jobid and destination on syslog
    std::string syslog_msg(
      "jobid=" + boost::lexical_cast<std::string>(jobid.toString()) += ", "
    );
    syslog_msg += "destination=" + ce_id;
    syslog(LOG_NOTICE, "%s", syslog_msg.c_str());
  } // no race condition if this check is missed, the cancel command
    // will be passed on next time the wm cancel request is considered

  if (is_cream_ce) {
    try {
      m_impl->to_ice->deliver(ad_str, boost::lexical_cast<std::string>(std::rand()));
      success = true;
    } catch (utilities::JobDirError const& e) {
      error = e.what();
    }
  } else { // ! cream CE
    try {
      m_impl->to_jc->deliver(ad_str, boost::lexical_cast<std::string>(std::rand()));
      success = true;
    } catch (utilities::JobDirError const& e) {
      error = e.what();
    }
  }

  if (success) {
    log_enqueued_ok(context, output, ad_str);
  } else {
    log_enqueued_fail(context, output, ad_str, error);
    throw CannotDeliverFatal(error);
  }
}

void
WMReal::submit(
  classad::ClassAd const& jdl,
  ContextPtr context,
  boost::shared_ptr<std::string> jw_template,
  bool is_replan
) {
  // we make a copy because we change the sequence code
  classad::ClassAd request_ad(jdl);

  std::string const sequence_code(get_lb_sequence_code(context));

  // before doing the planning, some helpers may need a
  // more recent sequence code than what appears in
  // the classad originally received
  jdl::set_lb_sequence_code(request_ad, sequence_code);

  std::auto_ptr<classad::ClassAd> planned_ad(Plan(request_ad, jw_template));
  submit(planned_ad, context, is_replan);
}

void
WMReal::submit_collection(
  classad::ClassAd const& jdl,
  ContextPtr context,
  PendingJobs& pending,
  boost::shared_ptr<std::string> jw_template
)
{
  ScheduledJobs jobs;
  do_bulk_mm(jdl, jobs, pending, jw_template);

  std::string const sequence_code(get_lb_sequence_code(context));

  ScheduledJobs::const_iterator job_b = jobs.begin();
  ScheduledJobs::const_iterator const job_e = jobs.end();

  for( ; job_b != job_e ; ++job_b ) {

    ClassAdPtr planned_ad = boost::tuples::get<planned_tag>(*job_b);

    std::string const brokerinfo_file(
      jdl::get_input_sandbox_path(*planned_ad) + "/.BrokerInfo"
    );

    ClassAdPtr brokerinfo(boost::tuples::get<brokerinfo_tag>(*job_b));
    std::ofstream bifs(brokerinfo_file.c_str());
    if (brokerinfo) {
      bifs << *brokerinfo << '\n';
    }
    bifs.close();

    jobid::JobId const job_id(
      jdl::get_edg_jobid(*planned_ad)
    );

    change_logging_job(context, sequence_code, job_id);

    change_logging_src(context, EDG_WLL_SOURCE_NETWORK_SERVER);
    if (
      !log_enqueued_ok(
        context, 
        get_wm_input(), 
        ca::unparse_classad(*planned_ad)
      )
    ) {
      Debug("log 'enqueued OK' failed for collection node");
    }
    change_logging_src(context, EDG_WLL_SOURCE_WORKLOAD_MANAGER);

    if (shallow_resubmission_is_enabled(*planned_ad)) {
      fs::path const token_file(get_reallyrunning_token(job_id, 0));
      create_token(token_file);
    }
    std::auto_ptr<classad::ClassAd> adapted_ad(
      static_cast<classad::ClassAd*>(boost::tuples::get<adapted_tag>(*job_b)->Copy())
    );
    submit(adapted_ad, context, 0 /* not a replan */);
  }
 
  if (pending.empty()) {
    return;
  }

  jdl::DAGAd dagad(jdl);
  jdl::DAGAdNodeIterator first_node, last_node;
  boost::tie(first_node, last_node) = dagad.nodes();

  PendingJobs::iterator p_first = pending.begin();
  PendingJobs::iterator const p_last = pending.end(); 
 
  change_logging_src(context, EDG_WLL_SOURCE_NETWORK_SERVER);
  
  for ( ; p_first != p_last; ++p_first) {  

    std::string node_name = *p_first;
    jdl::DAGAdNodeIterator it = dagad.find(node_name);
    assert(it != last_node);
    jdl::DAGNodeInfo const& node_info = it->second;
    classad::ClassAd node_jdl(
      *node_info.description_ad()
    );
    std::string const id_str = jdl::get_edg_jobid(node_jdl);
    jobid::JobId const id(id_str);

    change_logging_job(context, "", id);
    if (
      !log_enqueued_ok(
        context,
        get_wm_input(),
        ca::unparse_classad(node_jdl)
    )) {
      Debug("log 'enqueued OK' failed for collection node");
    }
  }

  change_logging_src(context, EDG_WLL_SOURCE_WORKLOAD_MANAGER);
}

namespace {

classad::ClassAd
cancel_command_create(
  std::string const& job_id,
  std::string const& sequence_code,
  std::string const& user_x509_proxy
)
{
  classad::ClassAd result;

  result.InsertAttr("Protocol", std::string("1.0.0"));
  result.InsertAttr("Command", std::string("Cancel"));
  result.InsertAttr("Source", 2);

  std::auto_ptr<classad::ClassAd> args(new classad::ClassAd);
  args->InsertAttr("Force", false);

  std::string const ad_file = get_classad_file(job_id).native_file_string();
  std::ifstream ifs(ad_file.c_str());
  if (ifs) {
    classad::ClassAdParser parser;
    classad::ClassAd ad;
    if (parser.ParseClassAd(ifs, ad)) {
      bool good = false;
      std::string const log_file(jdl::get_log(ad, good));
      if (!log_file.empty() && good) {
        args->InsertAttr("LogFile", log_file);
      }
    }
  }

  args->InsertAttr("ProxyFile", user_x509_proxy);
  args->InsertAttr("SequenceCode", sequence_code);
  args->InsertAttr("JobId", job_id);

  result.Insert("Arguments", args.get());
  args.release();

  return result;
}

}

void
WMReal::cancel(jobid::JobId const& id, ContextPtr context)
{
  std::string const sequence_code(get_lb_sequence_code(context));

  classad::ClassAd cmd(
    cancel_command_create(
      id.toString(),
      sequence_code,
      get_user_x509_proxy(id)
    )
  );

  std::string const ad_str(ca::unparse_classad(cmd));

  try {
    m_impl->to_ice->deliver(ad_str);
  } catch (utilities::JobDirError const& e) {
  }

  try {
    m_impl->to_jc->deliver(ad_str);
  } catch (utilities::JobDirError const& e) {
  }
}

}}}}
