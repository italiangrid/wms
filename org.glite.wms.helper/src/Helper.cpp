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

// File: Helper.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id: Helper.cpp,v 1.3.28.2.8.1.4.2 2010/04/08 13:52:15 mcecchi Exp $

#include "Helper.h"
#include "glite/wms/helper/HelperImpl.h"
#include "glite/wms/helper/HelperFactory.h"
#include "glite/wms/helper/exceptions.h"

namespace glite {
namespace wms {
namespace helper {

Helper::Helper(std::string const& id)
{
  try {
    m_impl = HelperFactory::instance()->create_helper(id);
  } catch (...) {
    throw NoSuchHelper(id);
  }

  if (!m_impl) {
    throw NoSuchHelper(id);
  }
}

Helper::~Helper()
{
  delete m_impl;
}

std::string
Helper::id() const
{
  return m_impl->id();
}

std::string
Helper::resolve(std::string const& input_file) const
{
  return m_impl->resolve(input_file);
};

classad::ClassAd*
Helper::resolve(
  classad::ClassAd const* input_ad,
  boost::shared_ptr<std::string> jw_template
) const
{
  return m_impl->resolve(input_ad, jw_template);
}

}}} // glite::wms::helper
