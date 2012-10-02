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

// File: ldap-dn-utils.cpp
// Author: Salvatore Monforte
// Copyright (c) 2002 EU DataGrid.

// $Id: ldap-dn-utils.cpp,v 1.1.8.1.2.2.4.1 2012/06/22 11:51:31 mcecchi Exp $

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>

using namespace std;

namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

bool is_gluecluster_info_dn(std::vector<std::string> const& dn)
{
 return dn.size() > 1 && 
   boost::algorithm::istarts_with(dn[0], "GlueClusterUniqueID") &&
   boost::algorithm::istarts_with(dn[1],"mds-vo-name");
}

bool is_gluece_info_dn(std::vector<std::string> const& dn)
{
 return dn.size() > 1 && 
   boost::algorithm::istarts_with(dn[0],"GlueCEUniqueID") &&
   boost::algorithm::istarts_with(dn[1],"mds-vo-name"); 
}

bool is_gluesubcluster_info_dn(std::vector<std::string> const& dn)
{
 return dn.size() > 2 && 
   boost::algorithm::istarts_with(dn[0],"GlueSubClusterUniqueID") && 
   boost::algorithm::istarts_with(dn[1],"GlueClusterUniqueID") &&
   boost::algorithm::istarts_with(dn[2],"mds-vo-name"); 
}

bool is_gluecesebind_info_dn(std::vector<std::string> const& dn)
{
 return dn.size() > 2 && 
   boost::algorithm::istarts_with(dn[0],"GlueCESEBindSEUniqueID") && 
   boost::algorithm::istarts_with(dn[1],"GlueCESEBindGroupCEUniqueID") && 
   boost::algorithm::istarts_with(dn[2],"mds-vo-name");
}

bool is_gluevoview_info_dn(std::vector<std::string> const& dn)
{
  return dn.size() > 2 &&
    boost::algorithm::istarts_with(dn[0],"GlueVOViewLocalID") &&
    boost::algorithm::istarts_with(dn[1],"GlueCEUniqueID") &&
    boost::algorithm::istarts_with(dn[2],"mds-vo-name");
}

bool is_gluese_info_dn(std::vector<std::string> const& dn)
{
  return dn.size() > 1 &&
    boost::algorithm::istarts_with(dn[0],"GlueSEUniqueID") &&
    boost::algorithm::istarts_with(dn[1],"mds-vo-name");
}

bool is_gluesa_info_dn(std::vector<std::string> const& dn)
{
  return dn.size() > 2 &&
    boost::algorithm::istarts_with(dn[0],"GlueSALocalID") &&
    boost::algorithm::istarts_with(dn[1],"GlueSEUniqueID") &&
    boost::algorithm::istarts_with(dn[2],"mds-vo-name");
}

bool is_gluese_access_protocol_info_dn(std::vector<std::string> const& dn)
{
  return dn.size() > 2 &&
    boost::algorithm::istarts_with(dn[0],"GlueSEAccessProtocolLocalID") &&
    boost::algorithm::istarts_with(dn[1],"GlueSEUniqueID") &&
    boost::algorithm::istarts_with(dn[2],"mds-vo-name");
}

bool is_gluese_control_protocol_info_dn(std::vector<std::string> const& dn)
{
  return dn.size() > 2 &&
    boost::algorithm::istarts_with(dn[0],"GlueSEControlProtocolLocalID") &&
    boost::algorithm::istarts_with(dn[1],"GlueSEUniqueID") &&
    boost::algorithm::istarts_with(dn[2],"mds-vo-name");
}

bool is_gluelocation_info_dn(std::vector<std::string> const& dn)
{
 return dn.size() > 3 &&
   boost::algorithm::istarts_with(dn[0], "GlueLocationLocalID") &&
   boost::algorithm::istarts_with(dn[1], "GlueSubClusterUniqueID") &&
   boost::algorithm::istarts_with(dn[2], "GlueClusterUniqueID");
}

bool is_glue2_service_dn(std::vector<std::string> const& dn)
{
 return dn.size() > 1 && 
   boost::algorithm::istarts_with(dn[0], "GLUE2ServiceID") &&
   boost::algorithm::istarts_with(dn[1],"GLUE2GroupID");
}

bool is_glue2_manager_dn(std::vector<std::string> const& dn)
{
 return dn.size() > 2 && 
   boost::algorithm::istarts_with(dn[0],"GLUE2ManagerID") &&
   boost::algorithm::istarts_with(dn[1],"GLUE2ServiceID") &&
   boost::algorithm::istarts_with(dn[2],"GLUE2GroupID");; 
}

bool is_glue2_share_dn(std::vector<std::string> const& dn)
{
 return dn.size() > 2 && 
   boost::algorithm::istarts_with(dn[0],"GLUE2ShareID") && 
   boost::algorithm::istarts_with(dn[1],"GLUE2ServiceID") &&
   boost::algorithm::istarts_with(dn[2],"GLUE2GroupID"); 
}

bool is_glue2_endpoint_dn(std::vector<std::string> const& dn)
{
 return dn.size() > 2 && 
   boost::algorithm::istarts_with(dn[0],"GLUE2EndpointID") && 
   boost::algorithm::istarts_with(dn[1],"GLUE2ServiceID") && 
   boost::algorithm::istarts_with(dn[2],"GLUE2GroupID");
}

bool is_glue2_to_storage_service_dn(std::vector<std::string> const& dn)
{
  return dn.size() > 2 &&
    boost::algorithm::istarts_with(dn[0],"GLUE2ToStorageServiceID") &&
    boost::algorithm::istarts_with(dn[1],"GLUE2ServiceID") &&
    boost::algorithm::istarts_with(dn[2],"GLUE2GroupID");
}

bool is_glue2_mapping_policy_dn(std::vector<std::string> const& dn)
{
  return dn.size() > 3 &&
    boost::algorithm::istarts_with(dn[0],"GLUE2PolicyID") &&
    boost::algorithm::istarts_with(dn[1],"GLUE2ShareID") &&
    boost::algorithm::istarts_with(dn[2],"GLUE2ServiceID") &&
    boost::algorithm::istarts_with(dn[3],"GLUE2GroupID");
}
bool is_glue2_access_policy_dn(std::vector<std::string> const& dn)
{
  return dn.size() > 3 &&
    boost::algorithm::istarts_with(dn[0],"GLUE2PolicyID") &&
    boost::algorithm::istarts_with(dn[1],"GLUE2EndpointID") &&
    boost::algorithm::istarts_with(dn[2],"GLUE2ServiceID") &&
    boost::algorithm::istarts_with(dn[3],"GLUE2GroupID");
}

bool is_glue2_resource_dn(std::vector<std::string> const& dn)
{
  return dn.size() > 3 &&
    boost::algorithm::istarts_with(dn[0],"GLUE2ResourceID") &&
    boost::algorithm::istarts_with(dn[1],"GLUE2ManagerID") &&
    boost::algorithm::istarts_with(dn[2],"GLUE2ServiceID") &&
    boost::algorithm::istarts_with(dn[3],"GLUE2GroupID");
}

bool is_glue2_benchmark_dn(std::vector<std::string> const& dn)
{
  return dn.size() > 3 &&
    boost::algorithm::istarts_with(dn[0],"GLUE2BenchmarkID") &&
    boost::algorithm::istarts_with(dn[1],"GLUE2ResourceID") &&
    boost::algorithm::istarts_with(dn[2],"GLUE2ServiceID") &&
    boost::algorithm::istarts_with(dn[3],"GLUE2GroupID");
}

bool is_glue2_application_env_dn(std::vector<std::string> const& dn)
{
  return dn.size() > 3 &&
    boost::algorithm::istarts_with(dn[0],"GLUE2ApplicationEnvironmentID") &&
    boost::algorithm::istarts_with(dn[1],"GLUE2ResourceID") &&
    boost::algorithm::istarts_with(dn[2],"GLUE2ServiceID") &&
    boost::algorithm::istarts_with(dn[3],"GLUE2GroupID");
}

bool is_glue2_service_capacity_dn(std::vector<std::string> const& dn)
{
  return dn.size() > 2 &&
    boost::algorithm::istarts_with(dn[0],"GLUE2StorageServiceCapacity") &&
    boost::algorithm::istarts_with(dn[1],"GLUE2ServiceID") &&
    boost::algorithm::istarts_with(dn[2],"GLUE2GroupID");
}

bool is_glue2_share_capacity_dn(std::vector<std::string> const& dn)
{
  return dn.size() > 3 &&
    boost::algorithm::istarts_with(dn[0],"GLUE2StorageShareCapacity") &&
    boost::algorithm::istarts_with(dn[1],"GLUE2ShareID") &&
    boost::algorithm::istarts_with(dn[2],"GLUE2ServiceID") &&
    boost::algorithm::istarts_with(dn[3],"GLUE2GroupID");
}

bool is_glue2_storage_access_protocol_dn(std::vector<std::string> const& dn)
{
  return dn.size() > 3 &&
    boost::algorithm::istarts_with(dn[0],"GLUE2StorageAccessProtocolID") &&
    boost::algorithm::istarts_with(dn[1],"GLUE2ServiceID") &&
    boost::algorithm::istarts_with(dn[2],"GLUE2GroupID") &&
    boost::algorithm::istarts_with(dn[3],"GLUE2DomainID");
}

} // namespace purchaser
} // namespace ism
} // namespace wms
} // namespace glite
