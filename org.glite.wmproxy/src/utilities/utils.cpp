
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

//
// File: utils.cpp
// Author: Giuseppe Avellino <egee@datamat.it>
//

#include "utils.h"

#include <iostream>
#include <fstream>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <sys/param.h> // MAXLENPATH
#include <fcntl.h> // O_RDONLY
#include <netdb.h> // getnameinfo/getaddrinfo
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h> // wait
#include <sys/file.h> // flock

// boost
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>

#include <classad_distribution.h>

// JobId
#include "glite/jobid/JobId.h"
#include "glite/wms/common/utilities/manipulation.h"

// Logging and Bookkeeping
#include "glite/lb/JobStatus.h"

// Logging
#include "logging.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"
// Event logger
#include "eventlogger/eventlogger.h"

#include "glite/wms/purger/purger.h"
#include "glite/wms/common/utilities/quota.h"

// Exceptions
#include "wmpexceptions.h"
#include "wmpexception_codes.h"

#include "server/common.h"

// ISB compression functionalities
#include "zlib.h"
#include "libtar.h"

#define local

// Global variables for configuration attributes
extern bool globusDNS_global;

namespace logger = glite::wms::common::logger;
namespace commonutilities = glite::wms::common::utilities;
namespace jobid = glite::jobid;
namespace purger  = glite::wms::purger;

using namespace std;

namespace glite {
namespace wms {
namespace wmproxy {
namespace utilities {

namespace {

const string FILE_SEP ="/";
const char* SSL_CLIENT_DN = "SSL_CLIENT_S_DN"; // mod_ssl env var
const char* DOCUMENT_ROOT = "DOCUMENT_ROOT";
const string INPUT_SB_DIRECTORY = "input";
const string OUTPUT_SB_DIRECTORY = "output";
const string PEEK_DIRECTORY = "peek";
const string USER_PROXY_NAME = "user.proxy"; //delegated proxy copied in sandbox
const string USER_PROXY_NAME_BAK = ".user.proxy.bak";
const string JDL_ORIGINAL_FILE_NAME = "JDLOriginal";
const string JDL_TO_START_FILE_NAME = "JDLToStart";
const string JDL_STARTED_FILE_NAME = "JDLStarted";
const string START_LOCK_FILE_NAME = ".startLockFile.lock";
const string GET_OUTPUT_LOCK_FILE_NAME = ".getOutputLockFile.lock";
const string ALL_PROTOCOLS = "all";
const string DEFAULT_PROTOCOL = "default";

}

vector<string>
computeOutputSBDestURIBase(vector<string> outputsb, const string& baseuri)
{
   GLITE_STACK_TRY("computeOutputSBDestURIBase()");
   std::string::size_type pos;
   int size;
   string path;
   vector<string> returnvector;

   vector<string>::iterator iter = outputsb.begin();
   vector<string>::iterator const end = outputsb.end();
   for (; iter != end; ++iter) {
      path = *iter;
      size = path.size();
      pos = path.rfind("/", size);
      if (pos != string::npos) {
         returnvector.push_back(baseuri + "/" + path.substr(pos + 1, size));
      } else {
         returnvector.push_back(baseuri + "/" + path);
      }
   }
   return returnvector;
   GLITE_STACK_CATCH();
}


vector<string>
computeOutputSBDestURI(vector<string> osbdesturi, const string& dest_uri)
{
   GLITE_STACK_TRY("computeOutputSBDestURI()");
   std::string::size_type pos;
   string uri;
   string path;
   string outputdir;
   vector<string> returnvector;

   vector<string>::iterator iter = osbdesturi.begin();
   vector<string>::iterator const end = osbdesturi.end();
   for (; iter != end; ++iter) {
      path = *iter;
      edglog(debug)<<"osbdesturi[i]: "<<*iter<<endl;
      pos = path.find("://");
      if (pos != string::npos) {
         // The path is an URL
         returnvector.push_back(path);
      } else {
         // The path is not an URL
         returnvector.push_back(dest_uri + "/" + OUTPUT_SB_DIRECTORY
                                + "/" + path);
      }
   }
   return returnvector;
   GLITE_STACK_CATCH();
}

// why using returning pointers?
// because it will have to fill a returning gSoap message
vector<string> *
getJobDirectoryURIsVector(vector<pair<string, int> > allProtocols,
                          const string& defaultprotocol, int defaultport, int httpsport,
                          const string& jid, const string& protocol, const string& extradir)
{
   GLITE_STACK_TRY("getJobDirectoryURIsVector()");
   edglog_fn("wmputils::getJobDirectoryURIsVector");
   edglog(debug)<<"Requested protocol: "<<protocol<<endl;
   // Protocol + host:port + path
   // extra might be empty/input/output/peek
   string extra = (extradir != "") ? (FILE_SEP + extradir) : "";
   // httppath is like: /SandboxDir/Tu/https_3a_2f_2fghemon.cnaf.infn.it_3a9000_2fTuhKg/output
   string httppath = to_filename(jobid::JobId(jid), 0) + extra;
   string path = getenv(DOCUMENT_ROOT) + FILE_SEP + httppath;
   string serverhost = getServerHost(); //e.g. "ghemon.cnaf.infn.it"
   vector<string> *returnvector = new vector<string>();
   vector<pair<string, int> > returnprotocols;  // Empty vectory
   if (protocol == ALL_PROTOCOLS) {
      // filling with all protocols (gsiftp & https)
      returnprotocols = allProtocols;
   } else if (protocol == DEFAULT_PROTOCOL) {
      // filling with default protocol (gsiftp)
      pair<string, int> itempair(defaultprotocol, defaultport);
      returnprotocols.push_back(itempair);
   } else if (protocol != "https") {
      // Custom protocol: try & find it inside allProtocols available
      int port = -1;
      for (unsigned int i = 0; i < allProtocols.size(); i++) {
         if (allProtocols[i].first == protocol) {
            port = allProtocols[i].second;
            break;
         }
      }
      if (port == -1) {
         // Unable to properly initialize protocol/port
         throw JobOperationException(__FILE__, __LINE__,
                                     "getJobDirectoryURIsVector()", WMS_INVALID_ARGUMENT,
                                     "requested protocol not available");
      }
      pair<string, int> itempair(protocol, port);
      returnprotocols.push_back(itempair);
   } else {
      // Protocol IS definitely HTTPS
      // doing Nothing!?
   }
   string item;
   // cycle returnprotocols (which is not the return/result)
   // returnprotocols has the format:  <protocol NAME >,<protocol PORT >
   for (unsigned int i = 0; i < returnprotocols.size(); i++) {
      item = returnprotocols[i].first + "://" + serverhost;
      // Append Port (if needed)
      if  (returnprotocols[i].second != 0) {
         item += ":" + boost::lexical_cast<string>(returnprotocols[i].second);

      }
      item += path;
      edglog(debug)<<"Job "<< returnprotocols[i].first << " URI: "<<item<<endl;
      returnvector->push_back(item);
   }
   // Adding https protocol
   if ((protocol == ALL_PROTOCOLS) || (protocol == "https")) {
      // New approach: always return HOST NAME (never IP)
      item = "https://" + serverhost;
      if (httpsport) {
         item += ":" + boost::lexical_cast<string>(httpsport);
      } else {
         // no https port in config file: append Server Port
         item += ":" + string(getenv ("SERVER_PORT"));
      }
      item+= httppath;
      edglog(debug)<<"Job https URI: "<<item<<endl;
      returnvector->push_back(item);
   }
   return returnvector;
   GLITE_STACK_CATCH();
}

vector<string>
parseFQAN(const string& fqan)
{
   vector<string> returnvector;
   boost::char_separator<char> separator("/");
   boost::tokenizer<boost::char_separator<char> > tok(fqan, separator);
   for (boost::tokenizer<boost::char_separator<char> >::iterator
         token = tok.begin(); token != tok.end(); ++token) {
      returnvector.push_back(*token);
   }
   return returnvector;
}

string
getGridsiteVO()
{
   return parseFQAN(getGridsiteFQANs().front()).front();
}

vector<string>
getGridsiteFQANs()
{
   GLITE_STACK_TRY("getEnvFQAN()");

   vector<string> fqans;
   static string const fqan_tag("fqan:");
   unsigned int const fqan_tag_size = std::string(fqan_tag).size();
   char *tmp;
   unsigned int i = 0;
   do {
      tmp = getenv(std::string("GRST_CRED_AURI_" + boost::lexical_cast<std::string>(i)).c_str());
      ++i;

      std::string grst_cred;
      if (tmp) {
         grst_cred = std::string(tmp);
      }
      if (
         grst_cred.size() > fqan_tag_size
         && grst_cred.substr(0, fqan_tag_size) == fqan_tag
      ) {
         fqans.push_back(grst_cred.substr(fqan_tag_size));
      }
   } while(tmp);

   return fqans;
   GLITE_STACK_CATCH();
}

std::pair<std::string, int>
parseLBAddress(const string& addressport)
{
   GLITE_STACK_TRY("parseAddressPort()");

   std::pair<std::string, int> addresspair;
   string addressportarg = addressport;
   std::string::size_type pos;
   unsigned int addressportsize = addressportarg.size();

   // Removing final slashes
   for (unsigned int i = 0; i < addressportsize; i++) {
      if (addressportarg.substr(addressportsize - 1, addressportsize - 1)
            == FILE_SEP) {
         addressportarg = addressportarg.substr(0, addressportsize - 1);
         addressportsize--;
      }
   }

   if (addressportarg != "") {
      addressportsize = addressportarg.size();
      if ((pos = addressportarg.find("://")) != string::npos) {
         addressportarg = addressportarg.substr(pos +1, addressportsize );
      }
      if ((pos = addressportarg.rfind(":", addressportarg.size()))
            != string::npos) {
         addresspair.first = addressportarg.substr(0, pos);
         addresspair.second =
            atoi(addressportarg.substr(pos + 1, addressportarg.size()).c_str());
      } else {
         addresspair.first = addressportarg;
         addresspair.second = 0;
      }
   } else {
      addresspair.first = "";
      addresspair.second = 0;
   }

   return addresspair;
   GLITE_STACK_CATCH();
}


bool checkGlobusVersion()
{
   edglog_fn("wmputils::checkGlobusVersion");
   const char *GLOBUS_LOCATION = "GLOBUS_LOCATION";
   const string DEF_GLOBUS_LOCATION= FILE_SEP+"opt"+FILE_SEP+"globus";
   string globusVersionFile="globus-version";

   // Check ENV var (if necessary set it) and check file globus-version file existence
   char* globusENV=getenv(GLOBUS_LOCATION);
   if(globusENV) {
      // ENV found
      globusVersionFile= string(globusENV)  + FILE_SEP + "bin" + FILE_SEP + globusVersionFile ;
   } else {
      // ENV not found, set it up
      edglog(warning)<<GLOBUS_LOCATION<<" variable not found, setting it to " << DEF_GLOBUS_LOCATION << endl ;
      setenv(GLOBUS_LOCATION, DEF_GLOBUS_LOCATION.c_str(),1);
      globusVersionFile= DEF_GLOBUS_LOCATION+ FILE_SEP + "bin" + FILE_SEP + globusVersionFile ;

   }
   // If file does not exists -> old version of globus assumed
   if (!fileExists(globusVersionFile)) {
      edglog(warning)<<"globus-version binary not found" << endl ;
      edglog(warning)<<"Assuming globus version is less than 3.0.2" << endl ;
      return false;
   }

   // prepare Std output/error files
   string outfile = "/tmp/wmp_glversion_call.out."+ boost::lexical_cast<std::string>(getpid());
   int fdO = open(outfile.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600);
   dup2(fdO, 1);
   close(fdO);
   string errorfile = "/tmp/wmp_glversion_call.err."+ boost::lexical_cast<std::string>(getpid());
   int fdE = open(errorfile.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600);
   dup2(fdE, 2);
   close(fdE);

   // prepare input parameters
   vector<string> params;
   params.push_back("1>");
   params.push_back(outfile);
   params.push_back("2>");
   params.push_back(errorfile);

   // System call
   string errormsg = "";
   edglog(debug)<<"Executing Globus version script file: "<<globusVersionFile<<endl;
   int outcome=doExecv(globusVersionFile, params, errormsg);
   switch (outcome) {
   case SUCCESS:
      // No error, break and continue
      break;
   case FORK_FAILURE:
   case COREDUMP_FAILURE:
      // either Unable to fork process or coredump
      edglog(error)<< "either Unable to fork process or coredump"  << endl;
      edglog(debug)<< "Assuming Globus version is less than 3.0.2" << endl;
      return false;
      break;
   default:
      // any possible error
      edglog(error)<<"Unable to execute Globus version script file:\n"<<errormsg<<endl;
      edglog(error)<<"Error code: "<<outcome<<endl;
      edglog(debug)<<"Assuming Globus version is less than 3.0.2" << endl ;
      return false;
   }
   // IF this point is reached, no error found
   // Try and Parse output result
   string globusVersionString=readTextFile(outfile);
   remove(errorfile.c_str());
   remove(outfile.c_str());
   boost::char_separator<char> separator(".");
   boost::tokenizer<boost::char_separator<char> > tok(globusVersionString,separator);
   // check there are 3 tokens:
   vector<string> tokens;
   boost::tokenizer<boost::char_separator<char> >::iterator it = tok.begin();
   boost::tokenizer<boost::char_separator<char> >::iterator const end = tok.end();
   for (; it != end; it++) {
      tokens.push_back(string(*it));
   }
   if (tokens.size() !=3) {
      edglog(warning)<<"Unable to parse returned Globus version"<< globusVersionString <<endl;
      edglog(warning)<<"Assuming Globus version is less than 3.0.2" << endl ;
      return false;
   }

   try {
      // Check The version
      int glmaj, glmed, glmin;
      const int GLMAJ=3;
      const int GLMED=0;
      const int GLMIN=2;
      bool glDetected_b=false;
      glmaj=boost::lexical_cast<int>(tokens[0]);
      glmed=boost::lexical_cast<int>(tokens[1]);
      glmin=boost::lexical_cast<int>(tokens[2]);

      if (glmaj>GLMAJ) {
         glDetected_b=true;
      } else if (glmaj==GLMAJ) {
         if (glmed>GLMED) {
            glDetected_b=true;
         } else if (glmed==GLMED) {
            if (glmin>=GLMIN) {
               glDetected_b=true;
            }
         }
      }

      if (glDetected_b) {
         edglog(debug)<<"Detected Globus version greater than/equal to 3.0.2: " << globusVersionString << endl ;
         return true;
      } else {
         edglog(debug)<<"Detected Globus version less than 3.0.2: " << globusVersionString << endl ;
         return false;
      }
   } catch(boost::bad_lexical_cast& exc) {
      edglog(warning)<<"Unable to cast Globus version "<< globusVersionString <<endl;
      edglog(debug)<<"Assuming Globus version is less than 3.0.2" << endl ;
      return false;
   }
   // This point should never be reached
   edglog(fatal) << "Unreachable point reached!" << endl ;
   edglog(debug) << "Assuming Globus version is less than 3.0.2" << endl ;
   return false;
}

int
fileExists(const string& path)
{
   GLITE_STACK_TRY("fileExists()");
   struct stat buffer;
   if (stat(path.c_str(), &buffer)) {
      return 0;
   }
   return 1;
   GLITE_STACK_CATCH();
}

string
getFileName(const string& path)
{
   GLITE_STACK_TRY("getFileName()");
   string filename = path;
   std::string::size_type pos = path.rfind("/");
   if (pos != string::npos) {
      filename = path.substr(pos + 1, string::npos);
   }
   return filename;
   GLITE_STACK_CATCH();
}

string
getDestURI(const string& jobid, const string& protocol,
           int port)
{
   GLITE_STACK_TRY("getDestURI()");
   string dest_uri(protocol + "://" + getServerHost()
                   + ((port == 0) ? "" : (":" + boost::lexical_cast<string>(port)))
                   + getenv(DOCUMENT_ROOT) + FILE_SEP
                   + to_filename(jobid));
   return dest_uri;
   GLITE_STACK_CATCH();
}

string
getJobReducedPath(jobid::JobId jid, int level)
{
   GLITE_STACK_TRY("getJobReducedPath()");
   return string(getenv(DOCUMENT_ROOT) + FILE_SEP
                 + glite::wms::common::utilities::get_reduced_part(jid, level));
   GLITE_STACK_CATCH();
}

string
getJobDirectoryPath(jobid::JobId jid, int level)
{
   GLITE_STACK_TRY("getJobDirectoryPath()");
   return string(getenv(DOCUMENT_ROOT) + FILE_SEP
                 + to_filename(jid, level));
   GLITE_STACK_CATCH();
}

string
getJobInputSBRelativePath(glite::jobid::JobId jid, int level)
{
   GLITE_STACK_TRY("getJobInputRelativePath()");
   return (  to_filename(jid, level) + FILE_SEP
             + INPUT_SB_DIRECTORY);
   GLITE_STACK_CATCH();
}

string
getJobStartLockFilePath(jobid::JobId jid, int level)
{
   GLITE_STACK_TRY("getJobStartLockFilePath()");
   return string(getenv(DOCUMENT_ROOT) + FILE_SEP
                 + to_filename(jid, level) + FILE_SEP + START_LOCK_FILE_NAME);
   GLITE_STACK_CATCH();
}


string
getGetOutputFileListLockFilePath(jobid::JobId jid, int level)
{
   GLITE_STACK_TRY("getGetOutputFileListLockFilePath()");
   return string(getenv(DOCUMENT_ROOT) + FILE_SEP
                 + to_filename(jid, level) + FILE_SEP + GET_OUTPUT_LOCK_FILE_NAME);
   GLITE_STACK_CATCH();
}

string
getInputSBDirectoryPath(jobid::JobId jid, int level)
{
   GLITE_STACK_TRY("getInputSBDirectoryPath()");
   return string(getenv(DOCUMENT_ROOT) + FILE_SEP
                 + to_filename(jid, level) + FILE_SEP + INPUT_SB_DIRECTORY);
   GLITE_STACK_CATCH();
}

string
getOutputSBDirectoryPath(jobid::JobId jid, int level)
{
   GLITE_STACK_TRY("getOutputSBDirectoryPath()");
   return string(getenv(DOCUMENT_ROOT) + FILE_SEP
                 + to_filename(jid, level) + FILE_SEP + OUTPUT_SB_DIRECTORY);
   GLITE_STACK_CATCH();
}

string
getPeekDirectoryPath(jobid::JobId jid, int level, bool docroot)
{
   GLITE_STACK_TRY("getPeekDirectoryPath()");
   string path;
   if (docroot) {
      path = string(getenv(DOCUMENT_ROOT) + FILE_SEP + to_filename(jid, level)
                    + FILE_SEP + PEEK_DIRECTORY);
   } else {
      path = string(FILE_SEP + to_filename(jid, level) + FILE_SEP
                    + PEEK_DIRECTORY);
   }
   return path;
   GLITE_STACK_CATCH();
}

string
getJobDelegatedProxyPath(jobid::JobId jid, int level)
{
   GLITE_STACK_TRY("getJobDelegatedProxyPath(JobId jid)");
   return string(getenv(DOCUMENT_ROOT) + FILE_SEP
                 + to_filename(jid, level)
                 + FILE_SEP + USER_PROXY_NAME);
   GLITE_STACK_CATCH();
}

string
getJobDelegatedProxyPathBak(jobid::JobId jid, int level)
{
   GLITE_STACK_TRY("getJobDelegatedProxyPath(JobId jid)");
   //TBD Check path
   return string(getenv(DOCUMENT_ROOT) + FILE_SEP
                 + to_filename(jid, level)
                 + FILE_SEP + USER_PROXY_NAME_BAK);
   GLITE_STACK_CATCH();
}

string
getJobJDLOriginalPath(jobid::JobId jid, bool isrelative, int level)
{
   GLITE_STACK_TRY("getJobJDLOriginalPath(JobId jid)");
   //TBD Check path
   if (!isrelative) {
      return string(getenv(DOCUMENT_ROOT) + FILE_SEP
                    + to_filename(jid, level)
                    + FILE_SEP + JDL_ORIGINAL_FILE_NAME);
   } else {
      return string(to_filename(jid, level)
                    + FILE_SEP + JDL_ORIGINAL_FILE_NAME);
   }
   GLITE_STACK_CATCH();
}

string
getJobJDLToStartPath(jobid::JobId jid, bool isrelative, int level)
{
   GLITE_STACK_TRY("getJobJDLToStartPath(JobId jid)");
   //TBD Check path
   if (!isrelative) {
      return string(getenv(DOCUMENT_ROOT)) + FILE_SEP
                    + to_filename(jid, level)
                    + FILE_SEP + JDL_TO_START_FILE_NAME;
   } else {
      return string(to_filename(jid, level)
                    + FILE_SEP + JDL_TO_START_FILE_NAME);
   }
   GLITE_STACK_CATCH();
}

string
getJobJDLStartedPath(jobid::JobId jid, bool isrelative, int level)
{
   GLITE_STACK_TRY("getJobJDLStartedPath()");
   //TBD Check path
   if (!isrelative) {
      return string(getenv(DOCUMENT_ROOT)
                    + to_filename(jid, level)
                    + FILE_SEP + JDL_STARTED_FILE_NAME);
   } else {
      return string(to_filename(jid, level)
                    + FILE_SEP + JDL_STARTED_FILE_NAME);
   }
   GLITE_STACK_CATCH();
}

string
getJobJDLExistingStartPath(jobid::JobId jid, bool isrelative, int level)
{
   GLITE_STACK_TRY("getJobJDLStartedPath()");

   string started = getJobJDLStartedPath(jid);
   if (fileExists(started)) {
      return started;
   } else {
      return getJobJDLToStartPath(jid);
   }
   GLITE_STACK_CATCH();
}

string
getEndpoint()
{
   GLITE_STACK_TRY("getEndpoint()");
   return ((string(getenv("HTTPS")) == "on") ? "https://" : "http://")
          + getServerHost() + ":"
          + string(getenv("SERVER_PORT"))
          + string(getenv("SCRIPT_NAME"));
   GLITE_STACK_CATCH();
}

string
getServerHost()
{
   GLITE_STACK_TRY("getServerHost()");
   edglog_fn("wmputils::getServerHost");
   char * servername = getenv("SERVER_NAME");
   string result = "";
   if (servername) {
      result = resolveIPv4_IPv6(string(servername));
      if (result.empty()) {
         edglog(critical)<<"Unable to get server address"<<endl;
         throw FileSystemException(__FILE__, __LINE__,
                                   "getServerHost()", WMS_PROXY_ERROR,
                                   "Unable to get server address");
      }
   } else {
      throw FileSystemException(__FILE__, __LINE__,
                                "getServerHost()", WMS_PROXY_ERROR,
                                "Environment variable SERVER_NAME null\n(please contact server administrator)");
   }
   return result;
   GLITE_STACK_CATCH();
}

string
resolveIPv4_IPv6(string host_tbr)
{

   struct addrinfo * result;
   struct addrinfo * res;
   int error;
   string resolved_host = "";


   /* resolve the domain name into a list of addresses */
   error = getaddrinfo((char*)host_tbr.c_str(), NULL, NULL, &result);

   if (error != 0) {
      //perror("error in getaddrinfo: ");
      //return "UnresolvedHost";
      throw FileSystemException(__FILE__,__LINE__,"resolveIPv4_IPv6", WMS_PROXY_ERROR,
                                "Unable to resolve hostname");

   }

   if (result == NULL) {
      throw FileSystemException(__FILE__,__LINE__,"resolveIPv4_IPv6", WMS_PROXY_ERROR,
                                "Unable to resolve hostname");
   }

   resolved_host = "UnresolvedHost";

   for (res = result; res != NULL; res = res->ai_next) {
      char hostname[NI_MAXHOST] = "";

      error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0);

      if (0 != error ) {
         continue;
      }

      if (*hostname) {
         resolved_host = hostname;
         break;
      }

   }

   if( resolved_host == "UnresolvedHost" ) {
      freeaddrinfo(result);
      throw FileSystemException(__FILE__,__LINE__,"resolveIPv4_IPv6",WMS_PROXY_ERROR,
                                "Unable to resolve hostname");
   }

   freeaddrinfo(result);

   return resolved_host;

}

bool
doPurge(string dg_jobid, bool force, bool is_parent)
{
   GLITE_STACK_TRY("doPurge()");
   edglog_fn("wmputils::doPurge");
   if (dg_jobid.length()) {
      eventlogger::WMPEventLogger wmplogger(utilities::getEndpoint());
      edglog(debug)<<"JobId object for purging created: "
                   <<dg_jobid<<endl;
      // DTMT issue 56 fix: (DAG node PURGE)
      purger::Purger ThePurger(wmplogger.getLBProxy());
      if (force) {
         // Forcing purge (needed for dag nodes)
         return ThePurger.force_dag_node_removal()(jobid::JobId(dg_jobid));
      } else {
         if (is_parent) {
            return ThePurger.skip_status_checking(true).threshold(0)(jobid::JobId(dg_jobid));
         } else {
            return ThePurger(jobid::JobId(dg_jobid));
         }
      }
   } else {
      edglog(critical)
            <<"Error in Purging: Invalid Job Id. Purge not done."<<endl;
      return false;
   }
   GLITE_STACK_CATCH();
}

bool
getUserQuota(pair<long, long>& result, string uname)
{
   GLITE_STACK_TRY("getUserQuota()");
   result = commonutilities::quota::getQuota(uname);
   return true;
   GLITE_STACK_CATCH();
}

bool
getUserFreeQuota(pair<long, long>& result, string uname)
{
   GLITE_STACK_TRY("getUserFreeQuota()");
   result = commonutilities::quota::getFreeQuota(uname);
   return true;
   GLITE_STACK_CATCH();
}

std::string
getDN_SSL()
{
   GLITE_STACK_TRY("getDN_SSL()");
   edglog_fn("wmputils::getDN_SSL");
   edglog(debug)<<"Getting user DN..."<<endl;
   char *p = 0;
   char *client_dn = 0;
   char *user_dn = 0;

   client_dn = getenv(SSL_CLIENT_DN);
   if ((client_dn == 0) || (client_dn == '\0')) {
      edglog(debug)<<"Environment variable "<<string(SSL_CLIENT_DN)
                   <<" not correctly defined"<<endl;
      throw ProxyOperationException(__FILE__, __LINE__,
                                    "getDN_SSL()", WMS_PROXY_ERROR, "Unable to get a valid user DN");
   }

   user_dn = strdup(client_dn);
   p = strstr(user_dn, "/CN=proxy");
   if (p != NULL) {
      *p = '\0';
   }
   p = strstr(user_dn, "/CN=limited proxy");
   if (p != NULL) {
      *p = '\0';
   }
   if ((user_dn == NULL) || (user_dn[0] == '\0')) {
      throw ProxyOperationException(__FILE__, __LINE__,
                                    "getDN_SSL()", WMS_PROXY_ERROR, "Unable to get a valid user DN");
   }
   // PATCH  FOR BUG  #30006: LCMAPS/Globus DN inconsistency for VDT 1.6 gridftp server
   edglog(debug) << "User DN: " << user_dn << endl;
   string dn_str(user_dn);
   free(user_dn);
   return dn_str;
   GLITE_STACK_CATCH();
}

void
waitForSeconds(int seconds)
{
   GLITE_STACK_TRY("waitForSeconds()");
   edglog_fn("wmputils::waitForSeconds");
   edglog(debug)<<"Waiting for "<<seconds<<" seconds..."<<endl;
   time_t startTime = time(NULL);
   time_t endTime = time(NULL);
   int counter = 0;
   while((endTime - startTime) < seconds) {
      if ((endTime%3600) != counter) {
         switch (counter%4) {
         case 0:
            edglog(debug)<<"-"<<endl;
            break;
         case 1:
            edglog(debug)<<"\\"<<endl;
            break;
         case 2:
            edglog(debug)<<"|"<<endl;
            break;
         case 3:
            edglog(debug)<<"/"<<endl;
            break;
         default:
            break;
         }
         counter = endTime%3600;
      }
      endTime = time(NULL);
   }
   edglog(debug)<<"End waiting"<<endl;
   GLITE_STACK_CATCH();
}

int
generateRandomNumber(int lowerlimit, int upperlimit)
{
   GLITE_STACK_TRY("generateRandomNumber()");
   edglog_fn("wmputils::generateRandomNumber");

   edglog(debug)<<"Generating random between "<<lowerlimit<<" and " <<upperlimit<<endl;
   // Setting seeed
   srand((unsigned) time(0));
   return lowerlimit + static_cast<int>(rand()%(upperlimit - lowerlimit + 1));
   GLITE_STACK_CATCH();
}

void
fileCopy(const string& source, const string& target)
{
   GLITE_STACK_TRY("fileCopy()");
   edglog_fn("wmputils::fileCopy");
   edglog(debug)<<"Copying file...\n\tSource: "
                <<source<<"\n\tTarget: "<<target<<endl;

   ifstream in(source.c_str());
   if (!in.good()) {
      edglog(severe)<<"Copy failed, !in.good(). \n\tSource: "
                    <<source<<" Target: "<<target<<endl;
      throw FileSystemException(__FILE__, __LINE__,
                                "fileCopy(const string& source, const string& target)",
                                WMS_IS_FAILURE, "Unable to copy file");
   }
   ofstream out(target.c_str());
   if (!out.good()) {
      edglog(severe)<<"Copy failed, !out.good(). \n\tSource: "
                    <<source<<"\n\tTarget: "<<target<<endl;
      throw FileSystemException(__FILE__, __LINE__,
                                "fileCopy(const string& source, const string& target)",
                                WMS_IS_FAILURE, "Unable to copy file");
   }
   out<<in.rdbuf(); // read original file into target

   struct stat from_stat;
   if (stat(source.c_str(), &from_stat) ||
         chown(target.c_str(), from_stat.st_uid, from_stat.st_gid) ||
         chmod(target.c_str(), from_stat.st_mode)) {
      edglog(severe)<<"Copy failed, chown/chmod. \n\tSource: "
                    <<source<<"\n\tTarget: "<<target<<endl;
      throw FileSystemException(__FILE__, __LINE__,
                                "fileCopy(const string& source, const string& target)",
                                WMS_IS_FAILURE, "Unable to copy file");
   }
   GLITE_STACK_CATCH();
}

string
to_filename(glite::jobid::JobId j, int level, bool extended_path)
{
   GLITE_STACK_TRY("to_filename()");
   string path(glite::wms::wmproxy::server::sandboxdir_global + string(FILE_SEP)
               + glite::wms::common::utilities::get_reduced_part(j, level));
   if (extended_path) {
      path.append(string(FILE_SEP) + glite::wms::common::utilities::to_filename(j));
   }
   return path;
   GLITE_STACK_CATCH();
}

long
computeFileSize(const string& path)
{
   GLITE_STACK_TRY("computeFileSize()");
   int fd = -1;
   long size = 0;
   fd = open(path.c_str(), O_RDONLY);
   if (fd != -1) {
      struct stat buf;
      if (!fstat(fd, &buf)) {
         size = buf.st_size;
      }
      close(fd);
   }
   // If file not found it returns 0
   return size;
   GLITE_STACK_CATCH();
}

string
searchForDirmanager()
{
   GLITE_STACK_TRY("searchForDirmanager()");

   char* p = getenv("WMS_LOCATION_LIBEXEC");
   string dirmanager_path(p ? p : "");
   if (dirmanager_path.empty()) {
      p = getenv("GLITE_LOCATION");
      dirmanager_path = string(p ? p : "");
      if (dirmanager_path.empty()) {
         dirmanager_path = "/usr/libexec";
      } else {
         dirmanager_path += "/bin";
      }
   }
   dirmanager_path += "/glite_wms_wmproxy_dirmanager";
   return dirmanager_path;
   GLITE_STACK_CATCH();
}


/**
Release Memory for all allocated char**
*/
void releaseChars( char **allocated, int size)
{
   for (int j = 0; j <= size; j++) {
      free(allocated[j]);
   }
   free(allocated);
}


/**
* Perform a system call (through a process forked)
*/
int
doExecv(const string& command, vector<string> &params, string& errormsg)
{
   GLITE_STACK_TRY("doExecv()");
   edglog_fn("wmputils::doExecv");

   char **argvs;
   int size = params.size() + 2;
   argvs = (char **) calloc(size, sizeof(char *));
   unsigned int i = 0;
   argvs[i] = (char *) malloc(command.length() + 1);
   strcpy(argvs[i++], command.c_str());
   vector<string>::iterator iter = params.begin();
   vector<string>::iterator const end = params.end();
   for (; iter != end; ++iter) {
      argvs[i] = (char *) malloc((*iter).length() + 1);
      strcpy(argvs[i++], (*iter).c_str());
   }
   argvs[i] = (char *) 0;

   edglog(debug)<<"Forking process..."<<endl;
   switch (fork()) {
   case -1:
      // Unable to fork
      errormsg = "Unable to fork process";
      edglog(critical)<<errormsg<<endl;
      return FORK_FAILURE;
      break;
   case 0:
      // child: execute the required command
      if (execv(command.c_str(), argvs)) {
         // execv failed
         errormsg = strerror(errno);
         edglog(severe) << "execv error, errno: " << errno
                        << " - Error message: " << errormsg <<endl;
         if (errno) {
            return EXEC_FAILURE;
         } else {
            return SCRIPT_FAILURE;
         }
      } else {
         edglog(debug)<<"execv successful"<<endl;
      }
      // the child does not return
      break;
   default:
      // parent
      int status = 0;
      wait(&status);
      if (WIFEXITED(status)) {
         edglog(debug)<<"Child wait succesfully (WIFEXITED(status))"<<endl;
         edglog(debug)<<"WEXITSTATUS(status): "<<WEXITSTATUS(status)<<endl;
      }
      if (WIFSIGNALED(status)) {
         edglog(severe)<<"WIFSIGNALED(status)"<<endl;
         edglog(severe)<<"WTERMSIG(status): "<<WTERMSIG(status)<<endl;
      }
#ifdef WCOREDUMP
      if (WCOREDUMP(status)) {
         errormsg = "Child dumped core";
         edglog(critical)<<"Child dumped core!!!"<<endl;
         releaseChars (argvs,i);
         return COREDUMP_FAILURE;
      }
#endif // WCOREDUMP
      if (status) {
         if (WIFEXITED(status)) {
            errormsg = strerror(WEXITSTATUS(status));
         } else {
            errormsg = "Child failure";
         }
         edglog(severe)<<"Child failure, exit code: "<<status<<endl;
         releaseChars (argvs,i);
         return WEXITSTATUS(status);
      }
      break;
   }
   releaseChars (argvs,i);
   return SUCCESS;
   GLITE_STACK_CATCH();
}

/*
* split the command to be executed when too long
* (example: when directory naming is used)
* and perform execv
*/
int
doExecvSplit(const string& command, vector<string> &params, const vector<string> &dirs,
             unsigned int startIndex, unsigned int endIndex)
{
   GLITE_STACK_TRY("doExecvSplit()");
   edglog_fn("wmputils::doExecvSplit");
   char **argvs;
   // +3 -> difference between index, command at first position, NULL at the end
   int size = params.size() + endIndex - startIndex + 3;
   argvs = (char **) calloc(size, sizeof(char *));

   unsigned int i = 0;

   argvs[i] = (char *) malloc(command.length() + 1);
   strcpy(argvs[i++], (command).c_str());

   vector<string>::iterator iter = params.begin();
   vector<string>::iterator const end = params.end();
   for (; iter != end; ++iter) {
      argvs[i] = (char *) malloc((*iter).length() + 1);
      strcpy(argvs[i++], (*iter).c_str());
   }
   for (unsigned int j = startIndex; j <= endIndex; j++) {
      argvs[i] = (char *) malloc(dirs[j].length() + 1);
      strcpy(argvs[i++], (dirs[j]).c_str());
   }
   argvs[i] = (char *) 0;

   if (execv(command.c_str(), argvs)) {
      unsigned int middle;
      switch (errno) {
      case E2BIG:
         edglog(debug)<<"Command line too long, splitting..."<<endl;
         middle = startIndex + (endIndex - startIndex) / 2;
         pid_t pid;
         switch (pid = fork()) {
         case -1:
            // Unable to fork
            edglog(critical)<<"Unable to fork process"<<endl;
            return FORK_FAILURE;
            break;
         case 0:
            edglog(debug)<<"Calling from index "<<startIndex
                         <<" to "<<middle<<endl;
            if (doExecvSplit(command, params, dirs, startIndex, middle)) {
               return EXEC_FAILURE;
            }
            break;
         default:
            // parent
            int status = SUCCESS;
            edglog(debug)<<"Parent PID wait: "<<getpid()
                         <<" waiting for: "<<pid<<endl;
            waitpid(pid, &status, 0);
            edglog(debug)<<"Parent PID after wait: "<<getpid()
                         <<" waiting for: "<<pid<<endl;
            if (WIFEXITED(status)) {
               edglog(debug)<<"Child wait succesfully (WIFEXITED(status))"<<endl;
               edglog(debug)<<"WEXITSTATUS(status): "<<WEXITSTATUS(status)<<endl;
            }
            if (WIFSIGNALED(status)) {
               edglog(severe)<<"WIFSIGNALED(status)"<<endl;
               edglog(severe)<<"WEXITSTATUS(status): "<<WTERMSIG(status)<<endl;
            }

#ifdef WCOREDUMP
            if (WCOREDUMP(status)) {
               edglog(critical)<<"Child dumped core!!!"<<endl;
            }
#endif // WCOREDUMP

            if (status) {
               edglog(severe)<<"Child failure, exit code: "<<status<<endl;
               return EXEC_FAILURE;
            }

            edglog(debug)<<"Calling from index "<<middle + 1
                         <<" to "<<endIndex<<endl;
            if (doExecvSplit(command, params, dirs, middle + 1, endIndex)) {
               return EXEC_FAILURE;
            }
            break;
         }

         break;

      default:
         edglog(severe)<<"execv error, errno: "<<errno
                       <<" - Error message: "<<strerror(errno)<<endl;
         break;
      }
   } else {
      edglog(debug)<<"execv succesfully"<<endl;
   }

   for (unsigned int j = 0; j <= i; j++) {
      free(argvs[j]);
   }
   free(argvs);

   return SUCCESS;

   GLITE_STACK_CATCH();
}

/*
* Perform execv (through doExecvSplit)
*/
int
doExecv(const string& command, vector<string> &params, const vector<string> &dirs,
        unsigned int startIndex, unsigned int endIndex)
{
   GLITE_STACK_TRY("doExecv()");
   edglog_fn("wmputils::doExecv");

   edglog(debug)<<"Forking process..."<<endl;
   pid_t pid;
   switch (pid = fork()) {
   case -1:
      // Unable to fork
      edglog(critical)<<"Unable to fork process"<<endl;
      return FORK_FAILURE;
      break;
   case 0:
      // child
      if (doExecvSplit(command, params, dirs, startIndex, endIndex)) {
         edglog(severe)<<"execv error!"<<endl ;
      }
      break;
   default:
      // parent
      int status = SUCCESS;
      edglog(debug)<<"Parent PID wait: "<<getpid()<<" waiting for: "<<pid<<endl;
      waitpid(pid, &status, 0);
      edglog(debug)<<"Parent PID after wait: "<<getpid()<<" waiting for: "<<pid<<endl;
      if (WIFEXITED(status)) {
         edglog(debug)<<"Child wait succesfully (WIFEXITED(status))"<<endl;
         edglog(debug)<<"WEXITSTATUS(status): "<<WEXITSTATUS(status)<<endl;
      }
      if (WIFSIGNALED(status)) {
         edglog(severe)<<"WIFSIGNALED(status)"<<endl;
         edglog(severe)<<"WEXITSTATUS(status): "<<WTERMSIG(status)<<endl;
      }
#ifdef WCOREDUMP
      if (WCOREDUMP(status)) {
         edglog(critical)<<"Child dumped core!!!"<<endl;
         return COREDUMP_FAILURE;
      }
#endif // WCOREDUMP
      if (status) {
         string errormsg = "";
         edglog(severe)<<"Child failure, exit code: "<<status<<endl;
         if (WIFEXITED(status)) {
            errormsg = strerror(WEXITSTATUS(status));
         } else {
            errormsg = "Child failure";
         }
         edglog(severe)<<"Child failure, exit code: "<<status<<endl;
         return WEXITSTATUS(status);
      }
      break;
   }
   return SUCCESS;
   GLITE_STACK_CATCH();
}

void
untarFile(const string& file, const string& untar_starting_path,
          uid_t userid, uid_t groupid)
{
   GLITE_STACK_TRY("untarFile()");
   edglog_fn("wmputils::untarFile");

   if (fileExists(file)) {
      string gliteDirmanExe = searchForDirmanager();

      // Creating parameters vector for zip file extraction
      vector<string> extparams;
      extparams.push_back("-c");
      extparams.push_back(boost::lexical_cast<string>(userid));
      extparams.push_back("-g");
      extparams.push_back(boost::lexical_cast<string>(groupid));
      extparams.push_back("-m");
      extparams.push_back("0770");
      extparams.push_back("-x");
      extparams.push_back(untar_starting_path);

      vector<string> extfiles;
      extfiles.push_back(file);

      // Extracting files
      if (doExecv(gliteDirmanExe, extparams, extfiles, 0, extfiles.size() - 1)) {
         edglog(critical)<<"Unable to untar ISB file:"<<file<<endl;
         throw FileSystemException(__FILE__, __LINE__,
                                   "untarFile()", WMS_FILE_SYSTEM_ERROR, "Unable to untar ISB file"
                                   "\n(please contact server administrator)");
      }

   } else {
      edglog(critical)<<"Unable to untar ISB file, file does not exist: "
                      <<file<<endl;
      throw FileSystemException(__FILE__, __LINE__,
                                "untarFile()", WMS_FILE_SYSTEM_ERROR,
                                "Unable to untar ISB file\n(please contact server administrator)");
   }

   GLITE_STACK_CATCH();
}

void
managedir(const string& document_root, uid_t userid, uid_t jobdiruserid,
          vector<string> jobids, jobdirectorytype dirtype)
{
   GLITE_STACK_TRY("managedir()");
   edglog_fn("wmputils::managedir");

   time_t starttime = time(NULL);

   unsigned int size = jobids.size();
   edglog(debug)<<"Job id vector size: "<<size<<endl;

   if (size) {
      string gliteDirmanExe = searchForDirmanager();

      string useridtxt = boost::lexical_cast<string>(userid);
      string grouptxt = boost::lexical_cast<string>(getgid());

      int level = 0;
      bool extended_path = true;
      // Vector contains at least one element
      string path = to_filename (glite::jobid::JobId(jobids[0]),
                                 level, extended_path);
      std::string::size_type pos = path.find(FILE_SEP, 0);

      // Creating SanboxDir directory if needed
      string sandboxdir = document_root + FILE_SEP + path.substr(0, pos)
                          + FILE_SEP;
      if (!fileExists(sandboxdir)) {
         string run = gliteDirmanExe
                      + " -c " + useridtxt
                      + " -g " + grouptxt
                      + " -m 0773 " + sandboxdir;
         edglog(debug)<<"Creating SandboxDir..."<<endl;
         edglog(debug)<<"Executing: \n\t"<<run<<endl;
         if (system(run.c_str())) {
            edglog(fatal)<<"Unable to create sandbox directory"<<endl;
            throw FileSystemException(__FILE__, __LINE__,
                                      "managedir()", WMS_FILE_SYSTEM_ERROR,
                                      "Unable to create sandbox directory\n"
                                      "(please contact server administrator)");
         }
      }

      // Creating parameters vector for reduced directories creation
      vector<string> redparams;
      redparams.push_back("-c");
      string juser = useridtxt;
      redparams.push_back(juser);
      redparams.push_back("-g");
      string group = grouptxt;
      redparams.push_back(group);
      redparams.push_back("-m");
      redparams.push_back("0773");

      // Creating parameters vector for job directories creation
      vector<string> jobparams;
      jobparams.push_back("-c");
      juser = boost::lexical_cast<string>(jobdiruserid);
      jobparams.push_back(juser);
      jobparams.push_back("-g");
      group = grouptxt;
      jobparams.push_back(group);
      jobparams.push_back("-m");
      jobparams.push_back("0770");

      vector<string> reddirs;
      vector<string> jobdirs;
      string allpath;
      string reduceddir;

      // Populating directories to create vector
      vector<string>::iterator iter = jobids.begin();
      vector<string>::iterator const end = jobids.end();

      for (; iter != end; ++iter) {
         path = to_filename(glite::jobid::JobId(*iter),
                            level, extended_path);
         allpath = path;
         pos = path.find(FILE_SEP, 0);
         sandboxdir = path.substr(0, pos);
         path.erase(0, pos);
         reduceddir = path.substr(1, path.find(FILE_SEP, 1));

         // Reduced directories
         reddirs.push_back(document_root + FILE_SEP + sandboxdir
                           + FILE_SEP + reduceddir);

         // Job directories
         path = document_root + FILE_SEP + allpath;
         jobdirs.push_back(path);
         path += FILE_SEP;
         switch (dirtype) {
         case DIRECTORY_ALL:
            jobdirs.push_back(path + INPUT_SB_DIRECTORY);
            jobdirs.push_back(path + OUTPUT_SB_DIRECTORY);
            jobdirs.push_back(path + PEEK_DIRECTORY);
            break;
         case DIRECTORY_INPUT:
            jobdirs.push_back(path + INPUT_SB_DIRECTORY);
            break;
         case DIRECTORY_OUTPUT:
            jobdirs.push_back(path + OUTPUT_SB_DIRECTORY);
            jobdirs.push_back(path + PEEK_DIRECTORY);
            break;
         default:
            break;
         }
      }

      // Creating reduced directories
      if (doExecv(gliteDirmanExe, redparams, reddirs, 0, reddirs.size() - 1)) {
         edglog(fatal)<<"Unable to create job local directory (reduced)"<<endl;
         throw FileSystemException(__FILE__, __LINE__,
                                   "managedir()", WMS_FILE_SYSTEM_ERROR,
                                   "Unable to create job local directory\n"
                                   "(please contact server administrator)");
      }

      // Creating job directories
      if (doExecv(gliteDirmanExe, jobparams, jobdirs, 0, jobdirs.size() - 1)) {
         edglog(fatal)<<"Unable to create job local directory (job)"<<endl;
         throw FileSystemException(__FILE__, __LINE__,
                                   "managedir()", WMS_FILE_SYSTEM_ERROR,
                                   "Unable to create job local directory\n"
                                   "(please contact server administrator)");
      }
   }
   time_t stoptime = time(NULL);
   edglog(debug)<<"Directory creation elapsed time: "
                <<(stoptime - starttime)<<endl;

   GLITE_STACK_CATCH();
}

void
setFlagFile(const string& file, bool flag)
{
   GLITE_STACK_TRY("setFlagFile()");
   edglog_fn("wmputils::setFlagFile");

   if (flag) {
      fstream outfile(file.c_str(), ios::out);
      if (!outfile.good()) {
         edglog(severe)<<file<<": !outfile.good()"<<endl;
         throw FileSystemException(__FILE__, __LINE__,
                                   "setFlagFile()", WMS_IS_FAILURE, "Unable to set flag file"
                                   "\n(please contact server administrator)");
      }
      outfile<<"flag";
      outfile.close();
   } else {
      remove(file.c_str());
   }
   GLITE_STACK_CATCH();
}

int
operationLock(const string& lockfile, const string& opname)
{
   GLITE_STACK_TRY("operationLock()");
   edglog_fn("wmputils::operationLock");

   edglog(debug)<<"Opening lock file: "<<lockfile<<endl;
   int fd = open(lockfile.c_str(), O_CREAT | O_RDWR, S_IRWXU);
   if (fd == -1) {
      edglog(debug)<<"Unable to open lock file: "<<lockfile<<endl;
      throw FileSystemException( __FILE__, __LINE__,
                                 "operationLock()", WMS_FILE_SYSTEM_ERROR,
                                 "unable to open lock file");
   }

   struct flock flockstruct;
   memset(&flockstruct, 0, sizeof(flockstruct));

   // [ Exclusive write lock
   flockstruct.l_type = F_WRLCK;

   // Blocking lock
   if (fcntl(fd, F_SETLKW, &flockstruct) == -1) {
      edglog(debug)<<"Unable to lock file: "<<lockfile<<endl;
      edglog(debug)<<strerror(errno)<<endl;
      close(fd);
      throw JobOperationException( __FILE__, __LINE__,
                                   "operationLock()", WMS_OPERATION_NOT_ALLOWED,
                                   opname + " operation already in progress");
   }
   // ]

   return fd;

   GLITE_STACK_CATCH();
}

void
operationUnlock(int fd)
{
   GLITE_STACK_TRY("operationUnlock()");
   edglog_fn("wmputils::operationUnlock");

   struct flock flockstruct;
   memset(&flockstruct, 0, sizeof(flockstruct));

   // [ Unlocking file
   flockstruct.l_type = F_WRLCK;
   if (fcntl(fd, F_SETLKW, &flockstruct) == -1) {
      edglog(severe)<<"Unable to remove lock file, fd: "<<fd<<endl;
   }
   close(fd);
   // ]

   GLITE_STACK_CATCH();
}

bool
isOperationLocked(const string& lockfile)
{
   GLITE_STACK_TRY("isOperationLocked()");
   edglog_fn("wmputils::isOperationLocked");

   edglog(debug)<<"Opening lock file: "<<lockfile<<endl;
   int fd = open(lockfile.c_str(), O_CREAT | O_WRONLY, S_IRWXU);
   if (fd == -1) {
      edglog(debug)<<"Unable to open lock file: "<<lockfile
                   <<" during lock check"<<endl;
      throw FileSystemException( __FILE__, __LINE__,
                                 "operationLock()", WMS_FILE_SYSTEM_ERROR,
                                 "Unable to open lock file");
   }

   struct flock flockstruct;
   memset(&flockstruct, 0, sizeof(flockstruct));

   // [ Checking lock
   flockstruct.l_type = F_WRLCK;
   flockstruct.l_start = 0;
   flockstruct.l_whence = SEEK_SET;
   flockstruct.l_len = 0; // EOF

   if (fcntl(fd, F_GETLK, &flockstruct) < 0) {
      edglog(debug)<<"Unable to check if the file is locked, fd: "<<fd<<endl;
      throw FileSystemException( __FILE__, __LINE__,
                                 "operationLock()", WMS_FILE_SYSTEM_ERROR,
                                 "Unable to check if the file is locked");
   }
   if (flockstruct.l_type != F_UNLCK) {
      return true;
   }
   // ]

   close(fd);
   return false;

   GLITE_STACK_CATCH();
}

void
createSuidDirectory(const string& directory)
{
   GLITE_STACK_TRY("createSuidDirectory()");
   edglog_fn("wmputils::createSuidDirectory");

   if (!fileExists(directory)) {
      string gliteDirmanExe = searchForDirmanager();

      string dirpermissions = " -m 0773 ";
      string user = " -c " + boost::lexical_cast<string>(getuid()); // UID
      string group = " -g " + boost::lexical_cast<string>(getgid()); // GROUP

      string command = gliteDirmanExe + user + group + dirpermissions + directory;
      edglog(debug)<<"Executing command: "<<command<<endl;
      if (system(command.c_str())) {
         edglog(critical)<<"Unable to create directory: "<<directory<<endl;
         throw FileSystemException(__FILE__, __LINE__,
                                   "createDirectory()", WMS_FILE_SYSTEM_ERROR,
                                   "Unable to create directory\n(please contact server "
                                   "administrator)");
      }
   }

   GLITE_STACK_CATCH();
}

void
writeTextFile(const string& file, const string& text)
{
   GLITE_STACK_TRY("writeTextFile()");
   edglog_fn("wmputils::writeTextFile");

   fstream outfile(file.c_str(), ios::out);
   if (!outfile.good()) {
      edglog(severe)<<file<<": !outfile.good()"<<endl;
      throw FileSystemException(__FILE__, __LINE__,
                                "writeTextFile()", WMS_IS_FAILURE, "Unable to write file: "
                                + file + "\n(please contact server administrator)");
   }
   outfile<<text;
   outfile.close();

   GLITE_STACK_CATCH();
}

string
readTextFile(const string& file)
{
   GLITE_STACK_TRY("readTextFile()");
   edglog_fn("wmputils::readTextFile");

   ifstream in(file.c_str(), ios::in);
   if (!in.good()) {
      edglog(debug)<<file<<": !in.good()"<<endl;
      throw FileSystemException(__FILE__, __LINE__,
                                "readTextFile()", WMS_IS_FAILURE, "Unable to read file: "
                                + file + "\n(please contact server administrator)");
   }
   string line;
   string text = "";
   while (getline(in, line, '\n')) {
      text += line + "\n";
   }
   in.close();
   return text;

   GLITE_STACK_CATCH();
}

bool
isNull(string field)
{
   GLITE_STACK_TRY("isNull()");
   bool is_null = false ;
   int p1 = field.size() - 5 ;
   int p2 = field.find("=NULL");

   if ((p1 > 0) && (p1 == p2)) {
      is_null = true;
   }
   return is_null;
   GLITE_STACK_CATCH();
}

/*
* Removes white spaces form the begininng and from the end of the input string
*/
const string
cleanString(string str)
{
   GLITE_STACK_TRY("cleanString()");
   int len = 0;
   string ws = " "; //white space char
   len = str.size( );
   if (len > 0) {
      // erases white space at the beginning of the string
      while (len>1) {
         if ( str.compare(0,1,ws) == 0) {
            str = str.substr(1, len);
         } else {
            break;
         }
         len = str.size();
      }
      // erases white space at the end of the string
      while (len>1) {
         if( str.compare(len-1,1,ws) == 0 ) {
            str = str.substr(0, len-1);
         } else {
            break;
         }
         len = str.size();
      }
      // 1 white space
      if (len == 1 & str.compare(ws)==0) {
         str = "";
      }
   }
   return str;

   GLITE_STACK_CATCH();
}

/**
* Converts all of the characters in this String to lower case
*/
const string
toLower(const string& src)
{
   GLITE_STACK_TRY("toLower()");

   string result(src);
   transform(result.begin(), result.end(), result.begin(), ::tolower);
   return result;

   GLITE_STACK_CATCH();
}

/**
* Cuts the input string in two pieces (label and value) according to
* the separator character "="
*/
void
split(const string& field, string& label, string& value)
{
   GLITE_STACK_TRY("split()");

   unsigned int size = field.size();
   if (size > 0) {
      std::string::size_type p = field.find("=") ;
      if (p != string::npos & (p < size)) {
         label = field.substr(0, p);
         value = field.substr(p+1, size-(p+1));
         // removes white spaces at the beginning and a the end of the
         // strings (if present) and converts the uppercase letters to the
         // corresponding lowercase
         label = toLower(cleanString(label));
         value = toLower(cleanString(value));
      }
   }

   GLITE_STACK_CATCH();
};


bool
hasElement(const std::vector<std::string> &vect, const std::string& elem)
{

   GLITE_STACK_TRY("hasElement()");

   bool result = false;
   int size = vect.size();
   for (int i=0; i < size; i++) {
      if (elem.compare(vect[i]) == 0) {
         result = true;
         break;
      }
   }
   return result;

   GLITE_STACK_CATCH();
};

/**
 * Removes '/' characters at the end of the of the input pathname
 */
const std::string
normalizePath( const std::string& fpath )
{
   GLITE_STACK_TRY("normalizePath()");

   string                   modified;
   string::const_iterator   last, next;
   string::reverse_iterator check;

   last = fpath.begin();
   do {
      next = find( last, fpath.end(), '/' );

      if( next != fpath.end() ) {
         modified.append( last, next + 1 );

         for( last = next; *last == '/'; ++last );
      } else {
         modified.append( last, fpath.end() );
      }
   } while( next != fpath.end() );

   check = modified.rbegin();
   if( *check == '/' ) {
      modified.assign( modified.begin(), modified.end() - 1 );
   }

   return modified;

   GLITE_STACK_CATCH();
}

/*
* Gets the absolute path of the file
*/
const std::string
getAbsolutePath(const string& file)
{
   GLITE_STACK_TRY("getAbsolutePath()");

   string path = file;
   char* pwd = getenv ("PWD");
   if (path.find("./")==0 || path.compare(".")==0) {
      // PWD path  (./)
      if (pwd) {
         string leaf = path.substr(1,string::npos);
         if (leaf.size()>0) {
            if ( leaf.find("/",0) !=0 ) {
               path = normalizePath(pwd) + "/"  + leaf;
            } else {
               path = normalizePath(pwd) + leaf;
            }
         } else {
            path = normalizePath(pwd) + leaf;
         }
      }
   } else if (path.find("/") ==0 ) {
      // ABsolute Path
      path = normalizePath(path);
   } else {
      // Relative path: append PWD
      if (pwd) {
         path = normalizePath(pwd) + "/" + path;
      }
   }
   return path;

   GLITE_STACK_CATCH();
}

glite::jobid::JobId getParent(glite::lb::JobStatus status)
{
   glite::jobid::JobId jid ;
   try {
      jid = status.getValJobId(glite::lb::JobStatus::PARENT_JOB);
   } catch (std::exception& exc) {
      // Do Nothing
   }
   return jid ;
}

bool hasParent(glite::lb::JobStatus status )
{
   glite::jobid::JobId pj ;
   bool res = false;
   try {
      pj = status.getValJobId(glite::lb::JobStatus::PARENT_JOB);
      res = true;
   } catch (std::exception& exc) {
      res = false;
   }
   return res;
}

} // namespace utilities
} // namespace wmproxy
} // namespace wms
} // namespace glite

