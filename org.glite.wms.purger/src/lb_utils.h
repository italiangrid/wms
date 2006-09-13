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

class LB_Events
{
  boost::shared_array<edg_wll_Event> m_events;
  size_t m_size;

  void free_events(edg_wll_Event* events)
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
    : m_events(events), m_size(0)
  {
    while (m_events[m_size].type) ;
  }
  bool empty() const
  {
    return m_events.get() == 0;
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

std::string get_original_jdl(edg_wll_Context context, wmsutils::jobid::JobId const& id);
std::string get_lb_message(ContextPtr const& context_ptr);
std::string get_lb_message(edg_wll_Context context);

}}} // glite::wms::purger

#endif
