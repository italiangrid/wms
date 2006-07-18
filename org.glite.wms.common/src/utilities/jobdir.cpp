// File: jobdir.cpp
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$
 
#include "jobdir.h"
#include <sstream>
#include <iomanip>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/lexical_cast.hpp>

namespace fs = boost::filesystem;

namespace glite {
namespace wms {
namespace common {
namespace utilities {

namespace {

bool link(fs::path const& old_path, fs::path const& new_path)
{
  std::string const old_path_str = old_path.native_file_string();
  std::string const new_path_str = new_path.native_file_string();

  return ::link(old_path_str.c_str(), new_path_str.c_str()) == 0;
}

}

JobDirError::JobDirError(std::string const& what)
  : std::runtime_error(what)
{
}

struct JobDir::Impl
{
  fs::path base_dir;
  fs::path tmp_dir;
  fs::path new_dir;
  fs::path old_dir;
  ::time_t last_time;
  int counter;
};

namespace {

std::string const tmp_tag("tmp");
std::string const new_tag("new");
std::string const old_tag("old");

}

JobDir::JobDir(fs::path const& base_dir)
  : m_impl(new Impl)
{
  m_impl->base_dir = base_dir;
  m_impl->tmp_dir = base_dir / tmp_tag;
  m_impl->new_dir = base_dir / new_tag;
  m_impl->old_dir = base_dir / old_tag;
  m_impl->last_time = ::time(0);
  m_impl->counter = 0;

  if (!
      (fs::exists(m_impl->base_dir) && fs::is_directory(m_impl->base_dir)
       && fs::exists(m_impl->tmp_dir) && fs::is_directory(m_impl->tmp_dir)
       && fs::exists(m_impl->new_dir) && fs::is_directory(m_impl->new_dir)
       && fs::exists(m_impl->old_dir) && fs::is_directory(m_impl->old_dir)
      )
     ) {
    throw JobDirError("invalid directory hierarchy");
  }
}

fs::path const& JobDir::base_dir() const
{
  return m_impl->base_dir;
}

fs::path JobDir::deliver(
  std::string const& contents,
  std::string const& tag
)
{
  ::time_t t = ::time(0);
  std::string file_name(boost::lexical_cast<std::string>(t));
  file_name += '_';
  if (t > m_impl->last_time) {
    m_impl->last_time = t;
    m_impl->counter = 0;
  } else {
    ++m_impl->counter;
  }
  std::ostringstream os;
  os << std::setw(6) << std::setfill('0') << m_impl->counter;
  file_name += os.str();
  if (!tag.empty()) {
    file_name += '_';
    file_name += tag;
  }
  fs::path const file(file_name, fs::native);
  fs::path const tmp_path(m_impl->tmp_dir / file);
  fs::path const new_path(m_impl->new_dir / file);

  fs::ofstream tmp_file(tmp_path);
  if (!tmp_file) {
    throw JobDirError("cannot create the tmp file");
  }

  tmp_file << contents << std::flush;
  if (!tmp_file) {
    throw JobDirError("cannot write the tmp file");
  }

  tmp_file.close(); // synch too?

  bool link_result = link(tmp_path, new_path);
  if (!link_result) {
    throw JobDirError("cannot link tmp and new files");
  }

  fs::remove(tmp_path);

  return new_path;
}

fs::path JobDir::set_old(fs::path const& file)
{
  fs::path const new_path(m_impl->new_dir / file.leaf());
  fs::path const old_path(m_impl->old_dir / file.leaf());

  bool link_result = link(new_path, old_path);
  if (!link_result) {
    throw JobDirError("cannot link new and old files");
  }

  fs::remove(new_path);

  return old_path;
}

std::pair<JobDir::iterator, JobDir::iterator> JobDir::new_entries()
{
  fs::directory_iterator const b(m_impl->new_dir);
  fs::directory_iterator const e;
  typedef std::vector<fs::path> container;
  boost::shared_ptr<container> entries(new container(b, e));
  sort(entries->begin(), entries->end());
  return boost::make_shared_container_range(entries);
}

std::pair<JobDir::iterator, JobDir::iterator> JobDir::old_entries()
{
  fs::directory_iterator const b(m_impl->old_dir);
  fs::directory_iterator const e;
  typedef std::vector<fs::path> container;
  boost::shared_ptr<container> entries(new container(b, e));
  sort(entries->begin(), entries->end());
  return boost::make_shared_container_range(entries);
}

bool JobDir::create(fs::path const& base_dir)
{
  return
    create_directories(base_dir / fs::path(tmp_tag, fs::native))
    && create_directories(base_dir / fs::path(new_tag, fs::native))
    && create_directories(base_dir / fs::path(old_tag, fs::native));
}

}}}} // glite::wms::common::utilities

