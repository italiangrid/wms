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

//
//  File :     DataLocationInterfaceSOAP.h
//
//
//  Authors :   Enzo Martelli ($Author: mcecchi $)
//  e-mail :   "enzo.martelli@mi.infn.it"
//              Marco Cecchi
//
//  Revision history :
//  02-12-2004 Original release
//
//  Description:
//  Wraps the DLI soap client API
//
//
//  All rights reserved.
//  See http://grid.infn.it/grid/license.html for license details.
//

#ifndef GLITE_WMS_BROKERINFO_DLI_DATALOCATIONINTERFACESOAP_H
#define GLITE_WMS_BROKERINFO_DLI_DATALOCATIONINTERFACESOAP_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include <stdsoap2.h>

extern "C" {
   #include "glite/security/glite_gsplugin.h"
}

#include <classad_distribution.h>

typedef struct soap gsoap_t;

namespace glite {
namespace wms {
namespace brokerinfo {
namespace dli {

class DLIerror: public std::exception {
  std::string m_error;
public:
  DLIerror(std::string const& error) : m_error(error) { }
  ~DLIerror() throw() { }
  char const* what() const throw()
  { 
    return m_error.c_str();
  }
};

class DataLocationInterfaceSOAP {
public:
  /**
   * Constructor for DataLocationInterface
   *
   */
  DataLocationInterfaceSOAP();

  /**
   * Constructor for DataLocationInterface
   *
   * @param timeout  connection and IO timeout
   */
  void timeout(int timeout);

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
   * @param endpoint SOAP endpoint (URL) of the remote catalogue
   *                           example: http://localhost:8085/
   *
   * @returns a vector of URLs that represent the locations of where
   *          the InputData is located. The URL can either be a full URL
   *          of the form    protocol://hostname/pathname
   *          or             hostname
   *          where hostname is a registered SEId.
   */
  virtual std::vector<std::string> listReplicas(std::string inputDataType,
						std::string inputData,
                                                const classad::ClassAd& ad,
                                                const std::string& endpoint);
  /**
   * Destructor: clean up the SOAP environment
   */
  virtual ~DataLocationInterfaceSOAP();

private:
  gsoap_t m_soap;     // gSOAP structure for message exchange
  glite_gsplugin_Context m_ctx; // gsoap plugin context
};

} // namespace dli
} // namespace brokerinfo
} // namespace wms
} // namespace glite

#endif // GLITE_WMS_BROKERINFO_DLI_DATALOCATIONINTERFACESOAP_H
