// File: dli_utils.cpp
// Author: Salvatore Monforte
// Author: Enzo Martelli

// $Id$

#include <dlfcn.h>

#include <DataLocationInterfaceSOAP.h>
#include <StorageIndexCatalogInterface.h>

#include <ServiceDiscovery.h>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/regex.hpp>
#include <boost/progress.hpp>

#include <classad_distribution.h>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"

#include "glite/wms/common/logger/logger_utils.h"

#include "glite/jdl/ManipulationExceptions.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"

#include "glite/wmsutils/exception/Exception.h"
#include "glite/wms/brokerinfo/brokerinfo.h"

#include "glite/wms/rls/ReplicaServiceReal.h"

#include "glue_attributes.h"
#include "glite/wms/ism/ism.h"

using namespace std;

namespace glite {

namespace requestad = jdl;

namespace wms {

namespace configuration = common::configuration;
namespace logger        = common::logger;
//namespace requestad     = jdl;
namespace brokerinfo {

namespace {
/*
 * Check the configuration file of the Networkserver if RLS is used for
 * a certain VO. In case the RLS is used, 0 is returned, Otherwise -1.
 */
int checkRlsUsage(const std::string& vo)
{
  const configuration::NSConfiguration* NSconf = configuration::Configuration::instance() -> ns();

  std::vector<std::string> rls = NSconf->rlscatalog();

  for (unsigned int i=0; i < rls.size(); i++) {
    if (rls[i] == vo) { return 0;}
  }

  return -1;
}// checkRlsUsage

/**
 * Contact the Information Service (IS) and return the URL (endpoint) of the
 * server the provides the DataLocationInterface(DLI).
 * If no service is found, "" is returned.
 */
void get_catalog_url(
  const std::string& vo, 
  const std::string& service_name,
  std::vector<std::string>& list
) {
//  string url = "";
                                                                                       
  //edglog_fn(get_catalog_url);
  
  //edglog(debug) << "Contacting IS for " << service_name<< " service. " << endl;       
//  const configuration::WMConfiguration* WMconf = configuration::Configuration::instance() -> wm();

   SDServiceList *sl=NULL;
   SDException ex;

   char **names = new (char*)[1];

   names[0] = new char[vo.length() +1];
   strcpy(names[0], vo.c_str());

   SDVOList vos = {1, names};

   sl = SD_listServices( service_name.c_str(), NULL, &vos, &ex);
   if (sl != NULL) { 

      if ( sl->numServices > 0 )  {

         for(int k=0; k < sl->numServices; k++) {
              //edglog(debug) <<"EndPoint["<<k<<"]: "<< sl->services[k]->endpoint  << endl;
              list.push_back( strdup(sl->services[k]->endpoint) ) ;
         }
      }
      else {
         // edglog(warning) << "No endpoints found" << endl;
      }

      SD_freeServiceList(sl);

   }
   else {
      if (ex.status == SDStatus_SUCCESS) {
         // edglog(warning) << "No such services" << endl;
      }
      else {
         // edglog(error) <<"Call failed: " <<  ex.reason << endl;
         SD_freeException(&ex);
      }
   }

   delete []names[0];
   delete []names;
}

} // anonymouns namespace

boost::shared_ptr<filemapping> 
resolve_filemapping_info(
  const classad::ClassAd& requestAd 
) 
{
  boost::shared_ptr<filemapping> fm(
    new filemapping()
  );

  bool rlsInUse = false;
  bool dli_siciInUse = false;
  
  dli::create_dli_t* createDli; dli::create_dli_with_timeout_t* createDli_with_timeout;
  dli::destroy_dli_t* destroyDli; 
  dli::DataLocationInterfaceSOAP* dli;

  rls::create_t* createRls;  rls::destroy_t* destroyRls; 
  rls::ReplicaServiceReal* replica;

  sici::create_t* createSici;  sici::create_t_with_timeout* createSici_with_timeout;
  sici::destroy_t* destroySici;
  sici::StorageIndexCatalogInterface* sici;

  string rlsLib = "libglite_wms_rls.so";
  string dli_siciLib = "libglite_wms_dli_sici.so";

  void *rlsLibHandle;
  void *dli_siciLibHandle;

                                                                                                        
  int timeout = (configuration::Configuration::instance()->ns())->dli_si_catalog_timeout();

  std::string dli_service_name = (configuration::Configuration::instance()->wm())->dli_service_name();
  std::string si_service_name = (configuration::Configuration::instance()->wm())->si_service_name();

  bool  rlsConfig = false; // Determine if RLS is configured

  bool  voInJdl   = false;
  string   vo = "";
                                                                                                
  Debug(
    "ClassAd..." << requestAd
  );

  try {
     // The vo in JDL is needed for RLS Catalog queries
     //
     vo = requestad::get_virtual_organisation(requestAd);
     voInJdl = true;
  }
  catch (...){
  }
                                                                                               
   /// Check if we have the RLS configured for the VO. If this is the case,
   // requests for InputDataType lfn and guid are sent to RLS.
   //
   if ( voInJdl ) {
      if (checkRlsUsage(vo) == 0) {
         rlsConfig = true;
      }
   }

   bool dataReq = false;
   classad::ExprTree* classAdList;
   try {
      classAdList = glite::jdl::get_data_requirements(requestAd);
      dataReq = true; 
   }
   catch(...) {
   }

   if ( dataReq ) {

      vector<string> dli_url_list; bool getDliFromIS = false;
//      if ( voInJdl ) {
//         get_catalog_url( vo, dli_service_name, dli_url_list );
//         if ( ! dli_url_list.empty() ) getDliFromIS = true;
//      }

      vector<string> si_url_list; bool getSiFromIS = false;
//      if ( voInJdl ) {
//         get_catalog_url( vo, si_service_name, si_url_list );
//         if ( ! si_url_list.empty() ) getSiFromIS = true;
//      }

      classad::ExprList* expr_list = static_cast<classad::ExprList*>(classAdList) ;
      for ( classad::ExprList::iterator it = expr_list->begin(); it < expr_list->end(); it++ ){

         string dataCatalogType;
         bool getDataCatalog = false;
         try {
            dataCatalogType = glite::jdl::get_data_catalog_type( *( static_cast<classad::ClassAd*>(*it) ) );
            getDataCatalog = true;
         } 
         catch (...){
         }

         string dataCatalogEndpoint;
         bool getDataCatalogEndpoint = false;
         try {
            vector<string> v;
            requestad::get_data_catalog(*( static_cast<classad::ClassAd*>(*it) ), v);
            dataCatalogEndpoint = v[0]; 
            getDataCatalogEndpoint = true;
         }
         catch (...){
            if ( dataCatalogType == "DLI" && (!getDliFromIS)) {
               if ( voInJdl ) {
                  get_catalog_url( vo, dli_service_name, dli_url_list );
                  if ( ! dli_url_list.empty() ) getDliFromIS = true;
               }
            }
            if ( dataCatalogType == "SI" && (!getSiFromIS) ) {
               if ( voInJdl ) {
                  get_catalog_url( vo, si_service_name, si_url_list );
                  if ( ! si_url_list.empty() ) getSiFromIS = true;
               }
            }
         }

         if ( getDataCatalog ) {

            bool validCatalogType = true;

            if ( dataCatalogType == "RLS" ) {

               if ( !rlsInUse ) {

                  // RLSCatalog property in WMS configuration not used anymore
                  //if ( rlsConfig ) {

                     rlsLibHandle = dlopen (rlsLib.c_str(), RTLD_NOW);
                     if (rlsLibHandle == NULL) {
                        Warning("cannot load RLS helper lib " << rlsLib);
                        Warning("dlerror returns: " << dlerror());
                     }
                     else {
                        createRls = (rls::create_t*)dlsym(rlsLibHandle,"create");
                        destroyRls = (rls::destroy_t*)dlsym(rlsLibHandle,"destroy");
                        if (!createRls || !destroyRls) {
                           Warning("cannot load RLS helper symbols");
                           Warning("dlerror returns: " << dlerror());
                           dlclose(rlsLibHandle);
                        }
                        else {
                           replica = createRls(vo);
                           rlsInUse = true;
                        }
                     }

                  //}
                  //else {
                  //   if ( voInJdl ) {
                  //      edglog(warning) << "RLS catalog is not set for " << vo << endl;
                  //   }
                  //   else {
                  //      edglog(warning) << "JDL doesn't contain the VO" << endl;
                  //   }
                  //}

               } //if ( !rlsInUse )

            }
            else if ( dataCatalogType == "DLI" || dataCatalogType == "SI") {

               if ( !dli_siciInUse ) {

                     dli_siciLibHandle = dlopen (dli_siciLib.c_str(), RTLD_NOW);
                     if (dli_siciLibHandle == NULL) {
                        Warning("cannot load DLI_SI helper lib " << dli_siciLib);
                        Warning("dlerror returns: " << dlerror());
                     }
                     else {
                        bool dlsym_Ok = true;
                        if ( timeout == 0 ) {
                           createDli = (dli::create_dli_t*)dlsym(dli_siciLibHandle,"create_dli");
                           destroyDli = (dli::destroy_dli_t*)dlsym(dli_siciLibHandle,"destroy_dli");
                           createSici = (sici::create_t*)dlsym(dli_siciLibHandle,"create");
                           destroySici = (sici::destroy_t*)dlsym(dli_siciLibHandle,"destroy");

                           if (!createDli || !destroyDli || !createSici || !destroySici) {
                              dlsym_Ok = false;
                              Warning("cannot load DLI_SI helper symbols");
                              Warning("dlerror returns: " << dlerror());
                              dlclose(dli_siciLibHandle);
                           }
                        }
                        else {
                           createDli_with_timeout = (dli::create_dli_with_timeout_t*)dlsym(dli_siciLibHandle,"create_dli_with_timeout");
                           destroyDli = (dli::destroy_dli_t*)dlsym(dli_siciLibHandle,"destroy_dli");
                           createSici_with_timeout = (sici::create_t_with_timeout*)dlsym(dli_siciLibHandle,"create_with_timeout");
                           destroySici = (sici::destroy_t*)dlsym(dli_siciLibHandle,"destroy");


                           if (!createDli_with_timeout || !destroyDli || !createSici_with_timeout || !destroySici) {
                              dlsym_Ok = false;
                              Warning("cannot load DLI_SI helper symbols");
                              Warning("dlerror returns: " << dlerror());
                              dlclose(dli_siciLibHandle);
                           }
                        }
                        if ( dlsym_Ok ) {
                           if ( timeout == 0 ) {
                              dli = createDli();
                              sici = createSici();
                           }
                           else {
                              dli = createDli_with_timeout(timeout);
                              sici = createSici_with_timeout(timeout);
                           }
                           dli_siciInUse = true;
                        }
                     }
//                  }
//                  else {
//                     edglog(warning) << "cannot get any dli endpoint, neither from JDL nor from IS" << endl;
//                  }

               } // if ( !dli_siciInUse )

                 
            }
            else {
               validCatalogType = false; 
               Warning(dataCatalogType<<": unknown DataCatalogType type");
            }

            if ( validCatalogType ) {
               // the list of LFNs from the JDL
               vector<string> input_data;
               bool getInputData = false;
               try {
                  requestad::get_input_data(*( static_cast<classad::ClassAd*>(*it) ), input_data);
                  getInputData = true;
               }
               catch (...){
                  Error("cannot get input data from jdl");
               }
               if ( getInputData ){
                  // the list of SFNs matching each LFN
                  vector<string> resolved_sfn;

                  for(vector<string>::const_iterator lfn = input_data.begin(); lfn != input_data.end();lfn++){
                     try {

                        Debug("trying to resolve " << *lfn);
                        // the list of SFNs matching each LFN
                        vector<string> resolved_sfn;
   
                        if ( rlsInUse && (dataCatalogType == "RLS") ) {
                           resolved_sfn = replica->listReplica(*lfn);
                        }
                        else if ( dli_siciInUse && (dataCatalogType == "DLI") ) {
///////////////////////////////////// this piece of code handles multiple endpoint
                           string prefix = "";
                           if ((*lfn).find("lds") == 0) prefix = "lds";
                           else if ((*lfn).find("query") == 0) prefix = "query";
                           else if  ((*lfn).find("lfn") == 0) prefix = "lfn";
                           else if ((*lfn).find("guid") == 0) prefix = "guid";
                           else Warning("unknown prefix while using DLI");

                           if  ( prefix != "" ) {
                              if ( getDataCatalogEndpoint ) {
                                 try {
                                    resolved_sfn = dli->listReplicas(prefix.c_str(), *lfn, requestAd, dataCatalogEndpoint);
                                 }
                                 catch (string& faultstring){
                                    Warning(faultstring);
                                 }
                              }
                              else if ( getDliFromIS ) {
                                 for ( vector<string>::const_iterator it=dli_url_list.begin(); it != dli_url_list.end(); it++){
                                    try{
                                       resolved_sfn = dli->listReplicas(prefix.c_str(), *lfn, requestAd, *it);
                                       if ( ! resolved_sfn.empty() ) break;
                                    }
                                    catch(string& faultstring) {
                                    //
                                    // Catch soap exceptions from the DataLocationInterface or StorageIndex Interface
                                    // or any other exception due to failures in getting the proxy from the classad
                                    Warning(faultstring);
                                    }
                                 } 
                              } 
                              else 
                                 Warning("Cannot get any dli endpoint, neither from JDL nor from IS");
                           }
//////////////////////////////////////////////////////////end
                        }
                        else if ( dli_siciInUse && (dataCatalogType == "SI") ) {

                           string noPrefix = *lfn;
                                                                                                                                
                           string::size_type colon_pos;
                           if ((colon_pos = noPrefix.find(":"))!=string::npos) {
                              // Remove any prefix before the leading colon. Including the colon.
                              colon_pos++;
                              noPrefix.erase(0,colon_pos);
                           }
/////////////////////////////////////////////this piece of code handles multiple endpoint
                           if ( getDataCatalogEndpoint ) { 
                              try {
                                 if ( (*lfn).find("lfn") == 0 ) {
                                    sici->listSEbyLFN(noPrefix.c_str(), resolved_sfn, requestAd, dataCatalogEndpoint);
                                 }
                                 else if ( (*lfn).find("guid") == 0 ){
                                    sici->listSEbyGUID(noPrefix.c_str(), resolved_sfn, requestAd, dataCatalogEndpoint);
                                 }
                                 else Warning("unknown prefix while using SI");
                              }
                              catch (string& faultstring) {
                                 Warning(faultstring);
                              }
                           }
                           else if ( getSiFromIS ){
                              for ( vector<string>::const_iterator it=si_url_list.begin(); it != si_url_list.end(); it++){
                                 try{
                                    if ( (*lfn).find("lfn") == 0 ) {
                                       sici->listSEbyLFN(noPrefix.c_str(), resolved_sfn, requestAd, *it);
                                    }
                                    else if ( (*lfn).find("guid") == 0 ){
                                       sici->listSEbyGUID(noPrefix.c_str(), resolved_sfn, requestAd, *it);
                                    }
                                    else {
                                       Warning("unknown prefix while using SI");
                                       break;
                                    }
   
                                    if ( ! resolved_sfn.empty() ) break;
                                 }
                                 catch(string& faultstring) {
                                    //
                                    // Catch soap exceptions from the DataLocationInterface or StorageIndex Interface
                                    // or any other exception due to failures in getting the proxy from the classad
                                    Warning(faultstring);
                                 } 
                              }
                           }
                           else
                              Warning("Cannot get any si endpoint, neither from JDL nor from IS");
/////////////////////////////////////////////////////////////end
                        }
                        else Warning("cannot perform " << *lfn <<"'s resolution");
   
                        if( !resolved_sfn.empty() ) {
                           fm->insert(
                             std::make_pair(*lfn, resolved_sfn)
                           );
                           for(int i = 0; i<resolved_sfn.size(); i++){
                              Debug(resolved_sfn[i]);
                           }
                        }
                        else {
                           Debug("No replica(s) found!");
                        }
         
                     }
                     catch( std::exception& ex ) {
                        Warning(ex.what());
                     }
                                                                                                                                
                  } // for

               
               } // if ( getInputData )
       

            }  // if  ( validCatalogType )
   

         } // ( getDataCatalog )
         else Warning("cannot get DataCatalogType");
            
      } // for



   }
   else {
      // This block assure the backward compatibility for what concern the data format in the JDL
      //
      // Due to the different SOAP versions used in the three catalog interfaces, we load the
      // client dynamically as a plug-in in order to avoid SOAP version
      // clashes.
      //
      // We check the prefix of InputData's fields we got from the JDL. We have the following
      // possibilities:
      //
      // PREFIX                                 CATALOG to be used
      //
      // 1)lfn, guid                            SI if the SIendpoint is set in the JDL
      //                                        i.e.:StorageIndex="http://lxb2021.cern.ch:9992/AliEn/Service/FC";
      //
      // 2)lfn, guid                            RLS if the SIendpoint is NOT set in the JDL -AND-
      //                                            RLSCatalog is set for the given VO.
      //                                        If RLS doesn't work for any reason, we try DLI
      //
      // 3)si-guid, si-lfn                      StorageIndex, no matter if the endpoint is set or not
      //
      // 4)query, lds                           DLI
      //
      //
      const string   lfnPrefix = "lfn";
      const string   guidPrefix = "guid";
      const string   ldsPrefix = "lds";
      const string   queryPrefix = "query";
      const string   silfnPrefix = "si-lfn";
      const string   siguidPrefix = "si-guid";
   
      string   dliEndpoint = "";
      string   siciEndpoint = "";

      vector<string> dli_url_list;
      vector<string> si_url_list;

      bool     dliEndpointSetInJdl = false;
      bool     dliEndpointSetInIS = false;
      bool     dliEndpointSet = false;
                                                                                                                                
      bool     siciEndpointSetInJdl = false;
      bool     siciEndpointSetInIS = false;
      bool     siciEndpointSet = false;
   
      // Check if Storage Index Catalog and Data Catalog endpoints are set in the jdl
      // If it is not the case, it try to get them from the Information Service
      try {
         vector<string> v;
         requestad::get_data_catalog(requestAd, v);
         dliEndpoint = v[0];
         dliEndpointSetInJdl = true;
      }
      catch(...) {
         if ( voInJdl ) {
            get_catalog_url(vo, dli_service_name, dli_url_list );
            if  ( ! dli_url_list.empty() ) {
               dliEndpointSetInIS = true;
            }
         }
      }
      try {
         vector<string> v;
         requestad::get_storage_index(requestAd, v);
         siciEndpoint = v[0];
         siciEndpointSetInJdl = true;
      }
      catch(...) {
         if ( voInJdl ) {
            get_catalog_url(vo, si_service_name, si_url_list);
            if  ( ! si_url_list.empty() ) {
               siciEndpointSetInIS = true;
            }
         }
      }
                                                                                                                                
      if ( siciEndpointSetInIS || siciEndpointSetInJdl ) {
         siciEndpointSet = true;
      }

      if ( dliEndpointSetInIS || dliEndpointSetInJdl ) {
         dliEndpointSet = true;
      }

   
      vector<string> input_data;
   
      try {
         // The InputData field is used to specify a list of LFN, GUID, LDS or Query
         //
         requestad::get_input_data(requestAd, input_data);
      }
      catch (...){
         Error("cannot get input data from jdl");
         fm.reset();
         return fm;
      }
   
      for(vector<string>::const_iterator lfn = input_data.begin(); lfn != input_data.end();lfn++) {
   
         Debug("trying to resolve: " << *lfn);
   
         try {
   
            vector<string> resolved_sfn;
   
            bool lfn_or_guid_found = ((*lfn).find(lfnPrefix.c_str()) == 0) || ((*lfn).find(guidPrefix.c_str()) == 0);
            bool rls_success = false;
   
            if ( lfn_or_guid_found && (!siciEndpointSetInJdl) ) {
   
               if ( !rlsInUse ) {
   
                  if ( rlsConfig ) {
                     rlsLibHandle = dlopen (rlsLib.c_str(), RTLD_NOW);
                     if (rlsLibHandle == NULL) {
                        // edglog(warning) << "cannot load RLS helper lib " << rlsLib << endl;
                        // edglog(warning) << "dlerror returns: " << dlerror() << endl;
                     }
                     else {
                        createRls = (rls::create_t*)dlsym(rlsLibHandle,"create");
                        destroyRls = (rls::destroy_t*)dlsym(rlsLibHandle,"destroy");
                        if (!createRls || !destroyRls) {
                           // edglog(warning) << "cannot load RLS helper symbols" << endl;
                           // edglog(warning) << "dlerror returns: " << dlerror() << endl;
                           dlclose(rlsLibHandle);
                        }
                        else {
                           replica = createRls(vo);
                           rlsInUse = true;
                        }
                     }
                  }
                  else {
                     if ( voInJdl ) { 
                        // edglog(warning) << "RLS catalog is not set for " << vo << endl;
                     }
                     else {
                        // edglog(warning) << "JDL doesn't contain the VO" << endl;
                     }
                  }
               } //if ( !rlsInUse ) 
   
               if ( rlsInUse ) {
                  resolved_sfn = replica->listReplica(*lfn);
                  if( !resolved_sfn.empty() ) rls_success = true;
               }
   
            } // if ( lfn_or_guid_found && (!siciEndpointSetInJdl) )
   
   
            // If the prefix is "lfn" or "guid" but we didn't manage to load the rls plug-in
            //      or the RLSCatalog is not configured for the given vo 
            //      or RLS didn't work for whatever reason,
            //      we try with the dli catalog

            bool dli_to_be_used =  
                 // conditions under which dli is used
                 (    ((*lfn).find(ldsPrefix.c_str()) == 0) || ((*lfn).find(queryPrefix.c_str()) == 0)       ) ||
                 (    ( lfn_or_guid_found && (!siciEndpointSetInJdl) ) && (!rls_success)                     ) ;

            bool sici_to_be_used = 
                 // conditions under which si is used
                 (    ((*lfn).find(silfnPrefix.c_str()) == 0) || ((*lfn).find(siguidPrefix.c_str()) == 0)    ) ||
                 (    lfn_or_guid_found && siciEndpointSetInJdl                                              ) ;

   
            if ( dli_to_be_used || sici_to_be_used ) {

               if ( !dli_siciInUse ) {
                     dli_siciLibHandle = dlopen (dli_siciLib.c_str(), RTLD_NOW);
                     if (dli_siciLibHandle == NULL) {
                        // edglog(warning) << "cannot load DLI_SI helper lib " << dli_siciLib << endl;
                        // edglog(warning) << "dlerror returns: " << dlerror() << endl;
                     }
                     else {
                        bool dlsym_Ok = true;
                        if ( timeout == 0 ) {
                           createDli = (dli::create_dli_t*)dlsym(dli_siciLibHandle,"create_dli");
                           destroyDli = (dli::destroy_dli_t*)dlsym(dli_siciLibHandle,"destroy_dli");
                           createSici = (sici::create_t*)dlsym(dli_siciLibHandle,"create");
                           destroySici = (sici::destroy_t*)dlsym(dli_siciLibHandle,"destroy");
                                                                                                                             
                           if (!createDli || !destroyDli || !createSici || !destroySici) {
                              dlsym_Ok = false;
                              // edglog(warning) << "cannot load DLI_SI helper symbols" << endl;
                              // edglog(warning) << "dlerror returns: " << dlerror() << endl;
                              dlclose(dli_siciLibHandle);
                           }
                        }
                        else {
                           createDli_with_timeout = (dli::create_dli_with_timeout_t*)dlsym(dli_siciLibHandle,"create_dli_with_timeout");
                           destroyDli = (dli::destroy_dli_t*)dlsym(dli_siciLibHandle,"destroy_dli");
                           createSici_with_timeout = (sici::create_t_with_timeout*)dlsym(dli_siciLibHandle,"create_with_timeout");
                           destroySici = (sici::destroy_t*)dlsym(dli_siciLibHandle,"destroy");
                                                                                                                             
                                                                                                                             
                           if (!createDli_with_timeout || !destroyDli || !createSici_with_timeout || !destroySici) {
                              dlsym_Ok = false;
                              // edglog(warning) << "cannot load DLI_SI helper symbols" << endl;
                              // edglog(warning) << "dlerror returns: " << dlerror() << endl;
                              dlclose(dli_siciLibHandle);
                           }
                        }
                        if ( dlsym_Ok ) {
                           if ( timeout == 0 ) {
                              dli = createDli();
                              sici = createSici();
                           }
                           else {
                              dli = createDli_with_timeout(timeout);
                              sici = createSici_with_timeout(timeout);
                           }
                           dli_siciInUse = true;
                        }
                     }

               } // if( !dli_siciInUse )


               if ( dli_to_be_used && !dliEndpointSet ) {
                  // edglog(warning) << "cannot find dli endpoint" << endl;
               }
               else if ( sici_to_be_used && !siciEndpointSet ) {
                  // edglog(warning) << "cannot find si endpoint" << endl;
               }
   
               if ( dli_siciInUse && dliEndpointSet && dli_to_be_used) { 
///////////////////////////////////// this piece of code handles multiple endpoint
                  string prefix = "";
                  if ((*lfn).find(ldsPrefix.c_str()) == 0) prefix = ldsPrefix;
                  else if ((*lfn).find(queryPrefix.c_str()) == 0) prefix = queryPrefix;
                  else if  ((*lfn).find(lfnPrefix.c_str()) == 0) prefix = lfnPrefix;
                  else if ((*lfn).find(guidPrefix.c_str()) == 0) prefix = guidPrefix;
                                                                                                                             
                  if ( dliEndpointSetInJdl ) {
                     try{
                        resolved_sfn = dli->listReplicas(prefix.c_str(), *lfn, requestAd, dliEndpoint);
                     }
                     catch (string& faultstring) {
                        // edglog(warning) << faultstring << endl;
                     } 
                  }
                  else if ( dliEndpointSetInIS ) {
                     for ( vector<string>::const_iterator it=dli_url_list.begin(); it != dli_url_list.end(); it++){
                        try{
                           resolved_sfn = dli->listReplicas(prefix.c_str(), *lfn, requestAd, *it);
                           if ( ! resolved_sfn.empty() ) break;
                        }
                        catch(string& faultstring) {
                           //
                           // Catch soap exceptions from the DataLocationInterface or StorageIndex Interface
                           // or any other exception due to failures in getting the proxy from the classad
                           // edglog(warning) << faultstring << endl;
                        }
                     }
                  }
//////////////////////////////////////////////////////////end
               }

               if ( dli_siciInUse && siciEndpointSet && sici_to_be_used) { 
                  string noPrefix = *lfn;
   
                  string::size_type colon_pos;
                  if ((colon_pos = noPrefix.find(":"))!=string::npos) {
                     // Remove any prefix before the leading colon. Including the colon.
                     colon_pos++;
                     noPrefix.erase(0,colon_pos);
                  }
///////////////////////////////////////////this piece of code handles multiple endpoint
                  if ( siciEndpointSetInJdl ) {
                     try {
                        if ( (*lfn).find(silfnPrefix.c_str()) == 0 || (*lfn).find(lfnPrefix.c_str()) == 0 ) {
                           sici->listSEbyLFN(noPrefix.c_str(), resolved_sfn, requestAd, siciEndpoint);
                        }
                        else 
                           sici->listSEbyGUID(noPrefix.c_str(), resolved_sfn, requestAd, siciEndpoint);
                     }
                     catch (string& faultstring) {
                        // edglog(warning) << faultstring << endl;
                     }
                  }
                  else if ( siciEndpointSetInIS ){
                     for ( vector<string>::const_iterator it=si_url_list.begin(); it != si_url_list.end(); it++){
                        try{
                           if ( (*lfn).find(silfnPrefix.c_str()) == 0 || (*lfn).find(lfnPrefix.c_str()) == 0 ) {
                              sici->listSEbyLFN(noPrefix.c_str(), resolved_sfn, requestAd, *it);
                           }
                           else 
                              sici->listSEbyGUID(noPrefix.c_str(), resolved_sfn, requestAd, *it);

                           if ( ! resolved_sfn.empty() ) break;
                        }
                        catch(string& faultstring) {
                            //
                            // Catch soap exceptions from the DataLocationInterface or StorageIndex Interface
                            // or any other exception due to failures in getting the proxy from the classad
                            // edglog(warning) << faultstring << endl;
                        }
                     }
                  }
////////////////////////////////////////////end
               }
            } // if ( dli_to_be_used || si_to_be_used )
   
            if ( ((*lfn).find(lfnPrefix.c_str()) != 0) && ((*lfn).find(guidPrefix.c_str()) != 0) &&                                                                                                
                 ((*lfn).find(ldsPrefix.c_str()) != 0) && ((*lfn).find(queryPrefix.c_str()) != 0) &&
                 ((*lfn).find(silfnPrefix.c_str()) != 0) && ((*lfn).find(siguidPrefix.c_str()) != 0)   ) {
                      
               // edglog(warning) << "wrong prefix: " << *lfn << endl;
            }
   
            if( !resolved_sfn.empty() ) {
               fm->insert(
                 std::make_pair(*lfn, resolved_sfn)
               );
               for(int i = 0; i<resolved_sfn.size(); i++){
                 Debug(resolved_sfn[i]);
               }
            }
            else {
               // edglog(debug) << "No replica(s) found!" << endl;
            }
                                                                                                   
         }
         catch( std::exception& ex ) {
              // edglog(warning) <<  ex.what() << endl;
         }
                                                                                                   
      } //for

   } //else dataReq


   if ( rlsInUse ) {
      destroyRls(replica);
      dlclose(rlsLibHandle);
   }

   if ( dli_siciInUse ) {
      destroyDli(dli);
      destroySici(sici);
      dlclose(dli_siciLibHandle);
   }
   return fm;
   // edglog(debug) << "finishing retrieveSFNsInfo" << endl;
}



} // namespace brokerinfo
} // namespace wms
} // namespace glite
