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

#include "expdagad.h"

namespace glite {
namespace wms {
namespace wmproxy {
namespace eventlogger {

void WMPExpDagAd::setReserved(const std::string attr_name,
                              const std::string attr_value)
{
   classad::Value v;
   v.SetStringValue(attr_value);
   dagad->set_generic(attr_name, classad::Literal::MakeLiteral(v));
}

std::size_t WMPExpDagAd::getDependenciesNumber()
{
   return dagad->num_dependencies();
}

}}}}
