/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
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

/*
 * File: classad-plugin-loader.cpp
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 */

// $Id$

#include <boost/utility.hpp>

namespace glite {
namespace wms {
namespace classad_plugin {

struct classad_plugin_loader : boost::noncopyable
{
  classad_plugin_loader();
};

// FIXME: the following global instantiation is commented out because,
// FIXME: if this creator is invoked -before- the creator for
// FIXME: the global classad::FunctionCall::functionTable, it will
// FIXME: cause a SEGV while trying to access the non-initialized 
// FIXME: functionTable. For the time being this was moved to the NS main
// FIXME: and to the broker Helper.cpp. This can come back when the
// FIXME: matchmaker is finally done only via the broker_helper DL.
// namespace { classad_plugin_loader init; }

} // namespace classad_plugin
} // namespace wms
} // namespace glite
