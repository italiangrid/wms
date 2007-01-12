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
 
#include "jobdir.h"
#include <sstream>
#include <iomanip>
#include <unistd.h>             // getpid()
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/mutex.hpp>

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
      last_time(::time(0)),
      counter(0),
      pid_str(boost::lexical_cast<std::string>(getpid()))
  {}
  fs::path const base_dir;
  fs::path const tmp_dir;
  fs::path const new_dir;
  fs::path const old_dir;
  ::time_t last_time;
  int counter;
  boost::mutex::mutex mx;  // to protect last_time and counter
  std::string const pid_str;
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
  ::time_t t = ::time(0);
  std::string file_name(boost::lexical_cast<std::string>(t));
  file_name += '_';
  boost::mutex::scoped_lock l(m_impl->mx);
  if (t > m_impl->last_time) {
    m_impl->last_time = t;
    m_impl->counter = 0;
  } else {
    ++m_impl->counter;
  }
  int counter = m_impl->counter;
  l.unlock();
  std::ostringstream os;
  os << std::setw(6) << std::setfill('0') << counter;
  file_name += os.str();
  file_name += '_';
  file_name += m_impl->pid_str;
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

bool JobDir::create(fs::path const& base_dir)
{
  return
    create_directories(base_dir / fs::path(tmp_tag, fs::native))
    && create_directories(base_dir / fs::path(new_tag, fs::native))
    && create_directories(base_dir / fs::path(old_tag, fs::native));
}

}}}} // glite::wms::common::utilities

