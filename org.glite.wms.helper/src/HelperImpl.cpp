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

// File: HelperImpl.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id$

#include "glite/wms/helper/HelperImpl.h"
#include <fstream>
#include <stdexcept>
#include <boost/scoped_ptr.hpp>
#include "glite/wmsutils/classads/classad_utils.h"

namespace utils = glite::wmsutils::classads;

namespace glite {
namespace wms {
namespace helper {

HelperImpl::HelperImpl()
{
}

HelperImpl::~HelperImpl()
{
}

std::string
HelperImpl::resolve(std::string const& input_file) const
{
  assert(!input_file.empty());

  std::string result = input_file + output_file_suffix(); // output file

  std::ifstream fin(input_file.c_str());
  assert(fin);

  std::ofstream fout(result.c_str());
  assert(fout);

  boost::scoped_ptr<classad::ClassAd> ad(utils::parse_classad(fin));
  boost::scoped_ptr<classad::ClassAd> resolved_ad(resolve(&*ad));

  if (resolved_ad.get() == 0) {
    throw std::logic_error(id() + " - cannot resolve");
  }

  fout << utils::unparse_classad(*resolved_ad) << '\n';

  return result;
}

}}} // glite::wms:helper
