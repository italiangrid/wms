// Copyright (c) Members of the EGEE Collaboration. 2004. 
// See http://www.eu-egee.org/partners/ for details on the copyright
// holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 

//     http://www.apache.org/licenses/LICENSE-2.0 

// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

#ifndef  GLITE_WMS_WMPROXYAPICPPUTILITIES_H
#define GLITE_WMS_WMPROXYAPICPPUTILITIES_H
#include "glite/wms/wmproxyapi/wmproxy_api.h"

/**
* \file wmproxy_api_utilities.h
* \brief wsdl wmproxy service wrapper
* A wrapper around wmproxy Web Service. It provides primitive or simple structure to access more complicated service methods
*/

#include <iostream>
#include <string>
#include <vector>

namespace glite {
namespace wms {
namespace wmproxyapiutils {

/*
* Gets the current time
* @return the seconds elapsed since Jan 1, 1970
*/
const time_t getTime( );
/**
* 	Checks if an absolute path exists on the local machine
*	@param path the pathname to be checked
*	@return the pathname string if it exists, NULL if there is no such path
*/
const char* checkPathExistence(const char* path);
/**
*	Returns the local pathname of the directory containing the trusted certificates.
*	This information retrieved by the input configuration context if the input context object is not NULL,
*	otherwise the environment variable X509_CERT_DIR is checked .
* 	If no valid information is found in both previous objects, the default path is checked (/etc/grid-security/certificates)
*	@param cfs the configuration context  (NULL otherwise)
*	@return the pathname string, NULL if no valid path has been found
*/
const char* getTrustedCert(glite::wms::wmproxyapi::ConfigContext *cfs=NULL);

/**
*	Returns the pathname of the user proxy file.
*	This information is retrieved by the input configuration context if the input context object is not NULL,
* 	otherwise the environment variable X509_PROXY_FILE is checked.
*	If no valid information is found in both previous objects, the default location is checked (/tmp/x509up_u(uid))
*	@param cfs The input configuration context (defines the location of the CA certificates, user proxy and the endpoint URL); NULL otherwise
*	@return The pathname string, NULL if no valid pathname has been found
**/
const char* getProxyFile(glite::wms::wmproxyapi::ConfigContext *cfs=NULL);
/**
*	Returns the endpoint URL registerred in the input configuration context
*	@param cfs The input configuration context (defines the location of the CA certificates, user proxy and the endpoint URL)
*	@return The URL string, NULL if no valid URL has been found
**/
const char* getEndPoint (glite::wms::wmproxyapi::ConfigContext *cfs=NULL);
/**
*	Returns time left of the input proxy certificate (in minutes)
*	@param pxfile The proxy file pathname
*	@return The number of minutes remaining until the expiration time
**/
const long getProxyTimeLeft(std::string pxfile);
/**
*	Returns time left of the input proxy certificate (in seconds)
*	@param pxfile The proxy file pathname
*	@return The number of seconds remaining until the expiration time
**/
const long getCertTimeLeft(std::string pxfile);

/**
*	Returns the list of the FQAN's of a voms-proxy file
*	@param pxfile the proxy file pathname
*	@return A vector contained the string list of the FQAN's
*	@throws BaseException If any error occurred during the reading of the proxy information
*	@see glite::wms::wmproxyapi::BaseException
**/
const std::vector<std::string> getFQANs(std::string pxfile);

} // wmproxy namespace
} // wms namespace
} // glite namespace
#endif
//EOF
