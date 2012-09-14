/*
 * File: retrieveCloseSEInfo.cpp
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$

#include <classad_distribution.h>
#include <fnCall.h>
#include <vector>
#include <string>
#include <numeric>
#include <functional>
#include <regex.h>
#include <glite/wms/ism/ism.h>

using namespace std;
#ifdef WANT_NAMESPACES
using namespace classad;
#endif

namespace glite {
namespace wms {
namespace classad_plugin {
namespace gangmatch {

namespace ism = glite::wms::ism;

namespace {

char const* sa_attributes[] = {
"GlueSAPath", "GlueSchemaVersionMajor", "GlueSAStateAvailableSpace",
"GlueSARoot", "GlueSAStateUsedSpace", "GlueSAPolicyMaxFileSize",
"GlueSchemaVersionMinor", "GlueSAPolicyMinFileSize", "GlueSALocalID",
"GlueSAPolicyMaxPinDuration", "GlueSAType", "GlueSAPolicyMaxData",
"GlueSAPolicyMaxNumFiles","GlueSAPolicyQuota", "GlueSAPolicyFileLifeTime",
"GlueSAAccessControlBaseRule",
0
};

char const* se_attributes[] = {
"GlueSEName", "GlueSEUniqueID", "GlueSEPort"
"GlueSESizeTotal","GlueSEArchitecture", "GlueSESizeFree"
"GlueInformationServiceURL",
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

struct generate_storage_info : std::binary_function<
  vector<classad::ExprTree*>*, 
  classad::ExprTree*, 
  vector<classad::ExprTree*>*
>
{
std::string _vo;
  generate_storage_info(
    std::string const& vo) 
  : _vo(vo)
  {
  }
  vector<classad::ExprTree*>*
  operator()(
    vector<classad::ExprTree*>* c,
    classad::ExprTree* e
  )
  {
    std::string name;
    static_cast<classad::ClassAd*>(e)->EvaluateAttrString(
      "name",name
    );
    ism::Ism const& the_ism = *ism::get_ism();
    std::vector<ism::MutexSlicePtr>::const_iterator first = the_ism.storage.begin();
    std::vector<ism::MutexSlicePtr>::const_iterator const last = the_ism.storage.end();
    for( ; first != last; ++first ) {
    
      ism::Mutex::scoped_lock l((*first)->mutex);
      ism::OrderedSliceIndex& ordered_index(
        (*first)->slice->get<ism::OrderedIndex>()
      );
      ism::OrderedSliceIndex::iterator const slice_end(ordered_index.end());
      ism::OrderedSliceIndex::iterator se_it = ordered_index.find(name);
      if ( se_it != slice_end ) {    
        boost::shared_ptr<classad::ClassAd> se_ad( se_it->ad );
        classad::ClassAd sesa_ad;
        for(int i=0; se_attributes[i]; ++i) {
          classad::ExprTree* expr = se_ad->Lookup(se_attributes[i]);
          if(expr) { 
            sesa_ad.Insert(
              se_attributes[i],
              expr->Copy()
            );
          }
        }
        vector<classad::ExprTree*> sa_ads;
        if (evaluate(*se_ad, "GlueSA", sa_ads)) {

          vector<classad::ExprTree*>::const_iterator sa_it(
            std::find_if(
              sa_ads.begin(), sa_ads.end(),
              gluesa_local_id_matches(_vo)
            )
          );
          if (sa_it != sa_ads.end()) {
            for(int i=0; sa_attributes[i]; ++i) {
              classad::ExprTree* expr = static_cast<classad::ClassAd*>(*sa_it)->Lookup(sa_attributes[i]);
              if (expr) {
                sesa_ad.Insert(
                  sa_attributes[i],expr->Copy()
                );
              }
            }
          }
        }
        c->push_back(sesa_ad.Copy());
        break; // no need to scan the next slice
      }
    }
   return c;
  }
};

}

bool retrieveCloseSEsInfo(
  const char         *name,
  const ArgumentList &arguments,
  EvalState          &state,
  Value              &result)
{
  bool  eval_successful = false;
  result.SetErrorValue();
  // We check to make sure that we passed exactly three arguments...
  if (arguments.size() == 1) {

    // ...the first and second arguments should evaluate to a string...
    Value       arg1;
    std::string vo;
    if (arguments[0] -> Evaluate(state, arg1) &&
      arg1.IsStringValue(vo)) {
	
      eval_successful = true;
      classad::ClassAd const* thiscead = 
        static_cast<const classad::ClassAd*>(arguments[0] -> GetParentScope());

      vector<classad::ExprTree*> close_storage_elements;
      if (evaluate(*thiscead, "CloseStorageElements", close_storage_elements)) {
          
        vector<classad::ExprTree*> storage_info_ads;

        std::accumulate(
          close_storage_elements.begin(),
          close_storage_elements.end(),
          &storage_info_ads,
          generate_storage_info(vo)
        );

        result.SetListValue(ExprList::MakeExprList(storage_info_ads));
        eval_successful=true;
      }
      else {
        result.SetUndefinedValue();
      }
    }
  }
  return eval_successful;
}

} // namespace gangmatch
} // namespace clasad_plugin
} // namespace wms
} // namespace glite
