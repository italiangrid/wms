// File: jobdir_reader.cpp
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

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
