//
//  File :     DataLocationInterfaceSOAP.cpp
//
//
//  Author :   Heinz Stockinger 
//  e-mail :   "heinz.stockinger@cern.ch"
//
//  Revision history :
//  Original release has been implemented for LCG 
//  02-12-2004 Integration in glite
//
//  Description:
//  Wraps the DLI soap client API
//
//
//  Copyright (c) 2004 Istituto Nazionale di Fisica Nucleare (INFN).
//  All rights reserved.
//  See http://grid.infn.it/grid/license.html for license details.
//

#include <DataLocationInterfaceSOAP.h>

#include <datalocationinterfaceH.h>
#include <datalocationinterface.nsmap>


extern "C" {
   #include "glite/security/glite_gsplugin.h"
}


#include <classad_distribution.h>
#include "glite/jdl/PrivateAdManipulation.h"


namespace dli = glite::wms::brokerinfo::dli;
namespace jdl = glite::jdl;

/*****************************************************************************/
/*                          c o n s t r u c t o r                            */
/*****************************************************************************/

/**
 * Constructor for DataLocationInterface
 *
 * @param endpoint SOAP endpoint (URL) of the remote catalogue
 *                 example: http://localhost:8085/
 */
dli::DataLocationInterfaceSOAP::DataLocationInterfaceSOAP()
{
  // Initialise SOAP
  //
  soap_init1(&m_soap, SOAP_IO_CHUNK);
  m_soap.namespaces = datalocationinterface_namespaces;

//  m_endpoint = endpoint;

  ctx = NULL;

} // Constructor

dli::DataLocationInterfaceSOAP::DataLocationInterfaceSOAP( int timeout)
{
  // Initialise SOAP
  //
  soap_init1(&m_soap, SOAP_IO_CHUNK);
  m_soap.namespaces = datalocationinterface_namespaces;

  // connection timeout 
  m_soap.connect_timeout = timeout;
  // IO timeouts
  m_soap.send_timeout = timeout;
  m_soap.recv_timeout = timeout;

  ctx = NULL;
                                                                                                     
//  m_endpoint = endpoint;
} // Constructor



/*****************************************************************************/
/*                          l i s t R e p l i c a s                          */
/*****************************************************************************/

/**
 * List all replicas of a given InputDataType. A replica needs to contain
 * a valid SEId that is registered with the Information Service.
 *
 * @param inputDataType Defines one of the following InputDataTypes:
 *                      lfn   ... LogicalFileName
 *                      guid  ... GUID Global Unique Idenifier
 *                      lds   ... LogicalDataSet
 *                      query ... generic query to the catalogue
 *        Further InputDataTypes can be extended in the future but need to
 *        be understood by the remote catalogue. 
 *        Note that a catalogue does not need to implement all of the four
 *        InputDataTypes but is free to support any subset.
 * @param inputData     Actutual InputData variable
 *
 * @returns a vector of URLs that represent the locations of where
 *          the InputData is located. The URL can either be a full URL
 *          of the form    protocol://hostname/pathname
 *          or             hostname
 *          where hostname is a registered SEId.
 */
std::vector<std::string> 
dli::DataLocationInterfaceSOAP::listReplicas(std::string inputDataType,
					     std::string inputData,
                                             const classad::ClassAd & ad,
                                             const std::string& endpoint)
{
///////////////////...for using secure endpoint

   bool proxyInJdl = true;
   std::string proxy;
   try {
      proxy =  jdl::get_x509_user_proxy(ad);
   }
   catch(...) {
      proxyInJdl = false;
   }
                                                                                                                                   
   if( 0 == strncasecmp(endpoint.c_str(),"https://",8) ){
                                                                                                                                   
      if ( proxyInJdl ){
         if ( !ctx ) {
            if ( glite_gsplugin_init_context(&ctx) ) {
               throw string("gsplugin_init_context FAILED");
            }
         }
         //glite_gsplugin_free_context will free *both* cert and key
         ctx->cert_filename = strdup(proxy.c_str());
         ctx->key_filename = strdup(proxy.c_str());
      }
      else throw string("UserProxy not specified in the ClassAd");
                                                                                                                                   
      if ( soap_register_plugin_arg(&m_soap, glite_gsplugin, ctx) ) {
         throw string("soap_register_plugin_arg FAILED");
      }
                                                                                                                                   
   }
                                                                                                                                   
////////////////////////////////////////////////////

   std::vector<std::string> urlVector;

   //URLArray urlList;

   struct ns1__listReplicasResponse theList;
  
   // Call listReplicas and handle potential SOAP Faults
   //
   if (soap_call_ns1__listReplicas(&m_soap, endpoint.c_str(), "",
                                  inputDataType, inputData, theList)) {
//   if (soap_call_ns__listReplicas(&m_soap, endpoint.c_str(), "",
//                                   inputDataType, inputData, &urlList)) {
      // In case of a SOAP Fault, throw an error string that the server sent
      //
      std::string ex;
      if( m_soap.error ) {
                                                                                                                             
         soap_set_fault( & m_soap );
                                                                                                                             
         const char** faultdetail_ptr = soap_faultdetail( & m_soap );
         std::string faultdetail;
         //if ( faultdetail_ptr[0] != NULL ) faultdetail = faultdetail_ptr[0];
         if ( *faultdetail_ptr != NULL ) faultdetail = *faultdetail_ptr;
            else faultdetail = "unknown";
                                                                                                                             
         const char** faultcode_ptr = soap_faultcode( & m_soap );
         std::string faultcode;
         if ( *faultcode_ptr != NULL ) faultcode = *faultcode_ptr;
            else faultcode = "unknown";
                                                                                                                             
         const char** faultstring_ptr = soap_faultstring( & m_soap );
         std::string faultstring;
         if (*faultstring_ptr != NULL ) faultstring = *faultstring_ptr;
            else faultstring = "unknown";
                                                                                                                             
         std::string SOAP_FAULTCODE = "SOAP_FAULTCODE: ";
         std::string SOAP_FAULTSTRING = "SOAP_FAULTSTRING: ";
         std::string SOAP_FAULT_DETAIL = "SOAP_FAULT_DETAIL: ";
         std::string new_line = "\n";
                                                                                                                             
         ex = new_line + SOAP_FAULTCODE + faultcode + new_line +
                         SOAP_FAULTSTRING +  faultstring + new_line +
                         SOAP_FAULT_DETAIL + faultdetail + new_line;
      }
      else {
         ex = "Error in soap request towards StorageIndex Catalog. Unknown error.";
      }
                                                                                                                             
      throw ex;
                                                                                                                             
   }

    
   // Convert the URLArray into a vector that we then return to the caller
   
   //for (int i=0; i < urlList.__size; i++ ) {
   //  urlVector.push_back(urlList[i]);
   //}

   for (int i=0; i < (theList.urlList)->__size; i++ )  {
      urlVector.push_back(  ((theList.urlList)->__ptritem)[i]  );
   }

   return urlVector;
} // listReplicas


/*****************************************************************************/
/*                            d e s t r u c t o r                            */
/*****************************************************************************/

dli::DataLocationInterfaceSOAP::~DataLocationInterfaceSOAP()
{
  // Finalise SOAP
  //
  soap_destroy(&m_soap);
  soap_end(&m_soap);
  soap_done(&m_soap);

  if ( ctx ) glite_gsplugin_free_context(ctx);

} // destructor

/*****************************************************************************/
/*  Class factories that can be used for a plug-in                           */
/*****************************************************************************/

extern "C" dli::DataLocationInterfaceSOAP* create_dli()
{
  return new dli::DataLocationInterfaceSOAP();
}

extern "C" dli::DataLocationInterfaceSOAP* create_dli_with_timeout(int timeout)
{
  return new dli::DataLocationInterfaceSOAP(timeout);
}


extern "C" void destroy_dli(dli::DataLocationInterfaceSOAP* p) {
  delete p;
}


/***************************************************************************/
/*                         URLArray                                        */
/***************************************************************************/

/**
 * URLArray: class to implement a string array in gSOAP. The class is
 *           defined in DataLocationInterface.h
 */
/*
URLArray::URLArray()
{ __ptr = NULL;
  __size = 0;
  soap = NULL;
} // URLArray constructor

std::string& URLArray::operator[](int i) const 
{
  assert(__ptr && i >= 0 && i < __size);
  return __ptr[i];
} // URLArray::operator
*/
