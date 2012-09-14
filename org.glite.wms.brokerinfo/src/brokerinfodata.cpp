// File: brokerinfodata.cpp
// Author: Monforte Salvatore, INFN
// $Id$

#include <classad_distribution.h>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include "glite/wms/brokerinfo/brokerinfo.h"

using namespace std;

namespace glite {
namespace wms {
namespace brokerinfo {

namespace {

classad::ClassAd*
make_input_file_names_section(brokerinfo::filemapping const& fm)
{
  //  [
  //    InputFNs = {
  //    [ name = FN1; SFNs = {SFN1,1, SFN1,2, ..., SFN1,m1} ],
  //    [ name = FN2; SFNs = {SFN2,1, SFN2,2, ..., SFN2,m2} ],
  //    ...
  //    [ name = FNn; SFNs = {SFNn,1, SFNn,2, ..., SFNn,mn} ]
  //    };
  //  ]
  classad::ClassAd ad;
  vector<classad::ExprTree*>  InputFN_exprs;

  filemapping::const_iterator fi = fm.begin();
  filemapping::const_iterator fe = fm.end();

  for( ; fi != fe; ++fi ) {
    
    classad::ClassAd fn_ad;

    string lfn(fi->first);
    fn_ad.InsertAttr("name", lfn);

    vector<string> const& sfns(fi->second);
    vector<string>::const_iterator sfni = sfns.begin();
    vector<string>::const_iterator const sfne = sfns.end();
    vector<classad::ExprTree*> SFN_exprs;
    for( ; sfni != sfne; ++sfni ) {
    
      classad::Value v;
      v.SetStringValue(*sfni);
      SFN_exprs.push_back(classad::Literal::MakeLiteral(v));
    }
    fn_ad.Insert(
      "SFNs", 
      classad::ExprList::MakeExprList(SFN_exprs)
    );
    InputFN_exprs.push_back( fn_ad.Copy() );
  }
  ad.Insert(
    "InputFNs", 
    classad::ExprList::MakeExprList(InputFN_exprs)
  );
  return (
    static_cast<classad::ClassAd*>(ad.Copy())
  );
}

classad::ClassAd*
make_storage_elements_section(brokerinfo::storagemapping const& sm)
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
// ]
  classad::ClassAd ad;
  std::vector<classad::ExprTree*>  StorageElement_exprs;
  brokerinfo::storagemapping::const_iterator sm_it(sm.begin());
  brokerinfo::storagemapping::const_iterator const sm_e(sm.end());
  for( ; sm_it != sm_e ; ++sm_it ) {
  
    classad::ClassAd s2p_ad;
    string name;
    vector<pair<string, int> > const& info = boost::tuples::get<0>(sm_it->second);
    s2p_ad.InsertAttr("name", sm_it->first);
    vector<classad::ExprTree*> protocol_exprs;

    vector<pair<string, int> >::const_iterator info_it(info.begin());
    vector<pair<string, int> >::const_iterator const info_e(info.end());
    for( ; info_it != info_e ; ++info_it ) {
      string protocol_name;
      int protocol_port;
      boost::tie(protocol_name, protocol_port) = *info_it;
      classad::ClassAd protocol_ad;
      protocol_ad.InsertAttr("name", protocol_name);
      protocol_ad.InsertAttr("port", protocol_port);
      protocol_exprs.push_back(protocol_ad.Copy());
    }
    s2p_ad.Insert("protocols", classad::ExprList::MakeExprList(protocol_exprs));
    StorageElement_exprs.push_back(s2p_ad.Copy());
  }
  ad.Insert(
    "StorageElements",
    classad::ExprList::MakeExprList(StorageElement_exprs)
  );
  return (
    static_cast<classad::ClassAd*>(ad.Copy())
  );

}

classad::ClassAd*
make_computing_element_section(classad::ClassAd const& ad)
{
  // [
  //   name = ResourceId;
  //   CloseStorageElements = {
  //     [ name = CloseSE1; mount = mountpoint1 ],
  //     [ name = CloseSE2; mount = mountpoint2 ],
  //     ...
  //     [ name = CloseSEn; mount = mountpointn ]
  //   };
  // ];
  classad::ExprTree* cse = ad.Lookup("CloseStorageElements");
  classad::ExprTree* cei = ad.Lookup("GlueCEUniqueID");
  if(cse && cei) {
    
    classad::ClassAd* ces(new classad::ClassAd);
    
    ces->Insert("CloseStorageElements", cse->Copy());
    ces->Insert("name", cei->Copy());
    return ces;
  }
  return 0; 
}

} // anonymous namespace

classad::ClassAd*
make_brokerinfo_ad(
    boost::shared_ptr<filemapping> fm,
    boost::shared_ptr<storagemapping> sm,
    classad::ClassAd const& ad
)
{
  classad::ClassAd biAd;
  boost::shared_ptr<classad::ClassAd> section;
  
  section.reset(make_computing_element_section(ad));
  if(section) biAd.Update(*section);
  
  section.reset(make_input_file_names_section(*fm));
  if(section) biAd.Update(*section);

  section.reset(make_storage_elements_section(*sm));
  if(section) biAd.Update(*section);
  
  return (
    static_cast<classad::ClassAd*>(biAd.Copy())
  );
}

} // namespace brokerinfo
} // namespace wms
} // namespace glite
