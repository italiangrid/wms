// File: TaskQueue.cpp
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#include "TaskQueue.hpp"

namespace glite {
namespace wms {
namespace manager {
namespace server {

TaskQueue& the_task_queue()
{
  static TaskQueue tq;

  return tq;
}

ContextPtr
get_context(glite::wmsutils::jobid::JobId const& id)
{
  TaskQueue& tq = the_task_queue();
  TaskQueue::iterator it = tq.find(id.toString());
  if (it != tq.end()) {
    return it->second->lb_context();
  } else {
    return ContextPtr();
  }
}

}}}}
