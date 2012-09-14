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

#ifndef _GLITE_WMS_WMPROXY_COMMANDS_LISTFILES_H_
#define _GLITE_WMS_WMPROXY_COMMANDS_LISTFILES_H_

#include "boost/filesystem/path.hpp"
#include <string>
#include <vector>

namespace glite {
namespace wms {
namespace wmproxy {
namespace commands {

extern "C++"
{
   void list_files( const boost::filesystem::path& p, std::vector<std::string>& v);
}

}}}}

#endif
