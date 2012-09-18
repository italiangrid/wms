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
// File: configuration.h
// Author: Giuseppe Avellino <egee@datamat.it>
//

#ifndef GLITE_WMS_WMPROXY_WMPCONFIGURATION_H
#define GLITE_WMS_WMPROXY_WMPCONFIGURATION_H

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMPConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/ModuleType.h"

/**
 * WMProxyConfiguration class provides a set of methods to parse configuration
 * file and to get the value of the configuration attributes in this file
 *
 * @version 1.0
 * @date 2005
 * @author Giuseppe Avellino <egee@datamat.it>
*/
class WMProxyConfiguration
{
public:
   /**
    * Constructor
    */
   WMProxyConfiguration();

   /**
    * Destructor
    */
   virtual ~WMProxyConfiguration() throw();

   /**
    * Initializes the configuration instance reading the configuration
    * attributes from the specified configuration file
    * @param opt_conf_file configuration file
    * @param module the module type representing the section to get the
    *       attributes values from
    */
   void init(const std::string& opt_conf_file,
             glite::wms::common::configuration::ModuleType::module_type module);

   /**
    * Refreshes configuration that is reads again the configuration attributes
    * from the configuration file
    */
   void refreshConfiguration();

   /**
    * Returns the value of the configuration attribute ListMatchRootPath
    * @return the attribute value
    */
   std::string getListMatchRootPath();

   /**
    * Returns the value of the configuration attribute MaxInputSandboxSize
    * @return the attribute value
    */
   double getMaxInputSandboxSize();

   /**
    * Returns the value of the configuration attribute MinPerusalTimeInterval
    * @return the attribute value
    */
   int getMinPerusalTimeInterval();

   /**
    * Returns the value of the configuration attribute SandboxStagingPath
    * @return the attribute value
    */
   std::string getSandboxStagingPath();

   /**
    * Returns the default protocol for file transfer
    * @return the default protocol
    */
   std::string getDefaultProtocol();

   /**
    * Returns the value of the configuration attribute HTTPSPort
    * @return the attribute value
    */
   int getHTTPSPort();

   /**
    * Returns the value of the configuration attribute GridFTPPort
    * @return the attribute value
    */
   int getDefaultPort();

   /**
    * Returns the available file transfer protocols
    * @return the available file transfer protocols
    */
   std::vector<std::pair<std::string, int> > getProtocols();

   /**
    * Checks if the LB Proxy is available in the server host
    * @return returns true if the LBServer attribute is present and set to
    *       true (the LB Proxy server is available), false otherwise.
    */
   bool isLBProxyAvailable();

   /**
    * Returns the value of the configuration attribute
    * WeightsCachePath
    * @return the attribute value
    */
   std::string getWeightsCachePath();

   /**
    * Returns the value of the configuration attribute
    * WeightsCacheValidity
    * @return the attribute value
    */
   long getWeightsCacheValidity();

   /**
    * Checks if the Service Discovery is enabled in the server host
    * @return returns true if the EnabledServiceDiscovery attribute is
    *       present and set to true, false otherwise.
    */
   bool isServiceDiscoveryEnabled();

   /**
    * Returns the value of the configuration attribute
    * ServiceDiscoveryInfoValidity
    * @return the attribute value
    */
   long getServiceDiscoveryInfoValidity();

   /**
    * Returns the value of the configuration attribute LBServiceDiscoveryType
    * @return the attribute value
    */
   std::string getLBServiceDiscoveryType();

   /**
    * Returns the value of the configuration attribute LBAddresses
    * @return the attribute value
    */
   std::vector<std::pair<std::string, int> > getLBServerEndpoints();

   /**
    * Returns the value of the configuration attribute LBServer
    * @return the attribute value
    */
   std::pair<std::string, int> getLBServerAddressPort();

   /**
    * Returns the value of the configuration attribute AsyncJobStart
    * @return the attribute value
    */
   bool getAsyncJobStart();

   /**
    * Returns the value of the configuration attribute SDJRequirements
    * @return the attribute value
    */
   classad::ExprTree * getSDJRequirements();

   /**
    * Returns the value of the attribute operation inside classad
    * configuration attribute OperationLoadScripts
    * @return the attribute value
    */
   std::string getOperationLoadScriptPath(const std::string& operation);

   /**
    * Returns the value of the configuration attribute MaxServedRequests
    * @return the attribute value
    */
   long getMaxServedRequests();

   /**
    * Returns the value of the configuration attribute listMatchTimeout
    * @return the attribute value
    */
   long getListMatchTimeout();

   /**
              * Returns the value of the configuration attribute MaxPerusalFiles
              * @return the attribute value
              */
   long getMaxPerusalFiles();
   /**
    * Returns the value of the configuration attribute MaxInputSandboxFiles
    * @return the attribute value
    */
   long getMaxInputSandboxFiles();
   /**
    * Returns the value of the configuration attribute MaxOutputSandboxFiles
    * @return the attribute value
    */
   long getMaxOutputSandboxFiles();

   glite::wms::common::configuration::WMPConfiguration const* wmp_config;
   glite::wms::common::configuration::CommonConfiguration const* common_config;
private:
   void loadConfiguration();

   std::string opt_conf_file;
   glite::wms::common::configuration::Configuration * config;
   glite::wms::common::configuration::ModuleType::module_type module;

   std::string listmatchrootpath;

   double maxinputsandboxsize;
   std::string sandboxstagingpath;

   int minperusaltimeinterval;
   long listmatchtimeout;
   long maxperusalfiles;
   long maxinputsandboxfiles;
   long maxoutputsandboxfiles;
   long maxservedrequests;
   bool lbproxyavailable;
   std::string weightscachepath;
   long weightscachevalidity;
   bool servicediscoveryenabled;
   long servicediscoveryinfovalidity;
   std::string lbservicediscoverytype;
   std::string lbserver;
   std::vector<std::pair<std::string, int> > lbservers;
   std::pair<std::string, int> lbserverpair;
   std::string lblocallogger;
   std::pair<std::string, int> lblocalloggerpair;

   int httpsport;
   std::pair<std::string, int> defaultProtocol;
   std::vector<std::pair<std::string, int> > protocols;

   bool asyncjobstart;

   classad::ExprTree * sdjrequirements;
   classad::ClassAd * operationloadscripts;
};

#endif // GLITE_WMS_WMPROXY_WMPCONFIGURATION_H
