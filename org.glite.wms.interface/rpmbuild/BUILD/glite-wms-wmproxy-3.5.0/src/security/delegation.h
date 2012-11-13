/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

//
// File: delegation.h
// Author: Giuseppe Avellino <egee@datamat.it>
//

#ifndef GLITE_WMS_WMPROXY_WMPDELEGATION_H
#define GLITE_WMS_WMPROXY_WMPDELEGATION_H

namespace glite {
namespace wms {
namespace wmproxy {
namespace security {

/**
 * Gets a delegated Proxy request. A Proxy request must be done in order
 * to obtain a request to create a delegated Proxy. To obtain a Proxy
 * request a delegation identifier must be provided
 * @param delegation_id the delegation id identifing the delegation session
 * @return the Proxy request
 * @see putProxy
 */
std::string getProxyRequest(const std::string& delegation_id);

/**
 * Creates the delegated Proxy. The Proxy is created inside the delegated
 * Proxy path
 * @param delegation_id the delegation id identifing the delegation session
 * @param proxy_req the request obtained with the method getProxyReq
 * @see getProxyRequest
 */
void putProxy(const std::string& delegation_id, const std::string& proxy_req);

/**
 * Restarts the delegation procedure by asking for a certificate signing
 * requeste from the server for an already existing delegation ID
 * The server answers with a certificate signing request which includes
 * the public key for new delegated credendials.
 * putProxy() has to be called to finish the procedure
 * @return the Proxy request
 * @see putProxy
 */
std::string renewProxyRequest(const std::string& delegation_id);

/**
 * Gets a delegated Proxy request. A Proxy request must be done in order
 * to obtain a request to create a delegated Proxy. The delegation identifier
 * is automatically generated.
 * @return a pair containing the generated delegation id and the Proxy request
 * @see putProxy
 */
std::pair<std::string, std::string> getNewProxyRequest();

/**
 * Destroys the delegated Proxy.
 * @param delegation_id the delegation id identifying the delegation session
 */
void destroyProxy(const std::string& delegation_id);

/**
 * Gets the delegated proxy termination time.
 * @param delegation_id the delegation id identifing the delegated session
 * @return the termination time
 */
time_t getTerminationTime(const std::string& delegation_id);

/**
 * Gets the delegated proxy path. This is the Proxy corresponding to the
 * delegation identifier passed as argument and to the specific user. The
 * user is represented by the Distinguished Name contained inside the request
 * environment
 * @param delegation_id the delegation id identifing the delegated session
 * @return the user DN
 */
std::string getDelegatedProxyPath(std::string const& delegation_id, std::string const& dn);

/**
* Retrieve version number of service implementation
 * @return delegation WSDL version
*/
std::string getDelegationVersion();

/**
* Retrieve version number of service interface
 * @return delegation WSDL interface version
*/
std::string getDelegationInterfaceVersion();

}}}}

#endif // GLITE_WMS_WMPROXY_WMPDELEGATION_H
