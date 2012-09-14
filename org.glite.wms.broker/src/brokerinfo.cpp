// File: brokerinfo.cpp

#include <classad_distribution.h>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>

#include "glite/wms/broker/match.h"
#include "storage_utils.h"

namespace glite {
namespace wms {
namespace broker {

namespace {

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

  FileMapping::const_iterator fi = data_info.fm.begin();
  FileMapping::const_iterator const fend = data_info.fm.end();

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
  result.Insert("InputFNs", classad::ExprList::MakeExprList(ifne));
}

void
insert_storage_elements_section(  
  classad::ClassAd& result,
  DataInfo const& data_info
)
{
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
  StorageMapping::const_iterator sm_it(data_info.sm.begin());
  StorageMapping::const_iterator const sm_end(data_info.sm.end());
  
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
  result.Insert(
    "StorageElements",
    classad::ExprList::MakeExprList(see)
  );
}

void
insert_computing_element_section(
  classad::ClassAd& result,
  classad::ClassAd const& ce_ad
)
{
  //   name = ResourceId;
  //   CloseStorageElements = {
  //     [ name = CloseSE1; mount = mountpoint1 ],
  //     [ name = CloseSE2; mount = mountpoint2 ],
  //     ...
  //     [ name = CloseSEn; mount = mountpointn ]
  //   };
  classad::ExprTree const* cse = ce_ad.Lookup("CloseStorageElements");
  classad::ExprTree const* cei = ce_ad.Lookup("GlueCEUniqueID");
  if (cse && cei) {    
    result.Insert("CloseStorageElements", cse->Copy());
    result.Insert("name", cei->Copy());
  }
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

  insert_computing_element_section(*result, ce_ad);
  insert_storage_elements_section(*result, data_info);
  insert_input_file_names_section(*result, data_info);

  classad::ExprTree const* dac = jdl_ad.Lookup("DataAccessProtocol");
  if (dac) {
    result->Insert("DataAccessProtocol", dac->Copy());
  }

  return result;
}

} // namespace broker
} // namespace wms
} // namespace glite
