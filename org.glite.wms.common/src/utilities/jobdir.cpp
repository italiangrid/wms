// File: jobdir.cpp
// Author: Francesco Giacomini
// 
// Copyright (c) Members of the EGEE Collaboration. 2004. 
// See http://www.eu-egee.org/partners/ for details on the copyright
// holders.  
// 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
// 
//     http://www.apache.org/licenses/LICENSE-2.0 
// 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

// $Id$
 
#include "glite/wms/common/utilities/jobdir.h"
#include <cerrno>
#include <sstream>
#include <iomanip>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <time.h>

namespace fs = boost::filesystem;
namespace pt = boost::posix_time;

namespace glite {
namespace wms {
namespace common {
namespace utilities {

namespace {

pt::ptime universal_time()
{
  timeval tv;
  gettimeofday(&tv, 0);
  std::time_t t = tv.tv_sec;
  int fs = tv.tv_usec;
  std::tm curr;
  std::tm* curr_ptr = ::gmtime_r(&t, &curr);
  boost::gregorian::date d(
    curr_ptr->tm_year + 1900,
    curr_ptr->tm_mon + 1,
    curr_ptr->tm_mday
  );

  pt::time_duration td(
    curr_ptr->tm_hour,
    curr_ptr->tm_min,
    curr_ptr->tm_sec,
    fs
  );

  return pt::ptime(d,td);
}

bool link(fs::path const& old_path, fs::path const& new_path)
{
  std::string const old_path_str = old_path.native_file_string();
  std::string const new_path_str = new_path.native_file_string();

  int const e = ::link(old_path_str.c_str(), new_path_str.c_str());

  return e == 0;
}

}

JobDirError::JobDirError(std::string const& what)
  : std::runtime_error(what)
{
}

namespace {

std::string const tmp_tag("tmp");
std::string const new_tag("new");
std::string const old_tag("old");

}

struct JobDir::Impl
{
  Impl(fs::path const& p)
    : base_dir(p),
      tmp_dir(base_dir / tmp_tag),
      new_dir(base_dir / new_tag),
      old_dir(base_dir / old_tag),
      id_str(boost::lexical_cast<std::string>(pthread_self()))
  {}
  fs::path const base_dir;
  fs::path const tmp_dir;
  fs::path const new_dir;
  fs::path const old_dir;
  std::string const id_str;
};

JobDir::JobDir(fs::path const& base_dir)
  : m_impl(new Impl(base_dir))
{
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
  pt::ptime t = universal_time();
  std::string file_name(to_iso_string(t));
  assert(
    file_name.size() == 15
    || file_name.size() == (unsigned)16 + pt::time_duration::num_fractional_digits()
  );

  if (file_name.size() == 15) { // without fractional seconds
    file_name += '.';
    file_name += std::string(pt::time_duration::num_fractional_digits(), '0');
  }

  file_name += '_';
  file_name += m_impl->id_str;
  if (!tag.empty()) {
    file_name += '_';
    file_name += tag;
  }

  fs::path const file(file_name, fs::native);
  fs::path tmp_path;
  fs::path new_path;
  try {
    tmp_path = m_impl->tmp_dir / file;
    new_path = m_impl->new_dir / file;
  } catch (boost::filesystem::filesystem_error const& e) {
    throw JobDirError(e.what());
  }

  fs::ofstream tmp_file(tmp_path);
  if (!tmp_file) {
    std::string msg("create failed for ");
    msg += tmp_path.string();
    msg += " (" + boost::lexical_cast<std::string>(errno) + ')';
    throw JobDirError(msg);
  }

  tmp_file << contents << std::flush;
  if (!tmp_file) {
    std::string msg("write failed for ");
    msg += tmp_path.string();
    msg += " (" + boost::lexical_cast<std::string>(errno) + ')';
    throw JobDirError(msg);
  }

  tmp_file.close(); // sync too? yes, rename doesn't sync (TODO)

  bool e = std::rename(tmp_path.string().c_str(), new_path.string().c_str());
  if (e) {
    std::string msg("rename failed for ");
    msg += tmp_path.string();
    msg += " (" + boost::lexical_cast<std::string>(errno) + ')';
    throw JobDirError(msg);
  }

  return new_path;
}

fs::path JobDir::set_old(fs::path const& file)
{
  fs::path new_path;
  fs::path old_path;
  try {
    fs::path new_path = m_impl->new_dir / file.leaf();
    fs::path old_path = m_impl->old_dir / file.leaf();
  } catch (boost::filesystem::filesystem_error const& e) {
    throw JobDirError(e.what());
  }
  bool e = std::rename(new_path.string().c_str(), old_path.string().c_str());
  if (e) {
    std::string msg("rename failed for ");
    msg += new_path.string();
    msg += " (" + boost::lexical_cast<std::string>(errno) + ')';
    throw JobDirError(msg);
  }

  return old_path;
}

namespace {

inline bool leaf_less(fs::path const& lhs, fs::path const& rhs)
{
  return lhs.leaf() < rhs.leaf();
}

}

std::pair<JobDir::iterator, JobDir::iterator> JobDir::new_entries()
{
  fs::directory_iterator const b(m_impl->new_dir);
  fs::directory_iterator const e;
  typedef std::vector<fs::path> container;
  boost::shared_ptr<container> entries(new container(b, e));
  sort(entries->begin(), entries->end(), leaf_less);
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

bool JobDir::create(fs::path const& base_dir) try
{
  return
    create_directories(base_dir / fs::path(tmp_tag, fs::native))
    && create_directories(base_dir / fs::path(new_tag, fs::native))
    && create_directories(base_dir / fs::path(old_tag, fs::native));
} catch (boost::filesystem::filesystem_error const& e) {
  throw JobDirError(e.what());
}

}}}} // glite::wms::common::utilities

