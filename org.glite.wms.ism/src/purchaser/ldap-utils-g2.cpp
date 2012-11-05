/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
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

// File: ldap-utils.cpp
// Author: Salvatore Monforte
// Copyright (c) 2002 EU DataGrid.

// $Id: ldap-utils-g2.cpp,v 1.1.2.22 2012/06/22 11:51:32 mcecchi Exp $
#include <sys/time.h>
#include <ldap.h>
#include <lber.h>

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <functional>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/tokenizer.hpp>
#include <boost/progress.hpp>
#include <boost/any.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_inserter.hpp>
#include "glite/wms/ism/purchaser/common.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wmsutils/classads/classad_utils.h"

#include "ldap-dn-utils-g2.h"
#include "schema_utils.h"

namespace cu  = glite::wmsutils::classads;
namespace ba  = boost::algorithm;
namespace bas = boost::assign;
namespace ism = glite::wms::ism;

namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

typedef boost::shared_ptr<classad::ClassAd> ClassAdPtr;

namespace {

struct ManagerInfo
{
  ClassAdPtr ad;
};
typedef std::map<std::string, ManagerInfo> ManagerInfoMap;

struct ServiceInfo;
typedef std::map<std::string, ServiceInfo> ServiceInfoMap;

struct ShareInfo;
typedef std::map<std::string, ShareInfo> ShareInfoMap;

struct AccessPolicyInfo {
  ClassAdPtr ad;
};
typedef std::map<std::string, AccessPolicyInfo> AccessPolicyInfoMap;

class EndpointInfo
{
public:
  ClassAdPtr ad;
  std::vector<classad::ExprTree*> policy_rules;
  bool has_servicelnk_iterator;
  bool has_policylnk_iterator;
  bool has_shareslnk_iterators;
  ServiceInfoMap::iterator service_lnk;
  AccessPolicyInfoMap::iterator policy_lnk;
  std::vector<ShareInfoMap::iterator> shares_lnk;
  EndpointInfo()
    : has_servicelnk_iterator(false), has_policylnk_iterator(false), has_shareslnk_iterators(false) { }
};
typedef std::map<std::string, EndpointInfo> EndpointInfoMap;

struct BenchMarkInfo
{
  ClassAdPtr ad;
};
typedef std::map<std::string, BenchMarkInfo> BenchMarkInfoMap;

struct ExecEnvInfo
{
  ClassAdPtr ad;
  std::set<std::string> applications;
  std::vector<BenchMarkInfoMap::iterator> bmarks_lnk;
};

struct DataStoreInfo
{ 
  ClassAdPtr ad;
};

typedef std::map<std::string, ExecEnvInfo> ExecEnvInfoMap;
typedef std::map<std::string, boost::any> ResourceInfoMap;

struct ServiceInfo
{
public:
  ServiceInfo()
    : has_managerlnk(false) { }
  ClassAdPtr ad;
  ManagerInfoMap::iterator manager_lnk;
  bool has_managerlnk;
  //////////////////////////////////////////////////////
  // The following is requireg since for StorageServices
  // the relevant Shares are most likely bound to the only
  // existing EndPoint for a given Service and do not
  // publish the ForeignKey describing such a relation.
  // 
  std::vector<ShareInfoMap::iterator> shares_lnk;
  /////////////////////////////////////////////////////
};

struct ShareInfo
{
  ClassAdPtr ad;
  std::vector<classad::ExprTree*> policy_rules;
  std::vector<ResourceInfoMap::iterator> resources_lnk;
};

//typedef std::map<std::string, ClassAdPtr> SubClusterInfoMap;
/*
struct ClusterInfo {
  std::string site_id;
  std::vector<CEInfoMap::iterator> ces_links;
  std::vector<SubClusterInfoMap::iterator> subclusters_links;
};
typedef std::map<std::string, ClusterInfo> ClusterInfoMap;

struct VoViewInfo
{
  VoViewInfo(std::string const& i, ClassAdPtr a) 
    : id(i), ad(a)
  {
  }
  std::string id;
  ClassAdPtr ad; 
};

typedef std::map<
  std::string, // ce id
  std::vector<VoViewInfo> // list of ce views per vo
> VoViewInfoMap;

struct SEInfo
{
  ClassAdPtr ad;
  std::vector<classad::ExprTree*> storage_areas;
  std::vector<classad::ExprTree*> control_protocols;
  std::vector<classad::ExprTree*> access_protocols;
  std::vector<ClassAdPtr> ces_binds;
};

typedef std::map<std::string, SEInfo> SEInfoMap;

*/

struct BDII_info
{
  ShareInfoMap shares;
  ServiceInfoMap services;
  ManagerInfoMap managers;
  EndpointInfoMap endpoints;
  AccessPolicyInfoMap policies;
  ResourceInfoMap resources;
  BenchMarkInfoMap benchmarks;
};

struct is_a_literal_node_starting_with
{
  is_a_literal_node_starting_with(std::string const& p) : prefix(p) {}
  bool operator()(classad::ExprTree* e) const {
    classad::Value v;
    std::string s;
    return (
      cu::is_literal(e) && 
      e->Evaluate(v) && v.IsStringValue(s) && ba::starts_with(s,prefix)
    );
  }
  std::string prefix;
};

struct is_a_literal_node_equals_to
{
  
  is_a_literal_node_equals_to(std::string const& v) : value(v) {}
  bool operator()(classad::ExprTree* e) const {
    classad::Value v;
    std::string s;
    return (
      cu::is_literal(e) && 
      e->Evaluate(v) && v.IsStringValue(s) && ba::equals(s,value)
    );
  }
  std::string value;
};

inline void cleanup_glue2_info(ClassAdPtr ad, std::list<std::string> a)
{
  a.push_back("objectClass");
  std::for_each(
   a.begin(), a.end(),
   boost::bind(&classad::ClassAd::Delete, ad, _1)
  );
}

inline bool is_objectclass(std::string const& item, const classad::ClassAd& ad)
{
  bool result = false;
  classad::ExprList* e = 0;
  ad.EvaluateAttrList("objectClass",e);
  if(e) result = std::find_if(e->begin(),e->end(), is_a_literal_node_equals_to(item)) != e->end();
  return result;
}

inline bool is_glue2_computing_share(const classad::ClassAd& ad)
{
  return is_objectclass("GLUE2ComputingShare", ad);
}

inline bool is_glue2_storage_share(const classad::ClassAd& ad)
{
  return is_objectclass("GLUE2StorageShare", ad);
}

inline bool is_glue2_execenv_resource(const classad::ClassAd& ad)
{
  return is_objectclass("GLUE2ExecutionEnvironment", ad);
}

inline bool is_glue2_datastore_resource(const classad::ClassAd& ad)
{
  return is_objectclass("GLUE2DataStore", ad);
}

void process_glue2_service_info(
  std::vector<std::string> const& ldap_dn_tokens, 
  ClassAdPtr ad, 
  BDII_info& bdii_info
) 
{
  std::string const service_id(
   ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=")+1)
  );
  ServiceInfoMap::iterator it;
  bool insert;
  boost::tie(it, insert) = bdii_info.services.insert(
    std::make_pair(service_id, ServiceInfo())
  );
  cleanup_glue2_info(ad,
    bas::list_of("CreationTime")
      ("AdminDomainForeignKey")
      ("OtherInfo")
  );
  if (insert) { // process_glue2_service_capacity_info might have inserted its ad before
    it->second.ad.reset(new classad::ClassAd());
    it->second.ad->Insert(
      "Service",
      ad->Copy()
    );
  }
  else {
    dynamic_cast<classad::ClassAd*>(
      it->second.ad->Lookup("Service")
    )->Update(*ad);
  }
}

void process_glue2_manager_info(
  std::vector<std::string> const& ldap_dn_tokens, 
  ClassAdPtr ad, 
  BDII_info& bdii_info
) 
{
  std::string const manager_id(
   ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=")+1)
  );
  std::string const service_id(
   ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=")+1)
  );
  ManagerInfoMap::iterator manager_it;
  bool insert;
  boost::tie(manager_it, insert) = bdii_info.managers.insert(
    std::make_pair(manager_id, ManagerInfo())
  );
  cleanup_glue2_info(ad,
    bas::list_of("CreationTime")
      ("ComputingServiceForeignKey")
      ("ServiceForeignKey")
      ("OtherInfo")
  );
  manager_it->second.ad.reset(new classad::ClassAd());
  manager_it->second.ad->Insert(
    "Manager",
    ad->Copy()
  );
   
  ServiceInfoMap::iterator service_it;
  boost::tie(service_it, insert) = bdii_info.services.insert(
    std::make_pair(service_id, ServiceInfo())
  );
  service_it->second.manager_lnk = manager_it;
  service_it->second.has_managerlnk = true;
}

template<typename T>
void extract_glue2_info_value_in(
  classad::ClassAd const& ad,
  std::string const& from, 
  std::string const& what, 
  classad::ClassAd* in)
{
  classad::ExprList* el = dynamic_cast<classad::ExprList*>(
    ad.Lookup(from)
  );
  if (el) {
    std::vector<classad::ExprTree*>::const_iterator it(
      std::find_if(
        el->begin(), el->end(),
        is_a_literal_node_starting_with(what+"=")
      )
    );
    if (it != el->end()) {
      classad::Value v;
      std::string s;
      (*it)->Evaluate(v) && v.IsStringValue(s);
      in->InsertAttr(what,boost::lexical_cast<T>(s.substr(s.find("=")+1)));
    }
  }
}

void process_glue2_resource_fk(
  ShareInfoMap::iterator& share_it, 
  ClassAdPtr ad, 
  BDII_info& bdii_info)
{
  classad::ExprList* el = dynamic_cast<classad::ExprList*>(
    ad->Lookup("ResourceForeignKey")
  );
  if (el && el->size()) {
    classad::Value v;
    std::string s;
    (*el->begin())->Evaluate(v) && v.IsStringValue(s);

    bool insert;
    boost::any resource;
    ResourceInfoMap::iterator resource_it;

    if (is_glue2_computing_share(*ad)) resource = ExecEnvInfo();
    else if (is_glue2_storage_share(*ad)) resource = DataStoreInfo();
    else {
      Error("Cannot discriminate between Computing or Storage share: " << *ad);
      return ;
    }
    boost::tie(resource_it, insert) = bdii_info.resources.insert(
      std::make_pair(s, resource)
    );
    share_it->second.resources_lnk.push_back(resource_it);
  }
}

EndpointInfoMap::iterator 
process_glue2_endpoint_fk_entry(
  BDII_info& bdii_info, 
  classad::ExprTree* e)
{
  classad::Value v;
  std::string s;
  EndpointInfoMap::iterator endpoint_it(
    bdii_info.endpoints.end()
  );
  if (e->Evaluate(v) && v.IsStringValue(s)) {
    bool insert;
    boost::tie(endpoint_it, insert) = bdii_info.endpoints.insert(
      std::make_pair(s, EndpointInfo())
    );
  }
  return endpoint_it;
}

bool process_glue2_endpoint_fk(
  ShareInfoMap::iterator& share_it, 
  ClassAdPtr ad, 
  BDII_info& bdii_info)
{
  std::string const fk(
    is_glue2_computing_share(*ad) ? 
      "ComputingEndpointForeignKey" : "StorageEndpointForeignKey"
  );
  bool result = false;
  classad::ExprList* el = dynamic_cast<classad::ExprList*>(
    ad->Lookup(fk)
  );

  if (el && el->size()) {
    for( // link each endpoint to the share it binds to 
      classad::ExprList::iterator el_it = el->begin() ;
      el_it != el->end(); ++el_it 
    ) {
      EndpointInfoMap::iterator endpoint_it(
        process_glue2_endpoint_fk_entry(bdii_info, *el_it)
      );
      if (endpoint_it != bdii_info.endpoints.end()) {
         endpoint_it->second.shares_lnk.push_back(share_it);
         result = true;
         endpoint_it->second.has_shareslnk_iterators = true;
      }
    }
  }
  return result;
}    
void bind_share_to_service(
  ShareInfoMap::iterator& share_it,
  std::string const& service_id,
  BDII_info& bdii_info
)
{
  bool insert;
  ServiceInfoMap::iterator service_it;
  boost::tie(service_it, insert) = bdii_info.services.insert(
    std::make_pair(service_id, ServiceInfo())
  );
  service_it->second.shares_lnk.push_back(share_it);
}

void process_glue2_share_info(
  std::vector<std::string> const& ldap_dn_tokens, 
  ClassAdPtr ad, 
  BDII_info& bdii_info
) 
{
  std::string const share_id(
   ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=")+1)
  );
  std::string const service_id(
   ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=")+1)
  );
  ShareInfoMap::iterator share_it;
  bool insert;
  boost::tie(share_it, insert) = bdii_info.shares.insert(
    std::make_pair(share_id, ShareInfo())
  );

  process_glue2_resource_fk(share_it, ad, bdii_info);
  
  if( !process_glue2_endpoint_fk(share_it, ad, bdii_info) ) {
    
    // For this Share the FK describing the relation between a Share
    // and the Endpoint exsposing it is missing.
    bind_share_to_service(share_it, service_id, bdii_info);
  }
  
  cleanup_glue2_info(ad,
    bas::list_of("CreationTime")
      ("ComputingEndpointForeignKey")
      ("ComputingServiceForeignKey")
      ("EndpointForeignKey")
      ("ResourceForeignKey")
      ("ServiceForeignKey")
      ("ExecutionEnvironmentForeignKey")
  );
  if (insert) { // process_glue2_share_capacity_info might have inserted its ad before
    share_it->second.ad.reset(new classad::ClassAd());
    share_it->second.ad->Insert(
      "Share",
      ad->Copy()
    );
  }
  else {
    classad::ClassAd* shareAd = 0;
    share_it->second.ad->EvaluateAttrClassAd("Share",shareAd);
    shareAd->Update(*ad); 
  }
}

void process_glue2_endpoint_info(
  std::vector<std::string> const& ldap_dn_tokens, 
  ClassAdPtr ad, 
  BDII_info& bdii_info
) 
{
  std::string const endpoint_id(
   ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=")+1)
  );
  std::string const service_id(
   ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=")+1)
  );
  EndpointInfoMap::iterator endpoint_it;
  bool insert;
  boost::tie(endpoint_it, insert) = bdii_info.endpoints.insert(
    std::make_pair(endpoint_id, EndpointInfo())
  );
  classad::ClassAd* oi = new classad::ClassAd;
  extract_glue2_info_value_in<std::string>(*ad, "OtherInfo", "HostDN", oi);
  extract_glue2_info_value_in<std::string>(*ad, "OtherInfo", "MiddlewareName", oi);
  extract_glue2_info_value_in<std::string>(*ad, "OtherInfo", "MiddlewareVersion", oi);
  ad->Insert("OtherInfo", oi);

  cleanup_glue2_info(ad,
    bas::list_of("CreationTime")
      ("ComputingServiceForeignKey")
      ("ServiceForeignKey")
  );
 
  endpoint_it->second.ad.reset(new classad::ClassAd());
  endpoint_it->second.ad->Insert(
    "Endpoint",
    ad->Copy()
  );
  ServiceInfoMap::iterator service_it;
  boost::tie(service_it, insert) = bdii_info.services.insert(
    std::make_pair(service_id, ServiceInfo())
  );
  endpoint_it->second.service_lnk = service_it;
  endpoint_it->second.has_servicelnk_iterator = true;
}

void process_glue2_execenv_info(
std::string const resource_id,
std::string const service_id,
ClassAdPtr ad,
  BDII_info& bdii_info
) {
  
  bool insert;
  ResourceInfoMap::iterator resource_it;
  boost::tie(resource_it, insert) = bdii_info.resources.insert(
    std::make_pair(resource_id, ExecEnvInfo())
  );
    
  classad::ClassAd* oi = new classad::ClassAd;
  extract_glue2_info_value_in<short>(*ad, "OtherInfo", "SmpSize", oi);
  extract_glue2_info_value_in<short>(*ad, "OtherInfo", "Cores", oi);
  ad->Insert("OtherInfo", oi);

  cleanup_glue2_info(ad, bas::list_of("CreationTime")
    ("ComputingManagerForeignKey")
    ("ManagerForeignKey")
  );

  ClassAdPtr resource_ad(new classad::ClassAd());
  resource_ad->Insert(
    "ExecutionEnvironment", ad->Copy()
  );
  ExecEnvInfo& eei = boost::any_cast<ExecEnvInfo&>(resource_it->second);
  eei.ad = resource_ad;
}

void process_glue2_datastore_info(
  std::string const resource_id,
  std::string const service_id,
  ClassAdPtr ad,
  BDII_info& bdii_info
) {
  bool insert;
  ResourceInfoMap::iterator resource_it;
  boost::tie(resource_it, insert) = bdii_info.resources.insert(
    std::make_pair(resource_id, DataStoreInfo())
  );
    
  cleanup_glue2_info(ad, bas::list_of("CreationTime")
    ("ComputingManagerForeignKey")
    ("ManagerForeignKey")
  );

  ClassAdPtr resource_ad(new classad::ClassAd());
  resource_ad->Insert(
    "DataStore", ad->Copy()
  );
  DataStoreInfo& dsi = boost::any_cast<DataStoreInfo&>(resource_it->second);
  dsi.ad = resource_ad;
}

void process_glue2_resource_info(
  std::vector<std::string> const& ldap_dn_tokens, 
  ClassAdPtr ad, 
  BDII_info& bdii_info
) 
{
  std::string const resource_id(
   ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=")+1)
  );
  std::string const service_id(
   ldap_dn_tokens[2].substr(ldap_dn_tokens[2].find("=")+1)
  );
  if (is_glue2_execenv_resource(*ad)) {
    process_glue2_execenv_info(resource_id, service_id, ad, bdii_info);
  }
  else if (is_glue2_datastore_resource(*ad)) {
    process_glue2_datastore_info(resource_id, service_id, ad, bdii_info);
  }
}

void process_glue2_application_env_info(
  std::vector<std::string> const& ldap_dn_tokens,
  ClassAdPtr ad,
  BDII_info& bdii_info
)
{
  std::string const appenv_id(
   ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=")+1)
  );
  std::string const resource_id(
   ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=")+1)
  );

  std::string application =
    cu::evaluate_attribute(*ad,"AppName");

  bool insert;
  ResourceInfoMap::iterator resource_it;
  boost::tie(resource_it, insert) = bdii_info.resources.insert(
    std::make_pair(resource_id, ExecEnvInfo())
  );

  boost::any_cast<ExecEnvInfo&>(
    resource_it->second
  ).applications.insert(application);
}

void process_glue2_benchmark_info(
  std::vector<std::string> const& ldap_dn_tokens, 
  ClassAdPtr ad, 
  BDII_info& bdii_info
) 
{
  std::string const bmark_id(
   ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=")+1)
  );

  std::string const resource_id(
   ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=")+1)
  );

  bool insert;
  BenchMarkInfoMap::iterator bmark_it;
  boost::tie(bmark_it, insert) = bdii_info.benchmarks.insert(
    std::make_pair(bmark_id, BenchMarkInfo())
  );

  cleanup_glue2_info(ad,
    bas::list_of("CreationTime")
      ("ComputingManagerForeignKey")
      ("ExecutionEnvironmentForeignKey")
      ("OtherInfo")
  );
  // The schema defines the BenchmarkValue to be a 32-bit float
  // However LDAP is rather deficient in data types and the only 
  // numeric type is integer, so it has to be stored as a string. 
  try {
    std::string v = cu::evaluate_attribute(*ad, "Value");
    ad->InsertAttr("Value", boost::lexical_cast<float>(v));
  } catch(boost::bad_lexical_cast &) {}
  
  bmark_it->second.ad = ad;

  ResourceInfoMap::iterator resource_it;
  boost::tie(resource_it, insert) = bdii_info.resources.insert(
    std::make_pair(resource_id, ExecEnvInfo())
  );
  boost::any_cast<ExecEnvInfo&>(
    resource_it->second
  ).bmarks_lnk.push_back(bmark_it);
}

void process_glue2_mapping_policy_info(
  std::vector<std::string> const& ldap_dn_tokens, 
  ClassAdPtr ad, 
  BDII_info& bdii_info
) 
{
  std::string const share_id(
   ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=")+1)
  );
  // Policy rules are inserted directly into the relevant ShareInfo
  bool insert;
  ShareInfoMap::iterator share_it;
  boost::tie(share_it, insert) = bdii_info.shares.insert(
    std::make_pair(share_id, ShareInfo())
  );
  classad::ExprList* el = dynamic_cast<classad::ExprList*>(
    ad->Lookup("Rule")
  );
  std::transform(
    el->begin(), el->end(),
    std::inserter(
      share_it->second.policy_rules, 
      share_it->second.policy_rules.end()
    ),
    boost::bind(&classad::ExprTree::Copy, _1)
  );
}

void process_glue2_access_policy_info(
  std::vector<std::string> const& ldap_dn_tokens, 
  ClassAdPtr ad, 
  BDII_info& bdii_info
) 
{
  std::string const policy_id(
   ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=")+1)
  );
  std::string const endpoint_id(
   ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=")+1)
  );
  bool insert;
  AccessPolicyInfoMap::iterator policy_it;
  boost::tie(policy_it, insert) = bdii_info.policies.insert(
    std::make_pair(policy_id, AccessPolicyInfo())
  );

  cleanup_glue2_info(ad,
    bas::list_of("CreationTime")
      ("UserDomainForeignKey")
      ("EndpointForeignKey")
      ("OtherInfo")
  );

  policy_it->second.ad = ad;

  EndpointInfoMap::iterator endpoint_it;
  boost::tie(endpoint_it, insert) = bdii_info.endpoints.insert(
    std::make_pair(endpoint_id, EndpointInfo())
  );
  endpoint_it->second.policy_lnk = policy_it;
  endpoint_it->second.has_policylnk_iterator = true;
}

void process_glue2_service_capacity_info(
  std::vector<std::string> const& ldap_dn_tokens,
  ClassAdPtr ad,
  BDII_info& bdii_info
)
{
  std::string const service_id(
   ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=")+1)
  );

  ServiceInfoMap::iterator it;
  bool insert;
  boost::tie(it, insert) = bdii_info.services.insert(
    std::make_pair(service_id, ServiceInfo())
  );
  if (ad) {
    cleanup_glue2_info(ad,
      bas::list_of("StorageServiceForeignKey")
    );
  }
  // process_glue2_service_info might have inserted its ad before
  if (insert) { 
    it->second.ad.reset(new classad::ClassAd());
    it->second.ad->Insert("Service", new classad::ClassAd());
  }
  if (it->second.ad && ad) {
    it->second.ad->DeepInsert(
      it->second.ad->Lookup("Service"),
      "Capacity",
      ad->Copy()
    );
  }
}

void process_glue2_share_capacity_info(
  std::vector<std::string> const& ldap_dn_tokens,
  ClassAdPtr ad,
  BDII_info& bdii_info
)
{
  std::string const share_id(
   ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=")+1)
  );

  ShareInfoMap::iterator it;
  bool insert;
  boost::tie(it, insert) = bdii_info.shares.insert(
    std::make_pair(share_id, ShareInfo())
  );
  cleanup_glue2_info(ad,
    bas::list_of("StorageShareForeignKey")
  );
  // process_glue2_service_info might have inserted its ad before
  if (insert) { 
    it->second.ad.reset(new classad::ClassAd());
    it->second.ad->Insert("Share", new classad::ClassAd());
  }
  it->second.ad->DeepInsert(
    it->second.ad->Lookup("Share"),
    "Capacity",
    ad->Copy()
  );
}

inline classad::ExprTree* make_literal(std::string const& s)
{
  classad::Value v;
  v.SetStringValue(s);
  return classad::Literal::MakeLiteral(v);
}

inline classad::ExprTree* parse_expression(std::string const& s)
{
  classad::ClassAdParser parser;
  return parser.ParseExpression(s);
}
inline classad::ExprTree* copy_benchmark_info(BenchMarkInfoMap::const_iterator bi)
{
  return dynamic_cast<classad::ExprTree*>(bi->second.ad->Copy());
}
 
typedef std::list<std::string> glue2_stripping_prefix;
typedef boost::function<
  bool(std::vector<std::string> const&)
> glue2_dn_test_predicate;

typedef boost::function<void(
  std::vector<std::string> const&, 
  ClassAdPtr,
  BDII_info&
)> glue2_info_processing_fn;

typedef boost::tuple<
  glue2_dn_test_predicate,
  glue2_stripping_prefix,
  glue2_info_processing_fn
> glue2_info_processor_tuple;

const glue2_stripping_prefix 
  service_pfx  = bas::list_of("GLUE2Entity")("GLUE2Service")("GLUE2StorageServiceCapacity"),
  manager_pfx  = bas::list_of("GLUE2Entity")("GLUE2Manager")("GLUE2ComputingManager")("GLUE2StorageManager"),
  share_pfx    = bas::list_of("GLUE2Entity")("GLUE2Share")("GLUE2ComputingShare")("GLUE2StorageShareCapacity")("GLUE2StorageShare"),
  endpoint_pfx = bas::list_of("GLUE2Entity")("GLUE2Endpoint")("GLUE2ComputingEndpoint"),
  resource_pfx = bas::list_of("GLUE2Entity")("GLUE2Resource")("GLUE2ExecutionEnvironment")("GLUE2DataStore"),
  mpolicy_pfx  = bas::list_of("GLUE2Entity")("GLUE2Policy")("GLUE2MappingPolicy"),
  apolicy_pfx  = bas::list_of("GLUE2Entity")("GLUE2Policy")("GLUE2AccessPolicy"),
  appenv_pfx   = bas::list_of("GLUE2Entity")("GLUE2ApplicationEnvironment"),
  bmark_pfx    = bas::list_of("GLUE2Entity")("GLUE2Benchmark");
  
std::list<glue2_info_processor_tuple> 
glue2_info_processors = bas::tuple_list_of
  (is_glue2_service_dn, service_pfx, process_glue2_service_info)
  (is_glue2_manager_dn, manager_pfx, process_glue2_manager_info)
  (is_glue2_share_dn, share_pfx, process_glue2_share_info)
  (is_glue2_endpoint_dn, endpoint_pfx, process_glue2_endpoint_info)
  (is_glue2_resource_dn, resource_pfx, process_glue2_resource_info)
  (is_glue2_mapping_policy_dn, mpolicy_pfx, process_glue2_mapping_policy_info)
  (is_glue2_access_policy_dn, apolicy_pfx, process_glue2_access_policy_info)
  (is_glue2_application_env_dn, appenv_pfx, process_glue2_application_env_info)
  (is_glue2_benchmark_dn, bmark_pfx, process_glue2_benchmark_info)
  (is_glue2_service_capacity_dn, service_pfx, process_glue2_service_capacity_info)
  (is_glue2_share_capacity_dn, share_pfx, process_glue2_share_capacity_info)
;
  // The Helper expects to find some attribute 
  // required to support LCGCE and GLUE13
  
  typedef std::pair< std::string, std::string > str_pair;
  struct parse_expression_and_insert
  {
    parse_expression_and_insert(ClassAdPtr _ad) : ad(_ad) {}
    inline void operator()(str_pair const& p) {
      ad->Insert(p.first, parse_expression(p.second));
    } 
    ClassAdPtr ad;
  };
  std::vector<str_pair> glue13_attr_refs = bas::list_of<str_pair>
    ("GlueCEInfoHostName", "GLUE2.Computing.Share.OtherInfo.InfoProviderHost")
    ("GlueHostApplicationSoftwareRunTimeEnvironment","GLUE2.ApplicationEnvironment.AppName")
    ("GlueCEAccessControlBaseRule","GLUE2.Computing.Endpoint.Policy")
    ("QueueName","GLUE2.Computing.Share.MappingQueue")
    ("LRMSType","GLUE2.Computing.Manager.ProductName")
    ("GlueCEUniqueID","CEid")
    ("GlobusResourceContactString","CEid")
    ;
 
} // anonymous namespace

void 
fetch_bdii_se_info_g2(
  std::string const& host,
  size_t port,
  std::string const& dn, 
  time_t timeout,
  std::string const& ldap_se_filter_ext,
  gluece_info_container_type& se_info_container)
{
  LDAP* ld = 0;
  int result = ldap_initialize(
    &ld, 
    std::string("ldap://" + host + ":" + boost::lexical_cast<std::string>(port)).c_str()
  );
  boost::shared_ptr<void> ld_guard(
    static_cast<void*>(0), 
    boost::bind(ldap_unbind_ext, ld, (LDAPControl**)(0), (LDAPControl**)(0))
  );
  if (result != LDAP_SUCCESS ) {

    throw LDAPException(
      std::string("ldap_simple_bind_s error: ").append(
        ldap_err2string(result)
      )
    );
  }

  struct timeval to;
  to.tv_sec = timeout;
  to.tv_usec = 0L;

  ldap_set_option(ld, LDAP_OPT_NETWORK_TIMEOUT, &to);

  LDAPMessage *ldresult = 0;
  if ( (result = ldap_search_ext_s(
    ld,
    dn.c_str(),
    LDAP_SCOPE_SUBTREE,
    ldap_se_filter_ext.c_str(),
    0,
    false,
    0,
    0,
    &to,
    LDAP_NO_LIMIT,
    &ldresult) != LDAP_SUCCESS)) {

    throw LDAPException(
      std::string("ldap_search error: ").append(
        ldap_err2string(result)
      )
    );
  }
  struct BDII_info bdii_info;

  boost::shared_ptr<void> ldresult_guard(
    ldresult, ldap_msgfree
  );


  time_t const t0 = std::time(0);
  size_t n_entries = 0;
  for (
    LDAPMessage* lde = ldap_first_entry(ld, ldresult);
    lde != 0; lde = ldap_next_entry(ld, lde)
  ) {

    boost::shared_ptr<char> dn_str(
      ldap_get_dn( ld, lde ),
      ber_memfree
    );
    ++n_entries;
    std::vector<std::string> ldap_dn_tokens;
    tokenize_ldap_dn(dn_str.get(), ldap_dn_tokens);
    for (
      std::list<glue2_info_processor_tuple>::const_iterator p_it = glue2_info_processors.begin() ;
      p_it != glue2_info_processors.end() ; ++p_it ) {
    
      glue2_dn_test_predicate check = p_it->get<0>();
      
      if (check(ldap_dn_tokens)) {
        
        ClassAdPtr ad(
          create_classad_from_ldap_entry(
            ld, lde, p_it->get<1>(), true
        ));
        glue2_info_processing_fn process = p_it->get<2>();
        process(ldap_dn_tokens, ad, bdii_info);
        break ; 
      } 
    }
  }
  Debug("#" << n_entries << " LDAP entries received in " << std::time(0) - t0 << " seconds");
  
  EndpointInfoMap::const_iterator ep_it(bdii_info.endpoints.begin());
  EndpointInfoMap::const_iterator const ep_e(bdii_info.endpoints.end());
  
  time_t const t1 = std::time(0);
  size_t n_shares = 0;
  for( ; ep_it != ep_e; ++ep_it) {

    ClassAdPtr storageAd(new classad::ClassAd);
    if (ep_it->second.ad) {
      storageAd->Update(*ep_it->second.ad);
      if (ep_it->second.has_servicelnk_iterator) {
        if (ep_it->second.service_lnk->second.ad) {
          storageAd->Update(*ep_it->second.service_lnk->second.ad);
        }
        if (ep_it->second.service_lnk->second.has_managerlnk &&
          ep_it->second.service_lnk->second.manager_lnk->second.ad) {
          storageAd->Update(*ep_it->second.service_lnk->second.manager_lnk->second.ad);
        }
      }
    }
    if (ep_it->second.has_policylnk_iterator &&
      ep_it->second.policy_lnk->second.ad && 
        storageAd->Lookup("Endpoint")) {
    storageAd->DeepInsert(
      storageAd->Lookup("Endpoint"),
      "Policy",
       ep_it->second.policy_lnk->second.ad->Lookup("Rule")->Copy()
    );
  }

    // If there is no Share bound to the Endpoint then we have to choose
    // Shares directly from the Service
  if (ep_it->second.has_shareslnk_iterators) {
    std::vector<ShareInfoMap::iterator> const& shares(
      ep_it->second.shares_lnk.empty() ? 
        ep_it->second.service_lnk->second.shares_lnk :
        ep_it->second.shares_lnk
    );

    std::vector<ShareInfoMap::iterator>::const_iterator sh_it(
      shares.begin()
    );
    std::vector<ShareInfoMap::iterator>::const_iterator const sh_e(
      shares.end());
    for( ; sh_it != sh_e; ++sh_it) {

      classad::ClassAd* storageAd_copy(
        dynamic_cast<classad::ClassAd*>(storageAd->Copy())
      );
      classad::ExprTree* shareAd_copy(
        (*sh_it)->second.ad->Lookup("Share")->Copy()
      );
      dynamic_cast<classad::ClassAd*>(shareAd_copy)->Insert(
        "Policy",
        classad::ExprList::MakeExprList((*sh_it)->second.policy_rules)
      );
      storageAd_copy->Insert("Share", shareAd_copy);
    
      if( !(*sh_it)->second.resources_lnk.empty() ) {
        DataStoreInfo const& dsi(
          boost::any_cast<DataStoreInfo const&>(
            (*sh_it)->second.resources_lnk.front()->second
          )
        );

        storageAd_copy->Update(*dsi.ad);
      }

      classad::ClassAd* g2Ad = new classad::ClassAd;

      g2Ad->Insert("Storage", storageAd_copy);
      ClassAdPtr result( new classad::ClassAd );
      result->Insert("GLUE2", g2Ad);
      std::string const id = cu::evaluate_expression(*result,
        "GLUE2.Storage.Share.ID"
      );
      se_info_container.insert(std::make_pair(id, result));
      n_shares++;
    }
  }
  }
  Debug("#" << n_shares << " GLUE2StorageShare's ClassAd(s) generated in " << std::time(0) - t1 << " seconds");
}

void 
fetch_bdii_ce_info_g2(
  std::string const& host,
  size_t port,
  std::string const& dn,
  time_t timeout,
  std::string const& ldap_ce_filter_ext,
  gluece_info_container_type& ce_info_container)
{
  LDAP* ld = 0;
  int result = ldap_initialize(
    &ld, 
    std::string("ldap://" + host + ":" + boost::lexical_cast<std::string>(port)).c_str()
  );
  boost::shared_ptr<void> ld_guard(
    static_cast<void*>(0), 
    boost::bind(ldap_unbind_ext, ld, (LDAPControl**)(0), (LDAPControl**)(0))
  );
  if (result != LDAP_SUCCESS ) {

    throw LDAPException(
      std::string("ldap_simple_bind_s error: ").append(
        ldap_err2string(result)
      )
    );
  }

  struct timeval to;
  to.tv_sec = timeout;
  to.tv_usec = 0L;

  ldap_set_option(ld, LDAP_OPT_NETWORK_TIMEOUT, &to);

  LDAPMessage *ldresult = 0;
  if ( (result = ldap_search_ext_s(
    ld,
    dn.c_str(),
    LDAP_SCOPE_SUBTREE,
    ldap_ce_filter_ext.c_str(),
    0,
    false,
    0,
    0,
    &to,
    LDAP_NO_LIMIT,
    &ldresult) != LDAP_SUCCESS)) {

    throw LDAPException(
      std::string("ldap_search error: ").append(
        ldap_err2string(result)
      )
    );
  }

  boost::shared_ptr<void> ldresult_guard(
    ldresult, ldap_msgfree
  );

  BDII_info bdii_info;

  time_t const t0 = std::time(0);
  size_t n_entries = 0;
  for (
    LDAPMessage* lde = ldap_first_entry(ld, ldresult);
    lde != 0; lde = ldap_next_entry(ld, lde)
  ) {

    boost::shared_ptr<char> dn_str(
      ldap_get_dn( ld, lde ),
      ber_memfree
    );
    ++n_entries;
    std::vector<std::string> ldap_dn_tokens;
    tokenize_ldap_dn(dn_str.get(), ldap_dn_tokens);
    for (
      std::list<glue2_info_processor_tuple>::const_iterator p_it = glue2_info_processors.begin() ;
      p_it != glue2_info_processors.end() ; ++p_it ) {
    
      glue2_dn_test_predicate check = p_it->get<0>();
      
      if (check(ldap_dn_tokens)) {
        
        ClassAdPtr ad(
          create_classad_from_ldap_entry(
            ld, lde, p_it->get<1>(), true
        ));
        glue2_info_processing_fn process = p_it->get<2>();
        process(ldap_dn_tokens, ad, bdii_info);
        break ; 
      } 
    }
  }
  Debug("#" << n_entries << " LDAP entries received in " << std::time(0) - t0 << " seconds");

  EndpointInfoMap::const_iterator ep_it(bdii_info.endpoints.begin());
  EndpointInfoMap::const_iterator const ep_e(bdii_info.endpoints.end());
  
  time_t const t1 = std::time(0);
  size_t n_shares = 0;
  for( ; ep_it != ep_e; ++ep_it) { 

    if ( ep_it->second.shares_lnk.empty() ) { // Not an Endpoint bound to Shares
       continue;
    }
    ClassAdPtr computingAd(new classad::ClassAd);

    if (ep_it->second.ad) {
      computingAd->Update(*ep_it->second.ad); 
    }

    computingAd->Update(*ep_it->second.service_lnk->second.ad); 
    computingAd->Update(*ep_it->second.service_lnk->second.manager_lnk->second.ad);

    computingAd->DeepInsert(
      computingAd->Lookup("Endpoint"),
      "Policy",
       ep_it->second.policy_lnk->second.ad->Lookup("Rule")->Copy()
    ); 

   std::vector<ShareInfoMap::iterator>::const_iterator sh_it(
      ep_it->second.shares_lnk.begin()
    );
    std::vector<ShareInfoMap::iterator>::const_iterator const sh_e(
      ep_it->second.shares_lnk.end()
    );

    for( ; sh_it != sh_e; ++sh_it) {

      classad::ClassAd* computingAd_copy(
        dynamic_cast<classad::ClassAd*>(computingAd->Copy())
      );
      computingAd_copy->Update(*(*sh_it)->second.ad);
      ExecEnvInfo const& eei(
        boost::any_cast<ExecEnvInfo>((*sh_it)->second.resources_lnk.front()->second)
      );

      std::vector<classad::ExprTree*> apps_el;
      std::transform(
        eei.applications.begin(),
        eei.applications.end(),
        std::back_inserter(apps_el),
        make_literal
      ); 
      classad::ClassAd* appenvAd = new classad::ClassAd();
      appenvAd->Insert("AppName", classad::ExprList::MakeExprList(apps_el));

      std::vector<classad::ExprTree*> bmark_ets;
      std::transform(
        eei.bmarks_lnk.begin(),
        eei.bmarks_lnk.end(),
        std::back_inserter(bmark_ets),
        copy_benchmark_info
       ); 

      classad::ClassAd* g2Ad = new classad::ClassAd;

      g2Ad->Insert("Benchmark", classad::ExprList::MakeExprList(bmark_ets));
      g2Ad->Insert("ApplicationEnvironment", appenvAd);
      if (eei.ad) {
        g2Ad->Update(*eei.ad);
      }
      g2Ad->Insert("Computing", computingAd_copy);
      g2Ad->DeepInsert(
        computingAd_copy->Lookup("Share"),
        "Policy", 
        classad::ExprList::MakeExprList((*sh_it)->second.policy_rules)
      );
      std::string interface_name = cu::evaluate_expression(
        *ep_it->second.ad, "EndPoint.InterfaceName"
      );
     classad::ClassAd* shareAd = dynamic_cast<classad::ClassAd*>(
        computingAd_copy->Lookup("Share")
      );
      
      classad::ExprList* el;
      shareAd->EvaluateAttrList("OtherInfo", el);
      // Find the CREAMCEId bound to the relevant EndPoint via InterfaceName
      std::vector<classad::ExprTree*>::const_iterator oi_it(
        std::find_if(el->begin(), el->end(),
          is_a_literal_node_starting_with(
            "CREAMCEId=" + ba::erase_first_copy(ep_it->first, "_"+interface_name)
          )
        )
      );
      // Handling OtherInfo conversion to ClassAd for the Share
      classad::ClassAd* oi = new classad::ClassAd;

      if (oi_it != el->end()) {
        classad::Value v;
        std::string s;
        (*oi_it)->Evaluate(v) && v.IsStringValue(s);
        oi->InsertAttr("CREAMCEId", s.substr(s.find("=")+1));
        
      }
      extract_glue2_info_value_in<std::string>(
        *shareAd  , "OtherInfo", "InfoProviderName", oi
      );
      extract_glue2_info_value_in<std::string>(
        *shareAd, "OtherInfo", "InfoProviderVersion", oi
      );
      extract_glue2_info_value_in<std::string>(
        *shareAd, "OtherInfo", "InfoProviderHost", oi
      );
      g2Ad->DeepInsert(
        computingAd_copy->Lookup("Share"),
        "OtherInfo", oi
      );

      ClassAdPtr result( new classad::ClassAd );
      result->Insert("GLUE2", g2Ad);

      std::string const id = cu::evaluate_expression(*result,
        "IsUndefined(GLUE2.Computing.Share.OtherInfo.CREAMCEId) ?"
        "    GLUE2.Computing.Share.ID : GLUE2.Computing.Share.OtherInfo.CREAMCEId"
      );
      std::string const policy = cu::evaluate_expression(*result,
        "GLUE2.Computing.Share.Policy[0]"
      );

      std::string const glue13Id(
        id + "/" + ba::erase_regex_copy(policy, boost::regex(".+:"))
      );
      std::for_each(
        glue13_attr_refs.begin(), glue13_attr_refs.end(),
        parse_expression_and_insert(result)
      );
      result->InsertAttr("CEid", id); 
      ce_info_container.insert(std::make_pair(glue13Id, result));
      n_shares++;
    }   
  }
  Debug("#" << n_shares << " GLUE2ComputingShare's ClassAd(s) generated in " << std::time(0) - t1 << " seconds");
}

void fetch_bdii_info_g2(
  std::string const& hostname,
  size_t port,
  std::string const& dn,
  time_t timeout,
  std::string const& ldap_ce_filter_ext,
  std::string const& ldap_se_filter_ext,
  gluece_info_container_type& gluece_info_container,
  gluece_info_container_type& gluese_info_container)
{
  fetch_bdii_ce_info_g2(hostname, port, dn, timeout, ldap_ce_filter_ext, gluece_info_container);
  fetch_bdii_se_info_g2(hostname, port, dn, timeout, ldap_se_filter_ext, gluese_info_container);
}

}}}}
