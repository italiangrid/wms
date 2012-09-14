//
//  File :     STorageInedxCatalogInterface.h
//
//
//  Author :   Enzo Martelli ($Author$)
//  e-mail :   "enzo.martelli@mi.infn.it"
//
//  Revision history :
//  02-12-2004 Original release
//
//  Description:
//  Wraps the Storage Index soap Catalog client API
//   
//   
//  Copyright (c) 2004 Istituto Nazionale di Fisica Nucleare (INFN). 
//  All rights reserved.
//  See http://grid.infn.it/grid/license.html for license details. 
//
#ifndef GLITE_WMS_BROKERINFO_STORAGE_INDEX_CATALOG_INTERFACE_H
#define GLITE_WMS_BROKERINFO_STORAGE_INDEX_CATALOG_INTERFACE_H 

#include <string>
#include <vector>

#include <stdsoap2.h>
#include <classad_distribution.h>

extern "C" {
//   #include "cgsi_plugin.h"
   #include "glite/security/glite_gsplugin.h"
}


namespace glite {
namespace wms {
namespace brokerinfo {
namespace sici {

class StorageIndexCatalogInterface
{
public:
  /**
   * Constructor for StorageIndexCatalogInterface
   *
   */
   StorageIndexCatalogInterface( );

  /**
   * Constructor for StorageIndexCatalogInterface
   *
   * @param timeout connenction and IO timeout 
   *
   */
   StorageIndexCatalogInterface(int timeout );

  /**
   * Fill the list with the SEs conteining replicas related to the guid
   *
   * @param guid guid passed as input parameter
   *
   * @param list output parameter
   *
   * @param classad JDL classad 
   *
   * @param endpoint SOAP endpoint (URL) of the remote catalogue
   *
   */
   virtual
   void listSEbyGUID ( const std::string &guid, std::vector<std::string> & list, 
                       const  classad::ClassAd & classad,
                       const std::string &endpoint);

  /**
   * Fill the list with the SEs containing replicas related to the lfn
   *
   * @param lfn lfn passed as input parameter
   *
   * @param list output parameter
   *
   * @param classad JDL classad
   *
   * @param endpoint SOAP endpoint (URL) of the remote catalogue
   *
   */
   virtual
   void listSEbyLFN ( const std::string &lfn, std::vector<std::string> & list,
                      const classad::ClassAd & classad,
                      const std::string &endpoint);

  /**
   * Destructor: clean up the SOAP environment
   */
   virtual
   ~StorageIndexCatalogInterface();
  
private:
  struct soap m_soap;     // gSOAP structure for message exchange
  //std::string m_endpoint; // endpoint (URL) of the data catalogue to contact
  glite_gsplugin_Context ctx; // gsoap plugin context

};

typedef StorageIndexCatalogInterface* create_t();

typedef StorageIndexCatalogInterface* create_t_with_timeout( int timeout);

typedef void destroy_t(StorageIndexCatalogInterface*);

} // end namespace sici
} // end namespace brokerinfo
} // end namespace wms
} // end namespace glite


#endif 
    
