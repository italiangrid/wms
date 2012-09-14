

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

// File: exceptions.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2003 EU DataGrid.

// $Id$

#include "exceptions.h"
#include <string>

namespace fs = boost::filesystem;

namespace glite {
namespace wms {
namespace helper {
namespace broker {

namespace {
std::string const helper_id("BrokerHelper");
std::string const empty_string;
fs::path const empty_path;
}

class CannotCreateBrokerinfo::Impl
{
public:
  std::string m_what;
  fs::path m_path;
};

CannotCreateBrokerinfo::CannotCreateBrokerinfo(fs::path const& p)
try
  : helper::HelperError(helper_id)
{
  m_impl.reset(new Impl);
  m_impl->m_path = p;
} catch (...) {
  m_impl.reset();
}

CannotCreateBrokerinfo::~CannotCreateBrokerinfo() throw()
{
}

fs::path
CannotCreateBrokerinfo::path() const
{
  return m_impl ? m_impl->m_path : empty_path;
}

char const*
CannotCreateBrokerinfo::what() const throw()
{
  if (m_impl) {
    if (m_impl->m_what.empty()) {
      m_impl->m_what = helper() + ": cannot create the brokerinfo file at " + m_impl->m_path.native_file_string();
    }
    return m_impl->m_what.c_str();
  } else {
    return "BrokerHelper: CannotCreateBrokerinfo";
  }
}

class NoAvailableCEs::Impl
{
public:
  std::string m_what;
  std::string m_reason;
};

NoAvailableCEs::NoAvailableCEs(std::string const& reason)
try
  : helper::HelperError(helper_id)
{
  m_impl.reset(new Impl);
  m_impl->m_reason = reason;
} catch (...) {
  m_impl.reset();
}

NoAvailableCEs::~NoAvailableCEs() throw()
{
}

std::string
NoAvailableCEs::reason() const
{
  return m_impl ? m_impl->m_reason : empty_string;
}

char const*
NoAvailableCEs::what() const throw()
{
  if (m_impl) {
    if (m_impl->m_what.empty()) {
      m_impl->m_what = helper() + ": " + reason();
    }
    return m_impl->m_what.c_str();
  } else {
    return "BrokerHelper: NoAvailableCEs";
  }
}

class NoCompatibleCEs::Impl
{
public:
  std::string m_what;
  std::string m_is_hostname;
  NoCompatibleCEs::reason_type m_reason;
};

NoCompatibleCEs::NoCompatibleCEs()
try
  : helper::HelperError(helper_id)
{
  m_impl.reset(new Impl);
  m_impl->m_reason = DONTKNOW;
} catch (...) {
  m_impl.reset();
}

NoCompatibleCEs::NoCompatibleCEs(std::string const& is_hostname, reason_type reason)
try
  : helper::HelperError(helper_id)
{
  m_impl.reset(new Impl);
  m_impl->m_is_hostname = is_hostname;
  m_impl->m_reason = reason;
} catch (...) {
  m_impl.reset();
}

NoCompatibleCEs::~NoCompatibleCEs() throw()
{
}

std::string
NoCompatibleCEs::is_hostname() const
{
  return m_impl ? m_impl->m_is_hostname : empty_string;
}

NoCompatibleCEs::reason_type
NoCompatibleCEs::reason() const
{
  return m_impl ? m_impl->m_reason : DONTKNOW;
}

char const*
NoCompatibleCEs::what() const throw()
{
  if (m_impl) {
    std::string& w = m_impl->m_what;
    std::string& h = m_impl->m_is_hostname;
    if (w.empty()) {
      w = helper() + ": ";
      switch (m_impl->m_reason) {
      case FALSE_REQS:
        w += "no resource currently registered in the information service"
          + h.empty() ? " " : (" (" + h + ") ")
          + "matches the requirements";
        break;
      case INVALID_REQS:
      case ERROR_REQS:
        w += "problems evaluating the 'requirements' expression "
             "(probably something wrong in the JDL expression)";
        break;
      case DONTKNOW:
        w += "no compatible resources";
      }
    }
    return w.c_str();
  } else {
    return "BrokerHelper: NoCompatibleCEs";
  }
}

}}}} // glite::wms::helper::broker
