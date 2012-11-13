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
// File: configuration.cpp
// Author: Giuseppe Avellino <egee@datamat.it>
//

#include "configuration.h"

#include <boost/regex.hpp>

// Utilities
#include "utilities/utils.h" // getServerHost()

// TRY CATCH macros
#include "utilities/exceptions.h"

#include "glite/jdl/Ad.h"


const int LB_SERVER_DEFAULT_PORT = 9000;
const int LB_LOCAL_LOGGER_DEFAULT_PORT = 9002;

const std::string DEFAULT_LISTMATCH_DIR = "/tmp";
const std::string DEFAULT_SERVER_ADDRESS = "localhost";
const std::string DEFAULT_FILE_TRANSFER_PROTOCOL = "gsiftp";

const std::string ALL_OPERATIONS = "AllOperations";


namespace utilities  = glite::wms::wmproxy::utilities;
namespace configuration = glite::wms::common::configuration;

using namespace std;


void
parseAddressPort(const string& addressport, string& server,
                 pair<string, int> &addresspair)
{
   GLITE_STACK_TRY("parseAddressPort()");

   std::string::size_type pos;
   if (addressport != "") {
      if ((pos = addressport.rfind(":", addressport.size()))
            != string::npos) {
         server = addressport.substr(0, pos);
         addresspair.first = server;
         addresspair.second =
            atoi(addressport.substr(pos + 1, addressport.size()).c_str());
      } else {
         server = addressport;
         addresspair.first = addressport;
         addresspair.second = 0;
      }
   } else {
      server = DEFAULT_SERVER_ADDRESS;
      addresspair.first = server;
      addresspair.second = 0;
   }

   GLITE_STACK_CATCH();
}

void
parseAddressPort(const string& addressport, pair<string, int> &addresspair)
{
   GLITE_STACK_TRY("parseAddressPort()");
   std::string::size_type pos;
   if (addressport != "") {
      if ((pos = addressport.rfind(":", addressport.size()))
            != string::npos) {
         addresspair.first = addressport.substr(0, pos);
         addresspair.second =
            atoi(addressport.substr(pos + 1, addressport.size()).c_str());
      } else {
         addresspair.first = addressport;
         addresspair.second = 0;
      }
   } else {
      addresspair.first = DEFAULT_SERVER_ADDRESS;
      addresspair.second = 0;
   }
   GLITE_STACK_CATCH();
}

string
resolveEnvironmentVariables(const string& envstring)
{
   GLITE_STACK_TRY("resolveEnvironmentVariables()");
   char *value = NULL;
   string result = envstring;
   string prima;
   string variabile;
   string dopo;
   boost::match_results<string::const_iterator> pieces;
   static boost::regex expression( "^(.*)\\$\\{(.+)\\}(.*)$" );
   static boost::regex other( "^\\[\\[(.*)\\]\\]$" );
   if (envstring != "") {
      while (boost::regex_match(result, pieces, expression)) {
         prima.assign(pieces[1].first, pieces[1].second);
         variabile.assign(pieces[2].first, pieces[2].second);
         dopo.assign(pieces[3].first, pieces[3].second);

         if ((value = getenv(variabile.c_str()))) {
            result = prima + string(value) + dopo;
         } else {
            result = prima + dopo;
         }
      }
   }
   return result;
   GLITE_STACK_CATCH();
}


// Constructor
WMProxyConfiguration::WMProxyConfiguration()
{
};

// Destructor
WMProxyConfiguration::~WMProxyConfiguration() throw()
{
};

// init method
void
WMProxyConfiguration::init(const std::string& opt_conf_file,
                           configuration::ModuleType::module_type module)
{
   GLITE_STACK_TRY("init()");

   this->opt_conf_file = opt_conf_file;
   this->module = module;
   if (config) {
      delete config;
   }
   config = new configuration::Configuration(opt_conf_file, module);
   wmp_config = config->wp();
   common_config = config->common();
   loadConfiguration();

   GLITE_STACK_CATCH();
}

void
WMProxyConfiguration::refreshConfiguration()
{
   GLITE_STACK_TRY("refreshConfiguration()");

   if (config) {
      delete config;
   }
   config = new configuration::Configuration(opt_conf_file, module);
   wmp_config = config->wp();
   common_config = config->common();
   loadConfiguration();

   GLITE_STACK_CATCH();
}

void
WMProxyConfiguration::loadConfiguration()
{
   GLITE_STACK_TRY("loadConfiguration()");

   this->lbproxyavailable = common_config->lbproxy();

   // [Service Discovery attributes
   this->servicediscoveryenabled = wmp_config->enable_service_discovery();
   this->lbservicediscoverytype = wmp_config->lbservice_discovery_type();
   this->servicediscoveryinfovalidity
      = wmp_config->service_discovery_info_validity();
   // ]

   this->weightscachepath = wmp_config->weights_cache_path();
   this->weightscachevalidity = wmp_config->weights_cache_validity();

   this->maxinputsandboxsize = wmp_config->max_input_sandbox_size();
   this->minperusaltimeinterval = wmp_config->min_perusal_time_interval();

   // If attribute ListMatchRootPath not present in configuration file then
   // wmp_config->list_match_root_path() returns ""
   string path = wmp_config->list_match_root_path();
   this->listmatchrootpath = (path != "") ? path : DEFAULT_LISTMATCH_DIR;

   this->sandboxstagingpath = wmp_config->sandbox_staging_path();

   // LB server addresses
   vector<string> lbaddresses = wmp_config->lbserver();
   pair<string, int> item;
   if (lbaddresses.size()) {
      for (unsigned int i = 0; i < lbaddresses.size(); i++) {
         parseAddressPort(lbaddresses[i], item);
         if (item.first != DEFAULT_SERVER_ADDRESS) {
            if (item.second == 0) {
               item.second = LB_SERVER_DEFAULT_PORT;
            }
            this->lbservers.push_back(item);
         }
      }
      // Checking for empty lbservers
      if (!this->lbservers.size()) {
         // Inserting default element
         item.first = DEFAULT_SERVER_ADDRESS;
         item.second = LB_SERVER_DEFAULT_PORT;
         this->lbservers.push_back(item);
      }
   } else {
      // Inserting default element
      item.first = DEFAULT_SERVER_ADDRESS;
      item.second = LB_SERVER_DEFAULT_PORT;
      this->lbservers.push_back(item);
   }

   this->lbserver = this->lbservers[0].first;
   this->lbserverpair.first = this->lbserver;
   this->lbserverpair.second = this->lbservers[0].second;

   // If attribute GridFTPPort not present in configuration file then
   // wmp_config->grid_ftpport() returns 0
   //pair<string, int> gsiprotocol(string("gsiftp"), wmp_config->grid_ftpport());
   string confdefprotocol = wmp_config->default_protocol();
   confdefprotocol = (confdefprotocol != "") ? confdefprotocol
                     : DEFAULT_FILE_TRANSFER_PROTOCOL;
   pair<string, int> gsiprotocol(confdefprotocol, wmp_config->grid_ftpport());
   this->defaultProtocol = gsiprotocol;
   this->protocols.push_back(gsiprotocol);

   this->httpsport = wmp_config->httpsport();
   /*pair<string, int> httpsprotocol("https", this->httpsport);
   protocols.push_back(httpsprotocol);*/

   this->asyncjobstart = wmp_config->async_job_start();

   this->operationloadscripts = wmp_config->operation_load_scripts();

   this->maxservedrequests = wmp_config->max_served_requests();

   this->listmatchtimeout = wmp_config->list_match_timeout();

   this->maxperusalfiles = wmp_config->max_perusal_files();

   this->maxinputsandboxfiles = wmp_config->max_input_sandbox_files();

   this->maxoutputsandboxfiles = wmp_config->max_output_sandbox_files();

   GLITE_STACK_CATCH();
}

int
WMProxyConfiguration::getHTTPSPort()
{
   return this->httpsport;
}

std::string
WMProxyConfiguration::getDefaultProtocol()
{
   return this->defaultProtocol.first;
}

int
WMProxyConfiguration::getDefaultPort()
{
   return this->defaultProtocol.second;
}

std::vector<std::pair<std::string, int> >
WMProxyConfiguration::getProtocols()
{
   return this->protocols;
}

bool
WMProxyConfiguration::isLBProxyAvailable()
{
   return this->lbproxyavailable;
};

string
WMProxyConfiguration::getWeightsCachePath()
{
   return this->weightscachepath;
};

long
WMProxyConfiguration::getWeightsCacheValidity()
{
   return this->weightscachevalidity;
};

bool
WMProxyConfiguration::isServiceDiscoveryEnabled()
{
   return this->servicediscoveryenabled;
};

long
WMProxyConfiguration::getServiceDiscoveryInfoValidity()
{
   return this->servicediscoveryinfovalidity;
};

string
WMProxyConfiguration::getLBServiceDiscoveryType()
{
   return this->lbservicediscoverytype;
};

double
WMProxyConfiguration::getMaxInputSandboxSize()
{
   return this->maxinputsandboxsize;
};

int
WMProxyConfiguration::getMinPerusalTimeInterval()
{
   return this->minperusaltimeinterval;
};

// Used only to check if the value is the same as $DOCUMENT_ROOT
string
WMProxyConfiguration::getSandboxStagingPath()
{
   return this->sandboxstagingpath;
};

string
WMProxyConfiguration::getListMatchRootPath()
{
   return this->listmatchrootpath;
}

vector<pair<string, int> >
WMProxyConfiguration::getLBServerEndpoints()
{
   return this->lbservers;
}

pair<string, int>
WMProxyConfiguration::getLBServerAddressPort()
{
   if (this->lbserverpair.first == DEFAULT_SERVER_ADDRESS) {
      this->lbserverpair.first = utilities::getServerHost();
   }
   return this->lbserverpair;
}

bool
WMProxyConfiguration::getAsyncJobStart()
{
   return this->asyncjobstart;
}

classad::ExprTree *
WMProxyConfiguration::getSDJRequirements()
{
   return this->sdjrequirements;
}

string
WMProxyConfiguration::getOperationLoadScriptPath(const string& operation)
{
   string returnvalue = "";
   if (this->operationloadscripts) {
      glite::jdl::Ad ad(*this->operationloadscripts);
      if (ad.hasAttribute(operation)) {
         returnvalue = resolveEnvironmentVariables(ad.getString(operation));
      } else if (ad.hasAttribute(ALL_OPERATIONS)) {
         returnvalue = resolveEnvironmentVariables(ad.getString(ALL_OPERATIONS));
      }
   }
   return returnvalue;
}

long
WMProxyConfiguration::getMaxServedRequests()
{
   return this->maxservedrequests;
}

long
WMProxyConfiguration::getListMatchTimeout()
{
   return this->listmatchtimeout;
}

long
WMProxyConfiguration::getMaxPerusalFiles()
{
   return this->maxperusalfiles;
}

long
WMProxyConfiguration::getMaxInputSandboxFiles()
{
   return this->maxinputsandboxfiles;
}

long
WMProxyConfiguration::getMaxOutputSandboxFiles()
{
   return this->maxoutputsandboxfiles;
}


