//
//  File :     DataLocationInterfaceSOAP.cpp
//
//
//  Authors :   Heinz Stockinger 
//  e-mail :   "heinz.stockinger@cern.ch"
//              Marco Cecchi
//
//  Revision history :
//  Original release has been implemented for LCG 
//  02-12-2004 Integration in glite
//
//  Description:
//  Wraps the DLI soap client API
//
//
// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

#include <DataLocationInterfaceSOAP.h>
#include <datalocationinterfaceH.h>
#include <datalocationinterface.nsmap>

extern "C" {
   #include "glite/security/glite_gsplugin.h"
   #include "glite/security/glite_gsplugin-int.h"
}

#include <classad_distribution.h>
#include "glite/jdl/PrivateAdManipulation.h"

//using namespace datalocationinterface;
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
  : m_ctx(0)
{
  soap_init1(&m_soap, SOAP_IO_KEEPALIVE); // SOAP_IO_KEEPALIVE/SOAP_IO_CHUNK
  m_soap.namespaces = datalocationinterface_namespaces;
}

void dli::DataLocationInterfaceSOAP::timeout(int timeout)
{
  //if (m_soap has been initialized) {
    m_soap.connect_timeout = timeout;
    // IO timeouts
    m_soap.send_timeout = timeout;
    m_soap.recv_timeout = timeout;
  //}
}

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
   } catch(...) {
      proxyInJdl = false;
   }
                                                                                                                                   
   if(0 == strncasecmp(endpoint.c_str(), "https://", 8)) {
      if (proxyInJdl) {
         if (!m_ctx) {
            if ( glite_gsplugin_init_context(&m_ctx) ) {
               throw DLIerror("gsplugin_init_context FAILED");
            }
         }
         if (glite_gsplugin_set_credential(m_ctx, proxy.c_str(), proxy.c_str())) {
            std::string gss_err(m_ctx->error_msg); 
            glite_gsplugin_free_context(m_ctx);
            m_ctx = NULL;
            throw DLIerror("Cannot set credentials in the gsoap-plugin context: " + gss_err);
         }
      } else {
        throw DLIerror("UserProxy not specified in the ClassAd");
      }
                                                                                                                                   
      if (soap_register_plugin_arg(&m_soap, glite_gsplugin, m_ctx)) {
         std::stringstream ss;
         ss << m_soap.error;
         std::string soap_err = ss.str();

         throw DLIerror("soap_register_plugin_arg FAILED: " + soap_err);
      }
   }

   std::vector<std::string> urlVector;
   struct datalocationinterface__listReplicasResponse theList;
  
   // Call listReplicas and handle potential SOAP Faults
   if (soap_call_datalocationinterface__listReplicas(
     &m_soap,
     endpoint.c_str(),
     "",
     inputDataType, inputData, theList)) {

      std::string ex;
      if (m_soap.error) {
         soap_set_fault(&m_soap);

         const char** faultdetail_ptr = soap_faultdetail(&m_soap);
         std::string faultdetail;
         if (*faultdetail_ptr != NULL) {
           faultdetail = *faultdetail_ptr;
         } else {
           faultdetail = "unknown";
         }                                                                                                                   
         const char** faultcode_ptr = soap_faultcode(&m_soap);
         std::string faultcode;
         if ( *faultcode_ptr != NULL ) faultcode = *faultcode_ptr;
            else faultcode = "unknown";

         const char** faultstring_ptr = soap_faultstring(&m_soap);
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
      } else {
         ex = "Error in soap request towards StorageIndex Catalog. Unknown error.";
      }
                                                                                                                             
      throw DLIerror(ex);
   }

   for (int i = 0; i < (theList.urlList)->__size; i++)  {
      std::string str( ((theList.urlList)->__ptritem)[i] );
      urlVector.push_back( str );
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

  if (m_ctx) {
    glite_gsplugin_free_context(m_ctx);
  }
} // destructor

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
