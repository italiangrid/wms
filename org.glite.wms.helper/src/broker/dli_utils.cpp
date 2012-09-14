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

// File: dli_utils.cpp
// Author: Salvatore Monforte
// Author: Enzo Martelli

// $Id$

#include <dlfcn.h>

#include <DataLocationInterfaceSOAP.h>

#ifdef USE_RESOURCE_DISCOVERY_API_C
#include <ServiceDiscovery.h>
#endif

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

#include "brokerinfo.h"
#include "glue_attributes.h"

Namespace namespaces[] = {}; 

using namespace std;

namespace glite {

namespace requestad = jdl;

namespace wms {

namespace configuration = common::configuration;
namespace logger        = common::logger;
namespace brokerinfo {
namespace dli {

namespace {

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

#ifdef USE_RESOURCE_DISCOVERY_API_C
   Debug("trying to get "<<service_name<<" information through SD...");

   SDServiceList *sl=NULL;
   SDException ex;

   //char **names = new (char*)[1];
   const char*s=vo.c_str();

   //names[0] = new char[vo.length() +1];
   //strcpy(names[0], vo.c_str());

   //SDVOList vos = {1, names};
   SDVOList vos = {1, const_cast<char**>(&s)};

   sl = SD_listServices( service_name.c_str(), NULL, &vos, &ex);
   if (sl != NULL) {

      if ( sl->numServices > 0 )  {

         for(int k=0; k < sl->numServices; k++) {
              //edglog(debug) <<"EndPoint["<<k<<"]: "
              //              << sl->services[k]->endpoint
              //              << endl;
              list.push_back( strdup(sl->services[k]->endpoint) ) ;
         }
      }
      else {
         Warning("No endpoints found");
      }

      SD_freeServiceList(sl);

   }
   else {
      if (ex.status == SDStatus_SUCCESS) {
         Warning(service_name<<": No such services");
      }
      else {
         Warning("Call failed: " <<  ex.reason);
         SD_freeException(&ex);
      }
   }

   //delete []names[0];
   //delete []names;
#else
   Warning("Service discovery not available");
#endif
}
} // anonymous namespace

boost::shared_ptr<FileMapping> 
resolve_filemapping_info(
  const classad::ClassAd& requestAd 
)
{
  DataLocationInterfaceSOAP dli;
  boost::shared_ptr<FileMapping> fm(
    new FileMapping()
  );

  int timeout = (configuration::Configuration::instance()->ns())->dli_si_catalog_timeout();
  std::string dli_service_name = (configuration::Configuration::instance()->wm())->dli_service_name();
  bool  voInJdl = false;
  string vo = requestad::get_virtual_organisation(requestAd, voInJdl);
  bool dataReq = false;
  classad::ExprTree* classAdList
    = glite::jdl::get_data_requirements(requestAd, dataReq);

  if (dataReq) {

    vector<string> dli_url_list; bool getDliFromIS = false;
    classad::ExprList* expr_list = 
      static_cast<classad::ExprList*>(classAdList) ;
    for (classad::ExprList::iterator it = expr_list->begin(); 
       it < expr_list->end(); 
       ++it) {

         bool getDataCatalog = false;
         string dataCatalogType = 
            glite::jdl::get_data_catalog_type( 
               *( static_cast<classad::ClassAd*>(*it) ),
               getDataCatalog
            );
         if (dataCatalogType == "DLI") {
           if (timeout) {
             dli.timeout(timeout);
           }
         }

         string dataCatalogEndpoint;
         bool getDataCatalogEndpoint = false;
         vector<string> v;

         requestad::get_data_catalog(
            *( static_cast<classad::ClassAd*>(*it) ), 
            v,
            getDataCatalogEndpoint
         );

         if( getDataCatalogEndpoint ) 
            dataCatalogEndpoint = v[0];
         else {
            if ( dataCatalogType == "DLI" ) {
               if ( voInJdl ) {
                  get_catalog_url( vo, dli_service_name, dli_url_list );
                  if ( ! dli_url_list.empty() ) getDliFromIS = true;
               }
            }
         }

         if (getDataCatalog && (getDataCatalogEndpoint || getDliFromIS) ) {
            bool validCatalogType = true;
            if (dataCatalogType != "DLI") {
               validCatalogType = false; 
               Warning(dataCatalogType<<": unknown DataCatalogType");
            }

            if (validCatalogType) {
               // the list of LFNs from the JDL
               std::vector<std::string> input_data;
               bool getInputData = false;
               requestad::get_input_data(
                  *(static_cast<classad::ClassAd*>(*it)), 
                  input_data,
                  getInputData
               );
               if (!getInputData ) {
                  Error("cannot get input data from jdl");
               } else {
                  // the list of SFNs matching each LFN
                  std::vector<std::string> resolved_sfn;
                  std::vector<std::string>::const_iterator lfn = input_data.begin();
                  std::vector<std::string>::const_iterator const lfne = input_data.end();
                  for( ; lfn != lfne ; ++ lfn ) {
                     try {

                        Debug("trying to resolve " << *lfn);
                        // the list of SFNs matching each LFN
                        std::vector<std::string> resolved_sfn;
   
                        if (dataCatalogType == "DLI") {
                           string prefix = "";
                           if ((*lfn).find("lds") == 0) prefix = "lds";
                           else if ((*lfn).find("query") == 0) prefix = "query";
                           else if  ((*lfn).find("lfn") == 0) prefix = "lfn";
                           else if ((*lfn).find("guid") == 0) prefix = "guid";
                           else Warning("unknown prefix while using DLI");

                           if  (prefix != "") {
                              if (getDataCatalogEndpoint) {
                                 try {
                                    resolved_sfn = 
                                       dli.listReplicas(
                                          prefix.c_str(), 
                                          *lfn, 
                                          requestAd, 
                                          dataCatalogEndpoint
                                       );
                                 } catch (DLIerror const& e){
                                    Warning(e.what());
                                 }
                              } else if (getDliFromIS) {
                                 for (vector<string>::const_iterator it=
                                          dli_url_list.begin(); 
                                          it != dli_url_list.end(); 
                                          ++it) {
                                    try {
                                       resolved_sfn = 
                                          dli.listReplicas(
                                             prefix.c_str(), 
                                             *lfn, 
                                             requestAd, 
                                             *it
                                          );
                                       if ( ! resolved_sfn.empty() ) break;
                                    }
                                    catch(DLIerror const& e) {
                                       Warning(e.what());
                                    }
                                 } 
                              } else {
                                 Warning("Cannot get any dli endpoint, " <<
                                         "neither from JDL nor from IS");
                              }
                           }
                        } else {
                           Warning("cannot perform " << *lfn <<"'s resolution");
                        }
   
                        if (!resolved_sfn.empty()) {
                           fm->insert(
                             std::make_pair(*lfn, resolved_sfn)
                           );
                           for(unsigned int i = 0; i < resolved_sfn.size(); ++i){
                              Debug(resolved_sfn[i]);
                           }
                        } else {
                           Debug("No replica(s) found!");
                        }
         
                     } catch( std::exception& ex ) {
                        Warning(ex.what());
                     }
                  } // for
               } // if ( getInputData )
            }  // if  ( validCatalogType )
         } else { // ( getDataCatalog )
           Warning("cannot get DataCatalogType or endpoint");
         }
      } // for expr_list
   } else {
      // This block assures backward compatibility for the data 
      // format in the JDL
      const string lfnPrefix = "lfn";
      const string guidPrefix = "guid";
      const string ldsPrefix = "lds";
      const string queryPrefix = "query";
   
      string dliEndpoint = "";

      vector<string> dli_url_list;

      bool dliEndpointSetInJdl = false;
      bool dliEndpointSetInIS = false;
      bool dliEndpointSet = false;
   
      // Check if Storage Index Catalog and Data Catalog endpoints are set in the jdl
      // If it is not the case, we try to get them from the Information Service
      vector<string> v;
      requestad::get_data_catalog(requestAd, v, dliEndpointSetInJdl);
      if (dliEndpointSetInJdl) {
         dliEndpoint = v[0];
      } else {
         if (voInJdl) {
            get_catalog_url(vo, dli_service_name, dli_url_list);
            if (!dli_url_list.empty()) {
               dliEndpointSetInIS = true;
            }
         }
      }

      if ( dliEndpointSetInIS || dliEndpointSetInJdl ) {
         dliEndpointSet = true;
      }
   
      vector<string> input_data;

      bool getInpuData = false; 
      requestad::get_input_data(requestAd, input_data, getInpuData);
      if(!getInpuData){
         Error("cannot get input data from jdl");
         fm.reset();
         return fm;
      }
   
      for(vector<string>::const_iterator lfn = input_data.begin(); lfn != input_data.end();lfn++) {
   
         Debug("trying to resolve: " << *lfn);
         try {
            vector<string> resolved_sfn;
            bool prefix_check = 
               ((*lfn).find(lfnPrefix.c_str()) == 0) || 
               ((*lfn).find(guidPrefix.c_str()) == 0)||
               ((*lfn).find(ldsPrefix.c_str()) == 0)||
               ((*lfn).find(queryPrefix.c_str()) == 0);

             if (dliEndpointSet && prefix_check) { 
               string prefix = "";
               if ((*lfn).find(ldsPrefix.c_str()) == 0) 
                 prefix = ldsPrefix;
               else if ((*lfn).find(queryPrefix.c_str()) == 0) 
                     prefix = queryPrefix;
               else if  ((*lfn).find(lfnPrefix.c_str()) == 0) 
                     prefix = lfnPrefix;
               else if ((*lfn).find(guidPrefix.c_str()) == 0) 
                     prefix = guidPrefix;
                                                                                                                             
               if (dliEndpointSetInJdl) {
                 try {
                        resolved_sfn = dli.listReplicas(
                                          prefix.c_str(), 
                                          *lfn, 
                                          requestAd, 
                                          dliEndpoint
                                       );
                     } catch (DLIerror const& e) {
                        Warning(e.what());
                     } 
               } else if ( dliEndpointSetInIS ) {
                 for ( vector<string>::const_iterator it=
                   dli_url_list.begin(); 
                   it != dli_url_list.end(); 
                   it++) {

                   try {
                     resolved_sfn = dli.listReplicas(
                       prefix.c_str(), 
                       *lfn, 
                       requestAd, 
                       *it);
                      if (!resolved_sfn.empty()) {
                        break;
                      }
                   } catch (DLIerror const& e) {
                     Warning(e.what());
                   }
                 }
              }
            }
            if (!resolved_sfn.empty()) {
               fm->insert(
                 std::make_pair(*lfn, resolved_sfn)
               );
               for (unsigned int i = 0; i < resolved_sfn.size(); ++i){
                 Debug(resolved_sfn[i]);
               }
            } else {
               Warning("No replica(s) found!");
            }
         } catch( std::exception& ex ) {
              Warning(ex.what());
         }
      } // for input_data
   }

   return fm;
}

} // namespace dli
} // namespace brokerinfo
} // namespace wms
} // namespace glite
