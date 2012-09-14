// File: lb_utils.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include "lb_utils.h"
#include "signal_handling.h"
#include <sys/types.h>
#include <sys/errno.h>
#include <unistd.h>
#include <map>
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/lb/producer.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/security/proxyrenewal/renewal.h"

namespace jobid = glite::wmsutils::jobid;
namespace configuration = glite::wms::common::configuration;

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {

unsigned int const five_seconds = 5;

template <typename P>
void sleep_while(unsigned int seconds, P condition)
{
  for (unsigned int i = 0; i < seconds && condition(); ++i) {
    ::sleep(1);
  }
}

// what's not retryable is considered to be fatal
bool is_retryable(int lb_error)
{
  return lb_error == ETIMEDOUT
    || lb_error == ENOTCONN
    || lb_error == ECONNREFUSED
    || lb_error == EAGAIN;
}

std::string
get_proxy_subject(std::string const& x509_proxy)
{
  static std::string const null_string;

  std::FILE* fd = std::fopen(x509_proxy.c_str(), "r");
  if (!fd) return null_string;
  boost::shared_ptr<std::FILE> fd_(fd, std::fclose);

  ::X509* const cert = ::PEM_read_X509(fd, 0, 0, 0);
  if (!cert) return null_string;
  boost::shared_ptr< ::X509> cert_(cert, ::X509_free);

  char* const s = ::X509_NAME_oneline(::X509_get_subject_name(cert), 0, 0);
  if (!s) return null_string;
  boost::shared_ptr<char> s_(s, ::free);

  return std::string(s);
}

bool not_received_quit_signal()
{
  return !received_quit_signal();
}

int set_logging_job(
  edg_wll_Context context,
  jobid::JobId const& id,
  std::string const& sequence_code
)
{
  int const flag = EDG_WLL_SEQ_NORMAL;

  while (not_received_quit_signal()) {
    
    int const result = edg_wll_SetLoggingJob(
      context,
      id,
      sequence_code.c_str(),
      flag
    );

    if (result == 0) {
      return result;
    }

    std::string message("edg_wll_SetLoggingJobProxy failed:");
    if (is_retryable(result)) {
      Warning(message << " retrying in " << five_seconds << " seconds");
      sleep_while(five_seconds, not_received_quit_signal);
    } else {
      Error(message << " LB is unavailable, giving up");
      assert(false);
    }
  }
  Error("set_logging_job_proxy: fatal error");
  assert(false);
}

int set_logging_job_proxy(
  edg_wll_Context context,
  jobid::JobId const& id,
  std::string const& x509_proxy
)
{
  std::string const user_dn = get_proxy_subject(x509_proxy);
  int const flag = EDG_WLL_SEQ_NORMAL;
  char const* const sequence_code = 0;

  while (not_received_quit_signal()) {
    
    int const result = edg_wll_SetLoggingJobProxy(
      context,
      id,
      sequence_code,
      user_dn.c_str(),
      flag
    );

    if (result == 0) {
      return result;
    }

    std::string message("edg_wll_SetLoggingJobProxy failed:");
    if (is_retryable(result)) {
      Warning(message << " retrying in " << five_seconds << " seconds");
      sleep_while(five_seconds, not_received_quit_signal);
    } else {
      Error(message << " LB is unavailable, giving up");
      assert(false);
    }
  }
  Error("set_logging_job_proxy: fatal error");
  assert(false);
}

int set_logging_job_proxy(
  edg_wll_Context context,
  jobid::JobId const& id,
  std::string const& x509_proxy,
  std::string const& sequence_code
)
{
  int result = 0;
  if (sequence_code.empty()) {
    result = set_logging_job_proxy(context, id, x509_proxy);
  } else {
    std::string const user_dn = get_proxy_subject(x509_proxy);
    int const flag = EDG_WLL_SEQ_NORMAL;
    result = edg_wll_SetLoggingJobProxy(
      context,
      id,
      sequence_code.c_str(),
      user_dn.c_str(),
      flag
    );
  }

  return result;
}

} // anonymous namespace

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
  if (errcode == ENOMEM) {
    throw std::bad_alloc();
  }
  else if (errcode) {
    Error("edg_wll_InitContext: unexpected error");
    assert(false);
  }

  ContextPtr result(context, edg_wll_FreeContext);

  errcode = edg_wll_SetParam(
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
  if (errcode) {
    Error("edg_wll_SetParam: unexpected error");
    assert(false);
  }

#ifdef GLITE_WMS_HAVE_LBPROXY
  set_logging_job_proxy(
    context,
    id,
    x509_proxy,
    sequence_code
  );
#else
  set_logging_job(context, id, sequence_code);
#endif

  return result;
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
      + jobid::get_reduced_part(jobid)
      + "/"
      + jobid::to_filename(jobid)
      + "/user.proxy";

    result = x509_proxy;
  }

  return result;
}

std::string get_host_x509_proxy()
{
  return configuration::Configuration::instance()->common()->host_proxy_file();
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

void lb_proxy_log(
  boost::function<int(edg_wll_Context)> log,
  ContextPtr context,
  std::string const& function_name
);

void lb_log(
  boost::function<int(edg_wll_Context)> log,
  ContextPtr context,
  std::string const& function_name
);

}

void log_dequeued(ContextPtr context, std::string const& from)
{
  static char const* const local_jobid = "";

#ifdef GLITE_WMS_HAVE_LBPROXY

  lb_proxy_log(
    boost::bind(edg_wll_LogDeQueuedProxy, _1, from.c_str(), local_jobid),
    context,
    "edg_wll_LogDeQueuedProxy"
  );

#else

  lb_log(
    boost::bind(edg_wll_LogDeQueued, _1, from.c_str(), local_jobid),
    context,
    "edg_wll_LogDeQueued"
  );

#endif
}

void log_cancel_req(ContextPtr context)
{
#ifdef GLITE_WMS_HAVE_LBPROXY

  lb_proxy_log(
    boost::bind(edg_wll_LogCancelREQProxy, _1, ""),
    context,
    "edg_wll_LogCancelREQProxy"
  );

#else

  lb_log(
    boost::bind(edg_wll_LogCancelREQ, _1, ""),
    context,
    "edg_wll_LogCancelREQ"
  );

#endif
}

void log_cancelled(ContextPtr context)
{
#ifdef GLITE_WMS_HAVE_LBPROXY

  lb_proxy_log(
    boost::bind(edg_wll_LogCancelDONEProxy, _1, ""),
    context,
    "edg_wll_LogCancelDONEProxy"
  );

#else

  lb_log(
    boost::bind(edg_wll_LogCancelDONE, _1, ""),
    context,
    "edg_wll_LogCancelDONE"
  );

#endif
}

void log_pending(ContextPtr context, std::string const& reason)
{
#ifdef GLITE_WMS_HAVE_LBPROXY

  lb_proxy_log(
    boost::bind(edg_wll_LogPendingProxy, _1, reason.c_str()),
    context,
    "edg_wll_LogPendingProxy"
  );

#else

  lb_log(
    boost::bind(edg_wll_LogPending, _1, reason.c_str()),
    context,
    "edg_wll_LogPending"
  );

#endif
}

void log_abort(ContextPtr context, std::string const& reason)
{
#ifdef GLITE_WMS_HAVE_LBPROXY

  lb_proxy_log(
    boost::bind(edg_wll_LogAbortProxy, _1, reason.c_str()),
    context,
    "edg_wll_LogAbortProxy"
  );

#else

  lb_log(
    boost::bind(edg_wll_LogAbort, _1, reason.c_str()),
    context,
    "edg_wll_LogAbort"
  );

#endif
}

void log_resubmission_shallow(
  ContextPtr context,
  std::string const& token_file
)
{
  static char const* const reason = "token still exists";

#ifdef GLITE_WMS_HAVE_LBPROXY

  lb_proxy_log(
    boost::bind(
      edg_wll_LogResubmissionSHALLOWProxy,
      _1,
      reason,
      token_file.c_str()
    ),
    context,
    "edg_wll_LogResubmissionSHALLOWProxy"
  );

#else

  lb_log(
    boost::bind(
      edg_wll_LogResubmissionSHALLOW,
      _1,
      reason,
      token_file.c_str()
    ),
    context,
    "edg_wll_LogResubmissionSHALLOW"
  );

#endif
}

void log_resubmission_deep(ContextPtr context)
{
  static char const* const reason = "shallow resubmission is disabled";
  static char const* const tag = "";

#ifdef GLITE_WMS_HAVE_LBPROXY

  lb_proxy_log(
    boost::bind(
      edg_wll_LogResubmissionWILLRESUBProxy,
      _1,
      reason,
      tag
    ),
    context,
    "edg_wll_LogResubmissionWILLRESUBProxy"
  );

#else

  lb_log(
    boost::bind(
      edg_wll_LogResubmissionWILLRESUB,
      _1,
      reason,
      tag
    ),
    context,
    "edg_wll_LogResubmissionWILLRESUB"
  );

#endif
}

void log_resubmission_deep(ContextPtr context, std::string const& token_file)
{
  static char const* const reason = "token was grabbed";
  char const* const tag = token_file.c_str();

#ifdef GLITE_WMS_HAVE_LBPROXY

  lb_proxy_log(
    boost::bind(
      edg_wll_LogResubmissionWILLRESUBProxy,
      _1,
      reason,
      tag
    ),
    context,
    "edg_wll_LogResubmissionWILLRESUBProxy"
  );

#else

  lb_log(
    boost::bind(
      edg_wll_LogResubmissionWILLRESUB,
      _1,
      reason,
      tag
    ),
    context,
    "edg_wll_LogResubmissionWILLRESUB"
  );

#endif
}

void log_match(ContextPtr context, std::string const& ce_id)
{
#ifdef GLITE_WMS_HAVE_LBPROXY

  lb_proxy_log(
    boost::bind(edg_wll_LogMatchProxy, _1, ce_id.c_str()),
    context,
    "edg_wll_LogMatchProxy"
  );

#else

  lb_log(
    boost::bind(edg_wll_LogMatch, _1, ce_id.c_str()),
    context,
    "edg_wll_LogMatch"
  );

#endif
}

void log_enqueued_start(
  ContextPtr context,
  std::string const& to
)
{
  char const* const queue = to.c_str();
  static char const* const job = "";
  static char const* const reason = "";

#ifdef GLITE_WMS_HAVE_LBPROXY

  lb_proxy_log(
    boost::bind(edg_wll_LogEnQueuedSTARTProxy, _1, queue, job, reason),
    context,
    "edg_wll_LogEnQueuedSTARTProxy"
  );

#else

  lb_log(
    boost::bind(edg_wll_LogEnQueuedSTART, _1, queue, job, reason),
    context,
    "edg_wll_LogEnQueuedSTART"
  );

#endif
}

void log_enqueued_ok(
  ContextPtr context,
  std::string const& to,
  std::string const& ad
)
{
  char const* const queue = to.c_str();
  char const* const job = ad.c_str();
  static char const* const reason = "";

#ifdef GLITE_WMS_HAVE_LBPROXY

  lb_proxy_log(
    boost::bind(edg_wll_LogEnQueuedOKProxy, _1, queue, job, reason),
    context,
    "edg_wll_LogEnQueuedOKProxy"
  );

#else

  lb_log(
    boost::bind(edg_wll_LogEnQueuedOK, _1, queue, job, reason),
    context,
    "edg_wll_LogEnQueuedOK"
  );

#endif
}

void log_enqueued_fail(
  ContextPtr context,
  std::string const& to,
  std::string const& ad,
  std::string const& reason_
)
{
  char const* const queue = to.c_str();
  char const* const job = ad.c_str();
  char const* const reason = reason_.c_str();

#ifdef GLITE_WMS_HAVE_LBPROXY

  lb_proxy_log(
    boost::bind(edg_wll_LogEnQueuedFAILProxy, _1, queue, job, reason),
    context,
    "edg_wll_LogEnQueuedFAILProxy"
  );

#else

  lb_log(
    boost::bind(edg_wll_LogEnQueuedFAIL, _1, queue, job, reason),
    context,
    "edg_wll_LogEnQueuedFAIL"
  );

#endif
}

void log_helper_called(
  ContextPtr context, 
  std::string const& name_
)
{
  char const* const name = name_.c_str();
  static char const* const args = "";
  
#ifdef GLITE_WMS_HAVE_LBPROXY

  lb_proxy_log(
    boost::bind(edg_wll_LogHelperCallCALLEDProxy, _1, name, args),
    context,
    "edg_wll_LogHelperCallCALLEDProxy"
  );

#else

  lb_log(
    boost::bind(edg_wll_LogHelperCallCALLED, _1, name, args),
    context,
    "edg_wll_LogHelperCallCALLED"
  );

#endif
}
  
void log_helper_return(
  ContextPtr context,
  std::string const& name_,
  int status
)
{
  std::string const retval_ = boost::lexical_cast<std::string>(status);
  char const* const retval = retval_.c_str();
  char const* const name = name_.c_str();

#ifdef GLITE_WMS_HAVE_LBPROXY

  lb_proxy_log(
    boost::bind(edg_wll_LogHelperReturnCALLEDProxy, _1, name, retval),
    context,
    "edg_wll_LogHelperReturnCALLEDProxy"
  );

#else

  lb_log(
    boost::bind(edg_wll_LogHelperReturnCALLED, _1, name, retval),
    context,
    "edg_wll_LogHelperReturnCALLED"
  );

#endif
}

bool flush_lb_events(ContextPtr context)
{
  struct timeval* timeout = 0;
  return edg_wll_LogFlush(context.get(), timeout);
}

namespace {

void free_events(edg_wll_Event* events);

}

LB_Events::LB_Events(edg_wll_Event* events)
  : m_events(events, &free_events), m_size(0)
{
  if (m_events) {
    while (m_events[m_size].type != EDG_WLL_EVENT_UNDEF) {
      ++m_size;
    }
  }
}

namespace {

std::string
format_log_message(std::string const& function_name, ContextPtr context);

bool is_deep_resubmission(edg_wll_Event const& event);
bool is_shallow_resubmission(edg_wll_Event const& event);
LB_Events::const_iterator find_last_deep_resubmission(LB_Events const& events);

}

LB_Events
get_interesting_events(ContextPtr context, jobid::JobId const& id)
{
  edg_wll_QueryRec jobid[2];
  jobid[0].attr    = EDG_WLL_QUERY_ATTR_JOBID;
  jobid[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  jobid[0].value.j = id.getId();
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

#ifdef GLITE_WMS_HAVE_LBPROXY
  static boost::function<
    int(
      edg_wll_Context,
      edg_wll_QueryRec const**,
      edg_wll_QueryRec const**,
      edg_wll_Event**
    )
  > const query_function(edg_wll_QueryEventsExtProxy);
  static char const* const function_name = "edg_wll_QueryEventsExtProxy";
#else
  static boost::function<
    int(
      edg_wll_Context,
      edg_wll_QueryRec const**,
      edg_wll_QueryRec const**,
      edg_wll_Event**
    )
  > const query_function(edg_wll_QueryEventsExt);
  static char const* const function_name = "edg_wll_QueryEventsExt";
#endif

  while (not_received_quit_signal()) {

    edg_wll_Event* events = 0;
    edg_wll_Context ctx = context.get();

    int const error(
      query_function(ctx, job_conditions, event_conditions, &events)
    );

    LB_Events result(events); // guarantees cleanup (even in case of failure)

    // no events is not necessarily an error
    if (error == 0 || error == ENOENT) {
      return result;
    }

    std::string message(format_log_message(function_name, context));
    if (is_retryable(error)) {
      Warning(message << " retrying in " << five_seconds << " seconds");
      sleep_while(five_seconds, not_received_quit_signal);
    } else {
      Error(message << " LB is unavailable, giving up");
      assert(false);
    }
  }

  return LB_Events(0);
}

std::vector<std::pair<std::string, int> >
get_previous_matches(LB_Events const& events)
{
  std::vector<std::pair<std::string, int> > result;

  LB_Events::const_iterator it(events.begin());
  LB_Events::const_iterator const last(events.end());
  for ( ; it != last; ++it) {
    edg_wll_Event const& event = *it;
    if (event.type == EDG_WLL_EVENT_MATCH) {
      std::string const ce_id(event.match.dest_id);
      int timestamp(event.match.timestamp.tv_sec);
      result.push_back(std::make_pair(ce_id, timestamp));
    }
  }

  return result;
}

boost::tuple<int, int>
get_retry_counts(LB_Events const& events)
{
  int const deep_count(
    std::count_if(events.begin(), events.end(), is_deep_resubmission)
  );
  assert(deep_count >= 0);

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
get_original_jdl(ContextPtr context, jobid::JobId const& id)
{
  edg_wll_QueryRec job_conditions[2];
  job_conditions[0].attr    = EDG_WLL_QUERY_ATTR_JOBID;
  job_conditions[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  job_conditions[0].value.j = id.getId();
  job_conditions[1].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

  edg_wll_QueryRec event_conditions[3];
  event_conditions[0].attr    = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  event_conditions[0].op      = EDG_WLL_QUERY_OP_EQUAL;
  event_conditions[0].value.i = EDG_WLL_EVENT_ENQUEUED;
  event_conditions[1].attr    = EDG_WLL_QUERY_ATTR_SOURCE;
  event_conditions[1].op      = EDG_WLL_QUERY_OP_EQUAL;
  event_conditions[1].value.i = EDG_WLL_SOURCE_NETWORK_SERVER;
  event_conditions[2].attr    = EDG_WLL_QUERY_ATTR_UNDEF;

#ifdef GLITE_WMS_HAVE_LBPROXY
  static boost::function<
    int(
      edg_wll_Context,
      edg_wll_QueryRec const*,
      edg_wll_QueryRec const*,
      edg_wll_Event**
    )
  > const query_function(edg_wll_QueryEventsProxy);
  static char const* const function_name = "edg_wll_QueryEventsProxy";
#else
  static boost::function<
    int(
      edg_wll_Context,
      edg_wll_QueryRec const*,
      edg_wll_QueryRec const*,
      edg_wll_Event**
    )
  > const query_function(edg_wll_QueryEvents);
  static char const* const function_name = "edg_wll_QueryEvents";
#endif

  while (not_received_quit_signal()) {
    edg_wll_Event* events = 0;
    edg_wll_Context ctx = context.get();

    int const error(
      query_function(ctx, job_conditions, event_conditions, &events)
    );

    // no events is not necessarily an error
    if (error == 0 || error == ENOENT) {

      std::string result;

      if (events) {
        for (int i = 0; events[i].type != EDG_WLL_EVENT_UNDEF; ++i) {
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

    std::string message(
      format_log_message(function_name, context)
    );
    if (is_retryable(error)) {
      Warning(message << " retrying in " << five_seconds << " seconds");
      sleep_while(five_seconds, not_received_quit_signal);
    } else {
      Error(message << " LB is unavailable, giving up");
      assert(false);
    }
  }

  return std::string();
}

namespace {

boost::tuple<int,std::string,std::string> get_error_info(ContextPtr context);
edg_wll_JobStat* create_job_status();
void delete_job_status(edg_wll_JobStat* p);

}

JobStatusPtr job_status(jobid::JobId const& id, ContextPtr context)
{
  JobStatusPtr result(create_job_status(), delete_job_status);
  if (result) {
    int const flags = 0;

    while (not_received_quit_signal()) {

      int const error(
        edg_wll_JobStatusProxy(
          context.get(),
          id.getId(),
          flags,
          result.get()
        )
      );
      if (error == 0) {
        return result;
      }

      std::string message("edg_wll_JobStatusProxy failed:");
      if (is_retryable(error)) {
        Warning(message << " retrying in " << five_seconds << " seconds");
        sleep_while(five_seconds, not_received_quit_signal);
      } else {
        Error(message << " LB is unavailable, giving up");
        assert(false);
      }
    }
  }
  Error("Cannot create job status");
  assert(false);
}

JobStatusPtr job_status(jobid::JobId const& id)
{
  std::string const x509_proxy = get_user_x509_proxy(id);
  std::string const sequence_code;
  ContextPtr context(
    create_context(id, x509_proxy, sequence_code)
  );
  return job_status(id, context);
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

bool is_waiting(JobStatusPtr status)
{
  return status && status->state == EDG_WLL_JOB_WAITING;
}

bool is_cancelled(JobStatusPtr status)
{
  return status && status->state == EDG_WLL_JOB_CANCELLED;
}

namespace {

void free_events(edg_wll_Event* events)
{
  if (events) {
    for (int i = 0; events[i].type != EDG_WLL_EVENT_UNDEF; ++i) {
      edg_wll_FreeEvent(&events[i]);
    }
    free(events);
  }
}

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
format_log_message(std::string const& function_name, ContextPtr context)
{
  std::string result(function_name);
  result += " failed for ";

  edg_wlc_JobId c_jobid;
  int e = edg_wll_GetLoggingJob(context.get(), &c_jobid);
  assert(e == 0);
  jobid::JobId jobid(c_jobid);
  edg_wlc_JobIdFree(c_jobid);
  
  result += jobid.toString();
  result += '(' + get_lb_message(context) + ')';

  return result;
}

void lb_proxy_log(
  boost::function<int(edg_wll_Context)> log,
  ContextPtr context,
  std::string const& function_name
)
{
  while (not_received_quit_signal()) {
    int const error = log(context.get());
    if (error == 0) {
      return;
    }
    std::string message(
      format_log_message(function_name, context)
    );
    if (is_retryable(error)) {
      Warning(message << " retrying in " << five_seconds << " seconds");
      sleep_while(five_seconds, not_received_quit_signal);
    } else {
      Error(message << " LB is unavailable, giving up");
      assert(false);
    }
  }
}

jobid::JobId
get_jobid(ContextPtr context)
{
  edg_wlc_JobId c_jobid;
  edg_wll_GetLoggingJob(context.get(), &c_jobid);
  jobid::JobId id(c_jobid);
  edg_wlc_JobIdFree(c_jobid);
  return id;
}

void lb_log(
  boost::function<int(edg_wll_Context)> log,
  ContextPtr context,
  std::string const& function_name
)
{
  bool try_with_host_proxy = false;

  while (not_received_quit_signal()) {

    int const error = log(context.get());

    if (error == EDG_WLL_ERROR_GSS) {
      try_with_host_proxy = true;
      break;
    } else if (error == 0) {
      return;
    }

    std::string message(
      format_log_message(function_name, context)
    );
    if (is_retryable(error)) {
      Warning(message << " retrying in " << five_seconds << " seconds");
      sleep_while(five_seconds, not_received_quit_signal);
    } else {
      Error(message << " LB is unavailable, giving up");
      assert(false);
    }
  }

  if (try_with_host_proxy) {
    std::string message(
      format_log_message(function_name, context)
    );
    Warning(message << " retrying with the host proxy");

    jobid::JobId const jobid(get_jobid(context));
    std::string const host_x509_proxy(get_host_x509_proxy());
    std::string const sequence_code(get_lb_sequence_code(context));
    ContextPtr host_context(
      create_context(jobid, host_x509_proxy, sequence_code)
    );

    while (not_received_quit_signal()) {
      int const error = log(host_context.get());
      if (error == 0) {
        return;
      }
      std::string message(
        format_log_message(function_name, host_context)
      );
      if (is_retryable(error)) {
        Warning(message << " retrying in " << five_seconds << " seconds");
        sleep_while(five_seconds, not_received_quit_signal);
      } else {
        Error(message << " LB is unavailable, giving up");
        assert(false);
      }
    }
  }
}

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

}}}}
