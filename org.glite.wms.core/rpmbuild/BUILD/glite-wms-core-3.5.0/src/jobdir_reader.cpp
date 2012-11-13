/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the
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

// File: jobdir_reader.cpp
// Author: Francesco Giacomini

// $Id: jobdir_reader.cpp,v 1.1.2.1.4.2 2010/04/07 14:02:46 mcecchi Exp $

#include "jobdir_reader.h"
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
namespace manager {
namespace server {

struct JobDirReader::Impl
{
  Impl(std::string const& source)
    : jd(fs::path(source, fs::native))
  {
  }
  utilities::JobDir jd;
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

JobDirReader::requests_type JobDirReader::read()
{
  requests_type result;

  std::pair<utilities::JobDir::iterator, utilities::JobDir::iterator> p(
    m_impl->jd.new_entries()
  );
  utilities::JobDir::iterator b = p.first;
  utilities::JobDir::iterator const e = p.second;

  classad::ClassAdParser parser;

  for ( ; b != e; ++b) {

    fs::path const& new_file = *b;
    fs::path const old_file = m_impl->jd.set_old(new_file);

    cleanup_type cleanup(boost::bind(fs::remove, old_file));
    // if the request is not valid, clean it up automatically
    utilities::scope_guard cleanup_guard(cleanup);

    fs::ifstream is(old_file);
    ClassAdPtr command_ad(parser.ParseClassAd(is));
    if (command_ad) {
      cleanup_guard.dismiss();
      result.push_back(std::make_pair(command_ad, cleanup));
    } else {
      Info("invalid request");
    }

  }

  return result;
}

}}}} // glite::wms::manager::server
