// File: storage_utils.h
// Author: Salvatore Monforte
// Author: Francesco Giacomini

// $Id$

#ifndef GLITE_WMS_BROKER_STORAGE_UTILS_H
#define GLITE_WMS_BROKER_STORAGE_UTILS_H

#include "glite/wms/broker/match.h"
#include "glite/wms/rls/catalog_access_utils.h"
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
namespace broker {

FileMapping resolve_filemapping_info(classad::ClassAd const& ad);

StorageMapping resolve_storagemapping_info(FileMapping const& fm);

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
