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

// $Id: exceptions.cpp,v 1.1.40.2 2010/04/08 13:52:15 mcecchi Exp $

#include "exceptions.h"

namespace glite {
namespace wms {
namespace helper {
namespace jobadapter {

class CannotCreateJobWrapper::Impl
{
public:
  std::string m_what;
  std::string m_path;
};

CannotCreateJobWrapper::CannotCreateJobWrapper(std::string const& path)
try
  : HelperError("JobAdapterHelper")
{
  m_impl.reset(new Impl);
  m_impl->m_path = path;
} catch (...) {
  m_impl.reset();
}

CannotCreateJobWrapper::~CannotCreateJobWrapper() throw()
{
}

std::string
CannotCreateJobWrapper::path() const
{
  return m_impl ? m_impl->m_path : "";
}

char const*
CannotCreateJobWrapper::what() const throw()
{
  if (m_impl) {
    if (m_impl->m_what.empty()) {
      m_impl->m_what = helper() + ": cannot create job wrapper at " + path();
    }
    return m_impl->m_what.c_str();
  } else {
    return "JobAdapter error: CannotCreateJobWrapper";
  }
}

} // namespace jobadapter
} // namespace helper
} // namespace wms
} // namespace glite

