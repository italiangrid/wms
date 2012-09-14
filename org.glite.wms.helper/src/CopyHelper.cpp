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

// File: CopyHelper.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id$

#include <fstream>
#include <classad_distribution.h>
#include "CopyHelper.h"
#include "glite/wms/helper/HelperFactory.h"

namespace glite {
namespace wms {
namespace helper {

namespace {

helper_type helper_id("CopyHelper");

HelperImpl* create_helper()
{
  return new CopyHelper;
}

struct Register
{
  Register()
  {
    HelperFactory::instance()->register_helper(helper_id, create_helper);
  }
  ~Register()
  {
    HelperFactory::instance()->unregister_helper(helper_id);
  }
};

Register r;

}

CopyHelper::CopyHelper()
{
}

CopyHelper::~CopyHelper()
{
}

helper_type
CopyHelper::type() const
{
  return helper_id;
}

filename_type
CopyHelper::resolve(filename_type const& input_file) const
{
  assert(input_file != "");

  filename_type result = input_file + ".cph"; // output file

  std::ifstream fin(input_file.c_str());
  if (! fin) {
    throw FileNotFound();
  }

  std::ofstream fout(result.c_str());
  if (! fout) {
    throw FileNotFound();
  }

  // read the input file

  std::string ad_str;

  char c;
  while (fin >> c) {
    //    ad_str.push_back(c);
    ad_str += c;		// BC
  }

  classad::ClassAdParser parser;

  classad::ClassAd* ad = parser.ParseClassAd(ad_str, true);

  if (ad == 0) {
    throw NotAClassAd();
  }

  // write the output file

  classad::ClassAdUnParser unparser;
  std::string unparsed;

  unparser.Unparse(unparsed, ad);

  fout << unparsed << '\n';

  delete ad;

  return result;
}

classad::ClassAd*
CopyHelper::resolve(classad::ClassAd const* input_ad) const
{
  assert(input_ad != 0);

  return new classad::ClassAd(*input_ad);
}

} // namespace helper
} // namespace wms
} // namespace glite
