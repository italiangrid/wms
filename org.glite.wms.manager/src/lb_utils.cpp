// File: lb_utils.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Author: Marco Cecchi <Marco.Cecchi@cnaf.infn.it>
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

// $Id: lb_utils.cpp,v 1.1.2.7.2.17.2.3.2.1.2.10.2.3 2012/02/07 16:41:01 mcecchi Exp $

#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <map>

#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <openssl/pem.h>
#include <openssl/x509.h>

#include "glite/jobid/JobId.h"
#include "glite/wms/common/utilities/manipulation.h"
#include "glite/lb/context.h"
#include "glite/lb/producer.h"
#include "glite/wms/common/utilities/scope_guard.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/security/proxyrenewal/renewal.h"
#include "glite/jdl/PrivateAttributes.h"

#include "submit_request.h"
#include "lb_utils.h"
#include "submission_utils.h"

namespace jobid = glite::jobid;
namespace utilities = glite::wms::common::utilities;
namespace configuration = glite::wms::common::configuration;
namespace ba = boost::algorithm;

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {

unsigned int const one_minute = 60;
bool have_lbproxy = true;

bool is_retryable(int error)
{
  return error == ETIMEDOUT
    || error == ENOTCONN
    || error == ECONNREFUSED
    //|| error == 1416 // LB server: store protocol error, resource temporarily unavailable
    || error == EAGAIN;
}

std::string
get_proxy_subject(std::string const& x509_proxy)
{
  static std::string const null_string;

  std::FILE* fd = std::fopen(x509_proxy.c_str(), "r");
  if (!fd) {
    return null_string;
  }
  boost::shared_ptr<std::FILE> fd_(fd, std::fclose);

  ::X509* const cert = ::PEM_read_X509(fd, 0, 0, 0);
  if (!cert) {
    return null_string;
  }
  boost::shared_ptr< ::X509> cert_(cert, ::X509_free);

  char* const s = ::X509_NAME_oneline(::X509_get_subject_name(cert), 0, 0);
  if (!s) {
    return null_string;
  }
  boost::shared_ptr<char> s_(s, ::free);

  return std::string(s);
}

std::string get_lb_proxy_user(ContextPtr context)
{
  char *s = 0;
  utilities::scope_guard free_char(
    boost::bind(std::free, s)
  );
  edg_wll_GetParam(&(*context), EDG_WLL_PARAM_LBPROXY_USER, &s);
  return std::string(s);
}

}

void init_lb() {
  have_lbproxy = configuration::Configuration::instance()->common()->lbproxy();
}

std::vector<std::string>
get_scheduled_jobs(ContextPtr context, int grace_period)
{
  edg_wll_QueryRec qr[3];

//  qr[0].attr = EDG_WLL_QUERY_ATTR_OWNER;
//  qr[0].op = EDG_WLL_QUERY_OP_EQUAL;
//  qr[0].value.c = 0; // all users jobs

  qr[0].attr = EDG_WLL_QUERY_ATTR_TIME;
  qr[0].op = EDG_WLL_QUERY_OP_LESS;
  qr[0].value.t.tv_sec = std::time(0) - grace_period;
  qr[0].attr_id.state = EDG_WLL_JOB_SCHEDULED;

  qr[1].attr = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_Context ctx = context.get();

  boost::function<
    int(
      edg_wll_Context,
      edg_wll_QueryRec const*,
      int,
      glite_jobid_t **,
      edg_wll_JobStat **
    )
  > query_function;
  if (have_lbproxy) {
    query_function = edg_wll_QueryJobsProxy;
  } else {
    edg_wll_LogFlush(ctx, 0 /* no timeout */);
    query_function = edg_wll_QueryJobs;
  }

  int flag = 0;
  std::vector<std::string> result;
  edg_wll_JobStat *job_statuses;
  int error = query_function(ctx, qr, flag, 0 /* jobids */, &job_statuses);
  if (error && error != ENOENT) {
    Debug("error (" << error << ") while querying for scheduled jobs");
  } else {
    if (error != ENOENT) {
      for (int i = 0; job_statuses[i].state != EDG_WLL_JOB_UNDEF; ++i) {
        if (EDG_WLL_JOB_SCHEDULED == job_statuses[i].state) { // could it be undefined, whatever?
          result.push_back(glite_jobid_unparse(job_statuses[i].jobId));
          Debug(result.back() << " is in state " << edg_wll_StatToString(job_statuses[i].state));
        }
        edg_wll_FreeStatus(&job_statuses[i]);
      }
      free(job_statuses);
    }
  }
  return result;
}

ContextPtr
create_context(
  jobid::JobId const& id,
  std::string const& x509_proxy,
  std::string const& sequence_code,
  edg_wll_Source source
)
{
  edg_wll_Context context;
  int errcode = edg_wll_InitContext(&context);
  if (errcode) {
    throw CannotCreateLBContext(errcode);
  }

  ContextPtr result(context, edg_wll_FreeContext);

  errcode |= edg_wll_SetParam(
    context,
    EDG_WLL_PARAM_SOURCE,
    source
  );
  errcode |= edg_wll_SetParam(
    context,
    EDG_WLL_PARAM_INSTANCE,
    boost::lexical_cast<std::string>(::getpid()).c_str()
  );

  errcode |= edg_wll_SetParam(
    context,
    EDG_WLL_PARAM_X509_PROXY,
    x509_proxy.c_str()
  );

  int const flag = EDG_WLL_SEQ_NORMAL;

  if (have_lbproxy) {
    std::string const user_dn = get_proxy_subject(x509_proxy);
    errcode |= edg_wll_SetLoggingJobProxy(
      context,
      id.c_jobid(),
      sequence_code.empty() ? 0 : sequence_code.c_str(),
      user_dn.c_str(),
      flag
    );
  } else {
    errcode |= edg_wll_SetLoggingJob(
      context,
      id.c_jobid(),
      sequence_code.c_str(),
      flag
    );
  }

  if (errcode) {
    throw CannotCreateLBContext(errcode);
  }

  return result;
}

void change_logging_job(
  ContextPtr context, 
  std::string const& sequence_code,
  jobid::JobId const& id
)
{
  int const flag = EDG_WLL_SEQ_NORMAL;

  if (have_lbproxy) {
    std::string const user_dn = get_lb_proxy_user(context);
    edg_wll_SetLoggingJobProxy(
      &(*context),
      id.c_jobid(),
      sequence_code.empty() ? 0 : sequence_code.c_str(),
      user_dn.c_str(),
      flag
    );
  } else {
    edg_wll_SetLoggingJob(
      &(*context),
      id.c_jobid(),
      sequence_code.c_str(),
      flag
    );
  }
}

LB_Events
do_query_events(
  ContextPtr context,
  edg_wll_QueryRec const** job_conditions, 
  edg_wll_QueryRec const** event_conditions,
  bool enoent_is_error
)
{
  edg_wll_Event* events = 0;
  edg_wll_Context ctx = context.get();

  boost::function<
    int(
      edg_wll_Context,
      edg_wll_QueryRec const**,
      edg_wll_QueryRec const**,
      edg_wll_Event**
    )
  > query_function;
  if (have_lbproxy) {
    query_function = edg_wll_QueryEventsExtProxy;
  } else {
    edg_wll_LogFlush(ctx, 0 /* no timeout */);
    query_function = edg_wll_QueryEventsExt;
  }

  int error = query_function(ctx, job_conditions, event_conditions, &events);
  if (!error || !enoent_is_error && error == ENOENT) {
    return LB_Events(events);
  }

  if (is_retryable(error)) {
    throw LB_QueryTemporarilyFailed();
  } else {
    throw LB_QueryFailed();
  }
}

LB_Events
get_cancel_events(ContextPtr context, jobid::JobId const& id)
{
  edg_wll_QueryRec jobid[2];
  jobid[0].attr    = EDG_WLL_QUERY_ATTR_JOBID;
  jobid[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  jobid[0].value.j = glite_jobid_t(id.c_jobid());
  jobid[1].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec const* job_conditions[2];
  job_conditions[0] = jobid;
  job_conditions[1] = 0;

  edg_wll_QueryRec enqueued_or_cancelled[3];
  enqueued_or_cancelled[0].attr    = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  enqueued_or_cancelled[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  enqueued_or_cancelled[0].value.i = EDG_WLL_EVENT_ENQUEUED;
  enqueued_or_cancelled[1].attr    = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  enqueued_or_cancelled[1].op      = EDG_WLL_QUERY_OP_EQUAL;
  enqueued_or_cancelled[1].value.i = EDG_WLL_EVENT_CANCEL;
  enqueued_or_cancelled[2].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec from_wm[2];
  from_wm[0].attr    = EDG_WLL_QUERY_ATTR_SOURCE;
  from_wm[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  from_wm[0].value.i = EDG_WLL_SOURCE_WORKLOAD_MANAGER;
  from_wm[1].attr    = EDG_WLL_QUERY_ATTR_UNDEF;
  
  edg_wll_QueryRec const* event_conditions[3];
  
  event_conditions[0] = enqueued_or_cancelled;
  event_conditions[1] = from_wm;
  event_conditions[2] = 0;

  return do_query_events(context, job_conditions, event_conditions, false);
}

LB_Events
get_recovery_events(ContextPtr context, jobid::JobId const& id)
{
  edg_wll_QueryRec jobid[2];
  jobid[0].attr    = EDG_WLL_QUERY_ATTR_JOBID;
  jobid[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  jobid[0].value.j = glite_jobid_t(id.c_jobid());
  jobid[1].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec const* job_conditions[2];
  job_conditions[0] = jobid;
  job_conditions[1] = 0;

  edg_wll_QueryRec enqueued_or_done[3];
  enqueued_or_done[0].attr    = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  enqueued_or_done[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  enqueued_or_done[0].value.i = EDG_WLL_EVENT_ENQUEUED;
  enqueued_or_done[1].attr    = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  enqueued_or_done[1].op      = EDG_WLL_QUERY_OP_EQUAL;
  enqueued_or_done[1].value.i = EDG_WLL_EVENT_DONE;
  enqueued_or_done[2].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec from_wm_or_lm[3];
  from_wm_or_lm[0].attr    = EDG_WLL_QUERY_ATTR_SOURCE;
  from_wm_or_lm[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  from_wm_or_lm[0].value.i = EDG_WLL_SOURCE_WORKLOAD_MANAGER;
  from_wm_or_lm[1].attr    = EDG_WLL_QUERY_ATTR_SOURCE;
  from_wm_or_lm[1].op      = EDG_WLL_QUERY_OP_EQUAL;
  from_wm_or_lm[1].value.i = EDG_WLL_SOURCE_LOG_MONITOR;  
  from_wm_or_lm[2].attr    = EDG_WLL_QUERY_ATTR_UNDEF;
  
  edg_wll_QueryRec const* event_conditions[3];
  
  event_conditions[0] = enqueued_or_done;
  event_conditions[1] = from_wm_or_lm;
  event_conditions[2] = 0;

  return do_query_events(context, job_conditions, event_conditions, false);
}

LB_Events
get_interesting_events(ContextPtr context, jobid::JobId const& id)
{
  edg_wll_QueryRec jobid[2];
  jobid[0].attr    = EDG_WLL_QUERY_ATTR_JOBID;
  jobid[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  jobid[0].value.j = glite_jobid_t(id.c_jobid());
  jobid[1].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

   edg_wll_QueryRec usertag[2];
   usertag[0].attr    = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
   usertag[0].op      = EDG_WLL_QUERY_OP_EQUAL;
   usertag[0].value.i = EDG_WLL_EVENT_USERTAG;
   usertag[1].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec const* job_conditions[2];
  job_conditions[0] = jobid;
  job_conditions[1] = 0;

  edg_wll_QueryRec match_or_resubmit_or_usertag[4];
  match_or_resubmit_or_usertag[0].attr    = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  match_or_resubmit_or_usertag[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  match_or_resubmit_or_usertag[0].value.i = EDG_WLL_EVENT_MATCH;
  match_or_resubmit_or_usertag[1].attr    = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  match_or_resubmit_or_usertag[1].op      = EDG_WLL_QUERY_OP_EQUAL;
  match_or_resubmit_or_usertag[1].value.i = EDG_WLL_EVENT_RESUBMISSION;
  match_or_resubmit_or_usertag[2].attr    = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  match_or_resubmit_or_usertag[2].op      = EDG_WLL_QUERY_OP_EQUAL;
  match_or_resubmit_or_usertag[2].value.i = EDG_WLL_EVENT_USERTAG;
  match_or_resubmit_or_usertag[3].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec from_wm_or_bh[3];
  from_wm_or_bh[0].attr    = EDG_WLL_QUERY_ATTR_SOURCE;
  from_wm_or_bh[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  from_wm_or_bh[0].value.i = EDG_WLL_SOURCE_WORKLOAD_MANAGER;
  from_wm_or_bh[1].attr    = EDG_WLL_QUERY_ATTR_SOURCE;
  from_wm_or_bh[1].op      = EDG_WLL_QUERY_OP_EQUAL;
  from_wm_or_bh[1].value.i = EDG_WLL_SOURCE_BIG_HELPER;
  from_wm_or_bh[2].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec const* event_conditions[3];
  event_conditions[0] = match_or_resubmit_or_usertag;
  event_conditions[1] = from_wm_or_bh;
  event_conditions[2] = 0;

  return do_query_events(context, job_conditions, event_conditions, true);
}

LB_Events
get_interesting_events_dags(ContextPtr context, jobid::JobId const& id)
{
  edg_wll_QueryRec jobid[2];
  jobid[0].attr    = EDG_WLL_QUERY_ATTR_JOBID;
  jobid[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  jobid[0].value.j = glite_jobid_t(id.c_jobid());
  jobid[1].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec const* job_conditions[2];
  job_conditions[0] = jobid;
  job_conditions[1] = 0;

  edg_wll_QueryRec match_or_resubmit[3];
  match_or_resubmit[0].attr    = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  match_or_resubmit[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  match_or_resubmit[0].value.i = EDG_WLL_EVENT_MATCH;
  match_or_resubmit[1].attr    = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  match_or_resubmit[1].op      = EDG_WLL_QUERY_OP_EQUAL;
  match_or_resubmit[1].value.i = EDG_WLL_EVENT_RESUBMISSION;
  match_or_resubmit[2].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec from_wm_or_bh[3];
  from_wm_or_bh[0].attr    = EDG_WLL_QUERY_ATTR_SOURCE;
  from_wm_or_bh[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  from_wm_or_bh[0].value.i = EDG_WLL_SOURCE_WORKLOAD_MANAGER;
  from_wm_or_bh[1].attr    = EDG_WLL_QUERY_ATTR_SOURCE;
  from_wm_or_bh[1].op      = EDG_WLL_QUERY_OP_EQUAL;
  from_wm_or_bh[1].value.i = EDG_WLL_SOURCE_BIG_HELPER;
  from_wm_or_bh[2].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec const* event_conditions[3];
  event_conditions[0] = match_or_resubmit;
  event_conditions[1] = from_wm_or_bh;
  event_conditions[2] = 0;

  boost::function<
    int(
      edg_wll_Context,
      edg_wll_QueryRec const**,
      edg_wll_QueryRec const**,
      edg_wll_Event**
    )
  > query_function;

  edg_wll_Context ctx = context.get();
  if (have_lbproxy) {
    query_function = edg_wll_QueryEventsExtProxy;
  } else {
    edg_wll_LogFlush(ctx, 0 /* no timeout */);
    query_function = edg_wll_QueryEventsExt;
  }

  for (int i = 0; i < 20; ++i) {

    edg_wll_Event* events = 0;

    int error = query_function(ctx, job_conditions, event_conditions, &events);
    LB_Events result(events);
    if (error == 0 || error == ENOENT) {
      return result;
    }

    unsigned int const five_seconds = 5;
    ::sleep(five_seconds);
  }

  throw LB_QueryFailed();
}
 
std::vector<std::pair<std::string, int> >
get_previous_matches(LB_Events const& events)
{
  std::vector<std::pair<std::string, int> > result;

  LB_Events::const_iterator it(events.begin());
  LB_Events::const_iterator const last(events.end());
  for ( ; it != last; ++it) {
    edg_wll_Event const& event = *it;
    if (event.type == EDG_WLL_EVENT_USERTAG && 
        ba::iequals( 
          std::string(event.userTag.name), glite::jdl::JDLPrivate::CE_INFO_HOST
        )) {
      std::string const host_id(event.userTag.value);
      int timestamp(event.userTag.timestamp.tv_sec);
      result.push_back(std::make_pair(host_id, timestamp));
    }
  }

  return result;
}

namespace {

bool is_deep_resubmission(edg_wll_Event const& event)
{
  return event.type == EDG_WLL_EVENT_RESUBMISSION
    && event.resubmission.result == EDG_WLL_RESUBMISSION_WILLRESUB;
}

bool is_shallow_resubmission(edg_wll_Event const& event)
{
  return event.type == EDG_WLL_EVENT_RESUBMISSION
    && event.resubmission.result == EDG_WLL_RESUBMISSION_SHALLOW;
}

LB_Events::const_iterator
find_last_deep_resubmission(LB_Events const& events)
{
  LB_Events::const_reverse_iterator it(
    std::find_if(events.rbegin(), events.rend(), is_deep_resubmission)
  );
  if (it != events.rend()) {
    return (++it).base();
  } else {
    return events.end();
  }
}

}

boost::tuple<int, int>
get_retry_counts(LB_Events const& events)
{
  int const deep_count(
    std::count_if(events.begin(), events.end(), is_deep_resubmission)
  );

  LB_Events::const_iterator last_deep_resubmission(
    find_last_deep_resubmission(events)
  );

  if (last_deep_resubmission == events.end()) {
    last_deep_resubmission = events.begin();
  }

  int const shallow_count(
    std::count_if(
      last_deep_resubmission,
      events.end(),
      is_shallow_resubmission
    )
  );
  assert(shallow_count >= 0);

  return boost::make_tuple(deep_count, shallow_count);
}

std::string
get_original_jdl(
  ContextPtr context,
  jobid::JobId const& id
)
{
  edg_wll_QueryRec job_conditions[2];
  job_conditions[0].attr    = EDG_WLL_QUERY_ATTR_JOBID;
  job_conditions[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  job_conditions[0].value.j = glite_jobid_t(id.c_jobid());
  job_conditions[1].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec event_conditions[3];
  event_conditions[0].attr    = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  event_conditions[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  event_conditions[0].value.i = EDG_WLL_EVENT_ENQUEUED;
  event_conditions[1].attr    = EDG_WLL_QUERY_ATTR_SOURCE;
  event_conditions[1].op      = EDG_WLL_QUERY_OP_EQUAL;
  event_conditions[1].value.i = EDG_WLL_SOURCE_NETWORK_SERVER;
  event_conditions[2].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_Event* events = 0;
  edg_wll_Context ctx = context.get();

  boost::function<
    int(
      edg_wll_Context,
      edg_wll_QueryRec const*,
      edg_wll_QueryRec const*,
      edg_wll_Event**
    )> query_function;
  if (have_lbproxy) {
    query_function = edg_wll_QueryEventsProxy;
  } else {
    edg_wll_LogFlush(ctx, 0 /* no timeout */);
    query_function = edg_wll_QueryEvents;
  }

  int const error(
    query_function(ctx, job_conditions, event_conditions, &events)
  );

  //FreeEvents: cleanup is guaranteed by edg_wll_QueryEvents
  if (error == 0) {
    std::string result;
    if (events) {
      for (int i = 0; events[i].type; ++i) {
        // in principle there is only one, so save the first one and ignore
        // the others
        if (result.empty()
          && events[i].type == EDG_WLL_EVENT_ENQUEUED
          && events[i].enQueued.result == EDG_WLL_ENQUEUED_OK) {
          result = events[i].enQueued.job;
        }

        edg_wll_FreeEvent(&events[i]);
      }

      free(events);
    }

    return result;
  }

  if (is_retryable(error)) {
    throw LB_QueryTemporarilyFailed();
  } else {
    throw LB_QueryFailed();
  }
}

std::string
get_last_matched_ceid(
  ContextPtr context,
  jobid::JobId const& id
)
{
  edg_wll_QueryRec jobid[2];
  jobid[0].attr    = EDG_WLL_QUERY_ATTR_JOBID;
  jobid[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  jobid[0].value.j = glite_jobid_t(id.c_jobid());
  jobid[1].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec const* job_conditions[2];
  job_conditions[0] = jobid;
  job_conditions[1] = 0;

  edg_wll_QueryRec match[2];
  match[0].attr    = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  match[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  match[0].value.i = EDG_WLL_EVENT_MATCH;
  match[1].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec from_wm[3];
  from_wm[0].attr    = EDG_WLL_QUERY_ATTR_SOURCE;
  from_wm[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  from_wm[0].value.i = EDG_WLL_SOURCE_WORKLOAD_MANAGER;
  from_wm[1].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec const* event_conditions[3];
  event_conditions[0] = match;
  event_conditions[1] = from_wm;
  event_conditions[2] = 0;

  LB_Events match_events(
    do_query_events(context, job_conditions, event_conditions, false)
  );

  LB_Events::reverse_iterator last(match_events.rbegin());
  LB_Events::reverse_iterator const beyond_first(match_events.rend());
  if (last != beyond_first) {
    return last->match.dest_id;
  }
  return "";
}

std::string
get_user_x509_proxy(jobid::JobId const& jobid)
{
  std::string result;

  char* c_x509_proxy = 0;
  int const err_code(
    glite_renewal_GetProxy(jobid.toString().c_str(), &c_x509_proxy)
  );

  if (err_code == 0) {

    result.assign(c_x509_proxy);
    free(c_x509_proxy);

  } else {

    // currently no proxy is registered if renewal is not requested
    // try to get the original user proxy from the input sandbox

    configuration::Configuration const& config(
      *configuration::Configuration::instance()
    );

    std::string x509_proxy(config.ns()->sandbox_staging_path());
    x509_proxy += "/"
      + utilities::get_reduced_part(jobid)
      + "/"
      + utilities::to_filename(jobid)
      + "/user.proxy";

    result = x509_proxy;

  }

  return result;
}

std::string get_host_x509_proxy()
{
  return configuration::Configuration::instance()->common()->host_proxy_file();
}

namespace {

boost::tuple<int,std::string,std::string>
get_error_info(ContextPtr context)
{
  int error;
  std::string error_txt;
  std::string description_txt;

  char* c_error_txt = 0;
  char* c_description_txt = 0;
  error = edg_wll_Error(context.get(), &c_error_txt, &c_description_txt);

  if (c_error_txt) {
    error_txt = c_error_txt;
  }
  free(c_error_txt);
  if (c_description_txt) {
    description_txt = c_description_txt;
  }
  free(c_description_txt);

  return boost::make_tuple(error, error_txt, description_txt);
}

} // anonymous

std::string
get_lb_message(ContextPtr context)
{
  std::string result;

  int error;
  std::string error_txt;
  std::string description_txt;
  boost::tie(error, error_txt, description_txt) = get_error_info(context);

  result += error_txt;
  result += " (";
  result += boost::lexical_cast<std::string>(error);
  result += ") - ";
  result += description_txt;

  return result;
}

std::string
get_lb_sequence_code(ContextPtr context)
{
  char* c_sequence_code = edg_wll_GetSequenceCode(context.get());
  std::string sequence_code(c_sequence_code);
  free(c_sequence_code);
  return sequence_code;
}

namespace {

bool
lb_log(boost::function<int(edg_wll_Context)> log_f, ContextPtr user_context)
{
  ContextPtr result_context = user_context;
  int lb_error = log_f(user_context.get());
  int const retries = 10;

  for (int i = 0; i < retries && lb_error; ++i) {

    if (lb_error == EDG_WLL_ERROR_GSS) { // try with the host proxy

      // get the sequence code
      std::string host_x509_proxy(get_host_x509_proxy());
      char* c_sequence_code = edg_wll_GetSequenceCode(user_context.get());
      if (!c_sequence_code) {
        break;
      }
      std::string sequence_code(c_sequence_code);
      free(c_sequence_code);

      // get the jobid
      glite_jobid_t c_jobid;
      int e = edg_wll_GetLoggingJob(user_context.get(), &c_jobid);
      if (e) {
        break;
      }
      jobid::JobId jobid(c_jobid);
      glite_jobid_free(c_jobid);

      // create the host context
      ContextPtr host_context(
        create_context(jobid, host_x509_proxy, sequence_code)
      );
      if (!host_context) {
        break;
      }

      lb_error = log_f(host_context.get());

      for (int i = 0; i < retries && lb_error; ++i) {
        if (!is_retryable(lb_error)) {
          Error("LB log failed (" << lb_error << ") for " << jobid.toString());
          Debug(get_lb_message(host_context));
          return false;
        }
        Error(
          "LB log failed (" << lb_error << ") for "
          << jobid.toString() << "retrying in " << one_minute << " seconds"
        );
        ::sleep(one_minute);
        lb_error = log_f(host_context.get());
      }

      if (lb_error) {
        return false;
      } else {
        return true;
      }
    } else {

      if (!is_retryable(lb_error)) {
        Error("LB log failed (" << lb_error << ')');
        Debug(get_lb_message(user_context));
        return false;
      }
      Error(
        "LB log failed (" << lb_error << ") retrying in " <<
        one_minute << " seconds"
      );
      ::sleep(one_minute);

    } // !EDG_WLL_ERROR_GSS
  }

  if (lb_error) {
    return false;
  } else {
    return true;
  }
}

}

bool log_dequeued(ContextPtr context, std::string const& from)
{
  boost::function<
    int(edg_wll_Context, char const*, char const*)
  > log_function;
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogDeQueuedProxy;
    log_function_name = "edg_wll_LogDeQueuedProxy";
  } else {
    log_function = edg_wll_LogDeQueued;
    log_function_name = "edg_wll_LogDeQueued";
  }

  char const* const local_jobid = ""; // not needed because no real local id
  return lb_log(
    boost::bind(log_function, _1, from.c_str(), local_jobid),
    context
  );
}

bool log_cancel_req(ContextPtr context)
{
  boost::function<
    int(edg_wll_Context, char const*)
  > log_function(edg_wll_LogCancelREQProxy);
  std::string log_function_name;
  if (have_lbproxy) {
    log_function =edg_wll_LogCancelREQProxy;
    std::string const log_function_name = "edg_wll_LogCancelREQProxy";
  } else {
    log_function = edg_wll_LogCancelREQ;
    log_function_name = "edg_wll_LogCancelREQ";
  }

  return lb_log(
    boost::bind(log_function, _1, ""),
    context
  );
}

bool log_cancelled(ContextPtr context)
{
  boost::function<
    int(edg_wll_Context, char const*)
  > log_function;
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogCancelDONEProxy;
    log_function_name = "edg_wll_LogCancelDONEProxy";
  } else {
    log_function = edg_wll_LogCancelDONE;
    log_function_name = "edg_wll_LogCancelDONE";
  }

  return lb_log(
    boost::bind(log_function, _1, ""),
    context
  );
}

bool log_done_cancelled(ContextPtr context)
{
  boost::function<
    int(edg_wll_Context, char const*, int const)
  > log_function;
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogDoneCANCELLEDProxy;
    log_function_name = "edg_wll_LogDoneCANCELLEDProxy";
  } else {
    log_function = edg_wll_LogDoneCANCELLED;
    log_function_name = "edg_wll_LogDoneCANCELLED";
  }

  return lb_log(
    boost::bind(log_function, _1, "Aborted by user", 0),
    context
  );
}

bool log_pending(ContextPtr context, std::string const& reason)
{
  boost::function<
    int(edg_wll_Context, char const*)
  > log_function;
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogPendingProxy;
    log_function_name = "edg_wll_LogPendingProxy";
  } else {
    log_function = edg_wll_LogPending;
    log_function_name = "edg_wll_LogPending";
  }

  return lb_log(
    boost::bind(log_function, _1, reason.c_str()),
    context
  );
}

bool log_abort(ContextPtr context, std::string const& reason)
{
  boost::function<
    int(edg_wll_Context, char const*)
  > log_function;
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogAbortProxy;
    log_function_name = "edg_wll_LogAbortProxy";
  } else {
    log_function = edg_wll_LogAbort;
    log_function_name = "edg_wll_LogAbort";
  }

  return lb_log(
    boost::bind(log_function, _1, reason.c_str()),
    context
  );
}

bool log_resubmission_shallow(
  ContextPtr context,
  std::string const& token_file
)
{
  boost::function<
    int(edg_wll_Context, char const*, char const*)
  > log_function;
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogResubmissionSHALLOWProxy;
    log_function_name = "edg_wll_LogResubmissionSHALLOWProxy";
  } else {
    log_function = edg_wll_LogResubmissionSHALLOW;
    log_function_name = "edg_wll_LogResubmissionSHALLOW";
  }

  return lb_log(
    boost::bind(
      log_function,
      _1,
      "token still exists",
      token_file.c_str()
    ),
    context
  );
}

bool log_resubmission_deep(ContextPtr context)
{
  boost::function<
    int(edg_wll_Context, char const*, char const*)
  > log_function;
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogResubmissionWILLRESUBProxy;
    log_function_name = "edg_wll_LogResubmissionWILLRESUBProxy";
  } else {
    log_function = edg_wll_LogResubmissionWILLRESUB;
    log_function_name = "edg_wll_LogResubmissionWILLRESUB";
  }

  return lb_log(
    boost::bind(
      log_function,
      _1,
      "shallow resubmission is disabled",
      ""
    ),
    context
  );
}

bool log_resubmission_deep(ContextPtr context, std::string const& token_file)
{
  boost::function<
    int(edg_wll_Context, char const*, char const*)
  > log_function;
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogResubmissionWILLRESUBProxy;
    log_function_name = "edg_wll_LogResubmissionWILLRESUBProxy";
  } else {
    log_function = edg_wll_LogResubmissionWILLRESUB;
    log_function_name = "edg_wll_LogResubmissionWILLRESUB";
  }

  return lb_log(
    boost::bind(
      log_function,
      _1,
      "token was grabbed",
      token_file.c_str()
    ),
    context
  );
}

bool log_usertag(ContextPtr context, std::string const& name, std::string const& value)
{
  boost::function<
    int(edg_wll_Context, char const*, char const*)
  > log_function(edg_wll_LogUserTagProxy);
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogUserTagProxy;
    log_function_name = "edg_wll_LogUserTagProxy";
  } else {
    log_function = edg_wll_LogUserTag;
    log_function_name = "edg_wll_LogUserTag";
  }
  return lb_log(
    boost::bind(log_function, _1, name.c_str(), value.c_str()),
    context
  );
}
 
bool log_match(ContextPtr context, std::string const& ce_id)
{
  boost::function<
    int(edg_wll_Context, char const*)
  > log_function(edg_wll_LogMatchProxy);
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogMatchProxy;
    log_function_name = "edg_wll_LogMatchProxy";
  } else {
    log_function = edg_wll_LogMatch;
    log_function_name = "edg_wll_LogMatch";
  }

  return lb_log(
    boost::bind(log_function, _1, ce_id.c_str()),
    context
  );
}

bool log_transfer_start(
  ContextPtr context,
  std::string const& logfile
)
{
  boost::function<
    int(
      edg_wll_Context,
      edg_wll_Source,
      char const*,
      char const*,
      char const*,
      char const*,
      char const*
    )
  > log_function;
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogTransferSTARTProxy;
    log_function_name = "edg_wll_LogTransferSTARTProxy";
  } else {
    log_function = edg_wll_LogTransferSTART;
    log_function_name = "edg_wll_LogTransferSTART";
  }

  return lb_log(
    boost::bind(
      log_function,
      _1,
      EDG_WLL_SOURCE_WORKLOAD_MANAGER,
      "localhost",
      logfile.c_str(),
      "unavailable",
      "unavailable",
      "unavailable")
    ,
    context
  );
}

bool log_transfer_ok(
  ContextPtr context,
  std::string const& logfile,
  std::string const& rsl,
  std::string const& reason
)
{
  boost::function<
    int(
      edg_wll_Context,
      edg_wll_Source,
      char const*,
      char const*,
      char const*,
      char const*,
      char const*
    )
  > log_function;
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogTransferOKProxy;
    log_function_name = "edg_wll_LogTransferOKProxy";
  } else {
    log_function = edg_wll_LogTransferOK;
    log_function_name = "edg_wll_LogTransferOK";
  }

  return lb_log(
    boost::bind(
      log_function,
      _1,
      EDG_WLL_SOURCE_LOG_MONITOR,
      "localhost",
      logfile.c_str(),
      rsl.c_str(),
      reason.c_str(),
      "unavailable")
    ,
    context
  );
}

bool log_running(ContextPtr context, std::string const& host)
{
  boost::function<
    int(edg_wll_Context, char const*)
  > log_function;
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogRunningProxy;
    log_function_name = "edg_wll_LogRunningProxy";
  } else {
    log_function = edg_wll_LogRunning;
    log_function_name = "edg_wll_LogRunning";
  }

  return lb_log(
    boost::bind(log_function, _1, host.c_str()),
    context
  );
}

bool log_done_ok(ContextPtr context, std::string const& reason, int retcode)
{
  boost::function<
    int(edg_wll_Context, char const*, int)
  > log_function;
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogDoneOKProxy;
    log_function_name = "edg_wll_LogDoneOKProxy";
  } else {
    log_function = edg_wll_LogDoneOK;
    log_function_name = "edg_wll_LogDoneOK";
  }

  return lb_log(
    boost::bind(log_function, _1, reason.c_str(), retcode),
    context
  );
}

bool log_enqueued_start(
  ContextPtr context,
  std::string const& to
)
{
  boost::function<
    int(edg_wll_Context, char const*, char const*, char const*)
  > log_function;
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogEnQueuedSTARTProxy;
    log_function_name = "edg_wll_LogEnQueuedSTARTProxy";
  } else {
    log_function = edg_wll_LogEnQueuedSTART;
    log_function_name = "edg_wll_LogEnQueuedSTART";
  }

  return lb_log(
    boost::bind(log_function, _1, to.c_str(), "", ""),
    context
  );
}

bool log_enqueued_ok(
  ContextPtr context,
  std::string const& to,
  std::string const& ad
)
{
  boost::function<
    int(edg_wll_Context, char const*, char const*, char const*)
  > log_function;
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogEnQueuedOKProxy;
    log_function_name = "edg_wll_LogEnQueuedOKProxy";
  } else {
    log_function = edg_wll_LogEnQueuedOK;
    log_function_name = "edg_wll_LogEnQueuedOK";
  }

  return lb_log(
    boost::bind(log_function, _1, to.c_str(), ad.c_str(), ""),
    context
  );
}

bool log_enqueued_fail(
  ContextPtr context,
  std::string const& to,
  std::string const& ad,
  std::string const& reason
)
{
  boost::function<
    int(edg_wll_Context, char const*, char const*, char const*)
  > log_function;
  std::string log_function_name;
  if (have_lbproxy) {
    log_function = edg_wll_LogEnQueuedFAILProxy;
    log_function_name = "edg_wll_LogEnQueuedFAILProxy";
  } else {
    log_function = edg_wll_LogEnQueuedFAIL;
    log_function_name = "edg_wll_LogEnQueuedFAIL";
  }

  return lb_log(
    boost::bind(log_function, _1, to.c_str(), ad.c_str(), reason.c_str()),
    context
  );
}

namespace {

edg_wll_JobStat* create_job_status()
{
  std::auto_ptr<edg_wll_JobStat> result(new edg_wll_JobStat);
  if (edg_wll_InitStatus(result.get()) == 0) {
     return result.release();
  } else {
     return 0;
  }
}

void delete_job_status(edg_wll_JobStat* p)
{
  edg_wll_FreeStatus(p);
  delete p;
}

}

JobStatusPtr job_status(jobid::JobId const& id, ContextPtr context, int flags)
{
  if (context) {
    JobStatusPtr status(create_job_status(), delete_job_status);
    if (status) {
      int err;
      if (have_lbproxy) {
        err = edg_wll_JobStatusProxy(
          context.get(),
          id.c_jobid(),
          flags,
          status.get()
        );
      } else {
        err = edg_wll_JobStatus(
          context.get(),
          id.c_jobid(),
          flags,
          status.get()
        );
      }
      if (err == 0) {
        return status;
      }
    }
  }

  return JobStatusPtr();
}

bool is_submitted(JobStatusPtr status)
{
  return status && status->state == EDG_WLL_JOB_SUBMITTED;
}

JobStatusPtr job_status(jobid::JobId const& id, int flags)
{
  std::string const x509_proxy = get_user_x509_proxy(id);
  std::string const sequence_code;
  ContextPtr context(
    create_context(id, x509_proxy, sequence_code)
  );
  if (context) {
    JobStatusPtr status(create_job_status(), delete_job_status);
    if (status) {
      int err;
      if (have_lbproxy) {
        err = edg_wll_JobStatusProxy(
          context.get(),
          id.c_jobid(),
          flags,
          status.get()
        );
      } else {
        err = edg_wll_JobStatus(
          context.get(),
          id.c_jobid(),
          flags,
          status.get()
        );
      }
      if (err == 0) {
        return status;
      }
    }
  }

  return JobStatusPtr();
}

std::string status_to_string(JobStatusPtr status)
{
  std::string result;
  if (status) {
    char* s = edg_wll_StatToString(status->state);
    if (s) {
      result = s;
      free(s);
    }
  }
  return result;
}

namespace {

bool is_enqueued_to_jc(edg_wll_Event const& event)
{
  return event.type == EDG_WLL_EVENT_ENQUEUED
    && event.any.source == EDG_WLL_SOURCE_WORKLOAD_MANAGER;
}

bool is_cancelled_event(edg_wll_Event const& event)
{
  return event.type == EDG_WLL_EVENT_CANCEL;
}

bool is_done_event(edg_wll_Event const& event)
{
  return event.type == EDG_WLL_EVENT_DONE
    && event.any.source == EDG_WLL_SOURCE_LOG_MONITOR;
}

LB_Events::const_iterator
find_last_enqueued_to_jc(LB_Events const& events)
{
  LB_Events::const_reverse_iterator it(
    std::find_if(events.rbegin(), events.rend(), is_enqueued_to_jc)
  );
  if (it != events.rend()) {
    return (++it).base();
  } else {
    return events.end();
  }
}

LB_Events::const_iterator
find_last_cancelled(LB_Events const& events)
{
  LB_Events::const_reverse_iterator it(
    std::find_if(events.rbegin(), events.rend(), is_cancelled_event)
  );
  if (it != events.rend()) {
    return (++it).base();
  } else {
    return events.end();
  }
}

LB_Events::const_iterator
find_last_done(LB_Events const& events)
{
  LB_Events::const_reverse_iterator it(
    std::find_if(events.rbegin(), events.rend(), is_done_event)
  );
  if (it != events.rend()) {
    return (++it).base();
  } else {
    return events.end();
  }
}

}

bool is_last_done(LB_Events const& events, edg_wll_DoneStatus_code status_code)
{
  LB_Events::const_iterator e(find_last_done(events));
  return e != events.end() && e->done.status_code == status_code;
}

bool is_waiting(JobStatusPtr status)
{
  return status && status->state == EDG_WLL_JOB_WAITING;
}

bool is_ready(JobStatusPtr status)
{
  return status && status->state == EDG_WLL_JOB_READY;
}

bool is_done(JobStatusPtr status)
{
  return status && status->state == EDG_WLL_JOB_DONE;
}

bool is_cancelled(JobStatusPtr status)
{
  return status && status->state == EDG_WLL_JOB_CANCELLED;
}

bool is_aborted(JobStatusPtr status)
{
  return status && status->state == EDG_WLL_JOB_ABORTED;
}

bool is_last_cancelled_by_wm(LB_Events const& events)
{
  LB_Events::const_iterator e(find_last_cancelled(events));
  return e != events.end() && EDG_WLL_SOURCE_WORKLOAD_MANAGER == e->any.source;
}

bool is_last_enqueued_by_wm(LB_Events const& events, edg_wll_EnQueuedResult result)
{
  LB_Events::const_iterator e(find_last_enqueued_to_jc(events));
  return e != events.end() && e->enQueued.result == result;
}

bool in_limbo(
  JobStatusPtr status,
  LB_Events const& events
) 
{
  bool result = false;
  bool last_enqueued_by_wm_is_start = 
    is_last_enqueued_by_wm(events, EDG_WLL_ENQUEUED_START);
  if (
    is_ready(status) && last_enqueued_by_wm_is_start
    // to avoid double submission here, the (remote) risk is that
    // the request expires (waiting in limbo) because the WM precedently
    // terminated after logging enqueued start, but before writing the request
    || (
      is_done(status)
      && is_last_done(events, EDG_WLL_DONE_FAILED)
      && last_enqueued_by_wm_is_start
    ) // this last check is to address a race condition:
      // before LM logs enqueued OK, the resubmit request could have
      // already been read and processed by the WM
  ) {
    result = true;
  }

  return result;
}

}}}}
