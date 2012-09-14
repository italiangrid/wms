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

// $Id$
#include <sys/time.h>
#include <ldap.h>
#include <lber.h>

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <functional>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/tokenizer.hpp>
#include <boost/progress.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "glite/wms/ism/purchaser/common.h"
#include "glite/wms/common/utilities/scope_guard.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wmsutils/classads/classad_utils.h"

#include "ldap-dn-utils.h"
#include "schema_utils.h"

namespace cu = glite::wmsutils::classads;
namespace ut = glite::wms::common::utilities;
namespace ba = boost::algorithm;
namespace ism = glite::wms::ism;

namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

typedef boost::shared_ptr<classad::ClassAd> ClassAdPtr;

class LDAPException: public std::exception
{
  std::string m_error;
public:
  LDAPException(std::string const& error)
    : m_error(error)
  {
  }
  ~LDAPException() throw()
  {
  }
  virtual char const* what() const throw()
  {
    return m_error.c_str();
  }
};

namespace {

struct timeval f_zerotime;

struct CEInfo
{
  ClassAdPtr ad;
  std::vector<ClassAdPtr> ses_binds;
};

typedef std::map<std::string, CEInfo> CEInfoMap;
typedef std::map<std::string, ClassAdPtr> SubClusterInfoMap;

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

struct BDIICEInfo
{
  CEInfoMap         ces;
  SubClusterInfoMap subclusters;
  ClusterInfoMap    clusters;
  VoViewInfoMap     voviews;
};

std::string 
cluster_name(classad::ClassAd const& ad)
{
  std::string cluster;
  static char const* reg_string("GlueClusterUniqueID\\s*=\\s*([^\\s]+)");
  ad.EvaluateAttrString("GlueCEInfoHostName", cluster);
  try {
    std::vector<std::string> foreignKeys;
    cu::EvaluateAttrList(ad, "GlueForeignKey", foreignKeys);
    static boost::regex get_cluid(reg_string);
    boost::smatch result_cluid;
    bool found = false;

    // Looking for one GlueForeignKey (it is possibly multi-valued)
    // With the specified attribute.
    for( std::vector<std::string>::const_iterator key = foreignKeys.begin();
   key != foreignKeys.end(); key++) {

      if( boost::regex_match(*key, result_cluid, get_cluid) ) {

  cluster.assign(result_cluid[1].first,result_cluid[1].second);
  found = true;
  break;
      }
    }
    if (!found) {

//       Warning(
//         "Cannot find GlueClusterUniqueID assignment. Using " << cluster << "."
//       );
    }
  } catch( boost::bad_expression& e ){

//     Error(
//       "Bad regular expression " << reg_string << " parsing GlueForeignKey."
//     );
  }
  return cluster;
}

std::string 
site_name(classad::ClassAd const& ad)
{
  std::string site;
  static char const* reg_string("GlueSiteUniqueID\\s*=\\s*([^\\s]+)");
  try {
    std::vector<std::string> foreignKeys;
    cu::EvaluateAttrList(ad, "GlueForeignKey", foreignKeys);
    static boost::regex get_gsuid(reg_string);
    boost::smatch result_gsuid;
    bool found = false;
    std::vector<std::string>::const_iterator key_it = foreignKeys.begin();
    std::vector<std::string>::const_iterator const key_e = foreignKeys.end();

    // Looking for one GlueForeignKey (it is possibly multi-valued)
    // With the specified attribute.
    for( ;
         key_it != key_e; ++key_it) {

      if( boost::regex_match(*key_it, result_gsuid, get_gsuid) ) {

        site.assign(result_gsuid[1].first,result_gsuid[1].second);
        found = true;
        break;
      }
    }
    if (!found) {

//       Warning("Cannot find GlueSiteUniqueID assignment.");
    }
  } catch( boost::bad_expression& e ){

//     Error(
//       "Bad regular expression " << reg_string  << " parsing GlueForeignKey."
//     );
  }
  return site;
}

void process_cluster_info(
  std::vector<std::string> const& ldap_dn_tokens, 
  ClassAdPtr ad, 
  BDIICEInfo& bdii_info
) 
{
  std::string const cluster_id(
   ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=")+1)
  );
 
  std::string const site_id(site_name(*ad));
  ClusterInfoMap::iterator it;

  bool insert;        
  boost::tie(it, insert) = bdii_info.clusters.insert( 
    std::make_pair(cluster_id, ClusterInfo())
  );
  it->second.site_id = site_id;
}

void process_ce_info(
  std::vector<std::string> const& ldap_dn_tokens,
  ClassAdPtr ad,
  BDIICEInfo& bdii_info
)
{
  std::string const ce_id(
    ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=")+1)
  );
  std::string const cluster_id(cluster_name(*ad));
  CEInfoMap::iterator it;
  bool insert;
  boost::tie(it, insert) = bdii_info.ces.insert(
    std::make_pair(ce_id, CEInfo())
  );
  it->second.ad = ad;
  bdii_info.clusters[cluster_id].ces_links.push_back(it);
}

void process_subcluster_info(
  std::vector<std::string> const& ldap_dn_tokens,
  ClassAdPtr ad,
  BDIICEInfo& bdii_info
)
{   
  std::string const subcluster_id(
    ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=")+1)
  );
  std::string const cluster_id(
    ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=")+1)
  );
 
  SubClusterInfoMap::iterator it;
  bool insert;
  boost::tie(it, insert) = bdii_info.subclusters.insert(
    std::make_pair(subcluster_id, ad)
  );

  bdii_info.clusters[cluster_id].subclusters_links.push_back(it);
}
inline bool iequals(std::string const& a, std::string const& b)
{
  return ba::iequals(a,b);
}

void insert_values(
  std::string const& name,
  boost::shared_array<char*> values,
  classad::ClassAd& ad
) {
  std::vector<std::string>::const_iterator list_b, number_b;
  std::vector<std::string>::const_iterator list_e, number_e;
    
  boost::tie(list_b, list_e) = bdii_schema_info().multi_valued();
  boost::tie(number_b, number_e) = bdii_schema_info().number_valued();

  bool is_list = std::find_if(
    list_b, list_e, boost::bind(iequals, _1, name)
  ) != list_e;

  bool is_number = std::find_if(
    number_b, number_e, boost::bind(iequals, _1, name)
  ) != number_e; 

  std::string result;
  for (size_t i=0; values[i] != 0; ++i) {

    if (i) result.append(",");
    if (is_number || ba::iequals("undefined", values[i]) || 
      ba::iequals("false", values[i]) || ba::iequals("true", values[i])) {  
    
      result.append(values[i]);
    }
    else {
      result.append("\"").append(values[i]).append("\"");
    }
  }
  if (is_list) {
    std::string s("{");
    s.append(result).append("}");
    result.swap(s);
  }
  classad::ExprTree* e = 0;
  classad::ClassAdParser parser;
  parser.ParseExpression(result, e);
  if (e) ad.Insert(name, e);
}

classad::ClassAd*
create_classad_from_ldap_entry(
  boost::shared_ptr<LDAP> ld, LDAPMessage* lde
) {
  classad::ClassAd* result = new classad::ClassAd;
  BerElement *ber = 0;
  ut::scope_guard ber_guard(boost::bind(ber_free, ber, 0));
  for(
    char* attr = ldap_first_attribute(ld.get(), lde, &ber);
    attr; attr = ldap_next_attribute(ld.get(), lde, ber)
  ) {
    ut::scope_guard attr_guard(boost::bind(ber_memfree, attr));
    boost::shared_array<char*> values(
      ldap_get_values(ld.get(), lde, attr), ldap_value_free
    );
    if (!values) continue; 
    insert_values(attr, values, *result);
  }
  return result;
}
      
} // anonymous namespace
namespace async {
void 
fetch_bdii_se_info(
  std::string const& host, size_t port, std::string const& dn, 
  time_t timeout, ism::purchaser::PurchaserInfoContainer& se_info_container) 
{
  static char const* filter(
    "(|(objectclass=gluecesebind)(objectclass=gluese)(objectclass=gluesa)"
    "(objectclass=glueseaccessprotocol)(objectclass=gluesecontrolprotocol))"
  );

  boost::shared_ptr<LDAP> ld( ldap_init(host.c_str(), port), ldap_unbind);
  int result = ldap_simple_bind_s(ld.get(),0,0);
  
  if (result != LDAP_SUCCESS ) {
//     Error("ldap_simple_bind_s failure: " << ldap_err2string(result));
    return;
  }

  struct timeval to;
  to.tv_sec = timeout;
  to.tv_usec = 0L;

  ldap_set_option(ld.get(), LDAP_OPT_NETWORK_TIMEOUT, &to);

  LDAPMessage *ldresult = 0;
  int msgid = 0;
  if ( (result = ldap_search_ext(
    ld.get(),
    dn.c_str(),
    LDAP_SCOPE_SUBTREE,
    filter,
    0,
    false,
    0,
    0,
    &to,
    -1,
    &msgid) != LDAP_SUCCESS)) {

    throw LDAPException(
      std::string("ldap_search error: ").append(
        ldap_err2string(result)
      )
    );
  }
 
  SEInfoMap  se_info;
  time_t const t0 = std::time(0);
  size_t n_entries = 0;
  while ( (result = ldap_result(
    ld.get(), 
    msgid, 
    LDAP_MSG_ONE, 
    &f_zerotime, 
    &ldresult)) != LDAP_RES_SEARCH_RESULT && result != -1) {

    ut::scope_guard ldresult_guard(
      boost::bind(ldap_msgfree, ldresult)
    );
 
    if (result != LDAP_RES_SEARCH_ENTRY) continue; // timeout or not an entry actually

    LDAPMessage* lde = ldap_first_entry(ld.get(), ldresult);

//    if (!lde) break; // all done;

    for ( ; lde != 0; lde = ldap_next_entry(ld.get(), lde)) {
      ++n_entries;
      boost::shared_ptr<char> dn_str(ldap_get_dn(ld.get(), lde), ber_memfree);

      std::vector<std::string> ldap_dn_tokens;
      tokenize_ldap_dn(dn_str.get(), ldap_dn_tokens);

      if (is_gluese_info_dn(ldap_dn_tokens) || 
        is_gluecesebind_info_dn(ldap_dn_tokens)) {

        ClassAdPtr ad(create_classad_from_ldap_entry(ld, lde));
      
        std::string const se_id(
          ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=")+1)
        );
      
        SEInfoMap::iterator it;
        bool insert;
        boost::tie(it, insert) = se_info.insert(std::make_pair(se_id, SEInfo()));
        if (is_gluese_info_dn(ldap_dn_tokens)) { 
          it->second.ad = ad;                          
        } 
        else if (is_gluecesebind_info_dn(ldap_dn_tokens)) {
          it->second.ces_binds.push_back(ad);
        }
      }                                                                   
      else if (is_gluesa_info_dn(ldap_dn_tokens) ||
        is_gluese_control_protocol_info_dn(ldap_dn_tokens) ||
        is_gluese_access_protocol_info_dn(ldap_dn_tokens)) {

        classad::ExprTree* ad(create_classad_from_ldap_entry(ld, lde));
        std::string const se_id(
          ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=")+1)
        );
        SEInfoMap::iterator it(se_info.find(se_id));
        if (it == se_info.end()) {
          bool insert; 
          boost::tie(it, insert) = se_info.insert(std::make_pair(se_id, SEInfo()));
        }
        if(is_gluesa_info_dn(ldap_dn_tokens)) {
          it->second.storage_areas.push_back(ad);
        }
        else if(is_gluese_control_protocol_info_dn(ldap_dn_tokens)) {
          it->second.control_protocols.push_back(ad);
        }
        else if(is_gluese_access_protocol_info_dn(ldap_dn_tokens)) {
          it->second.access_protocols.push_back(ad);
        }
      }
    }
  }
  Debug("#" << n_entries << " LDAP entries received in " << std::time(0)-t0 << " seconds");
  ut::scope_guard ldresult_guard(
    boost::bind(ldap_msgfree, ldresult)
  );
  if (result == -1) {
/*
    Error(
      std::string("ldap_result error: ").append(
        ldap_err2string(
          ldap_result2error(ld.get(), ldresult, 0)
        )
      )
    );
*/
    return;
  }
  SEInfoMap::const_iterator se_it(se_info.begin());
  SEInfoMap::const_iterator const se_e(se_info.end());
  time_t const t1 = std::time(0);
  for( ; se_it != se_e; ++se_it) {
    if (!se_it->second.ad) continue;
       
    if (!se_it->second.storage_areas.empty()) {
      se_it->second.ad->Insert(
        "GlueSA", classad::ExprList::MakeExprList(se_it->second.storage_areas)
      );
    }
    if (!se_it->second.control_protocols.empty()) {

      se_it->second.ad->Insert("GlueSEControlProtocol", 
        classad::ExprList::MakeExprList(se_it->second.control_protocols)
      );
    }
    if (!se_it->second.access_protocols.empty()) {
    
      se_it->second.ad->Insert("GlueSEAccessProtocol", 
        classad::ExprList::MakeExprList(se_it->second.access_protocols)
      );
    }
    std::vector<ClassAdPtr>::const_iterator it(se_it->second.ces_binds.begin());
    std::vector<ClassAdPtr>::const_iterator const e(
      se_it->second.ces_binds.end()
    );
    std::vector<classad::ExprTree*> exprs;
    for( ; it != e; ++it ) {
          
      classad::ClassAd* ad = static_cast<classad::ClassAd*>(it->get()->Copy());
      ad->Insert("name", classad::AttributeReference::MakeAttributeReference(
        0, "GlueCESEBindSEUniqueID"
      ));
      ad->Insert("mount", classad::AttributeReference::MakeAttributeReference(
        0,"GlueCESEBindCEAccesspoint"
      ));
      exprs.push_back(ad);
    }
    se_it->second.ad->Insert(
      "CloseComputingElements", classad::ExprList::MakeExprList(exprs)
    );
    se_info_container[se_it->first]=se_it->second.ad;
  }
  Debug("ClassAd representation built in " << std::time(0) - t1 << " seconds");
}

void 
fetch_bdii_ce_info(
  std::string const& host, size_t port, std::string const& dn, 
  time_t timeout, std::string const& ldap_ce_filter_ext,
  ism::purchaser::PurchaserInfoContainer& ce_info_container) 
{
  std::string filter(
    "(|(objectclass=gluecesebind)(objectclass=gluecluster)(objectclass=gluesubcluster)"
  );

  if (!ldap_ce_filter_ext.empty()) { 
    filter += "(&(|"; 
  };
  filter += "(objectclass=gluevoview)(objectclass=gluece)";
  if (!ldap_ce_filter_ext.empty()) { 
    filter.append(")").append(ldap_ce_filter_ext).append(")"); 
  }
  filter += ")";

  boost::shared_ptr<LDAP> ld( ldap_init(host.c_str(), port), ldap_unbind);
  int result = ldap_simple_bind_s(ld.get(),0,0);
  
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

  ldap_set_option(ld.get(), LDAP_OPT_NETWORK_TIMEOUT, &to);

  LDAPMessage *ldresult = 0;
  int msgid = 0;
  if ( (result = ldap_search_ext(
    ld.get(),
    dn.c_str(),
    LDAP_SCOPE_SUBTREE,
    filter.c_str(),
    0,
    false,
    0,
    0,
    &to,
    -1,
    &msgid) != LDAP_SUCCESS)) {

    throw LDAPException(
      std::string("ldap_search error: ").append(
        ldap_err2string(result)
      )
    );
  }
  
  BDIICEInfo bdii_info;

  time_t const t0 = std::time(0);
  size_t n_entries = 0;
  while ( (result = ldap_result( 
    ld.get(), 
    msgid, 
    LDAP_MSG_ONE, 
    &f_zerotime, 
    &ldresult)) != LDAP_RES_SEARCH_RESULT && result != -1) {

    ut::scope_guard ldresult_guard(
      boost::bind(ldap_msgfree, ldresult)
    );

    if (result != LDAP_RES_SEARCH_ENTRY) continue; // timeout or not a real entry

    LDAPMessage* lde = ldap_first_entry(ld.get(), ldresult);

//    if (!lde) break; // all done;

    for ( ; lde != 0; lde = ldap_next_entry(ld.get(), lde) ) {

      boost::shared_ptr<char> dn_str(
        ldap_get_dn( ld.get(), lde ),
        ber_memfree
      );
      ++n_entries;
      std::vector<std::string> ldap_dn_tokens;
      tokenize_ldap_dn(dn_str.get(), ldap_dn_tokens);
  
      if (is_gluecluster_info_dn(ldap_dn_tokens)) {

        ClassAdPtr ad(create_classad_from_ldap_entry(ld, lde));
        process_cluster_info(ldap_dn_tokens, ad, bdii_info);
      } 
      else if (is_gluece_info_dn(ldap_dn_tokens)) {

        ClassAdPtr ad(create_classad_from_ldap_entry(ld, lde));
        process_ce_info(ldap_dn_tokens, ad, bdii_info);
      }
      else if (is_gluesubcluster_info_dn(ldap_dn_tokens)) {
        ClassAdPtr ad(create_classad_from_ldap_entry(ld, lde));
        process_subcluster_info(ldap_dn_tokens, ad, bdii_info);
      }
      else if (is_gluecesebind_info_dn(ldap_dn_tokens)) {
        ClassAdPtr ad(create_classad_from_ldap_entry(ld, lde));
        std::string const ce_id(
          ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=")+1)
        );
        bdii_info.ces[ce_id].ses_binds.push_back(ad);
      }
      else if (is_gluevoview_info_dn(ldap_dn_tokens)) {
        ClassAdPtr ad(create_classad_from_ldap_entry(ld, lde));
        std::string const voview_id(
          ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=")+1)
        );
        std::string const ce_id(
          ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=")+1)
        );
        bdii_info.voviews[ce_id].push_back(VoViewInfo(voview_id, ad));
      }
    }
  }
  Debug("#" << n_entries << " LDAP entries received in " << std::time(0) - t0 << " seconds");
  ut::scope_guard ldresult_guard(
    boost::bind(ldap_msgfree, ldresult)
  );
  if (result == -1) {
/*
    Error(
      std::string("ldap_result error: ").append(
        ldap_err2string(
          ldap_result2error(ld.get(), ldresult, 0)
        )
      )
    );
*/
    return;
  }
 
  ClusterInfoMap::const_iterator cl_it(bdii_info.clusters.begin());
  ClusterInfoMap::const_iterator const cl_e(bdii_info.clusters.end());
  time_t const t1 = std::time(0);
  for( ; cl_it != cl_e; ++cl_it) { // for each cluster
        
    if (cl_it->second.subclusters_links.empty()) {
      continue; // Addresses bug #15098
    }

    std::vector<SubClusterInfoMap::iterator>::const_iterator sc_it(
      cl_it->second.subclusters_links.begin()
    );

    boost::shared_ptr<classad::ClassAd> sc_ad = (*sc_it)->second;

    std::string const cluster_id(cl_it->first);
    std::vector<CEInfoMap::iterator>::const_iterator ce_it(
      cl_it->second.ces_links.begin()
    );
    std::vector<CEInfoMap::iterator>::const_iterator const ce_e(
      cl_it->second.ces_links.end()
    );
        
    for( ; ce_it != ce_e; ++ce_it) {

      (*ce_it)->second.ad->Update(*sc_ad);
      (*ce_it)->second.ad->InsertAttr(
        "GlueSiteUniqueID", cl_it->second.site_id
      );
      std::vector<std::string> sebind;
      std::vector<classad::ExprTree*>  exprs;
      std::vector<ClassAdPtr>::const_iterator se_it(
        (*ce_it)->second.ses_binds.begin()
      );
      std::vector<ClassAdPtr>::const_iterator const se_e(
        (*ce_it)->second.ses_binds.end()
      );
          
      for( ; se_it != se_e; ++se_it) {
        exprs.push_back(new classad::ClassAd);
        static_cast<classad::ClassAd*>(exprs.back())->Update(*se_it->get());
 
        std::string cesebind_se_id;
        static_cast<classad::ClassAd*>(exprs.back())->EvaluateAttrString(
            "GlueCESEBindSEUniqueID", cesebind_se_id
        );
        sebind.push_back(cesebind_se_id);

        static_cast<classad::ClassAd*>(exprs.back())->Insert(
          "name", classad::AttributeReference::MakeAttributeReference(
            0, "GlueCESEBindSEUniqueID"
        ));
        static_cast<classad::ClassAd*>(exprs.back())->Insert(
          "mount", classad::AttributeReference::MakeAttributeReference(
            0,"GlueCESEBindCEAccesspoint"
        ));
     }
     (*ce_it)->second.ad->Insert(
       "CloseStorageElements", classad::ExprList::MakeExprList(exprs)
     );
     (*ce_it)->second.ad->Insert(
       "GlueCESEBindGroupSEUniqueID", cu::asExprList(sebind)
     );
     try {  
       ism::purchaser::expand_glueceid_info((*ce_it)->second.ad);
       ism::purchaser::insert_gangmatch_storage_ad((*ce_it)->second.ad);
     }
     catch(cu::InvalidValue) {
//        Error("Cannot extract GlueCEUniqueID from Ad");
       continue;
     }

     VoViewInfoMap::const_iterator const vo_views(
       bdii_info.voviews.find((*ce_it)->first)
     );
          
     if (vo_views!=bdii_info.voviews.end() && !vo_views->second.empty()) {
     
       std::set<std::string> left_acbr;

       cu::EvaluateAttrList(
        *((*ce_it)->second.ad), "GlueCEAccessControlBaseRule", left_acbr
       );
       std::vector<VoViewInfo>::const_iterator vo_view_it(
         vo_views->second.begin()
       );
       std::vector<VoViewInfo>::const_iterator const vo_view_e(
         vo_views->second.end()
       );

       for ( ; vo_view_it!=vo_view_e; ++vo_view_it) {
              
         std::set<std::string> view_acbr; 
         cu::EvaluateAttrList(
           *vo_view_it->ad, "GlueCEAccessControlBaseRule", view_acbr
         );

         if (view_acbr.empty()) {
//            Info(vo_view_it->id << ": skipped due to empty ACBR.");
           continue;
         }
         
         std::set<std::string> _;
         std::set_difference(
          left_acbr.begin(), left_acbr.end(), 
          view_acbr.begin(), view_acbr.end(),
            std::insert_iterator<std::set<std::string> >(_, _.begin())
         );
                 
         ClassAdPtr viewAd(
           dynamic_cast<classad::ClassAd*>((*ce_it)->second.ad->Copy())
         );
         viewAd->Update(*vo_view_it->ad);
         ce_info_container.insert(std::make_pair(
           (*ce_it)->first + "/" + boost::algorithm::trim_left_copy_if(
              vo_view_it->id, boost::algorithm::is_any_of("/")),viewAd
         ));
       }
       if (!left_acbr.empty()) {
         std::vector<std::string> v(left_acbr.begin(), left_acbr.end());
         (*ce_it)->second.ad->Insert(
           "GlueCEAccessControlBaseRule", cu::asExprList(v)
         );
         ce_info_container.insert(
           std::make_pair((*ce_it)->first, (*ce_it)->second.ad)
         );  
       }
     }
     else {
       ce_info_container.insert( 
         std::make_pair((*ce_it)->first, (*ce_it)->second.ad)
       );
     }
    }
  }
  Debug("ClassAd representation built in " << std::time(0) - t1 << " seconds");
}

void fetch_bdii_info(
  std::string const& hostname,
  size_t port,
  std::string const& dn,
  time_t timeout,
  std::string const& ldap_ce_filter_ext,
  ism::purchaser::PurchaserInfoContainer& ce_info_container,
  ism::purchaser::PurchaserInfoContainer& se_info_container)
{
  fetch_bdii_ce_info(hostname, port, dn, timeout, ldap_ce_filter_ext, ce_info_container);
  fetch_bdii_se_info(hostname, port, dn, timeout, se_info_container);
}
}
}}}}

