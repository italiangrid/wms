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
limitations under the License.  */

// Authors:
//      Marco Cecchi
//      Giuseppe Avellino

#ifndef GLITE_WMS_WMPROXY_WMPAUTHORIZER_H
#define GLITE_WMS_WMPROXY_WMPAUTHORIZER_H

#include <string>
#include <vector>
#include <sys/types.h>

#include "vomsauthn.h"

namespace glite {
namespace wms {
namespace wmproxy {
namespace security {

struct auth_info {
   auth_info(uid_t const& uid, gid_t const& gid, std::vector<std::string> const& fqans)
      : uid_(uid), gid_(gid), fqans_(fqans) { }
   uid_t uid_;
   gid_t gid_;
   std::vector<std::string> fqans_;
};

std::string do_authZ(std::string const& action);
auth_info do_authZ_jobid(std::string const& action, std::string const& job_id);
auth_info do_authZ(std::string const& action, std::string const& delegation_id);
void checkProxyValidity(const std::string& proxypath);
void checkProxyExistence(const std::string& proxypath, const std::string& jobid);
std::vector<std::pair<std::string, std::string> > parseFQAN(const std::string& fqan);
bool checkJobDrain();
void setGridsiteJobGacl(std::vector<std::string> &jobids);

class WMPAuthorizer
{
public:
   WMPAuthorizer(std::string const& operation);
   WMPAuthorizer(std::string const& operation, std::string const& proxycert);
   ~WMPAuthorizer() { }

   std::vector<std::string> getFQANs();
   std::string getUserName();
   uid_t getUserId();
   uid_t getUserGroup();
   void authorize();
private:
   void map_user_lcmaps();
   std::string username_;
   uid_t uid_;
   uid_t gid_;
   std::string userdn_;
   std::vector<std::string> fqans_;
   std::string action_;
   std::string userproxypath_;
};

}}}}

#endif // GLITE_WMS_WMPROXY_WMPAUTHORIZER_H
