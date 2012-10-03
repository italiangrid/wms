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

// File: common.cpp
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id: common.cpp,v 1.8.2.4.6.2.4.3 2012/06/22 11:51:31 mcecchi Exp $

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/regex.hpp>

#include "glite/wms/ism/ism.h"
#include "glite/wms/ism/purchaser/common.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"

#include "schema_utils.h"

using namespace std;
namespace utils = glite::wmsutils::classads;
namespace ba = boost::algorithm;

namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

bdii_schema_info_type& bdii_schema_info()
{
  static bdii_schema_info_type glue_schema("GLUE");
  return glue_schema;
}

bdii_schema_info_type& bdii_schema_info_g2()
{
  static bdii_schema_info_type glue2_schema("GLUE2");
  return glue2_schema;
}

boost::unordered_map<
  boost::flyweight<std::string>,
  boost::flyweight<std::string>,
  flyweight_hash
> classad2flyweight(ad_ptr ad)
{
    boost::unordered_map<
      boost::flyweight<std::string>,
      boost::flyweight<std::string>,
      flyweight_hash
    > indexed_ce_info;
    for (
      classad::ClassAd::iterator iter(ad->begin());
      iter != ad->end();
      ++iter
    ) {
      classad::ClassAd* ad_value(static_cast<classad::ClassAd*>(iter->second));
      indexed_ce_info[boost::flyweight<std::string>(iter->first)] // key
        = boost::flyweight<std::string>(utils::unparse_classad(*ad_value)); // value
    }

  return indexed_ce_info;
}

void merge_ism(
  boost::unordered_map<
    boost::flyweight<std::string>,
    boost::flyweight<std::string>,
    flyweight_hash
  >& keyvalue_info,
  ism_type::iterator const& it)
{
  for (
    boost::unordered_map<
      boost::flyweight<std::string>,
      boost::flyweight<std::string>,
      flyweight_hash>::iterator iter(keyvalue_info.begin());
    iter != keyvalue_info.end();
    ++iter
  ) {
    // merge GLUE1.3 (ism_entry) with GLUE2.0 representation
    boost::tuples::get<keyvalue_info_entry>(it->second)[iter->first] = iter->second;
  }
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueCECapability")
  ] = boost::flyweight<std::string>("GLUE2.Computing.Endpoint.Capability");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueCEImplementationName")
  ] = boost::flyweight<std::string>("GLUE2.Computing.Endpoint.ImplementationName");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueCEImplementationVersion")
  ] = boost::flyweight<std::string>("GLUE2.Computing.Endpoint.ImplementationVersion");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueCEInfoLRMSVersion")
  ] = boost::flyweight<std::string>("GLUE2.Manager.ProductVersion");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueCEPolicyMaxWallClockTime")
  ] = boost::flyweight<std::string>("GLUE2.Computing.Share.MaxWallTime");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueCEPolicyMaxCPUTime")
  ] = boost::flyweight<std::string>("GLUE2.Computing.Share.MaxCPUTime");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueCEPolicyMaxRunningJobs")
  ]  = boost::flyweight<std::string>("GLUE2.Computing.Share.MaxRunningJobs");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueCEStateStatus")
  ] = boost::flyweight<std::string>("GLUE2.Computing.Share.ServingState");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueCEStateWaitingJobs")
  ] = boost::flyweight<std::string>("GLUE2.Computing.Share.WaitingJobs");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueCEStateTotalJobs")
  ] = boost::flyweight<std::string>("GLUE2.Computing.Share.TotalJobs");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueCEStateFreeJobSlots")
  ] = boost::flyweight<std::string>("GLUE2.Computing.Share.FreeSlots");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueCEStateRunningJobs")
  ] = boost::flyweight<std::string>("GLUE2.Computing.Share.RunningJobs");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueCEStateEstimatedResponseTime")
  ] = boost::flyweight<std::string>("GLUE2.Computing.Share.EstimatedAverageWaitingTime");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueCEStateWorstResponseTime")
  ] = boost::flyweight<std::string>("GLUE2.Computing.Share.EstimatedWorstWaitingTime");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueHostArchitecturePlatformType")
  ] = boost::flyweight<std::string>("GLUE2.ExecutionEnvironment.Platform");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueHostArchitectureSMPSize")
  ] = boost::flyweight<std::string>("GLUE2.ExecutionEnvironment.OtherInfo.SmpSize");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueHostProcessorModel")
  ] = boost::flyweight<std::string>("GLUE2.ExecutionEnvironment.CPUModel");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueHostProcessorVendor")
  ] = boost::flyweight<std::string>("GLUE2.ExecutionEnvironment.CPUVendor");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueHostProcessorClockSpeed")
  ] = boost::flyweight<std::string>("GLUE2.ExecutionEnvironment.CPUClockSpeed");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueHostOperatingSystemName")
  ] = boost::flyweight<std::string>("GLUE2.ExecutionEnvironment.OSName");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueHostMainMemoryRAMSize")
  ] = boost::flyweight<std::string>("GLUE2.ExecutionEnvironment.MainMemorySize");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueHostMainMemoryVirtualSize")
  ] = boost::flyweight<std::string>("GLUE2.ExecutionEnvironment.VirtualMemorySize");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueHostNetworkAdapterInboundIP")
  ] = boost::flyweight<std::string>("GLUE2.ExecutionEnvironment.ConnectivityIn");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueHostNetworkAdapterOutboundIP")
  ] = boost::flyweight<std::string>("GLUE2.ExecutionEnvironment.ConnectivityOut");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueSubClusterLogicalCPUs")
  ] = boost::flyweight<std::string>("GLUE2.ExecutionEnvironment.LogicalCPUs");
  boost::tuples::get<keyvalue_info_entry>(it->second)[
    boost::flyweight<std::string>("GlueSubClusterPhysicalCPUs")
  ] = boost::flyweight<std::string>("GLUE2.ExecutionEnvironment.PhysicalCPUs");
}

inline bool iequals(std::string const& a, std::string const& b)
{
 return ba::iequals(a,b);
}

inline bool istarts_with(std::string const& a, std::string const& b)
{
 return ba::istarts_with(a,b);
}

inline std::string strip_prefix(std::string const& prefix, std::string const& s)
{
  return boost::algorithm::istarts_with(s,prefix) ?
    boost::algorithm::erase_head_copy(s, prefix.length()) : s;
}

void insert_values(
  bool is_schema_version_20,
  std::string const& name,
  boost::shared_array<struct berval*> values,
  std::list<std::string> const& prefix,
  classad::ClassAd& ad
) {
  std::vector<std::string>::const_iterator list_b, number_b;
  std::vector<std::string>::const_iterator list_e, number_e;

  if (is_schema_version_20) {
    boost::tie(list_b, list_e) = bdii_schema_info_g2().multi_valued();
    boost::tie(number_b, number_e) = bdii_schema_info_g2().number_valued();
  } else {
    boost::tie(list_b, list_e) = bdii_schema_info().multi_valued();
    boost::tie(number_b, number_e) = bdii_schema_info().number_valued();
  }

  bool is_list = ba::iequals(name,"objectclass") || std::find_if(
    list_b, list_e, boost::bind(iequals,_1, name)
  ) != list_e;

  bool is_number = std::find_if(
    number_b, number_e, boost::bind(iequals, _1, name)
  ) != number_e;

  std::string result;
  for (size_t i = 0; values[i] != 0; ++i) {

    if (i) {
      result.append(",");
    }
    if (is_number || ba::iequals("undefined", values[i]->bv_val) ||
      ba::iequals("false", values[i]->bv_val) || ba::iequals("true", values[i]->bv_val)) {

      result.append(values[i]->bv_val);
    }
    else {
      result.append("\"").append(values[i]->bv_val).append("\"");
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

  if (e) {
    std::list<std::string>::const_iterator it =  std::find_if(
      prefix.begin(), prefix.end(),
      boost::bind(istarts_with, name, _1)
    );
    ad.Insert(
      ( it != prefix.end() ) ? strip_prefix(*it, name) : name,
      e
    );
  }
}

classad::ClassAd*
create_classad_from_ldap_entry(
  LDAP* ld,
  LDAPMessage* lde,
  std::list<std::string> prefix,
  bool is_schema_version_20
) {
  classad::ClassAd* result(new classad::ClassAd);
  BerElement* ber = 0;
  for (char* attr = ldap_first_attribute(ld, lde, &ber);
    attr;
    attr = ldap_next_attribute(ld, lde, ber)
  ) {
    boost::shared_ptr<void> attr_guard(
      attr, ber_memfree
    );
    boost::shared_array<struct berval*> values(
      ldap_get_values_len(ld, lde, attr),
      ldap_value_free_len
    );

    if (!values) continue;
    insert_values(
      is_schema_version_20, attr, values, prefix, *result
    );
  }
  boost::shared_ptr<void> ber_guard(
    static_cast<void*>(0),boost::bind(ber_free, ber, 0)
  );
  return result;
}

void apply_skip_predicate(
  ism_type& glue_info_container,
  skip_predicate_type skip,
  std::string const& purchasedby
)
{
  ism_type::iterator it(glue_info_container.begin());
  ism_type::iterator const glue_info_container_end(
    glue_info_container.end()
  );

  while (it != glue_info_container_end) {
    if (!skip(it->first)) {
      ++it;
    } else {
      Debug("Skipping " << it->first << " due to skip predicate settings");
      glue_info_container.erase(it++);
    }
  }
}

void tokenize_ldap_dn(std::string const& s, std::vector<std::string> &v)
{
  boost::escaped_list_separator<char> ldap_dn_sep("",",","");
  boost::tokenizer<boost::escaped_list_separator<char> >
    ldap_dn_tok(s,ldap_dn_sep);
  boost::tokenizer< boost::escaped_list_separator<char> >::iterator
    ldap_dn_tok_it(
      ldap_dn_tok.begin()
    );
  boost::tokenizer< boost::escaped_list_separator<char> >::iterator const
    ldap_dn_tok_end(
       ldap_dn_tok.end()
    );
  for ( ; ldap_dn_tok_it != ldap_dn_tok_end; ++ldap_dn_tok_it) {
    v.push_back(
      boost::algorithm::trim_copy(*ldap_dn_tok_it)
    );
  }
}

namespace {

std::string const gangmatch_storage_ad_str(
  "["
  "  storage =  ["
  "     VO = parent.other.VirtualOrganisation;"
  "     CloseSEs = retrieveCloseSEsInfo( VO );"
  "  ];"
  "]"
);
boost::shared_ptr<classad::ClassAd> gangmatch_storage_ad;

}

void insert_gangmatch_storage_ad(classad::ClassAd& glue_info)
{
  if(!gangmatch_storage_ad) {
    gangmatch_storage_ad.reset(
      utils::parse_classad(gangmatch_storage_ad_str)
    );
  }
  glue_info.Update(*gangmatch_storage_ad);
}

void insert_gangmatch_storage_ad(ad_ptr glue_info)
{
  if(!gangmatch_storage_ad) {
    gangmatch_storage_ad.reset(
      utils::parse_classad(gangmatch_storage_ad_str)
    );
  }
  glue_info->Update(*gangmatch_storage_ad);
}

bool expand_glueid_info(classad::ClassAd& glue_info)
{
  string ce_str;
  ce_str.assign(utils::evaluate_attribute(glue_info, "GlueCEUniqueID"));
  static boost::regex  expression_ceid("(.+/[^\\-]+-([^\\-]+))-(.+)");
  boost::smatch  pieces_ceid;
  string gcrs, type, name;

  if (boost::regex_match(ce_str, pieces_ceid, expression_ceid)) {

    gcrs.assign(pieces_ceid[1].first, pieces_ceid[1].second);
    try {
      type.assign(utils::evaluate_attribute(glue_info, "GlueCEInfoLRMSType"));
    } catch(utils::InvalidValue const& e) {
      // Try to fall softly in case the attribute is missing...
      type.assign(pieces_ceid[2].first, pieces_ceid[2].second);
      Warning("Cannot evaluate GlueCEInfoLRMSType using value from contact string: " << type);
    }
    // ... or in case the attribute is empty.
    if (type.length() == 0) type.assign(pieces_ceid[2].first, pieces_ceid[2].second);
    name.assign(pieces_ceid[3].first, pieces_ceid[3].second);
  } else {
    Warning("Cannot parse CEid=" << ce_str);
    return false;
  }

  glue_info.InsertAttr("GlobusResourceContactString", gcrs);
  glue_info.InsertAttr("LRMSType", type);
  glue_info.InsertAttr("QueueName", name);
  glue_info.InsertAttr("CEid", ce_str);

  return true;
}

bool expand_glueid_info(ad_ptr glue_info)
{
  string ce_str;
  ce_str.assign(utils::evaluate_attribute(*glue_info, "GlueCEUniqueID"));
  static boost::regex  expression_ceid("(.+/[^\\-]+-([^\\-]+))-(.+)");
  boost::smatch  pieces_ceid;
  string gcrs, type, name;

  if (boost::regex_match(ce_str, pieces_ceid, expression_ceid)) {

    gcrs.assign(pieces_ceid[1].first, pieces_ceid[1].second);
    try {
      type.assign(utils::evaluate_attribute(*glue_info, "GlueCEInfoLRMSType"));
    } catch(utils::InvalidValue& e) {
      // Try to fall softly in case the attribute is missing...
      type.assign(pieces_ceid[2].first, pieces_ceid[2].second);
      Warning("Cannot evaluate GlueCEInfoLRMSType using value from contact string: " << type);
    }
    // ... or in case the attribute is empty.
    if (type.length() == 0) type.assign(pieces_ceid[2].first, pieces_ceid[2].second);
    name.assign(pieces_ceid[3].first, pieces_ceid[3].second);
  } else {
    Warning("Cannot parse CEid=" << ce_str);
    return false;
  }

  glue_info->InsertAttr("GlobusResourceContactString", gcrs);
  glue_info->InsertAttr("LRMSType", type);
  glue_info->InsertAttr("QueueName", name);
  glue_info->InsertAttr("CEid", ce_str);

  return true;
}

bool split_information_service_url(
  classad::ClassAd const& ad,
  boost::tuple<std::string, int, std::string>& i)
{
 try {
  std::string ldap_dn;
  std::string ldap_host;
  std::string ldap_url;

  ldap_url.assign( utils::evaluate_attribute(ad, "GlueInformationServiceURL") );
  static boost::regex expression_gisu( "\\S.*://(.*):([0-9]+)/(.*)" );
  boost::smatch pieces_gisu;
  std::string port;

  if (boost::regex_match(ldap_url, pieces_gisu, expression_gisu)) {

    ldap_host.assign (pieces_gisu[1].first, pieces_gisu[1].second);
    port.assign      (pieces_gisu[2].first, pieces_gisu[2].second);
    ldap_dn.assign   (pieces_gisu[3].first, pieces_gisu[3].second);

    i = boost::make_tuple(ldap_host, std::atoi(port.c_str()), ldap_dn);
  }
  else {
    return false;
  }
 } catch (utils::InvalidValue const& e) {
   return false;
 }
 return true;
}

}}}}
