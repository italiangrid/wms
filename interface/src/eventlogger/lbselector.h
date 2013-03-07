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
// File: lbselector.h
// Author: Giuseppe Avellino <egee@datamat.it>
//

#ifndef GLITE_WMS_WMPROXY_UTILITIES_WMPLBSELECTOR_H
#define GLITE_WMS_WMPROXY_UTILITIES_WMPLBSELECTOR_H

#include <string>
#include <vector>

namespace glite {

namespace jdl
{
class Ad;
}

namespace wms {
namespace wmproxy {
namespace eventlogger {

class WMPLBSelector
{
public:
   enum lbcallresult {
      SUCCESS,
      FAILURE
   };

   /**
    * Constructor
    */
   WMPLBSelector();

   /**
    * Constructor
    */
   WMPLBSelector(std::vector<std::pair<std::string, int> > lbservers,
                 std::string weightscachepath, long weightscachevaliditytime,
                 bool enableservicediscovery, long servicediscoveryinfovalidity,
                 const std::string& lbsdtype);

   /**
    * Destructor
    */
   ~WMPLBSelector() throw();

   std::pair<std::string, int> selectLBServer();
   void updateSelectedIndexWeight(lbcallresult result);

private:

   // Service Discovery service key
   std::string lbsdtype;
   // Current selected LB server
   std::string selectedlb;
   std::vector<std::string> conflbservers;

   std::string weightsfile;
   std::string weightscachepath;
   long weightscachevaliditytime;
   int weightupperlimit;

   bool enableservicediscovery;
   long servicediscoveryinfovalidity;


   void newLBServerAd(glite::jdl::Ad& lbserverad);
   void updateLBServerAd(glite::jdl::Ad& lbserveradref,
                         glite::jdl::Ad& lbserveradref);

   std::vector<std::string> callServiceDiscovery();

   void setWeightsFilePath();
   void updateWeight(glite::jdl::Ad& lbserverad, lbcallresult result);
   std::string toLBServerName(const std::string& inputstring);
   std::string toWeightsFileAttributeName(const std::string& inputstring);

   int generateRandomNumber(int lowerlimit, int upperlimit);

};

}}}}

#endif // GLITE_WMS_WMPROXY_UTILITIES_WMPLBSELECTOR_H
