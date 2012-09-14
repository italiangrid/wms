// File: storage_utils.cpp
// Author: Salvatore Monforte
// Author: Francesco Giacomini

// $Id$

#include "storage_utils.h"

#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/regex.hpp>

#include <classad_distribution.h>

#include "glite/wms/common/logger/logging.h"
#include "brokerinfo.h"
#include "glite/wms/ism/ism.h"
#include "glite/wmsutils/classads/classad_utils.h"

namespace classad_utils = glite::wmsutils::classads;

namespace glite {
namespace wms {
namespace broker {

namespace {

bool evaluate(
  classad::ClassAd const& ad,
  std::string const& name,
  vector<classad::ExprTree*>& v
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
        protos.push_back(make_storage_info(ad));
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
  vector<classad::ExprTree*> ads;
  if (evaluate(se_ad, "CloseComputingElements", ads)) {

    vector<classad::ExprTree*>::const_iterator expr_it(ads.begin());
    vector<classad::ExprTree*>::const_iterator const expr_e(ads.end());

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
  std::vector<std::pair<std::string,int> > const& m_protocols;

public:
  matches_any_protocol_in(
    std::vector<std::pair<std::string,int> > const& protocols
  )
    : m_protocols(protocols)
  {
  }

  bool operator()(std::string const& p) const
  {
    std::vector<std::pair<std::string,int> >::const_iterator it(
      m_protocols.begin()
    );
    std::vector<std::pair<std::string,int> >::const_iterator const e(
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

StorageMapping
resolve_storagemapping_info(FileMapping const& fm)
{
  StorageMapping result;

  FileMapping::const_iterator file_it = fm.begin();
  FileMapping::const_iterator const file_end = fm.end();

  ism::Ism const& the_ism = *ism::get_ism();
  for ( ; file_it != file_end; ++file_it) {
    std::vector<std::string> const& sfns(file_it->second);
    std::vector<std::string>::const_iterator sfni = sfns.begin();
    std::vector<std::string>::const_iterator const sfne = sfns.end();

    for( ; sfni != sfne; ++sfni ) {
       std::string const name = resolve_storage_name(*sfni);
       if (name.empty()) {
         continue;
       }

       ism::Ism::MutexSliceContainer::const_iterator slice_it(
         the_ism.storage.begin()
       );
       ism::Ism::MutexSliceContainer::const_iterator const last_slice(
         the_ism.storage.end()
       );

       for( ; slice_it != last_slice; ++slice_it ) {

         ism::OrderedSliceIndex& ordered_index(
           (*slice_it)->slice->get<ism::OrderedIndex>()
         );

         ism::Mutex::scoped_lock l((*slice_it)->mutex);
         ism::OrderedSliceIndex::iterator it = ordered_index.find(name);

         if (it != ordered_index.end()) {

           boost::shared_ptr<classad::ClassAd> se_ad( it->ad );

           StorageInfo& info = result[name];
           info.links.push_back(file_it);
           insert_protocols(info.protocols, *se_ad);
           insert_ce_mount_points(info.ce_mounts, *se_ad);

         break; // there is no need to continue the search in
                // the remaining slices
         }
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

FileMapping resolve_filemapping_info(classad::ClassAd const& ad)
{
  boost::shared_ptr<glite::wms::rls::filemapping> fm(
    glite::wms::rls::resolve_filemapping_info(ad)
  );
  return *fm;
}

}}}
