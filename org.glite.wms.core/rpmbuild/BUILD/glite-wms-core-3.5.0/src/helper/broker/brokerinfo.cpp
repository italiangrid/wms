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

// File: brokerinfo.cpp

#include <classad_distribution.h>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include "glite/wms/ism/ism.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "brokerinfo.h"

namespace classad_utils = glite::wmsutils::classads;

namespace glite {
namespace wms {
namespace brokerinfo {

namespace {

char const* sa_attributes[] = {
"GlueSAPath", "GlueSchemaVersionMajor", "GlueSAStateAvailableSpace",
"GlueSARoot", "GlueSAStateUsedSpace", "GlueSAPolicyMaxFileSize",
"GlueSchemaVersionMinor", "GlueSAPolicyMinFileSize", "GlueSALocalID",
"GlueSAPolicyMaxPinDuration", "GlueSAType", "GlueSAPolicyMaxData",
"GlueSAPolicyMaxNumFiles","GlueSAPolicyQuota", "GlueSAPolicyFileLifeTime",
"GlueSAAccessControlBaseRule","GlueSAUniqueID", "GlueSAName",
"GlueSATotalOnlineSize","GlueSAUsedOnlineSize","GlueSAFreeOnlineSize",
"GlueSAReservedOnlineSize","GlueSATotalNearlineSize","GlueSAUsedNearlineSize",
"GlueSAFreeNearlineSize","GlueSAReservedNearlineSize","GlueSARetentionPolicy",
"GlueSAAccessLatency", "GlueSAExpirationMode", "GlueSACapability",
0
};

char const* se_attributes[] = {
"GlueSEName", "GlueSEUniqueID", "GlueSEPort",
"GlueSESizeTotal","GlueSEArchitecture", "GlueSESizeFree",
"GlueInformationServiceURL", "GlueSEStatus","GlueSEHostingSL",
"GlueSEType","GlueSEImplementationName","GlueSEImplementationVersion",
"GlueSETotalOnlineSize","GlueSEUsedOnlineSize","GlueSETotalNearlineSize",
"GlueSEUsedNearlineSize","GlueSEStateCurrentIOLoad",
0
};

class gluesa_local_id_matches
{
  std::string m_id;
public:
  gluesa_local_id_matches(std::string const& id) 
    : m_id(id) {}
  bool operator()(classad::ExprTree* e) {
    std::string gluesa_local_id;
    static_cast<classad::ClassAd*>(e)->EvaluateAttrString(
      "GlueSALocalID", gluesa_local_id
    );
    return gluesa_local_id == m_id;
  }
};

void
insert_input_file_names_section(
  classad::ClassAd& result,
  DataInfo const& data_info
)
{
  //    InputFNs = {
  //    [ name = FN1; SFNs = {SFN1,1, SFN1,2, ..., SFN1,m1} ],
  //    [ name = FN2; SFNs = {SFN2,1, SFN2,2, ..., SFN2,m2} ],
  //    ...
  //    [ name = FNn; SFNs = {SFNn,1, SFNn,2, ..., SFNn,mn} ]
  //    };

  std::vector<classad::ExprTree*>  ifne;

  if(data_info.fm) {

    FileMapping::const_iterator fi = data_info.fm->begin();
    FileMapping::const_iterator const fend = data_info.fm->end();

    for( ; fi != fend; ++fi ) {
    
      classad::ClassAd* fad(new classad::ClassAd);
      fad->InsertAttr("name", fi->first);

      std::vector<std::string> const& sfns(fi->second);
      std::vector<std::string>::const_iterator sfni = sfns.begin();
      std::vector<std::string>::const_iterator const sfnend = sfns.end();
      std::vector<classad::ExprTree*> sfne;

      for( ; sfni != sfnend; ++sfni ) {
    
        classad::Value v;
        v.SetStringValue(*sfni);
        sfne.push_back(classad::Literal::MakeLiteral(v));
      }
      fad->Insert("SFNs", classad::ExprList::MakeExprList(sfne));
      ifne.push_back(fad);
    }
  }
  result.Insert("InputFNs", classad::ExprList::MakeExprList(ifne));
}

void
insert_storage_elements_section(  
  classad::ClassAd& result,
  DataInfo const& data_info
)
{
// [
//   StorageElements = {
//     [ name = SE1; protocols = { [ name = proto1,1; port = port1,1 ],
//                                 [ name = proto1,2; port = port1,2 ],
//                                 ...
//                                 [ name = proto1,p1; port = port1,p1 ] }
//     ],
//      ...
//     [ name = SEq; protocols = { [ name = protoq,1; port = portq,1 ],
//                                 [ name = protoq,2; port = portq,2 ],
//                                 ...
//                                 [ name = protoq,pq; port = portq,pq ] }
//     ]
//   };

  std::vector<classad::ExprTree*>  see;

  if (data_info.sm)  {

    StorageMapping::const_iterator sm_it(data_info.sm->begin());
    StorageMapping::const_iterator const sm_end(data_info.sm->end());
  
    for( ; sm_it != sm_end ; ++sm_it ) {
  
      classad::ClassAd* sad(new classad::ClassAd);

      StorageInfo::Protocols const& protocols = sm_it->second.protocols;
      sad->InsertAttr("name", sm_it->first);
      std::vector<classad::ExprTree*> pe;

      StorageInfo::Protocols::const_iterator p_it(protocols.begin());
      StorageInfo::Protocols::const_iterator const p_end(protocols.end());

      for( ; p_it != p_end ; ++p_it ) {

        std::string name;
        int port;
        boost::tie(name, port) = *p_it;
        classad::ClassAd* pad = new classad::ClassAd;
        pad->InsertAttr("name", name);
        pad->InsertAttr("port", port);
        pe.push_back(pad);
      }
      sad->Insert("protocols", classad::ExprList::MakeExprList(pe));
      see.push_back(sad);
    }
  }
  result.Insert(
    "StorageElements",
    classad::ExprList::MakeExprList(see)
  );
}

// This function is required as temporary patch to fix bug #53686 
// wich will be included in next 3.3 wms patch
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

void
fix_bug_53686(classad::ClassAd& a)
{   
  std::vector<classad::ExprTree*> ads;
  if (evaluate(a, "CloseStorageElements", ads)) {
  
    std::vector<classad::ExprTree*>::const_iterator expr_it(ads.begin());
    std::vector<classad::ExprTree*>::const_iterator const expr_e(ads.end());

    for (; expr_it != expr_e; ++expr_it) {
      if (classad_utils::is_classad(*expr_it)) {
        classad::ClassAd& ad(
          *static_cast<classad::ClassAd*>(*expr_it)
        );
        std::string id;
   ad.EvaluateAttrString("name", id);
        ad.InsertAttr("name", id);
      }
    }
  } 
} 

void retrieveCloseSEsInfo(classad::ClassAd& ad, std::string const& vo)
{
  std::vector<classad::ExprTree*> ads;
  if (evaluate(ad, "CloseStorageElements", ads)) {
          
    std::vector<classad::ExprTree*>::const_iterator it = ads.begin();
    std::vector<classad::ExprTree*>::const_iterator const e = ads.end();
    
    std::pair<boost::shared_ptr<void>, int> ret = ism::match_on_active_side();
    // ++matching threads on the currently active ISM side, as ret.first increases
    // by one the reference counting on that ISM side;
    // this will not prevent the ISM from
    // switching over with pending matches
    // insisting onto the target slice, but before
    // starting yet another purchasing (in background),
    // no pending matching_threads must exist
    ism::ism_type const& the_ism = ism::get_ism(ism::se, ret.second /* side */);

    for( ; it != e; ++it ) {
    
      if (!classad_utils::is_classad(*it)) continue;
      
      classad::ClassAd& ad(
        *static_cast<classad::ClassAd*>(*it)
      );

      ad.Insert("freespace", classad::AttributeReference::MakeAttributeReference(
        0, "GlueSAStateAvailableSpace"
      ));

      std::string name;
      ad.EvaluateAttrString("name",name);
      
      ism::ism_type::const_iterator se_it(
        the_ism.find(name)
      );
      if (se_it != the_ism.end()) {
        boost::shared_ptr<classad::ClassAd> se_ad_ptr(
          boost::tuples::get<2>(se_it->second)
        );

        for(int i=0; se_attributes[i]; ++i) {
          classad::ExprTree* e = se_ad_ptr->Lookup(se_attributes[i]);
          if(e) { 
             ad.Insert(se_attributes[i], e->Copy());
          }
        }

        std::vector<classad::ExprTree*> ads_sa;
        if (evaluate(*se_ad_ptr, "GlueSA", ads_sa)) {

           std::vector<classad::ExprTree*>::const_iterator it_sa(
             std::find_if(ads_sa.begin(), ads_sa.end(), gluesa_local_id_matches(vo))
           );
           if (it_sa != ads_sa.end()) {
             for(int i=0; sa_attributes[i]; ++i) {
               classad::ExprTree* e = static_cast<classad::ClassAd*>(*it_sa)->Lookup(sa_attributes[i]);
               if (e) {
                 ad.Insert(sa_attributes[i], e->Copy());
               }
             }
           }
        }
      }
    }
  }
}

void
insert_computing_element_section(
  classad::ClassAd& result,
  classad::ClassAd const& ce_ad,
  classad::ClassAd const& jdl_ad
)
{
  // [
  //   name = ResourceId;
  //   CloseStorageElements = {
  //     [ name = CloseSE1; mount = mountpoint1 ],
  //     [ name = CloseSE2; mount = mountpoint2 ],
  //     ...
  //     [ name = CloseSEn; mount = mountpointn ]
  //   };
  classad::ExprTree const* cse = ce_ad.Lookup("CloseStorageElements");
  classad::ExprTree const* cei = ce_ad.Lookup("GlueCEUniqueID");
  
  classad::ClassAd* ad = new classad::ClassAd;
  if (cse) {
    ad->Insert("CloseStorageElements", cse->Copy());
    fix_bug_53686(*ad); // TODO: remove this call once the relevant bug has been integrated

    std::string vo;
    jdl_ad.EvaluateAttrString("VirtualOrganisation",vo);
    retrieveCloseSEsInfo(*ad, vo);    
  }
  if (cei) {    
    ad->Insert("name", cei->Copy());
  }
  result.Insert("ComputingElement", ad);
}

} // anonymous namespace

classad::ClassAd*
create_brokerinfo(
  classad::ClassAd const& jdl_ad,
  classad::ClassAd const& ce_ad,
  DataInfo const& data_info
)
{
  classad::ClassAd* result(new classad::ClassAd);

  insert_computing_element_section(*result, ce_ad, jdl_ad);
  insert_storage_elements_section(*result, data_info);
  insert_input_file_names_section(*result, data_info);

  classad::ExprTree const* dac = jdl_ad.Lookup("DataAccessProtocol");
  if (dac) {
    result->Insert("DataAccessProtocol", dac->Copy());
  }
  classad::ExprTree const* vo = jdl_ad.Lookup("VirtualOrganisation");
  if (vo) {
    result->Insert("VirtualOrganisation", vo->Copy());
  }
  return result;
}

} // namespace brokerinfo
} // namespace wms
} // namespace glite
