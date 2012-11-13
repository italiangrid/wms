/* Copyright (c) Members of the EGEE Collaboration. 2004.
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
limitations under the License.  */

#include <string>
#include <iostream>
#include <boost/lexical_cast.hpp>

#include "utilities/wmpexceptions.h"
#include "utilities/wmpexception_codes.h"
#include "utilities/utils.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "utilities/logging.h"

extern "C" {
#include "gridsite.h"
}

namespace glite {
namespace wms {
namespace wmproxy {
namespace security {

const char* GRST_PROXYCACHE = "proxycache";
const char* DOCUMENT_ROOT = "DOCUMENT_ROOT";

// gLite environment variables
const char* GLITE_LOCATION = "GLITE_LOCATION";
const char* GLITE_WMS_LOCATION = "GLITE_WMS_LOCATION";
// This Number will follow loaded DELEGATION interface wsdl version
const std::string DELEGATION_VERSION_NUMBER= "2.0.0";
const std::string DELEGATION_INTERFACE_VERSION_NUMBER= "2.0.0";

using namespace std;

namespace logger       = glite::wms::common::logger;
namespace wmputilities = glite::wms::wmproxy::utilities;

namespace
{
string
getProxyDir()
{
   GLITE_STACK_TRY("getProxyDir()");
   edglog_fn("getProxyDir");

   char* docroot = getenv(DOCUMENT_ROOT);
   if (!docroot) {
      edglog(fatal)<<"Unable to get DOCUMENT_ROOT environment variable value"
                   <<endl;
      throw wmputilities::FileSystemException(__FILE__, __LINE__,
                                              "getProxyDir()", wmputilities::WMS_FILE_SYSTEM_ERROR,
                                              "Unable to get DOCUMENT_ROOT environment variable value\n(please "
                                              "contact server administrator)");
   }

   char* proxydir;
   asprintf(&proxydir, "%s/%s", docroot, GRST_PROXYCACHE);
   string returnproxydir = string(proxydir);
   free(proxydir);

   // Creating proxydir if it doesn't exist
   wmputilities::createSuidDirectory(returnproxydir);

   return returnproxydir;

   GLITE_STACK_CATCH();
}
}

string
getDelegatedProxyPath(string const& delegation_id, std::string const& dn)
{
   GLITE_STACK_TRY("getDelegatedProxyPath()");
   edglog_fn("getDelegatedProxyPath");

   char* delegated_proxy = GRSTx509CachedProxyFind((char*) getProxyDir().c_str(),
                           (char*) delegation_id.c_str(), const_cast<char*>(dn.c_str()));
   if (delegated_proxy == NULL) {
      edglog(critical)<<"Unable to get delegated Proxy"<<endl;
      throw wmputilities::JobOperationException(__FILE__, __LINE__,
            "regist()", wmputilities::WMS_DELEGATION_ERROR,
            "Unable to get delegated Proxy");
   }
   string path = "";
   int i = 0;
   while (delegated_proxy[i] != '\0') {
      path += delegated_proxy[i];
      i++;
   }
   return path;

   GLITE_STACK_CATCH();
}

string
getProxyRequest(const string& original_delegation_id)
{
   GLITE_STACK_TRY("getProxyRequest()");
   edglog_fn("getProxyRequest");

   // Initialise delegation_id
   string delegation_id = original_delegation_id;
   if (original_delegation_id== "" ) {
#ifndef GRST_VERSION
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "renewProxyRequest()", wmputilities::WMS_PROXY_ERROR,
            "Empty delegation id not allowed with delegation 1");
#else
      delegation_id=string(GRSTx509MakeDelegationID());
      edglog(debug)<<"Automatically generated Delegation ID: "<<delegation_id<<endl;
#endif
   }
   edglog(debug)<<"Delegation ID: "<<delegation_id<<endl;

   char* request = 0;
   if (int ret = GRSTx509MakeProxyRequest(&request, (char*) getProxyDir().c_str(),
                                          (char*) delegation_id.c_str(), const_cast<char*>(wmputilities::getDN_SSL().c_str()))) {
      edglog(critical)<<"Unable to complete Proxy request"<<endl;
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "getProxyReq()", wmputilities::WMS_PROXY_ERROR,
            "Unable to complete Proxy request"
            ", GRSTx509MakeProxyRequest returned " + boost::lexical_cast<std::string>(ret));
   }

   string proxy_req = "";
   int i = 0;
   while (request[i] != '\0') {
      proxy_req += request[i];
      i++;
   }
   return proxy_req;

   GLITE_STACK_CATCH();
}

void
putProxy(const string& original_delegation_id, const string& proxy_req)
{
   GLITE_STACK_TRY("putProxy()");
   edglog_fn("putProxy");

   // Initialise delegation_id
   string delegation_id = original_delegation_id;
   if (original_delegation_id== "" ) {
#ifndef GRST_VERSION
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "renewProxyRequest()", wmputilities::WMS_PROXY_ERROR,
            "Empty delegation id not allowed with delegation 1");
#else
      delegation_id=string(GRSTx509MakeDelegationID());
      edglog(debug)<<"Automatically generated Delegation ID: "<<delegation_id<<endl;
#endif
   }

   edglog(debug)<<"Proxy dir: "<<getProxyDir()<<endl;
   edglog(debug)<<"delegation id: "<<delegation_id<<endl;
   edglog(debug)<<"User DN: " << wmputilities::getDN_SSL() << endl;
   if (GRSTx509CacheProxy((char*) getProxyDir().c_str(),
                          (char*) delegation_id.c_str(), const_cast<char*>(wmputilities::getDN_SSL().c_str()),
                          (char*) proxy_req.c_str()) != GRST_RET_OK) {
      edglog(critical)<<"Unable to store client Proxy"<<endl;
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "putProxy()", wmputilities::WMS_PROXY_ERROR,
            "Unable to store client Proxy");
   }

   GLITE_STACK_CATCH();
}

string
renewProxyRequest(const std::string& original_delegation_id)
{
   GLITE_STACK_TRY("renewProxyRequest()");
   edglog_fn("renewProxyRequest");

   // Initialise delegation_id
   string delegation_id = original_delegation_id;
   if (original_delegation_id== "" ) {
#ifndef GRST_VERSION
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "renewProxyRequest()", wmputilities::WMS_PROXY_ERROR,
            "Empty delegation id not allowed with delegation 1");
#else
      delegation_id=string(GRSTx509MakeDelegationID());
      edglog(debug)<<"Automatically generated Delegation ID";
#endif
   }
   edglog(debug)<<"Delegation ID: "<<delegation_id<<endl;
   std::string dn = wmputilities::getDN_SSL();
   // Check Proxy Path: renewProxyRequest will only work with already existing delegated proxies
   if (!wmputilities::fileExists(getDelegatedProxyPath(delegation_id, dn.c_str()))) {
      edglog(critical)<<"Unable to renew Proxy request: Previous client delegated proxy not found"<<endl;
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "renewProxyRequest()", wmputilities::WMS_PROXY_ERROR,
            "No previous client delegated proxy found");
   } else {
      edglog(debug)<<"Previous client delegated proxy found: proceed with ProxyRequest renewal"<<endl;
   }

   char* request = 0;
   if (GRSTx509MakeProxyRequest(&request, (char*) getProxyDir().c_str(),
                                (char*) delegation_id.c_str(), const_cast<char*>(dn.c_str())) != 0) {
      edglog(critical)<<"Unable to complete Proxy request"<<endl;
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "renewProxyRequest()", wmputilities::WMS_PROXY_ERROR,
            "Unable to renew Proxy request");
   }
   string proxy_req = string(request);
   free(request);
   return proxy_req;
   GLITE_STACK_CATCH();
}

pair<string, string>
getNewProxyRequest()
{
   GLITE_STACK_TRY("getNewProxyRequest()");
   edglog_fn("getNewProxyRequest");
   char* delegation_id;
#ifndef GRST_VERSION
   throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
         "getNewProxyRequest()", wmputilities::WMS_PROXY_ERROR,
         "Empty delegation id not allowed with delegation 1");
#else
   delegation_id =  GRSTx509MakeDelegationID();
   edglog(debug)<<"Generated Delegation ID: "<<delegation_id<<endl;
#endif
   std::string dn = wmputilities::getDN_SSL();
   // Check Proxy Path
   if (!wmputilities::fileExists(getDelegatedProxyPath(delegation_id, dn))) {
      edglog(debug)<<"Previous client delegated proxy not found: proceed with new Proxy Request"<<endl;
   } else {
#ifndef GRST_VERSION
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "getNewProxyRequest()", wmputilities::WMS_PROXY_ERROR,
            "Check Termination Time not available with delegation 1");
#else
      // Check Termination time
      edglog(debug)<<"Previous client delegated proxy found: check Time validity"<<endl;
      time_t *start = (time_t*) malloc(sizeof(time_t));
      time_t *finish = (time_t*) malloc(sizeof(time_t));

      if (GRSTx509ProxyGetTimes((char*) getProxyDir().c_str(),
                                delegation_id, const_cast<char*>(dn.c_str()), start, finish) != GRST_RET_OK) {
         free(start);
         free(finish);
         free(delegation_id);
         edglog(critical)<<"Unable to check already present proxy time validity: Error while retrieving Termination Time"<<endl;
         throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
               "getTerminationTime()", wmputilities::WMS_PROXY_ERROR,
               "Unable to complete New Proxy request: Error while retrieving termination time");
      } else if  ( *finish < time(NULL)) {
         // ending time is lower than current time: the delegated proxy expired
         edglog(debug)<<"Previous client delegated proxy expired: proceed with new Proxy Request"<<endl;
         // TBD CHECK TIME!!!
         // PERFORM CHECK TIME!!!
         free(start);
         free(finish);
      } else {
         free(start);
         free(finish);
         free(delegation_id);
         free(start);
         free(finish);
         edglog(critical)<<"Unable to complete New Proxy request: Previous client delegated proxy still valid"<<endl;
         throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
               "getTerminationTime()", wmputilities::WMS_PROXY_ERROR,
               "Unable to complete New Proxy request: Previous client delegated proxy still valid");
      }
#endif
   }
   // Make Actual Proxy Request
   char* request = 0;
   if (GRSTx509MakeProxyRequest(&request, (char*) getProxyDir().c_str(),
                                delegation_id, const_cast<char*>(wmputilities::getDN_SSL().c_str())) != 0) {
      edglog(critical)<<"Unable to complete New Proxy request"<<endl;
      free(delegation_id);
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "getNewProxyRequest()", wmputilities::WMS_PROXY_ERROR,
            "Unable to complete New Proxy request");
   }
   // fill the returning result
   pair<string, string> retpair;
   retpair.first = string(delegation_id);
   retpair.second = string(request);

   free(delegation_id);
   free(request);

   return retpair;

   GLITE_STACK_CATCH();
}

void
destroyProxy(const string& original_delegation_id)
{
   GLITE_STACK_TRY("destroyProxy()");
   edglog_fn("destroyProxy");

   // Initialise delegation_id
   string delegation_id = original_delegation_id;
   if (original_delegation_id == "") {
#ifndef GRST_VERSION
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "destroyProxy()", wmputilities::WMS_PROXY_ERROR,
            "Empty delegation id not allowed with delegation 1");
#else
      delegation_id=string(GRSTx509MakeDelegationID());
      edglog(debug)<<"Automatically generated Delegation ID";
#endif
   }
   edglog(debug)<<"Delegation ID: "<<delegation_id<<endl;
   edglog(debug)<<"Proxy dir: "<<getProxyDir()<<endl;
   edglog(debug)<<"User DN: "<< wmputilities::getDN_SSL() << endl;

   std::string dn = wmputilities::getDN_SSL();
   if (!wmputilities::fileExists(getDelegatedProxyPath(delegation_id, dn))) {
      edglog(critical)<<"Client delegated proxy not found: destroy Proxy not allowed"<<endl;
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "destroyProxy()", wmputilities::WMS_PROXY_ERROR,
            "Client delegated proxy not found: destroy Proxy not allowed");
   }
#ifndef GRST_VERSION
   throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
         "destroyProxy()", wmputilities::WMS_PROXY_ERROR,
         "Unable to perform destroy Proxy with delegation 1");
#else
   if (GRSTx509ProxyDestroy((char*) getProxyDir().c_str(),
                            (char*) delegation_id.c_str(), const_cast<char*>(dn.c_str())) != GRST_RET_OK) {
      edglog(critical)<<"Unable to perform destroy Proxy"<<endl;
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "destroyProxy()", wmputilities::WMS_PROXY_ERROR,
            "Unable to perform destroy Proxy");
   }
#endif
   GLITE_STACK_CATCH();
}

time_t
getTerminationTime(const string& original_delegation_id)
{
   GLITE_STACK_TRY("getTerminationTime()");
   edglog_fn("getTerminationTime");
   // Initialise delegation_id
   string delegation_id = original_delegation_id;
   if (original_delegation_id=="") {
#ifndef GRST_VERSION
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "getTerminationTime()", wmputilities::WMS_PROXY_ERROR,
            "Empty delegation id not allowed with delegation 1");
#else
      delegation_id=string(GRSTx509MakeDelegationID());
      edglog(debug)<<"Automatically generated Delegation ID";
#endif
   }
   edglog(debug)<<"delegation ID: "<<delegation_id<<endl;
   std::string dn = wmputilities::getDN_SSL();
   // Check Proxy Path
   if (!wmputilities::fileExists(getDelegatedProxyPath(delegation_id, dn))) {
      edglog(critical)<<"Client delegated proxy not found: get termination time not allowed"<<endl;
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "getTerminationTime()", wmputilities::WMS_PROXY_ERROR,
            "Client delegated proxy not found:  get termination time not allowed");
   }

   time_t *start = (time_t*) malloc(sizeof(time_t));
   time_t *finish = (time_t*) malloc(sizeof(time_t));

   edglog(debug) << "Proxy dir: " << getProxyDir() << endl;
   edglog(debug) << "User DN: "<< dn << endl;
#ifndef GRST_VERSION
   throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
         "getTerminationTime()", wmputilities::WMS_PROXY_ERROR,
         "Unable to perform get termination time with delegation 1");
#else
   if (GRSTx509ProxyGetTimes((char*)getProxyDir().c_str(),
                             (char*)delegation_id.c_str(), const_cast<char*>(dn.c_str()), start, finish) != GRST_RET_OK) {
      edglog(critical)<<"Unable to perform get termination time"<<endl;
      free(start);
      free(finish);
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "getTerminationTime()", wmputilities::WMS_PROXY_ERROR,
            "Unable to perform get termination time");
   }
#endif
   time_t time = *finish;
   free(start);
   free(finish);
   return time;
   GLITE_STACK_CATCH();
}

string getDelegationInterfaceVersion()
{
   GLITE_STACK_TRY("getDelegationInterfaceVersion()");
   edglog_fn("getDelegationInterfaceVersion");
   return DELEGATION_VERSION_NUMBER;
   GLITE_STACK_CATCH();
}

string getDelegationVersion()
{
   GLITE_STACK_TRY("getDelegationVersion()");
   edglog_fn("getDelegationVersion");
   return DELEGATION_VERSION_NUMBER;
   GLITE_STACK_CATCH();
}

}}}}
