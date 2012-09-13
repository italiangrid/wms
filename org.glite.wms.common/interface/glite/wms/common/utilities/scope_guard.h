/*
Copyright (c) Members of the EGEE Collaboration. 2004. 
See http://www.eu-egee.org/partners/ for details on the copyright
holders.  

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

    http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
See the License for the specific language governing permissions and 
limitations under the License.
*/
// File: scope_guard.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// (adapted from a version taken from the boost mailing list)
// Copyright (c) 2003 EU DataGrid.

// $Id$

#ifndef GLITE_WMS_COMMON_UTILITIES_SCOPE_GUARD_H
#define GLITE_WMS_COMMON_UTILITIES_SCOPE_GUARD_H

#include <boost/function.hpp>
#include <boost/utility.hpp>

namespace glite {
namespace wms {
namespace common {
namespace utilities {

class scope_guard: private boost::noncopyable
{
  boost::function<void()> m_f;
  bool m_dismissed;

public:
  explicit scope_guard(boost::function<void()> const& v )
    : m_f(v), m_dismissed(false) 
  {
  }

  ~scope_guard() // throw()
  {
    if (!m_dismissed) {
      try {
        m_f();
      } catch (...) {
      }
    }
  }

  void dismiss() // throw() 
  {
    m_dismissed = true; 
  }

};

}}}} // glite::wms::common::utilities

#endif

// Local Variables:
// mode: c++
// End:
