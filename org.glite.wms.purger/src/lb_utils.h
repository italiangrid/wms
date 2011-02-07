// File: lb_utils.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Author: Salvatore Monforte <Salvatore.Monforte@cnaf.infn.it>

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

namespace jobid {
class JobId;
}

namespace wms {
namespace purger {

typedef boost::shared_ptr<boost::remove_pointer<edg_wll_Context>::type> ContextPtr;

ContextPtr
create_context(
  jobid::JobId const& id,
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
  jobid::JobId const& id,
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

void change_logging_job(
  ContextPtr context,
  jobid::JobId const& id,
  bool have_lbproxy
);
std::string get_original_jdl(edg_wll_Context context, jobid::JobId const& id);
std::string get_lb_message(ContextPtr const& context_ptr);
std::string get_lb_message(edg_wll_Context context);

}}} // glite::wms::purger

#endif
