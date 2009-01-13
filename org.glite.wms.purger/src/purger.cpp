// File: purger.cpp
// Author: Salvatore Monforte <salvatore.monforte@ct.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html
//
// $Id$

#include <boost/filesystem/operations.hpp> 
#include <boost/filesystem/exception.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include "ssl_utils.h"

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMPConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"

#include "glite/wmsutils/classads/classad_utils.h"

#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/utilities/scope_guard.h"

#include "glite/lb/Job.h"
#include "lb_utils.h"
#include "purger.h"

#include "glite/lb/context.h"
#include "glite/lb/producer.h"
#include "glite/lb/LoggingExceptions.h"
#include <string>
#include <time.h>

namespace fs            = boost::filesystem;
namespace jobid         = glite::wmsutils::jobid;
namespace logger	= glite::wms::common::logger::threadsafe;
namespace logging       = glite::lb;
namespace configuration = glite::wms::common::configuration;
namespace utilities     = glite::wms::common::utilities;
namespace utils		= glite::wmsutils::classads;

namespace {

inline std::string StatToString(edg_wll_JobStat const& status)
{
  std::string result;
  char* const s = edg_wll_StatToString(status.state);
  if (s) {
    result = s;
    free(s);
    result = boost::algorithm::to_upper_copy(result);
  }

  return result;
}

}

namespace glite {
namespace wms {
namespace purger {

namespace 
{

std::string const f_sequence_code(
  "UI=000009:NS=0000096669:WM=000000:BH=0000000000:JSS=000000:LM=000000:LRMS=000000:APP=000000:LBS=000000" 
);

const configuration::Configuration* f_conf = 0;

std::string 
get_host_x509_proxy()
{
  if (!f_conf) {
    f_conf = configuration::Configuration::instance();
    assert(f_conf);
  }
  static std::string const host_proxy_file(
    f_conf->common()->host_proxy_file()
  );
  return host_proxy_file;
}

std::string get_staging_path()
{
  if (!f_conf) {
    f_conf = configuration::Configuration::instance();
    assert(f_conf);
  }
  static std::string const sandbox_staging_path(
    f_conf->wp()->sandbox_staging_path()
  );
  return sandbox_staging_path;
}

bool
query_job_status(
  edg_wll_JobStat& job_status, 
  jobid::JobId const& jobid, 
  ContextPtr const& log_ctx 
) 
{
  if (edg_wll_JobStatus(
        log_ctx.get(), jobid,
        EDG_WLL_STAT_CLASSADS | EDG_WLL_STAT_CHILDREN,
        &job_status
      )
     ) {
    char* etxt = 0;
    char* edsc = 0;
    edg_wll_Error(log_ctx.get(), &etxt, &edsc);
    Error(
       jobid.toString() << ": edg_wll_JobStat " << std::string(etxt)
    );
     free(etxt);
    free(edsc);

    return false;
  }

  return true;
} 

fs::path
jobid_to_absolute_path(jobid::JobId const& id)
{
  std::string const unique(id.getUnique());
  return fs::path(
    fs::path(get_staging_path(), fs::native) /
    fs::path(unique.substr(0,2), fs::native) /
    fs::path(jobid::to_filename(id), fs::native)
  );
}

fs::path 
strid_to_absolute_path(std::string const& id)
{
  jobid::JobId const jobid(id);
  return jobid_to_absolute_path(jobid);
}

bool is_threshold_overcome(edg_wll_JobStat const& job_status, time_t threshold)
{
  return std::time(0) - job_status.lastUpdateTime.tv_sec > threshold;
}

bool is_status_removable(edg_wll_JobStat const& job_status)
{
  switch (job_status.state) {
  
  case EDG_WLL_JOB_CANCELLED:
  case EDG_WLL_JOB_CLEARED:
  case EDG_WLL_JOB_ABORTED:
  case EDG_WLL_JOB_DONE: 
    return true; 
  default:
    return false;
  }
}

} // anonymous namespace

Purger::Purger() :
  m_logging_fn(
#ifdef GLITE_WMS_HAVE_LBPROXY
    edg_wll_LogClearUSERProxy
#else
    edg_wll_LogClearUSER
#endif
  ), m_threshold(0),
  m_skip_status_checking(true),
  m_force_orphan_node_removal(false), m_force_dag_node_removal(false)
{
  //if (sslutils::proxy_expires_within(get_host_x509_proxy(), 21600)) // 6 hours
  //{

  //  std::string const cert(std::getenv("GLITE_HOST_CERT") ? 
  //    std::getenv("GLITE_HOST_CERT") : "/home/glite/.certs/hostcert.pem"
  //  );
  //  std::string const key(std::getenv("GLITE_HOST_KEY") ?
  //    std::getenv("GLITE_HOST_KEY") : "/home/glite/.certs/hostkey.pem"
  //  );
  //  if (!sslutils::proxy_init(cert, key, get_host_x509_proxy(), 86400)) // 24 hours
  //  {
  //    Error(
  //      "Unable to renew expired host proxy '" << get_host_x509_proxy() << "': check certificates "
  //      "and what GLITE_HOST_CERT / GLITE_HOST_KEY environment variables refer to."
  //    );
  //  }
  //}
}

Purger&
Purger::log_using(boost::function<int(edg_wll_Context)> fn)
{
  m_logging_fn = fn;
  return *this;
}

Purger&
Purger::threshold(time_t t)
{
  m_threshold = t;
  return *this;
}

Purger&
Purger::skip_status_checking(bool b)
{
  m_skip_status_checking = b;
  return *this;
}

Purger&
Purger::force_orphan_node_removal(bool b)
{
  m_force_orphan_node_removal = b;
  return *this;
}

Purger&
Purger::force_dag_node_removal(bool b)
{
  m_force_dag_node_removal = b;
  return *this;
}
bool 
Purger::remove_path(
 fs::path const& p,
 ContextPtr log_ctx
) 
{
  bool result = false;
  try {
    fs::remove_all(p);
    assert( !exists(p) );
    if (!(result = !(m_logging_fn( log_ctx.get() ) != 0))) {
      Error(
        "LB event logging failed " << get_lb_message(log_ctx)
      );
    }
  } catch(fs::filesystem_error& fse) {
    Error(fse.what());
  }
  return result;
}

bool
Purger::operator()()
{
  ContextPtr log_ctx;

  try {
    log_ctx = create_context(get_host_x509_proxy());
  }
  catch (CannotCreateLBContext& e) {
    Error(
      "CannotCreateLBContext from host proxy, error code #"
      << e.error_code()
    );
    return false;
  }
  edg_wll_SetParam(log_ctx.get(), EDG_WLL_PARAM_QUERY_RESULTS, EDG_WLL_QUERYRES_LIMITED);
  edg_wll_SetParam(log_ctx.get(), EDG_WLL_PARAM_QUERY_JOBS_LIMIT, 1000);
  for(;;) {
  size_t n = 0;
  struct removable_jobs removable_jobs_info(
    get_removable_jobs(log_ctx, m_threshold)
  );
  LB_Jobs::const_iterator job = removable_jobs_info.jobs.begin();
  LB_Jobs::const_iterator const job_end = removable_jobs_info.jobs.end();
  if (!std::distance(job_end, job)) break;
  for( size_t i=0; job != job_end; ++job, ++i ) {

    if (is_threshold_overcome(
      removable_jobs_info.states[i], m_threshold
    )) {

      boost::shared_ptr<char> s(
        edg_wlc_JobIdUnparse(*job), free
      );
      jobid::JobId const id(s.get());
      ContextPtr ctx;
      try {
        ctx = create_context_proxy(id, get_host_x509_proxy(), f_sequence_code);
      }
      catch (CannotCreateLBContext& e) {
        Error(
          s.get()
          << ": CannotCreateLBProxyContext from host proxy, error code #"
          << e.error_code()
        );
        break;
      }
      bool path_removed = remove_path(
        jobid_to_absolute_path(id),
        ctx
      );
      if (path_removed) {
        n++;
        Info(
          id.toString()<< ": removed " << StatToString(removable_jobs_info.states[i])
        );
      }  
    }
  }
  Debug("Purged " << n << "/" << removable_jobs_info.jobs.size() << " removable jobs");
  if(!n) break;
  }
  return true;
}

bool 
Purger::operator()(jobid::JobId const& id)
{
  ContextPtr log_ctx;

  try {
    log_ctx = create_context(id, get_host_x509_proxy(), f_sequence_code);
  }
  catch (CannotCreateLBContext& e) {
    Error(
      id.toString()
      << ": CannotCreateLBContext from host proxy, error code #"
      << e.error_code()
    );
    return false;
  }

#ifdef GLITE_WMS_HAVE_LBPROXY
  ContextPtr log_proxy_ctx;
  try {
    log_proxy_ctx = create_context_proxy(id, get_host_x509_proxy(), f_sequence_code);
  }
  catch (CannotCreateLBContext& e) {
    Error(
      id.toString()
      << ": CannotCreateLBProxyContext from host proxy, error code #"
      << e.error_code()
    );
    log_proxy_ctx = log_ctx;
  }
#endif

  edg_wll_JobStat job_status;
  edg_wll_InitStatus(&job_status);

  utilities::scope_guard free_job_status(
    boost::bind(edg_wll_FreeStatus, &job_status)
  );

  if (!query_job_status(job_status, id, log_ctx)) {
    Info(
      id.toString() << ": forced removal, unknown L&B job"
    );
    return remove_path(
      jobid_to_absolute_path(id),
#ifdef GLITE_WMS_HAVE_LBPROXY
      log_proxy_ctx
#else
      log_ctx
#endif
    );
  }
 // Reads the TYPE of the JOB...
  bool is_dag = 
    (job_status.jobtype == EDG_WLL_STAT_DAG ) || 
    (job_status.jobtype == EDG_WLL_STAT_COLLECTION);
  
  if (is_dag && 
      (m_skip_status_checking || is_status_removable(job_status)) &&
      ( !m_threshold || is_threshold_overcome(job_status, m_threshold))
  ) { // removing dag and children
    
    std::vector<std::string> children;
    if (job_status.children) {

      char** i = &job_status.children[0];
      while( *i ) {
        children.push_back(std::string(*i));
        i++;
      }
    }
    std::vector<std::string>::const_iterator i = children.begin();
    std::vector<std::string>::const_iterator const e = children.end();
    size_t n = 0;
    for( ; i != e; ++i ) {
      change_logging_job(
#ifdef GLITE_WMS_HAVE_LBPROXY
        log_proxy_ctx,
#else
        log_ctx,
#endif
        "",
        jobid::JobId(*i)
      );
      if (!remove_path(
        strid_to_absolute_path(*i), 
#ifdef GLITE_WMS_HAVE_LBPROXY
	log_proxy_ctx
#else
        log_ctx
#endif
      )) {
        ++n;
      }
    }
    Info(
      id.toString() << ": " 
      << children.size() - n << '/' << job_status.children_num << " nodes removed"
    );

    change_logging_job(
#ifdef GLITE_WMS_HAVE_LBPROXY
      log_proxy_ctx,
#else
      log_ctx,
#endif
      f_sequence_code, id
    );

    bool const path_removed(
      remove_path(
        jobid_to_absolute_path(id), 
#ifdef GLITE_WMS_HAVE_LBPROXY
        log_proxy_ctx
#else
        log_ctx
#endif
      )
    );
    if (path_removed) {
      Info(
        id.toString()<< ": removed " << StatToString(job_status) << " dag "
      );
    }
    return path_removed;
  }
  
  bool const is_dag_node = job_status.parent_job != 0;
  
  // if the job is a dag node we should skip its removal
  // unless it is an orphan node or so requested
  if (is_dag_node && m_force_dag_node_removal ) {
    bool const path_removed(
      remove_path(
        jobid_to_absolute_path(id), 
#ifdef GLITE_WMS_HAVE_LBPROXY
        log_proxy_ctx
#else
        log_ctx
#endif
      )
    );
    if (path_removed) {
      Info(
	id.toString() << ": removed "
        << StatToString(job_status) << " node"
      );
    }
    return path_removed;
  }
  
  if (is_dag_node && m_force_orphan_node_removal) try {

    jobid::JobId const p_id(job_status.parent_job);
    fs::path pp(jobid_to_absolute_path(p_id));
    if (!fs::exists(pp)) {
      bool const path_removed(
        remove_path(
          jobid_to_absolute_path(id), 
#ifdef GLITE_WMS_HAVE_LBPROXY
        log_proxy_ctx
#else
        log_ctx
#endif
        )
      );
      if (path_removed) {
        Info(
          id.toString() << ": removed "
          << StatToString(job_status) << " orphan node"
        );
      }
      return path_removed;
    }
  } catch(...) {                // TODO: refine catch
    return false;
  }

  if ((m_skip_status_checking || is_status_removable(job_status))
      && (!m_threshold
          || is_threshold_overcome(job_status, m_threshold)
         )
     ) { // removing normal job
    bool const path_removed(
      remove_path(
        jobid_to_absolute_path(id), 
#ifdef GLITE_WMS_HAVE_LBPROXY
        log_proxy_ctx
#else
        log_ctx
#endif
      )
    );
    if (path_removed) {
      Info(
        id.toString() << ": removed " << StatToString(job_status) << " job"
      );
      return path_removed;
    }
  }
  return false;
}
} // namespace purger
} // namespace wms
} // namesnapce glite
