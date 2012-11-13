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

// Authors:
// Marco Cecchi
// Giuseppe Avellino

#include <iostream>
#include <vector>
#include <string>
#include <pwd.h> // to build on IA64
#include <dlfcn.h>
#include <sys/types.h>

#include <openssl/pem.h>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMPConfiguration.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/security/proxyrenewal/renewal.h"

#include "utilities/logging.h"
#include "utilities/wmpexceptions.h"
#include "utilities/wmpexception_codes.h"
#include "gaclmanager.h"
#include "delegation.h"
#include "utilities/utils.h"
#include "authorizer.h"
#include "argus_authz.h"

extern "C" {
#include "lcmaps/lcmaps.h"
#include "lcmaps/lcmaps_return_poolindex_without_gsi.h"
}

namespace wmputilities = glite::wms::wmproxy::utilities;
namespace logger = glite::wms::common::logger;
namespace configuration = glite::wms::common::configuration;

using namespace std;
using namespace glite::wmsutils::exception;

namespace glite {
namespace wms {
namespace wmproxy {
namespace security {

namespace {

std::string const DOCUMENT_ROOT("DOCUMENT_ROOT");
std::string const GACL_FILE("glite_wms_wmproxy.gacl");
std::string const DRAIN_FILE(".drain");

long
getNotBefore(const string& pxfile)
{
   GLITE_STACK_TRY("getNotBefore()");
   edglog_fn("security::getNotBefore");

   long sec = 0;
   X509 *x = NULL;
   BIO *in = NULL;
   in = BIO_new(BIO_s_file());
   if (in) {
      BIO_set_close(in, BIO_CLOSE);
      if (BIO_read_filename(in, pxfile.c_str()) > 0) {
         x = PEM_read_bio_X509(in, NULL, 0, NULL);
         if (!x) {
            BIO_free(in);
            edglog(severe)<<"Error in PEM_read_bio_X509: Proxy file "
                          "doesn't exist or has bad permissions"<<endl;
            throw wmputilities::AuthorizationException(__FILE__, __LINE__,
                  "security::getProxyTimeLeft", wmputilities::WMS_AUTHORIZATION_ERROR,
                  "proxy file doesn't exist or has bad permissions");
         }
         sec = ASN1_UTCTIME_get(X509_get_notBefore(x));
         free(x);
      } else {
         BIO_free(in);
         edglog(error)<<"Unable to get Not Before date from proxy"<<endl;
         throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
               "getNotBefore()", wmputilities::WMS_PROXY_ERROR,
               "Unable to get Not Before date from proxy");
      }
      BIO_free(in);
   } else {
      edglog(error)<<"Unable to get Not Before date from Proxy (BIO SSL error)"
                   <<endl;
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "getNotBefore()", wmputilities::WMS_PROXY_ERROR,
            "Unable to get Not Before date from Proxy (BIO SSL error)");
   }
   return sec;

   GLITE_STACK_CATCH();
}

auth_info
authorize_and_map(std::string const& action, std::string const& delegatedproxy)
{
   edglog_fn("WMPAuthorizer::authorize_and_map()");
   checkProxyValidity(delegatedproxy);
   WMPAuthorizer auth(action, delegatedproxy);
   auth.authorize(); // throws
   return auth_info(auth.getUserId(), auth.getUserGroup(), auth.getFQANs());
}

void
checkGaclUserAuthZ(string const& fqan, string const& dn)
try
{
   edglog_fn("WMPAuthorizer::checkGaclUserAuthZ");

   bool exec = false;
   bool execDN = false;
   bool execAU = false;
   bool exist = false;
   bool existDN = false;
   bool existAU = false;

   // Gacl-Mgr
   string gaclfile;
   if (getenv("GLITE_WMS_CONFIG_DIR")) {
      gaclfile = string(getenv("GLITE_WMS_CONFIG_DIR"))
                 + '/' + GACL_FILE;
   } else if (getenv("GLITE_LOCATION")) {
      gaclfile = string(getenv("GLITE_LOCATION")) + "/etc/"
                 + GACL_FILE;
   } else if (getenv("WMS_LOCATION_ETC")) {
      gaclfile = string(getenv("WMS_LOCATION_ETC")) + '/'
                 + GACL_FILE;
   } else if (getenv("WMS_LOCATION")) {
      gaclfile = string(getenv("WMS_LOCATION")) + "/etc/"
                 + GACL_FILE;
   } else {
      gaclfile = "/etc/glite-wms/" + GACL_FILE;
   }
   GaclManager gacl(gaclfile);

   edglog(debug)<<"Checking gacl file entries..."<<endl;

   // checking credential types present in gacl file
   exist = gacl.checkCredentialEntries(GaclManager::WMPGACL_VOMS_CRED);
   existDN = gacl.checkCredentialEntries(GaclManager::WMPGACL_PERSON_CRED);
   existAU = gacl.checkCredentialEntries(GaclManager::WMPGACL_ANYUSER_CRED);

   if (exist) {
      edglog(debug)<<"VOMS credential type present"<<endl;
   }
   if (existDN) {
      edglog(debug)<<"person credential type present"<<endl;
   }
   if (existAU) {
      edglog(debug)<<"any-user credential type present"<<endl;
   }

   // checks exec permission
   if (!fqan.empty()) { // user proxy has FQAN
      // ANY USER authorization
      if (existAU) {
         execAU = gacl.checkAllowPermission(
                     GaclManager::WMPGACL_ANYUSER_TYPE,
                     "",GaclManager::WMPGACL_EXEC);
      }

      // FQAN authorization
      if (exist) {
         exec =  gacl.checkAllowPermission(
                    GaclManager::WMPGACL_VOMS_TYPE,
                    fqan,
                    GaclManager::WMPGACL_EXEC);
         // overrides any-user authorization if VO auth is fine
         if (exec) {
            execAU = true;
         }
      } else if (existAU || existDN) {
         exec = true;
      } else {
         exec = false;
      }

      // DN authorization
      if (existDN) {
         execDN = gacl.checkAllowPermission(
                     GaclManager::WMPGACL_PERSON_TYPE,
                     dn,GaclManager::WMPGACL_EXEC);
         // overrides VO and any-user authorization if DN is fine
         if (execDN) {
            exec = true;
            execAU = true;
         }
      } else if (existDN) {
         execDN = gacl.checkAllowPermission(
                     GaclManager::WMPGACL_PERSON_TYPE,
                     dn,
                     GaclManager::WMPGACL_EXEC);
         // overrides VO and any-user authorization if DN is fine
         if (execDN) {
            exec = true;
            execAU = true;
         }
      } else if (execAU || exec) {
         execDN = true;
      } else {
         execDN = false;
      }
   } else {
      // user proxy does not have FQAN
      // any-user authorization
      if (existAU) {
         execAU = gacl.checkAllowPermission(
                     GaclManager::WMPGACL_ANYUSER_TYPE,
                     "",
                     GaclManager::WMPGACL_EXEC);
      }
      // DN authorization
      if (existDN) {
         execDN = gacl.checkAllowPermission(
                     GaclManager::WMPGACL_PERSON_TYPE,
                     dn,
                     GaclManager::WMPGACL_EXEC )
                  ||
                  gacl.checkAllowPermission(
                     GaclManager::WMPGACL_PERSON_TYPE,
                     dn,
                     GaclManager::WMPGACL_EXEC );
         // overrides any-user authorization if DN auth is fine
         if (execDN) {
            execAU = true;
         }
      }

      // gacl file has no valid entries for user proxy without fqan
      if (execDN || execAU){
         exec = execDN = execAU = true;
      } 
   }

   // Final exec authorization value
   exec = exec && execDN && execAU;

   // checks exec permission
   if (!exec) {
      throw wmputilities::AuthorizationException(__FILE__, __LINE__,
            "checkGaclUserAuthZ()",
            wmputilities::WMS_AUTHORIZATION_ERROR,
            "Authorization error: user not authorized");
   }
} catch (wmputilities::GaclException& exc)
{
   string errmsg = "User not authorized:\n";
   errmsg += exc.what();
   throw wmputilities::GaclException(__FILE__, __LINE__, "checkGaclUserAuthZ()", wmputilities::WMS_GACL_FILE, errmsg);
}

} // anonymous namespace

auth_info
do_authZ_jobid(std::string const& action, std::string const& job_id)
{
   // retrieve delegated proxy in job directory
   std::string delegatedproxy = wmputilities::getJobDelegatedProxyPath(job_id);
   return authorize_and_map(action, delegatedproxy);
}

auth_info
do_authZ(std::string const& action, std::string const& delegation_id)
{
   std::string dn = wmputilities::getDN_SSL();
   std::string delegatedproxy = getDelegatedProxyPath(delegation_id, dn);
   return authorize_and_map(action, delegatedproxy);
}

std::string
do_authZ(std::string const& action)
{
   WMPAuthorizer auth(action);
   auth.authorize(); // throws
   return auth.getUserName();
}

void
WMPAuthorizer::map_user_lcmaps()
{
   edglog_fn("map_user_lcmaps");
   int retval;

   setenv("LCMAPS_POLICY_NAME", "standard:voms", 1);
   std::string log_file("/var/log/glite/lcmaps.log");
   char* log_dir = getenv("WMS_LOCATION_LOG");
   if (log_dir) {
      log_file = std::string(log_dir) + "/lcmaps.log";
   }
   lcmaps_init_and_logfile(
      const_cast<char *>(log_file.c_str()),
      0 /* no FILE* provided */,
      (unsigned short)1 /* user logging */);
   lcmaps_account_info_t plcmaps_account;
   retval = lcmaps_account_info_init(&plcmaps_account);
   if (retval) {
      throw wmputilities::AuthorizationException(__FILE__, __LINE__,
            "lcmaps_account_info_init()",
            wmputilities::WMS_USERMAP_ERROR,
            "LCMAPS info initialization failure");
   }

   // Send user mapping request to LCMAPS
   int mapcounter = 0; // single mapping result
   int fqan_num = 1; // N.B. Considering only one FQAN inside the list
   char* fqan_list[1]; // N.B. Considering only one FQAN inside the list
   fqan_list[0] = const_cast<char*>(fqans_.front().c_str());
   retval = lcmaps_return_account_without_gsi(const_cast<char*>(userdn_.c_str()),
            fqan_list, fqan_num, mapcounter, &plcmaps_account);

   if (retval) {
      retval = lcmaps_return_account_without_gsi(const_cast<char*>(userdn_.c_str()),
               fqan_list, fqan_num, mapcounter, &plcmaps_account);
      if (retval) {
         edglog(error) << "LCMAPS failed authorization: User "
                       << userdn_ << " is not authorized" << endl;
         throw wmputilities::AuthorizationException(__FILE__, __LINE__,
               "lcmaps_return_poolindex_without_gsi()",
               wmputilities::WMS_AUTHORIZATION_ERROR,
               "LCMAPS failed to map user credential");
      }
   }

   uid_ = plcmaps_account.uid;
   struct passwd* user_info = getpwuid(uid_);
   if (user_info == 0) {
      edglog(error)<<"LCMAPS: Unknown uid " << uid_ << endl;
      throw wmputilities::AuthorizationException(__FILE__, __LINE__,
            "getpwuidn()",wmputilities::WMS_USERMAP_ERROR,
            "LCMAPS could not find the username related to uid");
   }
   // Checking for mapped user group. The group of the assigned local user
   // MUST be different from the group of user running server
   if (user_info->pw_gid == getgid()) {
      edglog(error)<<"Mapping not allowed, mapped local user group equal "
                   "to group of user running server"<<endl;
      throw wmputilities::AuthorizationException(__FILE__, __LINE__,
            "map_user_lcmaps()", wmputilities::WMS_USERMAP_ERROR,
            "Mapping not allowed, mapped local user group equal to group"
            " of user running server\n(please contact server administrator)");
   }
   // Setting value for usergroup private member
   gid_ = user_info->pw_gid;
   // Cleaning structure
   retval = lcmaps_account_info_clean(&plcmaps_account);
   if (retval) {
      throw wmputilities::AuthorizationException(__FILE__, __LINE__,
            "lcmaps_account_info_clean()", wmputilities::WMS_USERMAP_ERROR,
            "LCMAPS info clean failure");
   }
   edglog(info)<<"LCMAPS uid: " << uid_ << ", gid: " << gid_ << endl;
}

void
setGridsiteJobGacl(std::vector<std::string> &jobids)
{
   edglog_fn("WMPAuthorizer::setGridsiteJobGacl()");

   if (jobids.size()) {
      string user_dn = wmputilities::getDN_SSL(); // taken from ssl
      string errmsg = "";

      // Creates a gacl file in the job directory
      WMPgaclPerm permissions =
         GaclManager::WMPGACL_LIST |
         GaclManager::WMPGACL_WRITE |
         GaclManager::WMPGACL_READ;

      // main user job directory
      string filename = wmputilities::getJobDirectoryPath(jobids[0]) + "/"
                        + GaclManager::WMPGACL_DEFAULT_FILE;

      try {
         GaclManager gacl(filename, true);
         // adds the new user credential entry
         gacl.addEntry(GaclManager::WMPGACL_PERSON_TYPE, user_dn.c_str());
         // allow permission
         gacl.allowPermission(GaclManager::WMPGACL_PERSON_TYPE,
                              user_dn.c_str(), permissions);
         gacl.saveGacl();
      } catch (wmputilities::GaclException& exc) {
         errmsg = "internal server error: unable to set the gacl permissions to user dn: ";
         errmsg += exc.what();
         throw wmputilities::GaclException(__FILE__, __LINE__, "setGridsiteJobGacl()",
                                           wmputilities::WMS_GACL_FILE, errmsg);
      }
      ifstream infile(filename.c_str());
      if (!infile.good()) {
         throw wmputilities::FileSystemException(__FILE__, __LINE__,
                                           "setGridsiteJobGacl()", wmputilities::WMS_IS_FAILURE, "Unable to open gacl "
                                           "input file\n(please contact server administrator)");
      }
      string gacltext = "";
      string s;
      while (getline(infile, s, '\n')) {
         gacltext += s + "\n";
      }
      infile.close();

      fstream outfile;

      std::vector<std::string>::iterator iter = jobids.begin();
      std::vector<std::string>::iterator const end = jobids.end();
      for (; iter != end; ++iter) {
         filename = wmputilities::getJobDirectoryPath(*iter) + "/"
                    + GaclManager::WMPGACL_DEFAULT_FILE;
         outfile.open(filename.c_str(), ios::out);
         if (!outfile.good()) {
            throw wmputilities::FileSystemException(__FILE__, __LINE__,
                                           "setGridsiteJobGacl()", wmputilities::WMS_IS_FAILURE, "Unable to open gacl "
                                           "output file\n(please contact server administrator)");
         }
         outfile<<gacltext;
         outfile.close();
      }
   }
}

bool
checkJobDrain()
{
   edglog_fn("WMPAuthorizer::checkJobDrain");

   bool ret = false;
   char* doc_root = getenv(DOCUMENT_ROOT.c_str());
   if (doc_root) {
      // gacl file: path location
      std::string drain_file = std::string(doc_root) + '/' + DRAIN_FILE;
      edglog(debug) <<"checking drain_file: "<<drain_file<<endl;
      if (utilities::fileExists(drain_file)) {
         return true;
      } else {
         return false;
      }
   }
   return ret;
}

void
checkProxyValidity(const string& proxy)
{
   edglog_fn("security::checkProxyValidity");

   edglog(debug)<<"Proxy path: " << proxy << endl;

   static int const PROXY_TIME_MISALIGNMENT_TOLERANCE = 5;
   time_t now = time(0);
   time_t proxytime = time(0);
   proxytime = getNotBefore(proxy);
   double timediff = proxytime - now;
   edglog(debug)<<"proxy time difference (proxy - now): "<< boost::lexical_cast<std::string>(timediff)<<endl;
   if (timediff > PROXY_TIME_MISALIGNMENT_TOLERANCE) {
      edglog(error)<<"validity starting time in the future ("<< timediff << " secs)"  <<endl;
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "checkProxyValidity()", wmputilities::WMS_PROXY_ERROR,
            "proxy validity starting time in the future"
            "\nPlease check client date/time");
   } else {
      if (timediff > 0) {
         edglog(debug)<<"tolerable Proxy validity starting time in the future ("
                      <<timediff<<" secs)"<<endl;
      }
   }
   long timeleft = getProxyTimeLeft(proxy);
   edglog(debug)<<"time left (should be positive): "<<timeleft<<endl;
   if (timeleft <= 1) {
      edglog(error)<<"the proxy has expired!"<<endl;
      throw wmputilities::ProxyOperationException(__FILE__, __LINE__,
            "checkProxyValidity()", wmputilities::WMS_PROXY_EXPIRED,
            "the proxy has expired");
   }
}

void
checkProxyExistence(const string& userproxypath, const string& jobid)
{
   edglog_fn("WMPAuthorizer::checkProxyExistence");

   string userproxypathbak = wmputilities::getJobDelegatedProxyPathBak(jobid);
   if (!wmputilities::fileExists(userproxypath)) {
      if (!wmputilities::fileExists(userproxypathbak)) {
         edglog(error)<<"Unable to find a Proxy file in the job directory for job:\n"
                      <<jobid<<endl;
         throw wmputilities::JobOperationException(__FILE__, __LINE__,
               "checkProxyExistence()", wmputilities::WMS_OPERATION_NOT_ALLOWED,
               "Unable to find a Proxy file in the job directory");
      } else {
         unlink(userproxypath.c_str());
         wmputilities::fileCopy(userproxypathbak, userproxypath);
      }
   } else {
      char* c_x509_proxy = 0;

      // Checking if the proxy is still registered to proxyrenewal
      int const err_code( glite_renewal_GetProxy(jobid.c_str(), &c_x509_proxy) );

      if (err_code == 0) {
         free(c_x509_proxy);
         // If the proxy is still registered i can override the back up
         wmputilities::fileCopy(userproxypath, userproxypathbak);
      } else {
         // If the proxy is not registered i ovverride the user.proxy with its back up
         unlink(userproxypath.c_str());
         wmputilities::fileCopy(userproxypathbak, userproxypath);
      }

   }
}

WMPAuthorizer::WMPAuthorizer(std::string const& action)
   : uid_(0), gid_(0), action_(action)
{
   // only the user DN can be taken (from grisite delegation as env var)
   userdn_ = wmputilities::getDN_SSL();
   // some simple requests do not  forward delegation id nor job id,
   // where the certificate path could be retrieved from
   // getting the FQANs from gridsite is and has been a source of trouble,
   // VOMS APIs should be used
   fqans_ = wmputilities::getGridsiteFQANs();
}

WMPAuthorizer::WMPAuthorizer(std::string const& action, std::string const& userproxypath)
   : uid_(0), gid_(0), action_(action), userproxypath_(userproxypath)
{
   // we have the delegated proxy, we can directly use the VOMS APIs
   VOMSAuthN authn(userproxypath);
   userdn_ = authn.getDN();
   fqans_ = authn.getFQANs();
}

std::vector<std::string>
WMPAuthorizer::getFQANs()
{
   return fqans_;
}

string
WMPAuthorizer::getUserName()
{
   struct passwd* user_info = 0;
   user_info = getpwuid(uid_);
   username_ = string(user_info->pw_name);
   return username_;
}

uid_t
WMPAuthorizer::getUserId()
{
   return uid_;
}

uid_t
WMPAuthorizer::getUserGroup()
{
   return gid_;
}

void
WMPAuthorizer::authorize()
{
   edglog_fn("WMPAuthorizer::authorize");

   bool use_argus =
      configuration::Configuration::instance()->wp()->argus_authz();
   if (use_argus) {
      if (!userproxypath_.empty()) {
         edglog(debug) << "Argus authZ and mapping" << endl;
         std::vector<std::string> endpoints =
            configuration::Configuration::instance()->wp()->argus_pepd_endpoints();
         if (!endpoints.empty()) {
            boost::tuple<bool, xacml_decision_t, uid_t, gid_t> ans;
            //boost::replace_all(userdn_, "/", ","); // MUST be RFC 2253 compliant
            //userdn_ = userdn_.substr(1, userdn_.size() - 1);
            ans = argus_authZ(
                  endpoints,
                  fqans_,
                  wmputilities::getEndpoint(),
                  action_,
                  userdn_,
                  userproxypath_);
            if (ans.get<0>()) {
               if (ans.get<1>() == XACML_DECISION_PERMIT) {
                  // set the mapping
                  uid_ = ans.get<2>();
                  gid_ = ans.get<3>();
                  edglog(debug) << "Argus returned XACML_DECISION_PERMIT"
                   " with mapping uid: " << uid_ << ", gid: " << gid_ << endl;
                  return;
               } else {
                  throw wmputilities::AuthorizationException(__FILE__, __LINE__,
                     "authorize()", wmputilities::WMS_AUTHORIZATION_ERROR,
                     "Argus denied authorization on " + action_ + " issued by DN: " + userdn_);
               }
            } else {

                  throw wmputilities::AuthorizationException(__FILE__, __LINE__,
                     "authorize()", wmputilities::WMS_AUTHORIZATION_ERROR,
                     "Argus request on " + action_ + " by DN: " + userdn_ +
                     " failed for some reason");
            }
         } else {
               throw wmputilities::AuthorizationException(__FILE__, __LINE__,
                  "authorize()", wmputilities::WMS_AUTHORIZATION_ERROR,
                  "no Argus endpoint was specified");
         }
      }
   } else {
      edglog(debug) << "Gridsite authZ and mapping" << endl;
      if (fqans_.empty()) {
         checkGaclUserAuthZ("", userdn_);
      } else {
         checkGaclUserAuthZ(fqans_.front(), userdn_);
      }
      map_user_lcmaps();
   }
}

}}}}
