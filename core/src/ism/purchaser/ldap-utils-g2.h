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

// Author: Salvatore Monforte
#ifndef GLITE_WMS_II_PURCHASER_LDAP_UTILS_G2_H
#define GLITE_WMS_II_PURCHASER_LDAP_UTILS_G2_H

#include <string>
#include "glite/wms/ism/purchaser/common.h"

namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

void
fetch_bdii_ce_info_g2(
  std::string const& hostname,
  size_t port,
  std::string const& basedn,
  time_t timeout,
  std::string const& ldap_ce_filter_ext,
  gluece_info_container_type& gluece_info_container
);

void
fetch_bdii_se_info_g2(
  std::string const& hostname,
  size_t port,
  std::string const& basedn,
  time_t timeout,
  std::string const& ldap_se_filter_ext,
  gluece_info_container_type& gluese_info_container
);

void 
fetch_bdii_info_g2(
  std::string const& hostname,
  size_t port,
  std::string const& basedn,
  time_t timeout,
  std::string const& ldap_ce_filter_ext,
  std::string const& ldap_se_filter_ext,
  gluece_info_container_type& gluece_info_container,
  gluece_info_container_type& gluese_info_container
);

}}}}

#endif
