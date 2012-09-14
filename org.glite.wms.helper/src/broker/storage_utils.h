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

// File: storage_utils.h
// Author: Salvatore Monforte
// Author: Francesco Giacomini

// $Id$

#ifndef GLITE_WMS_BROKERINFO_STORAGE_UTILS_H
#define GLITE_WMS_BROKERINFO_STORAGE_UTILS_H

#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <set>
#include <numeric>
#include <algorithm>

namespace classad
{
  class ClassAd;
}

namespace glite {
namespace wms {
namespace brokerinfo {

boost::shared_ptr<FileMapping>
resolve_filemapping_info(classad::ClassAd const& ad);

boost::shared_ptr<StorageMapping>
resolve_storagemapping_info(FileMapping const& fm);

std::vector<StorageMapping::const_iterator>
select_compatible_storage(
  StorageMapping const& sm,
  std::vector<std::string> const& dap // data access protocol
);

size_t
count_unique_logical_files(
  std::vector<StorageMapping::const_iterator>::const_iterator first,
  std::vector<StorageMapping::const_iterator>::const_iterator last
);

struct extract_computing_elements_id
{
  std::set<std::string>*
  operator()(
    std::set<std::string>* s,
    StorageMapping::const_iterator si
  ) const;
};

class is_storage_close_to
{
  std::string m_ceid;
public:
  is_storage_close_to(std::string const& id)
    : m_ceid(id)
  {
  }
  bool operator()(StorageMapping::const_iterator const& v) const;
};

}}}

#endif
