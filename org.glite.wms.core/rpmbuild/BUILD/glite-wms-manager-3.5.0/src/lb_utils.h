// File: lb_utils.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
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

// $Id: lb_utils.h,v 1.1.2.3.2.10.2.2.2.1.2.2.2.2 2011/12/01 11:51:44 mcecchi Exp $

#ifndef GLITE_WMS_MANAGER_SERVER_LB_UTILS_H
#define GLITE_WMS_MANAGER_SERVER_LB_UTILS_H

#include <vector>
#include <string>
#include <stdexcept>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/type_traits.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>

#include "glite/lb/context.h"
#include "glite/lb/consumer.h"

namespace glite {

namespace jobid {
class JobId;
}

namespace wms {
namespace manager {
namespace server {

class SubmitProcessor;

typedef boost::shared_ptr<
  boost::remove_pointer<edg_wll_Context>::type
> ContextPtr;

class CannotCreateLBContext: public std::exception
{
  std::string m_what;
  int m_errcode;

public:
  CannotCreateLBContext(int errcode)
    : m_errcode(errcode)
  {
    m_what = "cannot create LB context ("
      + boost::lexical_cast<std::string>(m_errcode) + ')';
  }
  ~CannotCreateLBContext() throw() { }
  char const* what() const throw()
  {
    return m_what.c_str();
  }
  int error_code() const
  {
    return m_errcode;
  }
};

void init_lb();

ContextPtr
create_context(
  jobid::JobId const& id,
  std::string const& x509_proxy,
  std::string const& sequence_code,
  edg_wll_Source = EDG_WLL_SOURCE_WORKLOAD_MANAGER
);

inline void change_logging_src(ContextPtr context, edg_wll_Source source)
{
  edg_wll_SetParam(context.get(), EDG_WLL_PARAM_SOURCE, source);
}

std::string get_user_x509_proxy(jobid::JobId const& jobid);
std::string get_host_x509_proxy();

std::string get_lb_message(ContextPtr context_ptr);
std::string get_lb_sequence_code(ContextPtr context_ptr);

bool log_transfer_start(
  ContextPtr context,
  std::string const& logfile
);
bool log_transfer_ok(
  ContextPtr context,
  std::string const& logfile,
  std::string const& rsl,
  std::string const& reason
);
bool log_running(ContextPtr context, std::string const& host);
bool log_done_ok(ContextPtr context, std::string const& reason, int retcode);
bool log_cancel_req(ContextPtr context);
bool log_cancelled(ContextPtr context);
bool log_done_cancelled(ContextPtr context);
bool log_pending(ContextPtr context, std::string const& reason);
bool log_abort(ContextPtr context, std::string const& reason);
bool log_resubmission_shallow(
  ContextPtr context,
  std::string const& token_file
);
bool log_resubmission_deep(ContextPtr context);
bool log_resubmission_deep(ContextPtr context, std::string const& token_file);
bool log_match(ContextPtr context, std::string const& ce_id);
bool log_usertag(ContextPtr context, std::string const& name, std::string const& value);
bool log_dequeued(ContextPtr context,std::string const& from);
bool log_enqueued_start(ContextPtr context, std::string const& to);
bool log_enqueued_ok(
  ContextPtr context,
  std::string const& to,
  std::string const& ad
);
bool log_enqueued_fail(
  ContextPtr context,
  std::string const& to,
  std::string const& ad,
  std::string const& reason
);
void change_logging_job(
  ContextPtr context,
  std::string const& sequence_code,
  jobid::JobId const& id
);

class LB_Events
{
  boost::shared_array<edg_wll_Event> m_events;
  size_t m_size;

  static void free_events(edg_wll_Event* events)
  {
    if (events) {
      for (int i = 0; events[i].type; ++i) {
        edg_wll_FreeEvent(&events[i]);
      }
      free(events);
    }
  }

public:
  typedef edg_wll_Event const* iterator;
  typedef iterator const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  LB_Events(edg_wll_Event* events)
    : m_events(events, &free_events), m_size(0)
  {
    if (m_events) {
      while (m_events[m_size].type) {
        ++m_size;
      }
    }
  }
  bool empty() const
  {
    return m_size == 0;
  }
  size_t size() const
  {
    return m_size;
  }
  edg_wll_Event const& operator[](std::size_t i) const
  {
    assert(0 <= i && i < m_size);
    return m_events[i];
  }
  const_iterator begin() const
  {
    return m_events ? &m_events[0] : 0;
  }
  const_iterator end() const
  {
    return m_events ? &m_events[m_size] : 0;
  }
  const_reverse_iterator rbegin() const
  {
    return const_reverse_iterator(end());
  }
  const_reverse_iterator rend() const
  {
    return const_reverse_iterator(begin());
  }
};

class LB_QueryFailed { };
class LB_QueryTemporarilyFailed { };

std::vector<std::string > get_scheduled_jobs(ContextPtr context, int grace_period);

LB_Events
get_interesting_events(ContextPtr context, jobid::JobId const& id);

LB_Events
get_interesting_events_dags(ContextPtr context, jobid::JobId const& id);

LB_Events
get_cancel_events(ContextPtr context, jobid::JobId const& id);

LB_Events
get_recovery_events(ContextPtr context, jobid::JobId const& id);

std::vector<std::pair<std::string, int> >
get_previous_matches(LB_Events const& events);

// return (deep_retry_count, shallow_retry_count)
boost::tuple<int, int>
get_retry_counts(LB_Events const& events);
std::string get_last_matched_ceid(ContextPtr context, jobid::JobId const& id);
std::string
get_original_jdl(ContextPtr context, jobid::JobId const& id);

typedef boost::shared_ptr<edg_wll_JobStat> JobStatusPtr;

enum {
  none = 0,
  children = EDG_WLL_STAT_CHILDREN,
  child_status = EDG_WLL_STAT_CHILDSTAT
};

JobStatusPtr job_status(
  jobid::JobId const& id,
  ContextPtr context,
  int flags = 0
);
JobStatusPtr job_status(jobid::JobId const& id, int flags = 0);
std::string status_to_string(JobStatusPtr status);

bool is_last_done(LB_Events const& events, edg_wll_DoneStatus_code status_code);
bool is_last_cancelled_by_wm(LB_Events const& events);
bool is_last_enqueued_by_wm(LB_Events const& events, edg_wll_EnQueuedResult result);
bool is_submitted(JobStatusPtr status);
bool is_waiting(JobStatusPtr status);
bool is_ready(JobStatusPtr status);
bool is_done(JobStatusPtr status);
bool is_cancelled(JobStatusPtr status);
bool is_aborted(JobStatusPtr status);
bool in_limbo(JobStatusPtr status, LB_Events const& events);

}}}}

#endif
