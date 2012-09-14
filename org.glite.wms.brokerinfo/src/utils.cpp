// File: utils.cpp
// Author: Salvatore Monforte, INFN
// $Id$
#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/regex.hpp>

#include <classad_distribution.h>

#include "glite/wms/common/logger/logger_utils.h"

#include "glite/wms/brokerinfo/brokerinfo.h"
#include "glite/wms/ism/ism.h"

using namespace std;

namespace glite {
namespace wms {
namespace logger        = common::logger;
namespace brokerinfo {

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

string
resolve_storage_name(string const& sfn)
{
  static boost::regex e("^\\s*([^:]*):[\\s/]*([^\\s:/]+)(:[0-9]+)?/.*");
  boost::smatch p;
  string name;

  if(boost::regex_match(sfn, p, e)) {
    name.assign(p[2].first, p[2].second);
  }
  else if (int p=sfn.find("://",0) != string::npos) {
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

std::pair<std::string, std::string>
make_ce_bind_info(classad::ClassAd const&ad)
{
  std::string id;
  std::string mount;
  ad.EvaluateAttrString("GlueCESEBindCEUniqueID", id);
  ad.EvaluateAttrString("GlueCESEBindCEAccesspoint", mount);
  return std::make_pair(
    id, mount
  );
}

struct matches_any_protocol_in
{
  matches_any_protocol_in(vector<pair<string,int> >const& protocols) :
    m_protocols(protocols) {}

  bool operator()(string const& p) {
    vector<pair<string,int> >::const_iterator it(
      m_protocols.begin()
    );
    vector<pair<string,int> >::const_iterator const e(
      m_protocols.end()
    );
    for( ; it!=e; ++it ) {
      if(it->first == p) return true;
    }
    return false;
  }
  vector<pair<string,int> > const& m_protocols;
};

bool is_mount_defined(pair<string,string>const& p)
{
    return !p.second.empty();
}

struct extract_mapped_file_names : binary_function<
  std::set<string>*,
  brokerinfo::storagemapping::const_iterator,
  std::set<string>*
>
{
  std::set<string>*
  operator()(
    std::set<string>* f,
    brokerinfo::storagemapping::const_iterator si
  )
  {
    vector<filemapping::const_iterator> const& info(
      boost::tuples::get<1>(si->second)
    );
    vector<filemapping::const_iterator>::const_iterator it(info.begin());
    vector<filemapping::const_iterator>::const_iterator const e(info.end());
    for( ; it != e ; ++it ) {
       f->insert((*it)->first);
    }
    return f;
  }
};

} // anonymous namespace
 
boost::shared_ptr<storagemapping>
resolve_storagemapping_info(
  boost::shared_ptr<filemapping> fm
)
{
  boost::shared_ptr<storagemapping> sm(new storagemapping);

  filemapping::const_iterator fi = fm->begin(); 
  filemapping::const_iterator fe = fm->end();

  for( ; fi != fe; ++fi ) {
    vector<string> const& sfns(
      fi->second
    );
    vector<string>::const_iterator sfni = sfns.begin();
    vector<string>::const_iterator const sfne = sfns.end();

    for( ; sfni != sfne; ++sfni ) {
       string name = resolve_storage_name(*sfni);
       if(!name.empty()) {
         ism::ism_slice_type::nth_index<1>::type::iterator it;
         bool found;
         boost::tie(it,found) = ism::find( 
           name, ism::ism_se_index, ism::ism_se_index_end
         );
         if ( found ) {
           boost::shared_ptr<classad::ClassAd> se_ad(
             boost::tuples::get<ism::ad_ptr_entry>(*it)
           );
           storagemapping::iterator i;
           bool ib;

           boost::tie(i,ib) = sm->insert(
             std::make_pair(
               name,   
               boost::tuples::make_tuple(
                 vector<pair<string, int > >(),
                 vector<filemapping::const_iterator>(),
                 vector<pair<string, string > >()
               )
             )
           );
           boost::tuples::get<1>(i->second).push_back(fi);
           {
             vector<classad::ExprTree*> ads;
             if (evaluate(*se_ad, "GlueSEAccessProtocol", ads)) {

               vector<classad::ExprTree*>::const_iterator 
               expr_it(ads.begin());
               vector<classad::ExprTree*>::const_iterator const 
               expr_e(ads.end());

               for (; expr_it != expr_e; ++expr_it) {
                 classad::ClassAd const& ad(
                   *static_cast<classad::ClassAd*>(*expr_it)
                 );
                 boost::tuples::get<0>(i->second).push_back(
                   make_storage_info(ad)
                 );
               }
             }
           }  
           {
             vector<classad::ExprTree*> ads;
             if (evaluate(*se_ad, "CloseComputingElements", ads)) {

               vector<classad::ExprTree*>::const_iterator 
               expr_it(ads.begin());
               vector<classad::ExprTree*>::const_iterator const 
               expr_e(ads.end());

               for (; expr_it != expr_e; ++expr_it) {
                 classad::ClassAd const& ad(
                   *static_cast<classad::ClassAd*>(*expr_it)
                 );
                 boost::tuples::get<2>(i->second).push_back(
                   make_ce_bind_info(ad)
                 );
               }
             }
           }  
        }
      }
    }
  }
  return sm;
}

vector<storagemapping::const_iterator>
select_compatible_storage(
  storagemapping const& sm,
  vector<string>const& dap
)
{
  vector<storagemapping::const_iterator> result;
  storagemapping::const_iterator storage_it(sm.begin());
  storagemapping::const_iterator const storage_end(sm.end());

  for( ; storage_it != storage_end; ++storage_it ) {

    vector<pair<string,int> > const& supported_protocols(
      boost::tuples::get<0>(storage_it->second)
    );
    vector<string>::const_iterator it = find_if(
      dap.begin(), dap.end(),
      matches_any_protocol_in(supported_protocols)
    );
    if (it!=dap.end()) {
      vector<pair<string,string> > const& close_ce_binds(
        boost::tuples::get<2>(storage_it->second)
      );
      if(*it=="file" &&
         find_if(
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
  vector<storagemapping::const_iterator>::const_iterator first,
  vector<storagemapping::const_iterator>::const_iterator last)
{
  std::set<string> unique_logical_file_names;
  std::accumulate(
    first,
    last,
    &unique_logical_file_names,
    extract_mapped_file_names()
  );
  return unique_logical_file_names.size();
}

} // namespace brokerinfo
} // namespace wms
} // namespace glite
