//
//  File :     StorageInedxCatalogInterface.cpp
//
//
//  Author :   Enzo Martelli ($Author$)
//  e-mail :   "enzo.martelli@mi.infn.it"
//
//  Revision history :
//  02-12-2004 Original release
//
//  Description:
//  Wraps the Storage Index Catalog soap client API
//
//
//  Copyright (c) 2004 Istituto Nazionale di Fisica Nucleare (INFN).
//  All rights reserved.
//  See http://grid.infn.it/grid/license.html for license details.
//


#include <string.h>
#include <malloc.h>

#include <storageindexH.h>
#include <storageindex.nsmap>


extern "C" {
//   #include "cgsi_plugin.h"
   #include "glite/security/glite_gsplugin.h"
}

#include "StorageIndexCatalogInterface.h"
#include <classad_distribution.h>
#include "glite/jdl/PrivateAdManipulation.h"
#include <string>

using namespace glite::wms;
namespace jdl = glite::jdl;

glite::wms::brokerinfo::sici::StorageIndexCatalogInterface::StorageIndexCatalogInterface()
{
   // Initialise SOAP
   //
   soap_init(&m_soap);
   m_soap.namespaces = storageindex_namespaces;
                                                                                                     
//   m_endpoint = endpoint;

   ctx = NULL;
}

glite::wms::brokerinfo::sici::StorageIndexCatalogInterface::StorageIndexCatalogInterface(int timeout)
{
   // Initialise SOAP
   //
   soap_init(&m_soap);
   m_soap.namespaces = storageindex_namespaces;
  
   // connection timeout 
   m_soap.connect_timeout = timeout;

   // IO timeouts
   m_soap.send_timeout = timeout;
   m_soap.recv_timeout = timeout;

//   m_endpoint = endpoint;

   ctx = NULL;
}

void glite::wms::brokerinfo::sici::StorageIndexCatalogInterface::listSEbyGUID( const std::string &guid, std::vector<std::string> & list, const classad::ClassAd & ad, const std::string& endpoint ) 
{

   //////////////...for using secure endpoint
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
         if ( !ctx ){
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

////////////////////////////////////////////////////////
   struct ns1__listSEbyGUIDResponse output;
   
   char  *guidCopy = new char[guid.length() + 1];
   
   strcpy(guidCopy, guid.c_str());
   
   if (soap_call_ns1__listSEbyGUID(&m_soap, endpoint.c_str(), NULL, guidCopy, output) ) {
      delete [] guidCopy;

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

   for (int i=0; i < output._listSEbyGUIDReturn->__size; i++ ) {
      std::string str = output._listSEbyGUIDReturn->__ptr[i];
      list.push_back(str);
   }


   delete [] guidCopy;

}

void 
glite::wms::brokerinfo::sici::StorageIndexCatalogInterface::listSEbyLFN( const std::string &lfn, std::vector<std::string> & list, const classad::ClassAd & ad, const std::string& endpoint) 
{
   //////////////...for using secure endpoint
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

   struct ns1__listSEbyLFNResponse output;
				                    
   char  *lfnCopy = new char[lfn.length() + 1];
   
   strcpy(lfnCopy, lfn.c_str());   
 
   if( soap_call_ns1__listSEbyLFN(&m_soap, endpoint.c_str(), NULL, lfnCopy, output) ) {
      delete [] lfnCopy;

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

   for (int i=0; i < output._listSEbyLFNReturn->__size; i++ ) {
      std::string str = output._listSEbyLFNReturn->__ptr[i];
      list.push_back(str);
   }

   delete [] lfnCopy;

}
    


glite::wms::brokerinfo::sici::StorageIndexCatalogInterface::~StorageIndexCatalogInterface()
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
extern "C" glite::wms::brokerinfo::sici::StorageIndexCatalogInterface* create()
{
  return new glite::wms::brokerinfo::sici::StorageIndexCatalogInterface();
}

extern "C" glite::wms::brokerinfo::sici::StorageIndexCatalogInterface* create_with_timeout( int timeout)
{
  return new glite::wms::brokerinfo::sici::StorageIndexCatalogInterface( timeout);
}

 
extern "C" void destroy(glite::wms::brokerinfo::sici::StorageIndexCatalogInterface* p) {
  delete p;
}

