// File: access_catalogs_utils.cpp
// Author: Salvatore Monforte
// Author: Enzo Martelli

#include <dlfcn.h>
#include <numeric>

#include "DLI_ReplicaService.h"
#include "glite/wms/rls/catalog_access_utils.h"

#include "ReplicaServiceException.h"

#include <ServiceDiscovery.h>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <classad_distribution.h>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"

#include "glite/wms/common/logger/logging.h"

#include "glite/jdl/ManipulationExceptions.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"

using namespace std;

namespace glite {

namespace requestad = jdl;

namespace wms {

namespace configuration = common::configuration;

namespace rls {

namespace {


typedef boost::shared_ptr< vector<string> > lfn_list_ptr;
typedef boost::shared_ptr< vector<string> > endpoints_ptr;

typedef boost::tuple< lfn_list_ptr,             //lfns to be resolved
                      endpoints_ptr             //endpoints gathered through SD
                    > dli_lfn;

///typedef boost::tuple< lfn_list_ptr,             //lfns to be resolved
///                      endpoints_ptr             //endpoints gathered through SD
///                    > si_lfn;

typedef vector<dli_lfn> dli_lfns;
//typedef vector<si_lfn> si_lfns;

typedef boost::shared_ptr<dli_lfns> dli_lfns_ptr;
//typedef boost::shared_ptr<si_lfns>  si_lfns_ptr;

typedef boost::tuple< 
           dli_lfns_ptr
//           si_lfns_ptr
        > lfns_2B_resolved;

enum {
   _DLI = 0
//   _SI
};

enum {
   DLI_SI_LFNS =0,
   ENDPOINTS
};


//void print_lfns(const lfns_2B_resolved& lfns){
//
//   dli_lfns_ptr dli_c = lfns.get<_DLI>();
//   //si_lfns_ptr si_c = lfns.get<_SI>();
//
//   Debug("DLI");
//   for( dli_lfns::iterator dli_it = dli_c->begin() ;
//        dli_it != dli_c->end();
//        dli_it++ ){
//      lfn_list_ptr elem = dli_it->get<DLI_SI_LFNS>();
//      for( vector<string>::iterator it = elem->begin();
//           it != elem->end();
//           it++){
//
//         Debug(*it);
//      }
//
//      endpoints_ptr elem2 = dli_it->get<ENDPOINTS>();
//      for( vector<string>::iterator i = elem2->begin();
//           i != elem2->end();
//           i++){
//
//         Debug(*i);
//      }
//
//   }
//   /*
//   Debug("SI");
//   for( si_lfns::iterator si_it = si_c->begin() ;
//        si_it != si_c->end();
//        si_it++ ){
//      lfn_list_ptr elem = si_it->get<DLI_SI_LFNS>();
//      for( vector<string>::iterator it = elem->begin();
//           it != elem->end();
//           it++){
//
//         Debug(*it);
//      }
//
//      endpoints_ptr elem2 = si_it->get<ENDPOINTS>();
//      for( vector<string>::iterator i = elem2->begin();
//           i != elem2->end();
//           i++){
//
//         Debug(*i);
//      }
//
//   }
//   */
//
//}

void get_catalog_url(
   const std::string& vo, 
   const std::string& service_name,
   std::vector<std::string>& list
) {

   SDServiceList *sl=NULL;
   SDException ex;

   const char*s=vo.c_str(); 

   SDVOList vos = {1, const_cast<char**>(&s)};

   sl = SD_listServices( service_name.c_str(), NULL, &vos, &ex);
   if (sl != NULL) { 

      if ( sl->numServices > 0 )  {

         for(int k=0; k < sl->numServices; k++) {
              //Debug("EndPoint["<<k<<"]: "
              //  << sl->services[k]->endpoint );
              list.push_back( strdup(sl->services[k]->endpoint) ) ;
         }
      }
      else {
         GLITE_LOG_ACCESS_ERROR << "No endpoints found";
      }

      SD_freeServiceList(sl);

   } else { 
      if (ex.status == SDStatus_SUCCESS) { 
         GLITE_LOG_ACCESS_ERROR <<"No "<<service_name<<" service found"; 
      }
      else {
         GLITE_LOG_ACCESS_ERROR <<"No "<<service_name<<" service found";
         GLITE_LOG_ACCESS_ERROR << ex.reason;
         SD_freeException(&ex);
      }
   }

}


struct old_jdl_check{

string m_vo;

old_jdl_check(string vo): m_vo(vo) {

}

lfns_2B_resolved*
operator()(
   lfns_2B_resolved* c,
   const string& e
){
   // we rely on the fact that all the containers in c
   // has only one element

   dli_lfns_ptr dli_c = c->get<_DLI>();

   lfn_list_ptr the_dli_lfns = (*dli_c)[0].get<DLI_SI_LFNS>();

   endpoints_ptr jdl_dli_endpoints = (*dli_c)[0].get<ENDPOINTS>();

   bool dli_endpoints_in_jdl = !jdl_dli_endpoints->empty();

/*
   bool lfn_or_guid_found = 
      boost::algorithm::starts_with(e,"lfn") ||
      boost::algorithm::starts_with(e,"guid");

   bool si_lfn_or_guid_found = !lfn_or_guid_found &&
      (
         boost::algorithm::starts_with(e,"si-lfn") ||
         boost::algorithm::starts_with(e,"si-guid")
      );

   bool lds_or_query_found = !lfn_or_guid_found && !si_lfn_or_guid_found &&
      (
         boost::algorithm::starts_with(e,"query") ||
         boost::algorithm::starts_with(e,"lds")
      );
*/

   the_dli_lfns->push_back(e);
   if ( !dli_endpoints_in_jdl ){
      string dli_service_name =
         (configuration::Configuration::instance()->wm())->
              dli_service_name();
      get_catalog_url( m_vo,
                       dli_service_name,
                       *jdl_dli_endpoints
      );
         
   }

   return c;

}

};

struct new_jdl_check{

string m_vo;
endpoints_ptr m_sd_endpoints;

new_jdl_check(string vo): m_vo(vo){

   m_sd_endpoints.reset( new vector< string > );
}


lfns_2B_resolved*
operator()(
   lfns_2B_resolved* c,
   classad::ExprTree* e
){

   string dataCatalogType;
   bool getDataCatalog;
   dataCatalogType =
      glite::jdl::get_data_catalog_type(
                  *( static_cast<classad::ClassAd*>(e)),getDataCatalog 
      );
   if( !getDataCatalog ) return c;

   lfn_list_ptr input_data ( new vector<string> );
   bool getInputData;
   requestad::get_input_data(
      *( static_cast<classad::ClassAd*>(e) ),
      *input_data,
      getInputData
   );
   if ( !getInputData ) return c;

   bool getEndpoint;
   endpoints_ptr endpoints( new vector<string> );
   requestad::get_data_catalog(
      *( static_cast<classad::ClassAd*>(e) ), 
      *endpoints,
      getEndpoint
   );

   if ( dataCatalogType == "DLI" ) {
      if ( !getEndpoint ) {
         if ( m_sd_endpoints->empty() ){
            string dli_service_name = 
               (configuration::Configuration::instance()->wm())->
                  dli_service_name();
            get_catalog_url( m_vo,
                             dli_service_name,           
                             *m_sd_endpoints
            ); 
         }
         endpoints = m_sd_endpoints;
      }
      c->get<_DLI>()->push_back( boost::make_tuple(
                                    input_data,
                                    endpoints
                                 )
      );
   }
   else 
      GLITE_LOG_ACCESS_ERROR <<"Unknown Data Catalog Type";

   return c;

}

};


boost::shared_ptr<filemapping>
resolve( const lfns_2B_resolved& lfns, const string& proxy){

//print_lfns(lfns);

   boost::shared_ptr<filemapping> fm(
      new filemapping()
   );

   dli_lfns_ptr dli_c = lfns.get<_DLI>();

   int timeout =
      (configuration::Configuration::instance()->ns())->
         dli_si_catalog_timeout();


///////////////DLI HANDLING

   dli_lfns::iterator dli_c_it = dli_c->begin();
   dli_lfns::const_iterator dli_c_e = dli_c->end();

   bool dli_plugin = false;
   void* dliLibHandle = NULL;
   DLI::create_DLI_t* createDli = NULL; 
   DLI::destroy_DLI_t* destroyDli = NULL;
   DLI::DLI_ReplicaService* the_dli = NULL;
   string dliLib = "libglite_wms_rls_dli.so";
   for(;dli_c_it != dli_c_e; dli_c_it++){

      lfn_list_ptr dli_lfn_group = dli_c_it->get<DLI_SI_LFNS>();
      endpoints_ptr dli_endpoints_group =
         dli_c_it->get<ENDPOINTS>();

      if ( dli_lfn_group->empty() ) continue;
      if( !dli_plugin ) {

         dliLibHandle = dlopen (dliLib.c_str(), RTLD_NOW);
         if (!dliLibHandle) {
            GLITE_LOG_ACCESS_ERROR<<"cannot load DLI helper lib " << dliLib
               <<". dlerror returns: " << dlerror();
            break;
         }
         else {
            createDli = (DLI::create_DLI_t*)dlsym(dliLibHandle,"create_DLI");
            destroyDli = (DLI::destroy_DLI_t*)dlsym(dliLibHandle,"destroy_DLI");
            if (!createDli || !destroyDli) {
               GLITE_LOG_ACCESS_ERROR << "cannot load DLI helper symbols"
                <<". dlerror returns: " << dlerror();
               dlclose(dliLibHandle);
               break;
            }
            else 
               dli_plugin = true;
         }
      }

      vector<string>::iterator lfns_it = dli_lfn_group->begin();
      vector<string>::const_iterator lfns_e = dli_lfn_group->end();
     
      for(;lfns_it!=lfns_e;lfns_it++){
         vector<string> sfns;
         vector<string>::iterator endpoints_it = 
            dli_endpoints_group->begin();
         vector<string>::const_iterator endpoints_e = 
            dli_endpoints_group->end();
         for(;endpoints_it!=endpoints_e; endpoints_it++){
            the_dli = createDli(
                            *endpoints_it,
                            proxy,
                            timeout
                      );

            try{
               GLITE_LOG_ACCESS_INFO << "Resolving " << *lfns_it << "...";
               the_dli->listReplica(*lfns_it, sfns);
            }
            catch(const ReplicaServiceException&  ex){
               GLITE_LOG_ACCESS_ERROR <<ex.what();
               destroyDli(the_dli);
               continue;
            }
            destroyDli(the_dli);
            fm->insert(std::make_pair(*lfns_it, sfns));
            break;
         }
 
      }

   }

   if(dli_plugin){
      dlclose(dliLibHandle);
   } 
   
   return fm;
}

} //anonymous namespace

boost::shared_ptr<filemapping> 
resolve_filemapping_info(
   const classad::ClassAd& requestAd 
) 
{

   bool voInJdl;
   string vo = requestad::get_virtual_organisation(requestAd, voInJdl);
   if ( !voInJdl ) {
      GLITE_LOG_ACCESS_ERROR<<"VO not defined in JDL";
      boost::shared_ptr<filemapping> fm(
         new filemapping()
      );
      return fm;
   }

   bool proxyInAd;
   string proxy = jdl::get_x509_user_proxy(requestAd, proxyInAd);
   
   lfns_2B_resolved lfns = boost::make_tuple (
                              dli_lfns_ptr(new dli_lfns)
                           );
                              
   bool exist_data_req = false;
   classad::ExprTree* classAdList = 
      glite::jdl::get_data_requirements(requestAd, exist_data_req);
   if(exist_data_req){
     classad::ExprList* expr_list =
         static_cast<classad::ExprList*>(classAdList) ;
     std::accumulate( 
            expr_list->begin(),
            expr_list->end(),
            &lfns,
            new_jdl_check(vo)
     );
   }
   else{
      bool getDataCatalog;
      endpoints_ptr dli_catalogs( new vector<string> );
      requestad::get_data_catalog(requestAd, *dli_catalogs, getDataCatalog);

      lfn_list_ptr the_dli_lfns( new vector<string> );

      lfns.get<_DLI>()->push_back( boost::make_tuple(
                                      the_dli_lfns,
                                      dli_catalogs 
                                   )
      );

      vector<string> input_data;
      bool exist_input_data;
      requestad::get_input_data(requestAd, input_data, exist_input_data);
      if( exist_input_data ) {
         std::accumulate(
            input_data.begin(),
            input_data.end(),
            &lfns,
            old_jdl_check(vo)
         );
      }

   }

   return resolve(lfns, proxy);

}


}  //namespace rls
}  //namespace wms
}  //namespace glite
