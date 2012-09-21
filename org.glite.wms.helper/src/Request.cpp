/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
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

// File: Request.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>

// $Id: Request.cpp,v 1.2.36.1.4.2 2010/04/08 13:52:15 mcecchi Exp $

#include "Request.h"
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <classad_distribution.h>
#include "RequestStateMachine.h"

namespace glite {
namespace wms {
namespace helper {

class Request::Impl: boost::noncopyable
{
  RequestStateMachine m_state_machine;

  classad::ClassAd const* m_original_ad;
  classad::ClassAd* m_current_ad;
  boost::shared_ptr<std::string> m_jw_template;

  void change_ad(classad::ClassAd* new_ad)
  {
    delete m_current_ad;
    m_current_ad = new_ad;
  }

public:
  explicit Impl(
    const classad::ClassAd* ad,
    boost::shared_ptr<std::string> jw_template
  );
  ~Impl();

  void resolve();
  bool is_resolved() const { return !m_state_machine; }

  classad::ClassAd const* original_ad() const { return m_original_ad; }
  classad::ClassAd const* current_ad() const { return m_current_ad; }
};

Request::Impl::Impl(
  const classad::ClassAd* ad,
  boost::shared_ptr<std::string> jw_template
) : m_state_machine(0),
    m_original_ad(ad),
    m_current_ad(new classad::ClassAd(*m_original_ad)),
    m_jw_template(jw_template)
{
  m_state_machine.start(m_original_ad);
}

Request::Impl::~Impl()
{
  delete m_current_ad;
}

void
Request::Impl::resolve()
try {
  assert(m_state_machine);

  std::auto_ptr<classad::ClassAd> resolved_ad(
    m_state_machine.next_step(m_current_ad, m_jw_template)
  );

  change_ad(resolved_ad.release());
} catch (helper::NoValidState const&) {
  throw std::invalid_argument("Invalid request");
}

Request::Request(
  const classad::ClassAd* ad,
  boost::shared_ptr<std::string> jw_template
)
try
  : m_impl(new Impl(ad, jw_template))
{
} catch (NoValidState const&) {
  throw std::invalid_argument("invalid request");
}

Request::~Request()
{
  delete m_impl;
}

void
Request::resolve()
{
  return m_impl->resolve();
}

bool
Request::is_resolved() const
{
  return m_impl->is_resolved();
}

classad::ClassAd const*
Request::original_ad() const
{
  return m_impl->original_ad();
}

classad::ClassAd const*
Request::current_ad() const
{
  return m_impl->current_ad();
}

} // namespace helper
} // namespace wms
} // namespace glite
