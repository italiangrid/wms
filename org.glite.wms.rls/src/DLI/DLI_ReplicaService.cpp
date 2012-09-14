//
//  File :     DLI_ReplicaService.cpp
//
//
//  Author :   Enzo Martelli 
//  e-mail :   "enzo.martelli@ct.infn.it"
//
//
//  Description:
//  Wraps the DLI soap client API
//
//
//  Copyright (c) 2004 Istituto Nazionale di Fisica Nucleare (INFN).
//  All rights reserved.
//  See http://grid.infn.it/grid/license.html for license details.
//


#include "ReplicaServiceException.h"
#include "DLI_ReplicaService.h"
//#include "gsoap_rls_utils.h"

#include "datalocationinterfaceH.h"
#include "datalocationinterface.nsmap"

#include <boost/algorithm/string/predicate.hpp>

using namespace datalocationinterface;
using namespace std;


namespace glite{
namespace wms{
namespace rls{
namespace DLI{

namespace{

const string 
exception_reason( struct soap& the_soap) {

  std::string ex;
  if( the_soap.error ) {

    soap_set_fault( & the_soap );


    const char** faultdetail_ptr = soap_faultdetail( & the_soap );
    std::string faultdetail;
    if ( *faultdetail_ptr != NULL ) faultdetail = *faultdetail_ptr;
       else faultdetail = "unknown";


    const char** faultcode_ptr = soap_faultcode( & the_soap );
    std::string faultcode;
    if ( *faultcode_ptr != NULL ) faultcode = *faultcode_ptr;
       else faultcode = "unknown";


    const char** faultstring_ptr = soap_faultstring( & the_soap );
    std::string faultstring;
    if (*faultstring_ptr != NULL ) faultstring = *faultstring_ptr;
       else faultstring = "unknown";


    std::string SOAP_FAULT_CODE = "SOAP_FAULT_CODE: ";
    std::string SOAP_FAULT_STRING = "SOAP_FAULT_STRING: ";
    std::string SOAP_FAULT_DETAIL = "SOAP_FAULT_DETAIL: ";
    std::string new_line = "\n";
                                                                                                 
    ex = new_line + SOAP_FAULT_CODE + faultcode + new_line +
                    SOAP_FAULT_STRING +  faultstring + new_line +
                    SOAP_FAULT_DETAIL + faultdetail + new_line;
  }
  else {
    ex = "Error in soap request towards the catalog. Unknown error.";
  }
  return ex;

}

}//anonymous namespace


DLI_ReplicaService::DLI_ReplicaService(
   const std::string& endpoint, 
   const std::string& proxy, 
   int timeout
): m_endpoint(endpoint), m_proxy(proxy)
{
  soap_init1(&m_soap, SOAP_IO_CHUNK);
  m_soap.namespaces = datalocationinterface_namespaces;

  if (timeout){
     m_soap.connect_timeout = timeout;

     m_soap.send_timeout = timeout;
     m_soap.recv_timeout = timeout;
  }

  m_ctx = NULL;
} 


void 
DLI_ReplicaService::listReplica(
   const std::string& inputData,
   std::vector<std::string>& sfns)
{

  if( 0 == strncasecmp(m_endpoint.c_str(),"https://",8) ){
    if ( !m_ctx ) { //if "ctx != NULL", it's supposed to be already initialized
      if ( glite_gsplugin_init_context(&m_ctx) ) {
               throw ReplicaServiceException("gsplugin_init_context FAILED");
      }
      //glite_gsplugin_free_context will release *both* cert and key
      m_ctx->cert_filename = strdup(m_proxy.c_str());
      m_ctx->key_filename = strdup(m_proxy.c_str());

      if ( soap_register_plugin_arg(&m_soap, glite_gsplugin, m_ctx) ) {
         throw ReplicaServiceException("soap_register_plugin FAILED");
      }
    }
  }

  std::string prefix;
  if ( boost::algorithm::starts_with(inputData,"lfn") ) prefix = "lfn";
  else if( boost::algorithm::starts_with(inputData, "guid") ) prefix = "guid";
  else if( boost::algorithm::starts_with(inputData,"query") ) prefix = "query";
  else if( boost::algorithm::starts_with(inputData,"lds") ) prefix = "lds";
  else throw ReplicaServiceException("wrong dli prefix");


  struct datalocationinterface__listReplicasResponse theList;
  
  if (soap_call_datalocationinterface__listReplicas(&m_soap, m_endpoint.c_str(), "",
                                  prefix, inputData, theList)) {
    string ex = "trying to resolve "+inputData+"\n";

    throw ReplicaServiceException(ex+exception_reason(m_soap));
  }

  for (int i=0; i < (theList.urlList)->__size; i++ )  {
    string str(*((theList.urlList)->__ptritem)[i]);
    sfns.push_back( str );
  }

} 



DLI_ReplicaService::~DLI_ReplicaService()
{
  soap_destroy(&m_soap);
  soap_end(&m_soap);
  soap_done(&m_soap);

  if ( m_ctx ) glite_gsplugin_free_context(m_ctx);

} 

extern "C" DLI_ReplicaService* create_DLI(
                       std::string endpoint, 
                       std::string proxy, 
                       int timeout
           )
{
  return new DLI_ReplicaService(endpoint, proxy, timeout);
}


extern "C" void destroy_DLI(DLI_ReplicaService* p) {
  delete p;
}

} //DLI
} //rls
} //wms
} //glite


