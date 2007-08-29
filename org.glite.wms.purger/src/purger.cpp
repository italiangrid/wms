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

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMPConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"

#include "glite/wmsutils/classads/classad_utils.h"

#include "glite/wms/common/logger/logger_utils.h"
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
namespace logging       = glite::lb;
namespace configuration = glite::wms::common::configuration;
namespace utilities     = glite::wms::common::utilities;

#define StatToString(s) boost::algorithm::to_upper_copy(std::string(edg_wll_StatToString(s.state)))

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
     log_ctx.get(),jobid,
     EDG_WLL_STAT_CLASSADS | EDG_WLL_STAT_CHILDREN,
     &job_status)
 ) {
     char  *etxt,*edsc;
     edg_wll_Error(log_ctx.get(),&etxt,&edsc);
     Info(jobid.toString() << ": " << "edg_wll_JobStat " << etxt);
     free(etxt); free(edsc);
     return false;
  }
  return true;
}

fs::path
jobid_to_absolute_path(jobid::JobId const& id)
{
  return fs::path(
    fs::path(get_staging_path(), fs::native) /
    fs::path(jobid::get_reduced_part(id), fs::native) /
    fs::path(jobid::to_filename (id), fs::native)
  );
}

fs::path 
strid_to_absolute_path(std::string const& id)
{
  const jobid::JobId _(id);
  return jobid_to_absolute_path(_);
}

bool is_threshold_overcome(edg_wll_JobStat const& job_status, time_t threshold)
{
  time_t now;
  time( &now);
  return (now -  job_status.lastUpdateTime.tv_sec > threshold);
}

bool is_status_removable(edg_wll_JobStat const& job_status)
{
  switch( job_status.state ) {
  
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
  m_logging_fn(edg_wll_LogClearUSER), m_threshold(3600),
  m_skip_status_checking(true), m_skip_threshold_checking(true),
  m_force_orphan_node_removal(false), m_force_dag_node_removal(false)
{
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
Purger::skip_threshold_checking(bool b)
{
  m_skip_threshold_checking = b;
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
    if( !(result = !(m_logging_fn( log_ctx.get() ) != 0)) ) {
      Error("LB event logging failed" << get_lb_message(log_ctx));
    }
  } 
  catch(fs::filesystem_error& fse) {
    Error(fse.what());
  }
  return result;
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
      id.toString() << ": "
      << "CannotCreateLBContext from host proxy, error code #" 
      << e.error_code()
    );
    return false;
  }
  edg_wll_JobStat job_status;
  edg_wll_InitStatus(&job_status);

  utilities::scope_guard free_job_status(
    boost::bind(edg_wll_FreeStatus, &job_status)
  );

  if (!query_job_status(job_status, id, log_ctx)) {
      return false;
  }

  // Reads the TYPE of the JOB...
  bool is_dag = 
    (job_status.jobtype == EDG_WLL_STAT_DAG ) || 
    (job_status.jobtype == EDG_WLL_STAT_COLLECTION);
  
  if (is_dag && 
      (m_skip_status_checking || is_status_removable(job_status)) &&
      (m_skip_threshold_checking || is_threshold_overcome(job_status, m_threshold))
  ) { // removing dag and children
    
    std::vector<std::string> children;
    if (job_status.children) {
      std::copy(
        &job_status.children[0],
        &job_status.children[job_status.children_num],
        std::back_inserter(children)
      );
    }
    std::vector<std::string>::const_iterator i = children.begin();
    std::vector<std::string>::const_iterator const e = children.end();
    size_t n = 0;
    for( ; i != e; ++i ) {
      if (!remove_path(strid_to_absolute_path(*i), log_ctx)) {
        ++n;
      }
    }
    Info(
      id.toString() << ": " 
      << children.size() - n << "/" << children.size() << " nodes removed"
    );
    bool _(
      remove_path(jobid_to_absolute_path(id), log_ctx)
    );
    if (_) {
      Info(
        id.toString()<< ": removed " << StatToString(job_status) << " dag "
      );
    }
    return _;
  }
  
  bool is_dag_node = (job_status.parent_job != 0);
  
  // if the job is a dag node we should skip its removal
  // unless it is an orphan node or so requested
  if ( is_dag_node &&
       m_force_dag_node_removal )
  {
    bool _(
      remove_path(jobid_to_absolute_path(id), log_ctx)
    );
    if (_) {
      Info(
        id.toString() << ": removed " << StatToString(job_status) << " node "
      );
    }
    return _;
  } 
  
  if ( is_dag_node &&
       m_force_orphan_node_removal ) try {

    jobid::JobId const p_id(job_status.parent_job);
    fs::path pp(jobid_to_absolute_path(p_id));
    if (!fs::exists(pp)) {
      bool _(
        remove_path(jobid_to_absolute_path(id), log_ctx)
      );
      if (_) {
        Info(
          id.toString() << ": removed " << StatToString(job_status) << " orphan node "
        );
      }
      return _;
    }
  }
  catch(...) {
    return false;
  }

  if ((m_skip_status_checking || is_status_removable(job_status)) &&
      (m_skip_threshold_checking || is_threshold_overcome(job_status, m_threshold))
  ) { // removing normal job
    bool _(
      remove_path(jobid_to_absolute_path(id), log_ctx)
    );
    if (_) {
      Info(
        id.toString() << ": removed " << StatToString(job_status) << " job "
      );
      return _;
    }
  }
  return false;
}

} // namespace purger
} // namespace wms
} // namesapce glite
