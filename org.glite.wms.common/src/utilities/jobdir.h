// File: jobdir.h
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
 
#ifndef GLITE_WMS_COMMON_UTILITIES_JOBDIR_H
#define GLITE_WMS_COMMON_UTILITIES_JOBDIR_H

#include <string>
#include <stdexcept>
#include <vector>
#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_container_iterator.hpp>

namespace glite {
namespace wms {
namespace common {
namespace utilities {

class JobDirError: public std::runtime_error
{
public:
  JobDirError(std::string const& what);
};

class JobDir
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  JobDir(boost::filesystem::path const& base_dir);
  boost::filesystem::path const& base_dir() const;

  boost::filesystem::path deliver(
    std::string const& contents,
    std::string const& tag = std::string()
  );
  boost::filesystem::path set_old(boost::filesystem::path const& file);

  typedef boost::shared_container_iterator<
    std::vector<boost::filesystem::path>
  > iterator;
  std::pair<iterator, iterator> new_entries();
  std::pair<iterator, iterator> old_entries();

public:
  static bool create(boost::filesystem::path const& base_dir);
};

}}}} // glite::wms::common::utilities

#endif
