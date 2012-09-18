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
// File: jobdir_reader.cpp
// Author: Francesco Giacomini

// $Id$

#include "glite/wms/common/utilities/jobdir_reader.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/bind.hpp>
#include <classad_distribution.h>
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/common/utilities/scope_guard.h"
#include "glite/wms/common/utilities/jobdir.h"

namespace utilities = glite::wms::common::utilities;
namespace fs = boost::filesystem;

namespace glite {
namespace wms {
namespace common {
namespace utilities {

struct JobDirItem::Impl
{
  fs::path item_path;
};

JobDirItem::JobDirItem(fs::path const& p)
  : m_impl(new Impl)
{
  m_impl->item_path = p;
}

std::string
JobDirItem::value() const
{
  std::string result;
  result.reserve(fs::file_size(m_impl->item_path));
  fs::ifstream is(m_impl->item_path);
  getline(is, result, '\0');
  return result;
}

void
JobDirItem::remove_from_input()
{
  fs::remove(m_impl->item_path);
}

struct JobDirReader::Impl
{
  Impl(std::string const& source)
    : jd(fs::path(source, fs::native))
  {
  }
  JobDir jd;
};

JobDirReader::JobDirReader(std::string const& source)
  : m_impl(new Impl(source))
{
}

std::string JobDirReader::name() const
{
  return "jobdir";
}

std::string JobDirReader::source() const
{
  return m_impl->jd.base_dir().native_file_string();
}

JobDirReader::InputItems JobDirReader::read()
{
  InputItems result;

  std::pair<utilities::JobDir::iterator, utilities::JobDir::iterator> p(
    m_impl->jd.new_entries()
  );
  utilities::JobDir::iterator b = p.first;
  utilities::JobDir::iterator const e = p.second;

  for ( ; b != e; ++b) {
    fs::path const& new_file = *b;
    fs::path const old_file = m_impl->jd.set_old(new_file);
    InputItemPtr item(new JobDirItem(old_file));
    result.push_back(item);
  }

  return result;
}

}}}}
