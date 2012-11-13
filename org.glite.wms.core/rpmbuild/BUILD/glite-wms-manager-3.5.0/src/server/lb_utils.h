// File: lb_utils.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

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

namespace wmsutils {
namespace jobid {
class JobId;
}}

namespace wms {
namespace manager {
namespace server {

typedef boost::shared_ptr<
  boost::remove_pointer<edg_wll_Context>::type
> ContextPtr;

ContextPtr
create_context(
  wmsutils::jobid::JobId const& id,
  std::string const& x509_proxy,
  std::string const& sequence_code,
  edg_wll_Source = EDG_WLL_SOURCE_WORKLOAD_MANAGER
);

std::string get_user_x509_proxy(wmsutils::jobid::JobId const& jobid);
std::string get_host_x509_proxy();
std::string get_lb_sequence_code(ContextPtr context_ptr);

void log_dequeued(ContextPtr context, std::string const& from);
void log_cancel_req(ContextPtr context);
void log_cancelled(ContextPtr context);
void log_pending(ContextPtr context, std::string const& reason);
void log_abort(ContextPtr context, std::string const& reason);
void log_resubmission_shallow(
  ContextPtr context,
  std::string const& token_file
);
void log_resubmission_deep(ContextPtr context);
void log_resubmission_deep(ContextPtr context, std::string const& token_file);
void log_match(ContextPtr context, std::string const& ce_id);
void log_enqueued_start(
  ContextPtr context,
  std::string const& to
);
void log_enqueued_ok(
  ContextPtr context,
  std::string const& to,
  std::string const& ad
);
void log_enqueued_fail(
  ContextPtr context,
  std::string const& to,
  std::string const& ad,
  std::string const& reason
);
void log_helper_called(
  ContextPtr context,
  std::string const& name
);
void log_helper_return(
  ContextPtr context,
  std::string const& name,
  int status
);

bool flush_lb_events(ContextPtr context);

class LB_Events
{
  boost::shared_array<edg_wll_Event> m_events;
  size_t m_size;

public:
  typedef edg_wll_Event const* iterator;
  typedef iterator const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  LB_Events(edg_wll_Event* events);
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

LB_Events
get_interesting_events(
  ContextPtr context,
  wmsutils::jobid::JobId const& id
);

std::vector<std::pair<std::string, int> >
get_previous_matches(LB_Events const& events);

// return (deep_retry_count, shallow_retry_count)
boost::tuple<int, int>
get_retry_counts(LB_Events const& events);

std::string
get_original_jdl(ContextPtr context, wmsutils::jobid::JobId const& id);

typedef boost::shared_ptr<edg_wll_JobStat> JobStatusPtr;
JobStatusPtr job_status(wmsutils::jobid::JobId const& id, ContextPtr context);
JobStatusPtr job_status(wmsutils::jobid::JobId const& id);
std::string status_to_string(JobStatusPtr status);
bool is_waiting(JobStatusPtr status);
bool is_cancelled(JobStatusPtr status);

}}}}

#endif
