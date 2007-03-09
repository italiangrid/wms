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
