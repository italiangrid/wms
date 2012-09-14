/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// File: events.cpp
// Francesco Giacomini
// Marco Cecchi

#include <map>
#include <queue>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "wm_real.h"
#include "events.h"
#include "dispatcher_utils.h"
#include "signal_handling.h"

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace utilities = glite::wms::common::utilities;

struct TimePriority
{
  TimePriority(std::time_t t, int p)
    : time(t), priority(p)
  { }
  std::time_t time;
  int priority;
};

struct time_first
{
  bool operator()(TimePriority const& lhs, TimePriority const& rhs) {
    return lhs.time <= rhs.time;
  }
};

struct priority_first
{
  bool operator()(TimePriority const& lhs, TimePriority const& rhs)
  {
    return lhs.priority < rhs.priority
      || lhs.priority == rhs.priority && lhs.time < rhs.time;
  }
};

typedef std::multimap<
  TimePriority,
  boost::function<void()>,
  priority_first
> ready_queue_type;

typedef std::multimap<
  TimePriority,
  boost::function<void()>,
  time_first
> waiting_queue_type;

namespace {

// turn all the expired waiting events into ready
void
expired_to_ready(
  std::time_t now,
  waiting_queue_type& waiting,
  ready_queue_type& ready
) {
  TimePriority const comp(now, 0 /* the priority is irrelevant */ );
  waiting_queue_type::iterator last_expired(waiting.lower_bound(comp));
  ready.insert(waiting.begin(), last_expired);
  waiting.erase(waiting.begin(), last_expired);
}
}

struct Events::Impl
{
  Impl()
  : shutdown(false), ready(priority_first()), waiting(time_first())
  {
  }

  boost::mutex mx;
  boost::condition cond;
  bool shutdown;

  ready_queue_type ready;
  waiting_queue_type waiting;
};

Events::Events()
  : m_impl(new Impl)
{
}

int
Events::ready_size()
{
  return m_impl->ready.size();
}

int
Events::waiting_size()
{
  return m_impl->waiting.size();
}

void
Events::schedule(boost::function<void()> f, int priority)
{
  std::time_t now(std::time(0));
  TimePriority const tp(now, priority);
  std::pair<TimePriority, boost::function<void()> > const v(
    std::make_pair(tp, f)
  );

  boost::mutex::scoped_lock l(m_impl->mx);
  m_impl->ready.insert(v);
  l.unlock();

  m_impl->cond.notify_one();
}

void
Events::schedule_at(boost::function<void()> f, std::time_t t, int priority)
{
  TimePriority const tp(t, priority);
  std::pair<TimePriority, boost::function<void()> > const v(
    std::make_pair(tp, f)
  );

  Debug(
    "timed event scheduled at " << boost::lexical_cast<std::string>(t)
    << " with priority " << boost::lexical_cast<std::string>(priority)
  );
  boost::mutex::scoped_lock l(m_impl->mx);
  m_impl->waiting.insert(v);
  l.unlock();

  m_impl->cond.notify_one();  
}

void Events::run()
{
  while (!m_impl->shutdown) {

    boost::mutex::scoped_lock l(m_impl->mx);

    while (!m_impl->shutdown
           && m_impl->ready.empty()
           && m_impl->waiting.empty()) {
      m_impl->cond.wait(l);
    }

    if (m_impl->shutdown) {
      break;
    }

    std::time_t now(std::time(0));
    expired_to_ready(now, m_impl->waiting, m_impl->ready);

    while (!m_impl->shutdown && m_impl->ready.empty()) {
      boost::xtime wait_time;
      wait_time.sec = m_impl->waiting.begin()->first.time;
      wait_time.nsec = 0;
      m_impl->cond.timed_wait(l, wait_time);
      now = std::time(0);
      expired_to_ready(now, m_impl->waiting, m_impl->ready);
    }

    if (m_impl->shutdown) {
      break;
    }

    ready_queue_type::iterator it(m_impl->ready.begin());
    boost::function<void()> f = it->second;
    m_impl->ready.erase(it);

    l.unlock();

    if (!f.empty()) {
      try {
        f();
      } catch (std::bad_alloc const& e) {
        stop();
        Error("bad alloc exception (" << e.what() << ") caught");
      } catch (std::exception const& e) {
        Error(e.what());
      } catch (...) {
      }
    }
  }

  Info("Worker thread: exiting");
}

void
Events::stop()
{
  boost::mutex::scoped_lock l(m_impl->mx);
  m_impl->shutdown = true;
  l.unlock();
  m_impl->cond.notify_all();
}

}}}} // glite::wms::manager::server
