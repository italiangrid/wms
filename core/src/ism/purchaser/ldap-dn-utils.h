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

// File: ldap-dn-utils.h
// Author: Salvatore Monforte
// Copyright (c) 2004 EU DataGrid.

// $Id: ldap-dn-utils.h,v 1.1.8.1.2.2 2010/04/08 13:54:43 mcecchi Exp $

#ifndef GLITE_WMS_ISM_PURCHASER_LDAP_DN_UTILS_H
#define GLITE_WMS_ISM_PURCHASER_LDAP_FN_UTILS_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include "glite/wms/ism/purchaser/common.h"

namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

void tokenize_ldap_dn(std::string const&, std::vector<std::string>&);
bool is_gluecluster_info_dn(std::vector<std::string> const&);
bool is_gluece_info_dn(std::vector<std::string> const&);
bool is_gluesubcluster_info_dn(std::vector<std::string> const&);
bool is_gluecesebind_info_dn(std::vector<std::string> const&);
bool is_gluelocation_info_dn(std::vector<std::string> const&);
bool is_gluevoview_info_dn(std::vector<std::string> const&);
bool is_gluese_info_dn(std::vector<std::string> const&);
bool is_gluesa_info_dn(std::vector<std::string> const&);
bool is_gluese_access_protocol_info_dn(std::vector<std::string> const&);
bool is_gluese_control_protocol_info_dn(std::vector<std::string> const&);

} // namespace purchaser
} // namespace ism
} // namespace wms
} // namespace glite

#endif
