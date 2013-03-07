// File: storage_utils.cpp
// Author: Salvatore Monforte
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

// $Id: storage_utils.cpp,v 1.1.2.4.2.2.2.1 2011/09/21 08:24:59 mcecchi Exp $

#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/regex.hpp>

#include <classad_distribution.h>

#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/ism/ism.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "brokerinfo.h"
#include "storage_utils.h"

namespace classad_utils = glite::wmsutils::classads;

namespace glite {
namespace wms {
namespace brokerinfo {

namespace dli {
boost::shared_ptr<FileMapping>
resolve_filemapping_info(classad::ClassAd const&);
}

namespace {

bool evaluate(
  classad::ClassAd const& ad,
  std::string const& name,
  std::vector<classad::ExprTree*>& v
) {
  bool result = false;
  classad::Value value;
  const classad::ExprList *l;
  if (ad.EvaluateAttr(name, value) &&
    value.IsListValue(l)) {
      l->GetComponents(v);
      result = true;
  }
  return result;
}

std::string
resolve_storage_name(std::string const& sfn)
{
  static boost::regex e("^\\s*([^:]*):[\\s/]*([^\\s:/]+)(:[0-9]+)?/.*");
  boost::smatch p;
  std::string name;

  if(boost::regex_match(sfn, p, e)) {
    name.assign(p[2].first, p[2].second);
  }
  else if (int p=sfn.find("://",0) != std::string::npos) {
      name.assign(sfn.substr(p+3));
  }
  return name;
}

std::pair<std::string, int>
make_storage_info(classad::ClassAd const&ad)
{
  std::string protocol;
  int port;
  ad.EvaluateAttrString("GlueSEAccessProtocolType", protocol);
  ad.EvaluateAttrInt("GlueSEAccessProtocolPort", port);
  return std::make_pair(protocol, port);
}

void
insert_protocols(StorageInfo::Protocols& protos, classad::ClassAd const& se_ad)
{
  std::vector<classad::ExprTree*> ads;
  if (evaluate(se_ad, "GlueSEAccessProtocol", ads)) {

    std::vector<classad::ExprTree*>::const_iterator expr_it(ads.begin());
    std::vector<classad::ExprTree*>::const_iterator const expr_e(ads.end());

    for (; expr_it != expr_e; ++expr_it) {
      if (classad_utils::is_classad(*expr_it)) {
        classad::ClassAd const& ad(
          *static_cast<classad::ClassAd*>(*expr_it)
        );
        protos.insert(make_storage_info(ad));
      }
    }
  }
}

std::pair<std::string, std::string>
make_ce_bind_info(classad::ClassAd const& ad)
{
  std::string id;
  std::string mount;
  ad.EvaluateAttrString("GlueCESEBindCEUniqueID", id);
  ad.EvaluateAttrString("GlueCESEBindCEAccesspoint", mount);
  return std::make_pair(id, mount);
}

void
insert_ce_mount_points(
  StorageInfo::CE_Mounts& mounts,
  classad::ClassAd const& se_ad
)
{
  std::vector<classad::ExprTree*> ads;
  if (evaluate(se_ad, "CloseComputingElements", ads)) {

    std::vector<classad::ExprTree*>::const_iterator expr_it(ads.begin());
    std::vector<classad::ExprTree*>::const_iterator const expr_e(ads.end());

    for (; expr_it != expr_e; ++expr_it) {
      if (classad_utils::is_classad(*expr_it)) {
        classad::ClassAd const& ad(
          *static_cast<classad::ClassAd*>(*expr_it)
        );
        mounts.push_back(make_ce_bind_info(ad));
      }
    }
  }
}

class matches_any_protocol_in
{
  std::set<std::pair<std::string,int> > const& m_protocols;

public:
  matches_any_protocol_in(
    std::set<std::pair<std::string,int> > const& protocols
  )
    : m_protocols(protocols)
  {
  }

  bool operator()(std::string const& p) const
  {
    std::set<std::pair<std::string,int> >::const_iterator it(
      m_protocols.begin()
    );
    std::set<std::pair<std::string,int> >::const_iterator const e(
      m_protocols.end()
    );
    for ( ; it != e; ++it ) {
      if (it->first == p) {
        return true;
      }
    }
    return false;
  }
};

bool is_mount_defined(std::pair<std::string, std::string> const& p)
{
    return !p.second.empty();
}

struct extract_mapped_file_names
{
  std::set<std::string>*
  operator()(std::set<std::string>* f, StorageMapping::const_iterator si) const
  {
    StorageInfo::Links::const_iterator it(si->second.links.begin());
    StorageInfo::Links::const_iterator const e(si->second.links.end());
    for( ; it != e ; ++it ) {
       f->insert((*it)->first);
    }
    return f;
  }
};

} // anonymous namespace

boost::shared_ptr<StorageMapping>
resolve_storagemapping_info(FileMapping const& fm)
{
  boost::shared_ptr<StorageMapping> result(
    new StorageMapping
  );
  
  FileMapping::const_iterator file_it = fm.begin();
  FileMapping::const_iterator const file_end = fm.end();

  std::pair<boost::shared_ptr<void>, int> ret = ism::match_on_active_side();
  // ++matching threads on the currently active ISM side, as ret.first increases
  // by one the reference counting on that ISM side;
  // this will not prevent the ISM from
  // switching over with pending matches
  // insisting onto the target slice, but before
  // starting yet another purchasing (in background),
  // no pending matching_threads must exist
  ism::ism_type const& the_ism = ism::get_ism(ism::se, ret.second /* side */);

  for ( ; file_it != file_end; ++file_it) {
    std::vector<std::string> const& sfns(file_it->second);
    std::vector<std::string>::const_iterator sfni = sfns.begin();
    std::vector<std::string>::const_iterator const sfne = sfns.end();

    for( ; sfni != sfne; ++sfni ) {
       std::string const name = resolve_storage_name(*sfni);
       if (name.empty()) {
         continue;
       }
       ism::ism_type::const_iterator it(
         the_ism.find(name)
       );
       
       if (it != the_ism.end()) {

         boost::shared_ptr<classad::ClassAd> se_ad( 
            boost::tuples::get<ism::ad_ptr_entry>(it->second)
         );

         StorageInfo& info = (*result)[name];
         info.links.push_back(file_it);
         insert_protocols(info.protocols, *se_ad);
         insert_ce_mount_points(info.ce_mounts, *se_ad);

       }
    }
  }

  return result;
}

std::vector<StorageMapping::const_iterator>
select_compatible_storage(
  StorageMapping const& sm,
  std::vector<std::string>const& dap
)
{
  std::vector<StorageMapping::const_iterator> result;
  StorageMapping::const_iterator storage_it(sm.begin());
  StorageMapping::const_iterator const storage_end(sm.end());

  for( ; storage_it != storage_end; ++storage_it ) {

    StorageInfo::Protocols const& supported_protocols(
      storage_it->second.protocols
    );
    std::vector<std::string>::const_iterator it = find_if(
      dap.begin(),
      dap.end(),
      matches_any_protocol_in(supported_protocols)
    );
    if (it != dap.end()) {
      StorageInfo::CE_Mounts const& close_ce_binds(
        storage_it->second.ce_mounts
      );
      if (*it == "file"
          && find_if(
            close_ce_binds.begin(),
            close_ce_binds.end(),
            is_mount_defined
          ) == close_ce_binds.end()) {
        continue;
      }
      result.push_back(storage_it);
    }
  }
  return result;
}

size_t
count_unique_logical_files(
  std::vector<StorageMapping::const_iterator>::const_iterator first,
  std::vector<StorageMapping::const_iterator>::const_iterator last
)
{
  std::set<std::string> unique_logical_file_names;
  std::accumulate(
    first,
    last,
    &unique_logical_file_names,
    extract_mapped_file_names()
  );
  return unique_logical_file_names.size();
}

std::set<std::string>*
extract_computing_elements_id::operator()(
  std::set<std::string>* s,
  StorageMapping::const_iterator si
) const
{
  StorageInfo::CE_Mounts const& mounts = si->second.ce_mounts;
  StorageInfo::CE_Mounts::const_iterator it(mounts.begin());
  StorageInfo::CE_Mounts::const_iterator const e(mounts.end());
  for ( ; it != e ; ++it) {
    s->insert(it->first);
  }
  return s;
}

bool
is_storage_close_to::operator()(StorageMapping::const_iterator const& v) const
{
  StorageInfo::CE_Mounts const& mounts = v->second.ce_mounts;
  StorageInfo::CE_Mounts::const_iterator it(mounts.begin());
  StorageInfo::CE_Mounts::const_iterator const e(mounts.end());
  for ( ; it != e ; ++it) {
    if (boost::starts_with(m_ceid, it->first)) {
      return true;
    }
  }
  return false;
}

boost::shared_ptr<FileMapping>
resolve_filemapping_info(classad::ClassAd const& ad)
{
  boost::shared_ptr<FileMapping> fm(
    dli::resolve_filemapping_info(ad)
  );
  return fm;
}
}}}

