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

#include "common.h"

#include <ctype.h> // isspace

// WMP Configuration
#include "configuration.h"

// Utilities
#include "utilities/utils.h"

// WMP Exceptions
#include "utilities/wmpexceptions.h"
#include "utilities/wmpexception_codes.h"

#include "glite/jdl/ExpDagAd.h"

// JDL Attributes
#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/jdl_attributes.h"

#include "glite/jobid/JobId.h"

// Logger
#include "utilities/logging.h"
#include "glite/wms/common/logger/edglog.h"

#include "wmproxyserve.h"
#include "security/delegation.h"
#include "security/authorizer.h"
#include "security/gaclmanager.h"
#include "security/vomsauthn.h"

// Global variables for configuration
extern WMProxyConfiguration conf;

// Global variables for configuration attributes (ENV dependant)
extern std::string dispatcher_type_global;

using namespace std;
using namespace glite::jdl; // Ad
using namespace glite::wms::wmproxy::utilities; //Exception

//namespace logger = glite::wms::common::logger;
namespace wmputilities  = glite::wms::wmproxy::utilities;
namespace server = glite::wms::wmproxy::server;
namespace jobid = glite::jobid;

namespace glite {
namespace wms {
namespace wmproxy {
namespace server {

std::string sandboxdir_global;

namespace {

const char* DOC_ROOT = "DOCUMENT_ROOT";
const std::string FILE_SEPARATOR = "/";

} // anonymous namespace

/**
* check Document Root & Staging path
*/
void
setGlobalSandboxDir()
{
   GLITE_STACK_TRY("setGlobalSandboxDir()");
   edglog_fn("wmpcommon::setGlobalSandboxDir");

   string sandboxstagingpath = conf.getSandboxStagingPath();

   if (!getenv(DOC_ROOT)) {
      edglog(fatal)<<"DOCUMENT_ROOT variable not set"<<endl;
      throw FileSystemException( __FILE__, __LINE__,
                                 "setGlobalSandboxDir()", wmputilities::WMS_FILE_SYSTEM_ERROR,
                                 "DOCUMENT_ROOT variable not set");
   }
   string documentroot = getenv(DOC_ROOT);
   if (documentroot == "") {
      edglog(fatal)<<"DOCUMENT_ROOT is an empty string"<<endl;
      throw FileSystemException( __FILE__, __LINE__,
                                 "setGlobalSandboxDir()", wmputilities::WMS_FILE_SYSTEM_ERROR,
                                 "DOCUMENT_ROOT is an empty string"
                                 "\n(please contact server administrator)");
   }

   // Checking if sandbox staging path starts with $DOCUMENT_ROOT
   if (sandboxstagingpath.find(documentroot) != 0) {
      edglog(severe)<<"ERROR: SandboxStagingPath inside "
                    "configuration file MUST start with $DOCUMENT_ROOT"<<endl;
      throw FileSystemException( __FILE__, __LINE__,
                                 "setGlobalSandboxDir()", wmputilities::WMS_FILE_SYSTEM_ERROR,
                                 "SandboxStagingPath inside configuration file MUST start "
                                 "with $DOCUMENT_ROOT\n(please contact server administrator)");
   }

   if (sandboxstagingpath != documentroot) {
      sandboxdir_global = sandboxstagingpath.substr(documentroot.length() + 1,
                          sandboxstagingpath.length() - 1);
      if (sandboxdir_global.find(FILE_SEPARATOR) != string::npos) {
         string msg = "SandboxStagingPath configuration attribute MUST be "
                      "in the form:"
                      "\n$DOCUMENT_ROOT/<single directory name>"
                      "\nwhere DOCUMENT_ROOT MUST be as defined in httpd configuration file"
                      "\n(please contact server administrator)";
         edglog(fatal)<<msg<<endl;
         throw FileSystemException( __FILE__, __LINE__,
                                    "setGlobalSandboxDir()", wmputilities::WMS_FILE_SYSTEM_ERROR,
                                    msg);
      }
   } else {
      sandboxdir_global = documentroot;
   }
   edglog(debug)<<"Sandbox directory: "<<sandboxdir_global<<endl;

   GLITE_STACK_CATCH();
}


/**
* used by initWMProxyOperation
*/
string displayENV(const string& title, char* envNAME)
{
   string msg = title+": ";
   if (getenv(envNAME)) {
      msg += string(getenv(envNAME));
   } else {
      msg += "Not Available";
   }
   return msg;
}

void
initWMProxyOperation(const std::string& operation)
{
   GLITE_STACK_TRY("initWMProxyOperation()");
   edglog_fn("wmpcommon::initWMProxyOperation");

   edglog(info)<< "Called Operation: "<< operation <<endl;
   // Logging Remote Host Info
   edglog(info) << displayENV("Remote Host Address","REMOTE_ADDR");
   if (getenv("REMOTE_PORT")) {
      edglog(debug) <<":" << string(getenv("REMOTE_PORT"))  << endl ;
   } else {
      edglog(debug)<< endl;
   }
   edglog(info) << displayENV("Remote Host Name","REMOTE_HOST")<< endl;
   edglog(info) << displayENV("Remote CLIENT S DN","SSL_CLIENT_S_DN")<<endl;
   edglog(info) << displayENV("Remote GRST CRED","GRST_CRED_2")<<endl;
   edglog(info) << displayENV("Service GRST PROXY LIMIT","GRST_GSIPROXY_LIMIT")<<endl;
   edglog(info)<<"WMProxy instance serving core request N.: " <<++server::servedrequestcount_global<<endl;

   setGlobalSandboxDir(); // throws
   if (operation != "jobStart") { // bug # 40370
      callLoadScriptFile(operation);
   }

   GLITE_STACK_CATCH();
}

void
checkJobDirectoryExistence(jobid::JobId jid, int level)
{
   GLITE_STACK_TRY("checkJobDirectoryExistence()");
   edglog_fn("wmpcommon::checkJobDirectoryExistence");
   // TODO change it with boost::isDirectory (or similar)
   if (!wmputilities::fileExists(wmputilities::getJobDirectoryPath(jid))) {
      edglog(error)<<"Job Directory Path: " << wmputilities::getJobDirectoryPath(jid) << "\ndoes not exist!"<< endl ;
      edglog(error)<<"The job has not been registered from this Workload Manager Proxy server or it has been purged"<<endl;
      throw JobOperationException(__FILE__, __LINE__,
                                  "checkJobDirectoryExistence()",
                                  wmputilities::WMS_JOB_NOT_FOUND,
                                  "The job has not been registered from this Workload Manager Proxy "
                                  "server (" + wmputilities::getEndpoint() + ") or it has been purged");
   }
   GLITE_STACK_CATCH();
}

/**
 * Returns an int value representing the type of the job described by the jdl
 * If type is not specified -> Job  // TBC change it in the future?
 */
int
getType(string jdl, Ad * ad)
{
   GLITE_STACK_TRY("getType()");
   edglog_fn("wmpcommon::getType");

   // Default type is JOB
   int return_value = TYPE_JOB;
   Ad *in_ad = new Ad(jdl);
   if (in_ad->hasAttribute(JDL::TYPE)) {
      string type =
         glite_wms_jdl_toLower((in_ad->getString(JDL::TYPE)));

      edglog(info) <<"JDL Type: "<<type<<endl;
      if (type == JDL_TYPE_DAG) {
         return_value = TYPE_DAG;
      } else if (type == JDL_TYPE_JOB) {
         return_value = TYPE_JOB;
      } else if (type == JDL_TYPE_COLLECTION) {
         return_value = TYPE_COLLECTION;
      }
   } else {
      edglog(info) <<"JDL Type: Job (by default)"<<endl;
   }
   if (ad) {
      *ad = *in_ad;
   }
   delete in_ad;
   return return_value;

   GLITE_STACK_CATCH();
}

void
callLoadScriptFile(const string& operation)
{
   GLITE_STACK_TRY("callLoadScriptFile()");
   edglog_fn("wmpcommon::callLoadScriptFile");

   string path = conf.getOperationLoadScriptPath(operation);
   if (path != "") {
      // Default values:
      // glite-load-monitor --oper jobRegister --load1 10 --load5 10
      //        --load15 10 --memusage 95 --diskusage 95 --fdnum 500

      string str = "";
      string command = "";
      vector<string> params;
      bool commandfound = false;
      string::const_iterator iter = path.begin();
      string::const_iterator const end = path.end();
      edglog(debug)<<"Executing command: " ;
      while (iter != end) {
         // Removing spaces
         while ((iter != end) && isspace((char)*iter)) {
            iter++;
         }
         while ((iter != end) && !isspace((char)*iter)) {
            str += *iter;
            iter++;
         }
         edglog(debug)<<" "<<str;
         if (commandfound) {
            params.push_back(str);
         } else {
            command = str;
            commandfound = true;
         }
         str = "";
      }
      edglog(debug) << endl;

      params.push_back("2>");
      string errorfile = "/tmp/wmpscriptcall.err."
                         + boost::lexical_cast<std::string>(getpid());
      edglog(debug)<<"Script error file: "<<errorfile<<endl;
      params.push_back(errorfile);

      if (!wmputilities::fileExists(command)) {
         edglog(warning)<<"Operation \""<<operation
                        <<"\" load script file does not exist:\n"<<command
                        <<"\nIgnoring load script call..."<<endl;
      } else {
         // Creating stderr file
         int fd = open(errorfile.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600);
         dup2(fd, 2);
         close(fd);

         // Executing load script file
         string errormsg = "";
         edglog(debug)<<"Executing load script file: "<<command<<endl;
         if (int outcome = wmputilities::doExecv(command, params, errormsg)) {
            // EXIT CODE !=0
            switch (outcome) {
               // otherwise the caller is forked anyway!!!
            case EXEC_FAILURE:
            case FORK_FAILURE:
            case COREDUMP_FAILURE:
               // and then? we continue with two instances of the same forked function??
               // ... better raise something
               throw FileSystemException( __FILE__, __LINE__,
                                          wmputilities::readTextFile(errorfile),
                                          wmputilities::WMS_FILE_SYSTEM_ERROR,
                                          "Load script failed");
               break;
            case SCRIPT_FAILURE: {
               // Exit Code is 1 => throw ServerOverLoaded exc
               string stderrormsg= wmputilities::readTextFile(errorfile);
               // If error message is consistent => append information
               if (!stderrormsg.empty()) {
                  stderrormsg=":\n" + stderrormsg;
               }
               remove(errorfile.c_str());
               throw ServerOverloadedException(__FILE__, __LINE__,
                                               "callLoadScriptFile()",
                                               wmputilities::WMS_SERVER_OVERLOADED,
                                               "System load is too high" + stderrormsg);
            }
            break;
            default:
               // Exit Code > 1 => Error executing script
               edglog(error)<<"Unable to execute load script file, error code: " << outcome << ". ";
               edglog(error)<<"Reason: " << wmputilities::readTextFile(errorfile) << '\n';
               break;
            }
         }
         remove(errorfile.c_str());
      }
   }

   GLITE_STACK_CATCH();
}
}}}}
