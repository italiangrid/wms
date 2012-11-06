/* Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
#include <algorithm>
#include <boost/mem_fn.hpp>
#include <boost/tokenizer.hpp>
#include <boost/progress.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/bind.hpp>
#include <ldap.h>

#include "ldap-dn-utils.h"
#include "glite/wms/ism/purchaser/common.h"

#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wmsutils/classads/classad_utils.h"

using namespace std;
namespace classadutils = glite::wmsutils::classads;

namespace glite {
namespace wms {

namespace ism {
namespace purchaser {

typedef boost::shared_ptr<classad::ClassAd> classad_shared_ptr;
typedef std::vector<classad_shared_ptr> vector_classad_shared_ptr;
typedef std::pair<std::string, classad_shared_ptr> pair_string_classad_shared_ptr;

typedef std::map<
  std::string, // ce id
  std::pair<classad_shared_ptr, vector_classad_shared_ptr> // cead, closes bind ad
> gluece_info_map_type;

typedef std::map<
  std::string,       // subcluster id
  classad_shared_ptr // subcluster ad
> gluesubcluster_info_map_type;

typedef std::map<
  std::string, // cluster id
  boost::tuple<
    string, // site name
    vector<gluece_info_map_type::iterator>,         // list of ce in cluster
    vector<gluesubcluster_info_map_type::iterator> // list of subcluster in cluster
  >
> gluecluster_info_map_type;

typedef std::map<
  std::string, // ce id
  std::vector<pair_string_classad_shared_ptr> // list of ce views per vo
> gluece_voview_info_map_type;

typedef std::map<
  std::string, // se id
  boost::tuple<
    classad_shared_ptr, // sead
    std::vector<classad::ExprTree*>, // list of sa info
    std::vector<classad::ExprTree*>, // list of control protocol info
    std::vector<classad::ExprTree*>, // list of access  protocol info
    vector_classad_shared_ptr
  >
> gluese_info_map_type;

namespace {

bool is_access_control_vo_rule(const string& r)
{
 return boost::algorithm::istarts_with(r, "VO:");
}

string get_cluster_name(boost::shared_ptr<classad::ClassAd> ce_ad)
{
  if (!ce_ad) {
    return std::string();
  }
  std::string cluster;
  ce_ad->EvaluateAttrString("GlueCEInfoHostName", cluster);
  try {
    std::vector<std::string> foreignKeys;
    classadutils::EvaluateAttrList(*ce_ad, "GlueForeignKey", foreignKeys);
    static boost::regex get_cluid("GlueClusterUniqueID\\s*=\\s*([^\\s]+)");
    boost::smatch result_cluid;
    bool found = false;

    // Looking for one GlueForeignKey (it is possibly multi-valued)
    // With the specified attribute.
    for (std::vector<std::string>::const_iterator key = foreignKeys.begin();
      key != foreignKeys.end(); key++)
    {
      if (boost::regex_match(*key, result_cluid, get_cluid)) {
        cluster.assign(result_cluid[1].first,result_cluid[1].second);
        found = true;
        break;
      }
    }

    if (!found) {

      Warning("Cannot find GlueClusterUniqueID assignment. Using "
        << cluster << ".");
    }
  } catch (boost::bad_expression const& e) {

    Error("Bad regular expression. Cannot parse GlueForeignKey. Using " << cluster << ".");
  } catch (...) {

    // The GlueForeignKey was not found.
    // We keep the gatekeeper name.
  }

  return cluster;
}

string get_site_name(boost::shared_ptr<classad::ClassAd> cluster_ad)
{
  if (!cluster_ad) {
    return std::string();
  }
  std::string site;
  try {
    std::vector<std::string> foreignKeys;
    classadutils::EvaluateAttrList(*cluster_ad, "GlueForeignKey", foreignKeys);
    static boost::regex get_gsuid("GlueSiteUniqueID\\s*=\\s*([^\\s]+)");
    boost::smatch result_gsuid;
    bool found = false;
    std::vector<std::string>::const_iterator key_it = foreignKeys.begin();
    std::vector<std::string>::const_iterator const key_e = foreignKeys.end();

    // Looking for one GlueForeignKey (it is possibly multi-valued)
    // With the specified attribute.
    for ( ; key_it != key_e; ++key_it) {

      if (boost::regex_match(*key_it, result_gsuid, get_gsuid)) {
        site.assign(result_gsuid[1].first,result_gsuid[1].second);
        found = true;
        break;
      }
    }
    if (!found) {

      Warning("Cannot find GlueSiteUniqueID assignment for " << *cluster_ad);
    }
  } catch (boost::bad_expression const& e){

    Error("Bad regular expression. Cannot parse GlueForeignKey.");
  } catch (...) {

    // The GlueForeignKey was not found.
    // We keep the gatekeeper name.
  }
  return site;
}

} // anonymous

void 
fetch_bdii_se_info(
  std::string const& hostname,
  size_t port,
  std::string const& dn,
  time_t timeout,
  gluese_info_container_type& gluese_info_container)
{
  string filter("(|(objectclass=gluecesebind)(|(objectclass=gluese)(|(objectclass=gluesa)"
    "(|(objectclass=glueseaccessprotocol)(objectclass=gluesecontrolprotocol)))))");
  
  LDAP* ld = 0;
  int result = ldap_initialize(
    &ld,
    std::string("ldap://" + hostname + ":" + boost::lexical_cast<std::string>(port)).c_str()
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
    return;
  }

  struct timeval to;
  to.tv_sec = timeout;
  to.tv_usec = 0L;

  ldap_set_option(ld, LDAP_OPT_NETWORK_TIMEOUT, &to);
  LDAPMessage *ldresult = 0;
  if ((result = ldap_search_ext_s(
    ld,
    dn.c_str(),
    LDAP_SCOPE_SUBTREE,
    filter.c_str(),
    0,
    false,
    0,
    0,
    &to,
    LDAP_NO_LIMIT,
    &ldresult) != LDAP_SUCCESS)
  ) {
    throw LDAPException(
      std::string("ldap_search error: ").append(
        ldap_err2string(result)
      )
    );
    return;
  }

  boost::shared_ptr<void> ldresult_guard(
    ldresult, ldap_msgfree
  );
  std::vector<std::string> ldap_dn_tokens;
  gluese_info_map_type gluese_info_map;
  for (
    LDAPMessage* lde = ldap_first_entry(ld, ldresult);
    lde != 0; lde = ldap_next_entry(ld, lde)
  )
  {
    boost::shared_ptr<char> dn_str(
      ldap_get_dn(ld, lde),
      ber_memfree
    );
    vector<string> ldap_dn_tokens;
    tokenize_ldap_dn(dn_str.get(), ldap_dn_tokens);

    if (is_gluese_info_dn(ldap_dn_tokens) ||
      is_gluecesebind_info_dn(ldap_dn_tokens)) {

      boost::shared_ptr<classad::ClassAd> seAd(
        create_classad_from_ldap_entry(
            ld, lde, std::list<std::string>()
        )
      );
      cleanup_glue_info(seAd, std::list<std::string>());
      string const gluese_unique_id(
        ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=") + 1)
      );
      gluese_info_map_type::iterator it;
      bool gluese_info_map_insert;
      boost::tie(it, gluese_info_map_insert) = gluese_info_map.insert(
        std::make_pair(
          gluese_unique_id, 
          boost::make_tuple(
            ad_ptr(),
            std::vector<classad::ExprTree*>(),
            std::vector<classad::ExprTree*>(),
            std::vector<classad::ExprTree*>(),
            vector_classad_shared_ptr()
          )
        )
      );
      if (is_gluese_info_dn(ldap_dn_tokens)) {
        boost::tuples::get<0>(it->second) = seAd;
      } else if (is_gluecesebind_info_dn(ldap_dn_tokens)) {
        boost::tuples::get<4>(it->second).push_back(seAd);
      }
    } else if (
      is_gluesa_info_dn(ldap_dn_tokens) ||
      is_gluese_control_protocol_info_dn(ldap_dn_tokens) ||
      is_gluese_access_protocol_info_dn(ldap_dn_tokens)) {

      classad::ExprTree* ad(
        create_classad_from_ldap_entry(
            ld, lde, std::list<std::string>()
        )
      ); // freed up when seAd is deleted
      //cleanup_glue_info(, std::list<std::string>());

      string const gluese_unique_id(
        ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=") + 1)
      );
      gluese_info_map_type::iterator it(
        gluese_info_map.find(gluese_unique_id)
      );
      if (it == gluese_info_map.end()) {
        bool gluese_info_map_insert; 
        boost::tie(it, gluese_info_map_insert) = gluese_info_map.insert(
          std::make_pair(
            gluese_unique_id,
            boost::make_tuple(
              ad_ptr(),
              std::vector<classad::ExprTree*>(),
              std::vector<classad::ExprTree*>(),
              std::vector<classad::ExprTree*>(),
              vector_classad_shared_ptr()
            )
          )
        );
      }
      if(is_gluesa_info_dn(ldap_dn_tokens)) {
        boost::tuples::get<1>(it->second).push_back(ad);
      } else if(is_gluese_control_protocol_info_dn(ldap_dn_tokens)) {
        boost::tuples::get<2>(it->second).push_back(ad);
      } else if(is_gluese_access_protocol_info_dn(ldap_dn_tokens)) {
        boost::tuples::get<3>(it->second).push_back(ad);
      }
    }
  } // for()

  gluese_info_map_type::const_iterator se_it(
    gluese_info_map.begin()
  );
  gluese_info_map_type::const_iterator const se_e(
    gluese_info_map.end()
  );

  for ( ; se_it != se_e; ++se_it) {

    if (!boost::tuples::get<0>(se_it->second)) {
      Debug("Skipping se " << se_it->first << " due to empty classad representation ");
      continue;
    }       
    if (!boost::tuples::get<1>(se_it->second).empty()) {
      boost::tuples::get<0>(se_it->second)->
        Insert("GlueSA", classad::ExprList::MakeExprList(
          boost::tuples::get<1>(se_it->second)
        ));
    }
    if (!boost::tuples::get<2>(se_it->second).empty()) {
      boost::tuples::get<0>(se_it->second)->
        Insert("GlueSEControlProtocol", classad::ExprList::MakeExprList(
          boost::tuples::get<2>(se_it->second)
        ));
    }
    if (!boost::tuples::get<3>(se_it->second).empty()) {
      boost::tuples::get<0>(se_it->second)->
        Insert("GlueSEAccessProtocol", classad::ExprList::MakeExprList(
          boost::tuples::get<3>(se_it->second)
        ));
    }
    std::vector<ad_ptr>::const_iterator it(
      boost::tuples::get<4>(se_it->second).begin()
    );
    std::vector<ad_ptr>::const_iterator const e(
      boost::tuples::get<4>(se_it->second).end()
    );
    std::vector<classad::ExprTree*> exprs;

    for ( ; it != e; ++it ) {
      
      classad::ClassAd* ad = static_cast<classad::ClassAd*>(it->get()->Copy());
      ad->Insert("name", classad::AttributeReference::MakeAttributeReference(
       0, "GlueCESEBindSEUniqueID"
      ));
      ad->Insert("mount", classad::AttributeReference::MakeAttributeReference(
       0,"GlueCESEBindCEAccesspoint"
      ));
      exprs.push_back(ad);
    }
    boost::tuples::get<0>(se_it->second)->Insert(
      "CloseComputingElements", classad::ExprList::MakeExprList(exprs)
    );

    gluese_info_container[se_it->first] =
      boost::tuples::get<0>(se_it->second);
  }
}

void 
fetch_bdii_ce_info(
  std::string const& hostname,
  size_t port,
  std::string const& dn,
  time_t timeout,
  std::string const& ldap_ce_filter_ext,
  gluece_info_container_type& gluece_info_container)
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

  LDAP* ld = 0;
  int result = ldap_initialize(
    &ld,
    std::string("ldap://" + hostname + ":" + boost::lexical_cast<std::string>(port)).c_str()
  );
  boost::shared_ptr<void> ld_guard(
    static_cast<void*>(0),
    boost::bind(ldap_unbind_ext, ld, (LDAPControl**)(0), (LDAPControl**)(0))
  );
  if (result != LDAP_SUCCESS ) {

    throw LDAPException(
      std::string("ldap_simple_bind_s errorerror: ").append(
        ldap_err2string(result)
      )
    );
    return;
  }

  struct timeval to;
  to.tv_sec = timeout;
  to.tv_usec = 0L;

  ldap_set_option(ld, LDAP_OPT_NETWORK_TIMEOUT, &to);
  LDAPMessage *ldresult = 0;
  if ((result = ldap_search_ext_s(
    ld,
    dn.c_str(),
    LDAP_SCOPE_SUBTREE,
    filter.c_str(),
    0,
    false,
    0,
    0,
    &to,
    LDAP_NO_LIMIT,
    &ldresult) != LDAP_SUCCESS)
  ) {
    throw LDAPException(
      std::string("ldap_search error: ").append(
        ldap_err2string(result)
      )
    );
    return;
  }
  boost::shared_ptr<void> ldresult_guard(
    ldresult, ldap_msgfree
  );

  gluece_info_map_type         gluece_info_map;
  gluesubcluster_info_map_type gluesubcluster_info_map;
  gluecluster_info_map_type    gluecluster_info_map;
  gluece_voview_info_map_type  gluece_voview_info_map;

  for (
    LDAPMessage* lde = ldap_first_entry(ld, ldresult);
    lde != 0;
    lde = ldap_next_entry(ld, lde))
  {
    boost::shared_ptr<char> dn_str(
      ldap_get_dn(ld, lde),
      ber_memfree
    );
    std::vector<string> ldap_dn_tokens;
    tokenize_ldap_dn(dn_str.get(), ldap_dn_tokens);
  
    if (is_gluecluster_info_dn(ldap_dn_tokens)) {
      string glue_cluster_unique_id(
        ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=") + 1)
      );
      boost::shared_ptr<classad::ClassAd> cluster_ad(
        create_classad_from_ldap_entry(
          ld, lde, std::list<std::string>()
        )
      );
      cleanup_glue_info(cluster_ad, std::list<std::string>());
      string glue_site_unique_id(get_site_name(cluster_ad));
      gluecluster_info_map_type::iterator it;
      bool gluecluster_info_map_insert;
      boost::tie(it, gluecluster_info_map_insert) =
        gluecluster_info_map.insert(
          std::make_pair(glue_cluster_unique_id,
            boost::make_tuple(
              glue_site_unique_id,
              vector<gluece_info_map_type::iterator>(),
              vector<gluesubcluster_info_map_type::iterator>()
            )
          )
        );
      if (!gluecluster_info_map_insert) {
        boost::tuples::get<0>(it->second) = glue_site_unique_id;
      }
    } else if (is_gluece_info_dn(ldap_dn_tokens)) {
      boost::shared_ptr<classad::ClassAd> ceAd(
        create_classad_from_ldap_entry(
            ld, lde, std::list<std::string>()
        )
      );
      cleanup_glue_info(ceAd, std::list<std::string>());
      string gluece_unique_id(
        ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=") + 1)
      );
      string glue_cluster_unique_id(get_cluster_name(ceAd));
      gluece_info_map_type::iterator it;
      bool gluece_info_map_insert;
      boost::tie(it, gluece_info_map_insert) =
        gluece_info_map.insert(
          std::make_pair(gluece_unique_id, 
          std::make_pair(ceAd, vector_classad_shared_ptr())
        )
      );
      if (!gluece_info_map_insert) {
        it->second.first = ceAd;
      }
      boost::tuples::get<1>(
        gluecluster_info_map[glue_cluster_unique_id]
      ).push_back(it);
    } else if (is_gluesubcluster_info_dn(ldap_dn_tokens)) {
      boost::shared_ptr<classad::ClassAd> scAd(
       create_classad_from_ldap_entry(
            ld, lde, std::list<std::string>()
        )
      );
      cleanup_glue_info(scAd, std::list<std::string>());
      string gluesubcluster_unique_id(
        ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=")+1)
      );
      string glue_cluster_unique_id(
        ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=")+1)
      );
      gluesubcluster_info_map_type::iterator it;
      bool gluesubcluster_info_map_insert;
      boost::tie(it, gluesubcluster_info_map_insert) =
        gluesubcluster_info_map.insert(
          std::make_pair(gluesubcluster_unique_id, scAd)
        );
      if (gluesubcluster_info_map_insert) 
        boost::tuples::get<2>(
          gluecluster_info_map[glue_cluster_unique_id]
        ).push_back(it);
    } else if (is_gluecesebind_info_dn(ldap_dn_tokens)) {
      boost::shared_ptr<classad::ClassAd> bnAd(
       create_classad_from_ldap_entry(
            ld, lde, std::list<std::string>()
        )
      );
      cleanup_glue_info(bnAd, std::list<std::string>());
      string glueceuniqueid(
        ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=") + 1)
      );
      gluece_info_map[glueceuniqueid].second.push_back(bnAd);
    } else if (is_gluevoview_info_dn(ldap_dn_tokens)) {
      boost::shared_ptr<classad::ClassAd> voAd(
        create_classad_from_ldap_entry(
            ld, lde, std::list<std::string>()
        )
      );
      cleanup_glue_info(voAd, std::list<std::string>());
      string gluevoviewlocalid(
        ldap_dn_tokens[0].substr(ldap_dn_tokens[0].find("=")+1)
      );
      string glueceuniqueid(
        ldap_dn_tokens[1].substr(ldap_dn_tokens[1].find("=")+1)
      );
      gluece_voview_info_map[glueceuniqueid].push_back(
        std::make_pair(gluevoviewlocalid, voAd)
      );
    }
  } // for()
      
  gluecluster_info_map_type::const_iterator cl_it(
    gluecluster_info_map.begin()
  );

  gluecluster_info_map_type::const_iterator const cl_e(
    gluecluster_info_map.end()
  );

  for ( ; cl_it != cl_e; ++cl_it) { // for each cluster
    if (boost::tuples::get<2>(cl_it->second).empty()) {
          
      Debug("Skipping cluster " << cl_it->first << " due to empty subcluster definition");
      vector<gluece_info_map_type::iterator>::const_iterator ce_it(
        boost::tuples::get<1>(cl_it->second).begin()
      );
      vector<gluece_info_map_type::iterator>::const_iterator const ce_e( 
        boost::tuples::get<1>(cl_it->second).end()
      );
      for ( ; ce_it != ce_e ; ++ce_it) {
        Debug("Skipping ce " << (*ce_it)->first << " belonging to cluster " <<  cl_it->first);  
      }
      continue; // Addresses bug #15098
    }

    vector<gluesubcluster_info_map_type::iterator>::const_iterator sc_it(
      boost::tuples::get<2>(cl_it->second).begin()
    );

    boost::shared_ptr<classad::ClassAd> sc_ad = (*sc_it)->second;

    string cluster_id(cl_it->first);
    vector<gluece_info_map_type::iterator>::const_iterator ce_it(
      boost::tuples::get<1>(cl_it->second).begin()
    );
    vector<gluece_info_map_type::iterator>::const_iterator const ce_e(
      boost::tuples::get<1>(cl_it->second).end()
    );

    for ( ; ce_it != ce_e; ++ce_it) {

      (*ce_it)->second.first->Update(*sc_ad.get());
      (*ce_it)->second.first->InsertAttr(
        "GlueSiteUniqueID",
        boost::tuples::get<0>(cl_it->second)
      );
      std::vector<std::string> sebind;
      std::vector<classad::ExprTree*>  exprs;
      vector<classad_shared_ptr>::const_iterator se_it(
        (*ce_it)->second.second.begin()
      );
      vector<classad_shared_ptr>::const_iterator const se_e(
        (*ce_it)->second.second.end()
      );
          
      for ( ; se_it != se_e; ++se_it) {
        exprs.push_back(new classad::ClassAd);
        static_cast<classad::ClassAd*>(
          exprs.back())->Update(*se_it->get()
        );

        std::string gluecesebindseuniqueid;
        static_cast<classad::ClassAd*>(exprs.back())->
          EvaluateAttrString(
            "GlueCESEBindSEUniqueID", gluecesebindseuniqueid
        );
        sebind.push_back(gluecesebindseuniqueid);

        static_cast<classad::ClassAd*>(exprs.back())->Insert(
          "name",
          classad::AttributeReference::MakeAttributeReference(
            0, "GlueCESEBindSEUniqueID"
          )
        );
        static_cast<classad::ClassAd*>(exprs.back())->Insert(
          "mount", 
          classad::AttributeReference::MakeAttributeReference(
            0,"GlueCESEBindCEAccesspoint"
          )
        );
      }
      (*ce_it)->second.first->Insert(
        "CloseStorageElements", 
        classad::ExprList::MakeExprList(exprs)
      );
      (*ce_it)->second.first->Insert(
        "GlueCESEBindGroupSEUniqueID",
        classadutils::asExprList(sebind)
      );
      try {  
        expand_glueceid_info((*ce_it)->second.first);
        insert_gangmatch_storage_ad((*ce_it)->second.first);
      } catch(classadutils::InvalidValue) {
        Error("Cannot extract GlueCEUniqueID from Ad");
        continue;
      }

      gluece_voview_info_map_type::const_iterator const vo_views(
        gluece_voview_info_map.find((*ce_it)->first)
      );
          
      if (vo_views!=gluece_voview_info_map.end() &&
        !vo_views->second.empty()) {
            
        set<string> left_access_control_base_rules;

        classadutils::EvaluateAttrList(
          *((*ce_it)->second.first),
          "GlueCEAccessControlBaseRule",
          left_access_control_base_rules
        );
        vector<pair_string_classad_shared_ptr>::const_iterator
          vo_view_it(vo_views->second.begin());
        vector<pair_string_classad_shared_ptr>::const_iterator const
          vo_view_e(vo_views->second.end());


        for ( ; vo_view_it!=vo_view_e; ++vo_view_it) {
          
          set<string> view_access_control_base_rules; 
          classadutils::EvaluateAttrList(
            *vo_view_it->second,
            "GlueCEAccessControlBaseRule",
            view_access_control_base_rules
          );

          if (view_access_control_base_rules.empty() )
          {
            Debug("Skipping view " << vo_view_it->first << " for ce " << (*ce_it)->first << " due to empty ACBR.");
            continue;
          }
          set<string> _;
          std::set_difference(
            left_access_control_base_rules.begin(),
            left_access_control_base_rules.end(),
            view_access_control_base_rules.begin(),
            view_access_control_base_rules.end(),
            insert_iterator<set<string> >(_, _.begin())
          );

          left_access_control_base_rules.swap(_);

          classad_shared_ptr viewAd(
            dynamic_cast<classad::ClassAd*>(
              (*ce_it)->second.first->Copy()
            )
          );
          viewAd->Update(*vo_view_it->second);
          gluece_info_container.insert(
            std::make_pair(
              (*ce_it)->first + 
              "/" + 
              boost::algorithm::trim_left_copy_if(
                vo_view_it->first,
                boost::algorithm::is_any_of("/")
              ),
              viewAd
            )
          );
        }
        if (!left_access_control_base_rules.empty()) {
          std::vector<std::string> v(
            left_access_control_base_rules.begin(),
            left_access_control_base_rules.end()
          );
          (*ce_it)->second.first->Insert(
            "GlueCEAccessControlBaseRule",
            classadutils::asExprList(v)
          );
          gluece_info_container.insert(
            std::make_pair((*ce_it)->first, (*ce_it)->second.first)
          );  
        }
      } else {
        gluece_info_container.insert( 
          std::make_pair((*ce_it)->first, (*ce_it)->second.first)
        );
      }
    }
  }
}

void fetch_bdii_info(
      std::string const& hostname,
      size_t port,
      std::string const& dn,
      time_t timeout,
      std::string const& ldap_ce_filter_ext,
      gluece_info_container_type& gluece_info_container,
      gluese_info_container_type& gluese_info_container)
{
  fetch_bdii_ce_info(
    hostname,
    port,
    dn,
    timeout,
    ldap_ce_filter_ext,
    gluece_info_container
  );
  fetch_bdii_se_info(
    hostname,
    port,
    dn,
    timeout,
    gluese_info_container
  );
}

}}}}

