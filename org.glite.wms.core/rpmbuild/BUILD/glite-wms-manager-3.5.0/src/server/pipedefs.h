// File: pipedefs.h
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#ifndef GLITE_WMS_MANAGER_SERVER_PIPEDEFS_H
#define GLITE_WMS_MANAGER_SERVER_PIPEDEFS_H

#include <queue>
#include "glite/wms/common/task/Task.h"
#include "TaskQueue.hpp"

namespace glite {
namespace wms {
namespace manager {
namespace server {

inline bool operator<(RequestPtr const& lhs, RequestPtr const& rhs)
{
  if (!lhs->marked_match() && rhs->marked_match()) {
    return true;
  } else if (lhs->marked_match() && rhs->marked_match()) {
    bool lhs_has_include_brokerinfo = lhs->match_parameters().get<2>();
    bool rhs_has_include_brokerinfo = rhs->match_parameters().get<2>();
    if (lhs_has_include_brokerinfo && !rhs_has_include_brokerinfo) {
      // lhs comes from the dag planner && rhs is interactive
      return true;
    }
  }

  return lhs->last_processed() > rhs->last_processed();
}

template<typename Q> class queue_adaptor;

template<typename T>
class queue_adaptor<std::priority_queue<T> >
{
  std::priority_queue<T> m_q;
public:
  queue_adaptor() {}
  T front() const
  {
    return m_q.top();
  }
  void push(T const& t)
  {
    m_q.push(t);
  }
  bool empty() const
  {
    return m_q.empty();
  }
  typename std::priority_queue<T>::size_type size() const
  {
    return m_q.size();
  }
  void pop()
  {
    m_q.pop();
  }
};

template<typename T>
struct adapted_priority_queue
{
  typedef queue_adaptor<std::priority_queue<T> > type;
};

typedef RequestPtr pipe_value;
typedef adapted_priority_queue<RequestPtr>::type queue_type;
typedef glite::wms::common::task::Pipe<RequestPtr, queue_type> pipe_type;

}}}}

#endif
