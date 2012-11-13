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
limitations under the License. */

#ifndef GLITE_WMS_WMPROXY_WMPEXPDAGAD_H
#define GLITE_WMS_WMPROXY_WMPEXPDAGAD_H

#include "glite/jdl/ExpDagAd.h"

namespace glite {
namespace wms {
namespace wmproxy {
namespace eventlogger {

class WMPExpDagAd:public glite::jdl::ExpDagAd
{
public:
   WMPExpDagAd(const std::string& jdl):ExpDagAd(jdl) {};
   WMPExpDagAd(std::ifstream& jdl_in):ExpDagAd(jdl_in) {};
   WMPExpDagAd(const ExpDagAd& dag):ExpDagAd(dag) {};
   WMPExpDagAd(glite::jdl::Ad *ad):ExpDagAd(ad) {};
   WMPExpDagAd(const classad::ClassAd& classAd):ExpDagAd(classAd) {};
   void setReserved(const std::string attr_name, const std::string attr_value);
   std::size_t getDependenciesNumber();

};

}}}}

#endif // GLITE_WMS_WMPROXY_WMPEXPDAGAD_H
