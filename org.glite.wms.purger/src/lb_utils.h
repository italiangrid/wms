// File: lb_utils.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Author: Salvatore Monforte <Salvatore.Monforte@cnaf.infn.it>

#ifndef GLITE_WMS_PURGER_LB_UTILS_H
#define GLITE_WMS_PURGER_LB_UTILS_H

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/type_traits.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include "glite/lb/context.h"
#include "glite/lb/consumer.h"
#include "glite/wmsutils/jobid/cjobid.h"

namespace glite {

namespace wmsutils {
namespace jobid {
class JobId;
}}

namespace wms {
namespace purger {

typedef boost::shared_ptr<boost::remove_pointer<edg_wll_Context>::type> ContextPtr;

ContextPtr
create_context(
  std::string const& x509_proxy
);

ContextPtr
create_context(
  wmsutils::jobid::JobId const& id,
  std::string const& x509_proxy,
  std::string const& sequence_code
);

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
  ~CannotCreateLBContext() throw ()
  {
  }
  char const* what() const throw()
  {
    return m_what.c_str();
  }
  int error_code() const
  {
    return m_errcode;
  }
};

ContextPtr
create_context_proxy(
  wmsutils::jobid::JobId const& id,
  std::string const& x509_proxy,
  std::string const& sequence_code
);

class LB_Jobs
{
  boost::shared_array<edg_wlc_JobId> m_jobs;
  size_t m_size;

  static void free_jobs(edg_wlc_JobId* jobs)
  {
    if (jobs) {
      for (int i = 0; jobs[i]; ++i) {
        edg_wlc_JobIdFree(jobs[i]);
      }
      free(jobs);
    }
  }

public:
  typedef edg_wlc_JobId const* iterator;
  typedef iterator const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  LB_Jobs(edg_wlc_JobId* jobs)
    : m_jobs(jobs, &free_jobs), m_size(0)
  {
    if (m_jobs) {
      while (m_jobs[m_size]) {
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
  edg_wlc_JobId const& operator[](std::size_t i) const
  {
    assert(0 <= i && i < m_size);
    return m_jobs[i];
  }
  const_iterator begin() const
  {
    return m_jobs ? &m_jobs[0] : 0;
  }
  const_iterator end() const
  {
    return m_jobs ? &m_jobs[m_size] : 0;
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

class LB_States
{
  boost::shared_array<edg_wll_JobStat> m_states;
  size_t m_size;

  static void free_states(edg_wll_JobStat* states)
  {
    if (states) {
      for (int i = 0; states[i].jobId; ++i) {
        edg_wll_FreeStatus(&states[i]);
      }
      free(states);
    }
  }

public:
  typedef edg_wll_JobStat const* iterator;
  typedef iterator const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  LB_States(edg_wll_JobStat* states)
    : m_states(states, &free_states), m_size(0)
  {
    if (m_states) {
      while (m_states[m_size].jobId) {
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
  edg_wll_JobStat const& operator[](std::size_t i) const
  {
    assert(0 <= i && i < m_size);
    return m_states[i];
  }
  const_iterator begin() const
  {
    return m_states ? &m_states[0] : 0;
  }
  const_iterator end() const
  {
    return m_states ? &m_states[m_size] : 0;
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

struct removable_jobs
{
  LB_Jobs jobs;
  LB_States states;

  removable_jobs(  
    LB_Jobs j,
    LB_States s
  ) : jobs(j), states(s) {}
};

struct removable_jobs
get_removable_jobs(ContextPtr const& context, time_t threshold);

std::string get_original_jdl(edg_wll_Context context, wmsutils::jobid::JobId const& id);
std::string get_lb_message(ContextPtr const& context_ptr);
std::string get_lb_message(edg_wll_Context context);
void change_logging_job(ContextPtr, std::string const& sequence_code, wmsutils::jobid::JobId const&);

}}} // glite::wms::purger

#endif
