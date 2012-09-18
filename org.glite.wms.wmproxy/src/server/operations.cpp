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
// File: operations.cpp
// Author: Giuseppe Avellino <egee@datamat.it>
//

#include <fstream>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

// Boost
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include "boost/filesystem/path.hpp"

#include "common.h"
#include "operations.h"
#include "configuration.h"
#include "structconverter.h"

#include "glite/wms/common/utilities/manipulation.h"

// Utilities
#include "utilities/utils.h"

// Eventlogger
#include "eventlogger/expdagad.h"
#include "eventlogger/eventlogger.h"

#include "security/vomsauthn.h"
#include "security/authorizer.h"
#include "security/gaclmanager.h"
#include "security/delegation.h"

//Logger
#include "utilities/logging.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/logger_utils.h"

#include "commands/listfiles.h"


// WMP Exceptions
#include "utilities/wmpexceptions.h"
#include "utilities/wmpexception_codes.h"

// RequestAd
#include "glite/jobid/JobId.h"
#include "glite/jdl/PrivateAttributes.h"
#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/jdl_attributes.h"
#include "glite/jdl/DAGAdManipulation.h"
#include "glite/jdl/RequestAdExceptions.h"

// Logging and Bookkeeping
#include "glite/lb/JobStatus.h"

// Configuration
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"

using namespace std;
using namespace glite::lb; // JobStatus
using namespace glite::jdl; // DagAd, AdConverter
using namespace glite::jobid; //JobId

using namespace glite::wms::wmproxy::server;
using namespace glite::wms::wmproxy::utilities; //Exception
using namespace glite::wms::wmproxy::eventlogger;
using namespace glite::wms::common::configuration; // Configuration

namespace logger         = glite::wms::common::logger;
namespace configuration  = glite::wms::common::configuration;
namespace wmputilities   = glite::wms::wmproxy::utilities;
namespace eventlogger    = glite::wms::wmproxy::eventlogger;
namespace security     = glite::wms::wmproxy::security;

// Global variables for configuration
extern WMProxyConfiguration conf;

// WMProxy software version
const std::string WMP_VERSION = WMP_SOFTWARE_VERSION;

// DONE job output file
const std::string MARADONA_FILE = "Maradona.output";

// Perusal functionality
const std::string PERUSAL_FILE_2_PEEK_NAME = "files2peek";
const std::string TEMP_PERUSAL_FILE_NAME = "tempperusalfile";
const std::string PERUSAL_DATE_INFO_SEPARATOR = "-";

// File size limit of globus URL copy
const long FILE_TRANSFER_SIZE_LIMIT = 2147000000;
// 2 Giga = 2 * 1.073.741.824 = 2.147.483.648

// Defining File Separator
#ifdef WIN
// Windows File Separator
const std::string FILE_SEPARATOR = "\\";
#else
// Linux File Separator
const std::string FILE_SEPARATOR = "/";
#endif

const std::string ALL_PROTOCOLS = "all";
const std::string DEFAULT_PROTOCOL = "default";

//
// WM Web Service available operations
//

// To get more information see WMProxy service wsdl file
void
getVersion(getVersionResponse& getVersion_response)
{
   GLITE_STACK_TRY("getVersion()");
   edglog_fn("wmpoperations::getVersion");

   initWMProxyOperation("getVersion");
   security::do_authZ("getVersion");
   getVersion_response.version = WMP_VERSION;
   edglog(info) << "Version retrieved: "<< getVersion_response.version << endl;
   GLITE_STACK_CATCH();
}

void
getJDL(const std::string& job_id, JdlType jdltype, getJDLResponse& getJDL_response)
{
   GLITE_STACK_TRY("getJDL()");
   edglog_fn("wmpoperations::getJDL");
   edglog(debug) << "getJDL requested for job: " << job_id <<endl;

   initWMProxyOperation("getJDL");
   security::do_authZ_jobid("getJDL", job_id);
   getJDL_response.jdl = "";
   JobId jid(job_id);
   switch (jdltype) {
   case WMS_JDL_ORIGINAL:
      getJDL_response.jdl = wmputilities::readTextFile(
                               wmputilities::getJobJDLOriginalPath(jid));
      break;
   case WMS_JDL_REGISTERED:
      getJDL_response.jdl = wmputilities::readTextFile(
                               wmputilities::getJobJDLExistingStartPath(jid));
      break;
   default:
      break;
   }
   edglog(debug) << "retrieved JDL: " << getJDL_response.jdl << endl;
   GLITE_STACK_CATCH();
}

void
getMaxInputSandboxSize(getMaxInputSandboxSizeResponse
                       &getMaxInputSandboxSize_response)
{
   GLITE_STACK_TRY("getMaxInputSandboxSize()");
   edglog_fn("wmpoperations::getMaxInputSandboxSize");

   initWMProxyOperation("getMaxInputSandboxSize");
   security::do_authZ("getMaxInputSandboxSize");
   try {
      getMaxInputSandboxSize_response.size =
         // WARNING: Temporal cast TBD
         // WARNING: double temporarely casted into long (soon long will be returned directly
         // currently parameter is a double - casting needed
         (long)conf.wmp_config->max_input_sandbox_size();
   } catch (exception& ex) {
      edglog(severe)<<"Unable to get max input sandbox size: "
                    <<ex.what()<<endl;
      throw JobOperationException(__FILE__, __LINE__,
                                  "getMaxInputSandboxSize()", wmputilities::WMS_CONFIGURATION_ERROR,
                                  "Unable to get max input sandbox size");
   }

   GLITE_STACK_CATCH();
}

void
getSandboxDestURI(getSandboxDestURIResponse& getSandboxDestURI_response,
                  const string& jid, const string& protocol)
{
   GLITE_STACK_TRY("getSandboxDestURI()");
   edglog_fn("wmpoperations::getSandboxDestURI");

   edglog(debug) << "getSandboxDestURI requested for job: " << jid << endl;
   initWMProxyOperation("getSandboxDestURI");
   security::do_authZ_jobid("getSandboxDestURI", jid);
   getSandboxDestURI_response.path = new StringList;
   getSandboxDestURI_response.path->Item = new vector<string>(0);
   JobId jobid(jid);
   vector<string> *jobdiruris = getJobDirectoryURIsVector(conf.getProtocols(),
                                conf.getDefaultProtocol(), conf.getDefaultPort(), conf.getHTTPSPort(),
                                jid, protocol, "input");

   getSandboxDestURI_response.path->Item = jobdiruris;

   GLITE_STACK_CATCH();
}

void
getSandboxBulkDestURI(getSandboxBulkDestURIResponse& getSandboxBulkDestURI_response,
                      const string& jid, const string& protocol)
{
   GLITE_STACK_TRY("getSandboxBulkDestURI()");
   edglog_fn("wmpoperations::getSandboxBulkDestURI");

   edglog(debug) << "getSandboxBulkDestURI requested for job: " << jid << endl;
   initWMProxyOperation("getSandboxBulkDestURI");
   security::do_authZ_jobid("getSandboxBulkDestURI", jid);
   // Initializing logger
   WMPEventLogger wmplogger(wmputilities::getEndpoint());
   wmplogger.setLBProxy(conf.isLBProxyAvailable(), wmputilities::getDN_SSL());
   JobId jobid(jid);
   wmplogger.setUserProxy(wmputilities::getJobDelegatedProxyPath(jobid));
   wmplogger.init_and_set_logging_job("", 0, &jobid);

   DestURIsStructType *destURIsStruct = new DestURIsStructType();
   destURIsStruct->Item = new vector<DestURIStructType*>(0);

   // Getting job status to get children number
   JobStatus status = wmplogger.getStatus(true);
   vector<string> jids = status.getValStringList(JobStatus::CHILDREN);
   edglog(debug)<<"Children count: "<<status.getValInt(JobStatus::CHILDREN_NUM)<<endl;
   vector<string>::iterator jidsiterator = jids.begin();
   jids.insert(jidsiterator, 1, jid);

   vector<string>::iterator iter = jids.begin();
   vector<string>::iterator const end = jids.end();
   for (; iter != end; ++iter) {
      vector<string> *uris = getJobDirectoryURIsVector(conf.getProtocols(),
                             conf.getDefaultProtocol(), conf.getDefaultPort(), conf.getHTTPSPort(),
                             *iter, protocol, "input");

      DestURIStructType *destURIStruct = new DestURIStructType();
      destURIStruct->id = *iter;
      destURIStruct->destURIs = uris;
      destURIsStruct->Item->push_back(destURIStruct);
   }
   getSandboxBulkDestURI_response.destURIsStruct = destURIsStruct;

   GLITE_STACK_CATCH();
}

void
getQuota(getQuotaResponse& getQuota_response)
{
   GLITE_STACK_TRY("getQuota()");
   edglog_fn("wmpoperations::getQuota");

   initWMProxyOperation("getQuota");
   std::string user_name = security::do_authZ("getQuota"); // throws
   pair<long, long> quotas;
   if (!wmputilities::getUserQuota(quotas, user_name)) {
      edglog(severe)<<"Unable to get total quota"<<endl;
      throw FileSystemException(__FILE__, __LINE__,
                                "getQuota()", wmputilities::WMS_IS_FAILURE,
                                "Unable to get total quota");
   }
   getQuota_response.softLimit = quotas.first;
   getQuota_response.hardLimit = quotas.second;

   GLITE_STACK_CATCH();
}

void
getFreeQuota(getFreeQuotaResponse& getFreeQuota_response)
{
   GLITE_STACK_TRY("getFreeQuota()");
   edglog_fn("wmpoperations::getFreeQuota");

   initWMProxyOperation("getFreeQuota");
   std::string user_name = security::do_authZ("getFreeQuota"); //throws
   pair<long, long> quotas;
   if (!wmputilities::getUserFreeQuota(quotas, user_name)) {
      edglog(severe)<<"Unable to get free quota"<<endl;
      throw FileSystemException(__FILE__, __LINE__, "getFreeQuota()",
                                wmputilities::WMS_IS_FAILURE, "Unable to get free quota");
   }
   getFreeQuota_response.softLimit = quotas.first;
   getFreeQuota_response.hardLimit = quotas.second;

   GLITE_STACK_CATCH();
}

void
getOutputFileList(getOutputFileListResponse& getOutputFileList_response,
                  const string& jid, const string& protocol)
{
   int fd = -1;
   try {
      edglog_fn("wmpoperations::getOutputFileList");
      edglog(debug) << "getOutputFileList requested for job: " << jid << endl;
      initWMProxyOperation("getOutputFileList");
      security::do_authZ_jobid("getOutputFileList", jid);

      // File descriptor for operation lock system:
      // During jobPurge operation, a check to getOutputFileList lock file is done
      // If a getOutputFileList is requested, jobPurge operation is aborted
      JobId jobid(jid);
      fd = wmputilities::operationLock(
              wmputilities::getGetOutputFileListLockFilePath(jobid),
              "getOutputFileList");

      string jobdirectory = wmputilities::getJobDirectoryPath(jobid);

      // Checking for maradona file, created if and only if the job is in DONE state
      edglog(debug)<<"Searching for MARADONA file: "<<jobdirectory + FILE_SEPARATOR
                   + MARADONA_FILE<<endl;
      if (!wmputilities::fileExists(jobdirectory + FILE_SEPARATOR + MARADONA_FILE)) {
         // MARADONA file NOT found
         // Initializing logger
         WMPEventLogger wmplogger(wmputilities::getEndpoint());
         wmplogger.setLBProxy(conf.isLBProxyAvailable(), wmputilities::getDN_SSL());
         wmplogger.init_and_set_logging_job("", 0, &jobid);

         string userProxyJob =  wmputilities::getJobDelegatedProxyPath(jobid);
         long timeleft = security::getProxyTimeLeft(userProxyJob);

         // Setting user proxy, checks whether the user proxy is still valid
         // switching to host proxy if the user proxy expired
         if ( timeleft <= 1) {
            string hostProxy = configuration::Configuration::instance()->common()->host_proxy_file();
            wmplogger.setUserProxy(hostProxy);
         } else {
            wmplogger.setUserProxy(userProxyJob);
         }

         // Getting job status to check if is a job and its status
         JobStatus status = wmplogger.getStatus(false);
         if (status.getValInt(JobStatus::CHILDREN_NUM) != 0) {
            string msg = "getOutputFileList operation not allowed for dag or collection type";
            edglog(error)<<msg<<": "<<jobid.toString()<<endl;
            throw JobOperationException(__FILE__, __LINE__, "getOutputFileList()",
                                        wmputilities::WMS_OPERATION_NOT_ALLOWED, msg);
         }
         // getOutputFileList allowed when:
         // STATUS is eithr Done (yet done_code!= FAILED) or Aborted
         if (((status.status == JobStatus::DONE)
               &&(status.getValInt(JobStatus::DONE_CODE) == JobStatus::DONE_CODE_FAILED))
               || ((status.status != JobStatus::DONE) && (status.status != JobStatus::ABORTED))) {
            edglog(error)<<
                         "Job current status doesn't allow getOutputFileList operation"<<endl;
            throw JobOperationException(__FILE__, __LINE__,
                                        "getOutputFileList()", wmputilities::WMS_OPERATION_NOT_ALLOWED,
                                        "Job current status doesn't allow getOutputFileList operation");
         }
      }  // else ...... If MARADONA file is found, output files are definitely present on WMS
      string outputpath = wmputilities::getOutputSBDirectoryPath(jid);
      vector<string> *jobdiruris = getJobDirectoryURIsVector(conf.getProtocols(),
                                   conf.getDefaultProtocol(), conf.getDefaultPort(), conf.getHTTPSPort(),
                                   jid, protocol, "output");
      unsigned int jobdirsize = jobdiruris->size();

      // Searching files inside directory
      const boost::filesystem::path p(outputpath, boost::filesystem::native);
      std::vector<std::string> found;
      glite::wms::wmproxy::commands::list_files(p, found);
      edglog(debug)<<"List size is (hidden files included): "
                   <<found.size()<<endl;

      // Creating the list
      StringAndLongList *list = new StringAndLongList();
      vector<StringAndLongType*> *file = new vector<StringAndLongType*>;
      StringAndLongType *item = NULL;
      string filename;
      long filesize;

      vector<string>::iterator iter = found.begin();
      vector<string>::iterator const end = found.end();
      for (; iter != end; ++iter) {
         // Checking for hidden files
         filename = wmputilities::getFileName(string(*iter));
         if ((filename != MARADONA_FILE) && (filename
                                             != security::GaclManager::WMPGACL_DEFAULT_FILE)) {
            filesize = wmputilities::computeFileSize(*iter);
            edglog(debug)<<"Inserting file name: " <<filename<<endl;
            edglog(debug)<<"Inserting file size: " <<filesize<<endl;
            for (unsigned int i = 0; i < jobdirsize; i++) {
               item = new StringAndLongType();
               item->name = (*jobdiruris)[i] + FILE_SEPARATOR + filename;
               item->size = filesize;
               file->push_back(item);
            }
         }
      }
      list->file = file;
      getOutputFileList_response.OutputFileAndSizeList = list;
      edglog(debug)<<"Successfully retrieved files: "<<found.size()<<endl;
      edglog(debug)<<"Removing lock..."<<std::endl;
      wmputilities::operationUnlock(fd);
   } catch (glite::wmsutils::exception::Exception const& e) {
      edglog(debug)<<"Removing lock..."<<std::endl;
      wmputilities::operationUnlock(fd);
      edglog(debug) << "wmsutils::Exception: " << e.what() << std::endl;
      throw e;
   } catch (exception& ex) {
      edglog(debug)<<"Removing lock..."<<std::endl;
      wmputilities::operationUnlock(fd);
      edglog(debug)<<"std::exception: "<<ex.what()<<std::endl;
      throw ex;
   }
}

void
getJobTemplate(getJobTemplateResponse& getJobTemplate_response,
               JobTypeList jobType, const string& executable,
               const string& arguments, const string& requirements, const string& rank)
{
   GLITE_STACK_TRY("getJobTemplate()");
   edglog_fn("wmpoperations::getJobTemplate");

   initWMProxyOperation("getJobTemplate");
   security::do_authZ("getJobTemplate");

   getJobTemplate_response.jdl =
      (AdConverter::createJobTemplate(convertJobTypeListToInt(jobType),
                                      executable, arguments, requirements, rank))->toString();

   edglog(debug)<<"Job Template successful"<<endl;

   GLITE_STACK_CATCH();
}

void
getDAGTemplate(getDAGTemplateResponse& getDAGTemplate_response,
               GraphStructType dependencies, const string& requirements,
               const string& rank)
{
   GLITE_STACK_TRY("getDAGTemplate()");
   edglog_fn("wmpoperations::getDAGTemplate");

   initWMProxyOperation("getDAGTemplate");
   security::do_authZ("getDAGTemplate");

   getDAGTemplate_response.jdl = AdConverter::createDAGTemplate(
                                    convertGraphStructTypeToNodeStruct(dependencies),
                                    requirements, rank)->toString();

   edglog(debug)<<"DAG Template successful"<<endl;

   GLITE_STACK_CATCH();
}

void
getCollectionTemplate(getCollectionTemplateResponse
                      &getCollectionTemplate_response, int jobNumber,
                      const string& requirements, const string& rank)
{
   GLITE_STACK_TRY("getCollectionTemplate()");
   edglog_fn("wmpoperations::getCollectionTemplate");

   initWMProxyOperation("getCollectionTemplate");
   security::do_authZ("getCollectionTemplate");
   getCollectionTemplate_response.jdl =
      AdConverter::createCollectionTemplate(jobNumber, requirements,
            rank)->toString();

   edglog(debug)<<"Collection Template successful"<<endl;

   GLITE_STACK_CATCH();
}

void
getIntParametricJobTemplate(getIntParametricJobTemplateResponse
                            &getIntParametricJobTemplate_response, StringList *attributes, int param,
                            int parameterStart, int parameterStep, const string& requirements,
                            const string& rank)
{
   GLITE_STACK_TRY("getIntParametricJobTemplate()");
   edglog_fn("wmpoperations::getIntParametricJobTemplate");

   initWMProxyOperation("getIntParametricJobTemplate");
   security::do_authZ("getIntParametricJobTemplate");

   getIntParametricJobTemplate_response.jdl =
      AdConverter::createIntParametricTemplate(*(attributes->Item), param,
            parameterStart, parameterStep, requirements, rank)->toString();

   edglog(debug)<<"Int Parametric Job Template successful"<<endl;

   GLITE_STACK_CATCH();
}

void
getStringParametricJobTemplate(getStringParametricJobTemplateResponse
                               &getStringParametricJobTemplate_response, StringList *attributes,
                               StringList *param, const string& requirements, const string& rank)
{
   GLITE_STACK_TRY("getStringParametricJobTemplate()");
   edglog_fn("wmpoperations::getStringParametricJobTemplate");

   initWMProxyOperation("getStringParametricJobTemplate");
   security::do_authZ("getStringParametricJobTemplate");
   getStringParametricJobTemplate_response.jdl =
      AdConverter::createStringParametricTemplate(*(attributes->Item),
            *(param->Item), requirements, rank)->toString();

   edglog(debug)<<"String Parametric Job Template successful"<<endl;

   GLITE_STACK_CATCH();
}

// DELEGATION OPERATION DESCRIPTION

void
getDelegationVersion(getVersionResponse& getVersion_response)
{
   GLITE_STACK_TRY("getDelegationVersion()");
   edglog_fn("wmpoperations::getDelegationVersion");

   initWMProxyOperation("getDelegationVersion");
   security::do_authZ("getDelegationVersion");
   getVersion_response.version = security::getDelegationVersion();
   edglog(info)<<"Version retrieved: "<<getVersion_response.version<<endl;

   GLITE_STACK_CATCH();
}

void
getDelegationIntefaceVersion(getVersionResponse& getVersion_response)
{
   GLITE_STACK_TRY("getDelegationInterfaceVersion()");
   edglog_fn("wmpoperations::getDelegationInterfaceVersion");

   initWMProxyOperation("getDelegationInterfaceVersion");
   security::do_authZ("getDelegationVersion");
   getVersion_response.version = security::getDelegationInterfaceVersion();
   edglog(info)<<"Version retrieved: "<<getVersion_response.version<<endl;

   GLITE_STACK_CATCH();
}

void
getProxyReq(getProxyReqResponse& getProxyReq_response,
            const string& delegation_id)
{
   GLITE_STACK_TRY("getProxyReq()");
   edglog_fn("wmpoperations::getProxyReq");

   initWMProxyOperation("getProxyReq");
   security::do_authZ("getProxyReq"); // the delegated proxy is not there

#ifndef GRST_VERSION
   if (delegation_id == "") {
      edglog(error)<<"Empty delegation id not allowed with delegation 1"<<endl;
      throw ProxyOperationException(__FILE__, __LINE__,
                                    "getProxyReq()", wmputilities::WMS_INVALID_ARGUMENT,
                                    "Provided delegation id not valid");
   }
#endif

   getProxyReq_response.request =
      security::getProxyRequest(delegation_id);
   edglog(debug)<<"Proxy request successful"<<endl;

   GLITE_STACK_CATCH();
}

void
putProxy(putProxyResponse& putProxyReq_response, const string& delegation_id,
         const string& proxy)
{
   GLITE_STACK_TRY("putProxy()");
   edglog_fn("wmpoperations::putProxy");

   initWMProxyOperation("putProxy");
   security::do_authZ("putProxy"); // the delegated proxy is not there

   security::putProxy(delegation_id, proxy);
   edglog(debug)<<"Proxy put successful"<<endl;

   GLITE_STACK_CATCH();
}

void
renewProxyReq(string& renewProxyReq_response,
              const string& delegation_id)
{
   GLITE_STACK_TRY("renewProxyReq()");
   edglog_fn("wmpoperations::renewProxyReq");

   initWMProxyOperation("renewProxyReq");
   security::do_authZ("renewProxyReq", delegation_id);

   renewProxyReq_response = security::renewProxyRequest(delegation_id);
   edglog(debug)<<"Proxy renewed successful"<<endl;

   GLITE_STACK_CATCH();
}

void
getNewProxyReq(pair<string, string> &retpair)
{
   GLITE_STACK_TRY("getNewProxyReq()");
   edglog_fn("wmpoperations::getNewProxyReq");

   initWMProxyOperation("getNewProxyReq");
   security::do_authZ("getNewProxyReq");

   edglog(debug)<<"found delegationID: "<<retpair.first<<endl;
   edglog(debug)<<"found proxyRequest "<<retpair.second<<endl;
   edglog(debug)<<"Proxy requested successful"<<endl;

   GLITE_STACK_CATCH();
}

void
destroyProxy(const string& delegation_id)
{
   GLITE_STACK_TRY("destroyProxy()");
   edglog_fn("wmpoperations::destroyProxy");
   initWMProxyOperation("destroyProxy");

   security::do_authZ("destroyProxy", delegation_id);

   edglog(debug)<<"destroyProxy successful"<<endl;

   GLITE_STACK_CATCH();
}


void
getProxyTerminationTime(time_t
                        &getProxyTerminationTime_response, const string& delegation_id)
{
   GLITE_STACK_TRY("getProxyTerminationTime()");
   edglog_fn("wmpoperations::getProxyTerminationTime");

   initWMProxyOperation("getProxyTerminationTime");
   security::do_authZ("getProxyTerminationTime", delegation_id);
   getProxyTerminationTime_response = security::getTerminationTime(delegation_id);
   edglog(debug)<<"getProxyTerminationTime successful"<<endl;

   GLITE_STACK_CATCH();
}

void
getACLItems(getACLItemsResponse& getACLItems_response, const string& job_id)
{
   GLITE_STACK_TRY("getACLItems()");
   edglog_fn("wmpoperations::getACLItems");

   edglog(debug)<<"Operation requested for job: "<<job_id<<endl;
   initWMProxyOperation("getACLItems");
   security::do_authZ_jobid("getACLItems", job_id);
   JobId jid(job_id);
   string jobpath = wmputilities::getJobDirectoryPath(jid);
   edglog(debug)<<"GACL File: "<<jobpath + FILE_SEPARATOR
                + security::GaclManager::WMPGACL_DEFAULT_FILE<<endl;
   security::GaclManager gaclmanager(jobpath + FILE_SEPARATOR
                                     + security::GaclManager::WMPGACL_DEFAULT_FILE);
   getACLItems_response =
      gaclmanager.getItems(security::GaclManager::WMPGACL_PERSON_TYPE);
   edglog(debug) << "getACLItems successful" << endl;

   GLITE_STACK_CATCH();
}

void
addACLItems(addACLItemsResponse& addACLItems_response, const string& job_id,
            StringList * dnlist)
{
   GLITE_STACK_TRY("addACLItems()");
   edglog_fn("wmpoperations::addACLItems");

   edglog(debug)<<"Operation requested for job: "<<job_id<<endl;
   initWMProxyOperation("addACLItems");
   security::do_authZ_jobid("addACLItems", job_id);

   JobId jid(job_id);
   string jobpath = wmputilities::getJobDirectoryPath(jid);

   edglog(debug)<<"GACL File: "<<jobpath + FILE_SEPARATOR
                + security::GaclManager::WMPGACL_DEFAULT_FILE<<endl;
   security::GaclManager gaclmanager(jobpath + FILE_SEPARATOR
                                     + security::GaclManager::WMPGACL_DEFAULT_FILE);

   vector<pair<security::GaclManager::WMPgaclCredType, string> > gaclvect;
   pair<security::GaclManager::WMPgaclCredType, string> gaclpair;
   for (unsigned int i = 0; i < dnlist->Item->size(); i++) {
      gaclpair.first = security::GaclManager::WMPGACL_PERSON_TYPE;
      edglog(debug)<<"Item to add: "<<(*(dnlist->Item))[i]<<endl;
      gaclpair.second = (*(dnlist->Item))[i];
      gaclvect.push_back(gaclpair);
   }
   gaclmanager.addEntries(gaclvect);
   gaclmanager.saveGacl();

   edglog(debug)<<"addACLItems successful"<<endl;

   GLITE_STACK_CATCH();
}

void
removeACLItem(removeACLItemResponse& removeACLItem_response,
              const string& job_id, const string& item)
{
   GLITE_STACK_TRY("removeACLItem()");
   edglog_fn("wmpoperations::removeACLItem");

   edglog(debug)<<"Operation requested for job: "<<job_id<<endl;
   initWMProxyOperation("removeACLItem");
   security::do_authZ_jobid("removeACLItem", job_id);

   string errors = "";
   JobId jid (job_id);
   // TBD change test in: item == owner
   if (item == wmputilities::getDN_SSL()) {
      edglog(error)<<"Removal of the item representing user that has "
                   "registered the job is not allowed"<<endl;
      throw JobOperationException(__FILE__, __LINE__,
                                  "removeACLItem()", wmputilities::WMS_OPERATION_NOT_ALLOWED,
                                  "Removal of the item representing user that has registered the "
                                  "job is not allowed");
   }
   string jobpath = wmputilities::getJobDirectoryPath(jid);

   edglog(debug)<<"GACL File: "<<jobpath + FILE_SEPARATOR
                + security::GaclManager::WMPGACL_DEFAULT_FILE<<endl;
   security::GaclManager gaclmanager(jobpath + FILE_SEPARATOR
                                     + security::GaclManager::WMPGACL_DEFAULT_FILE);
   gaclmanager.removeEntry(security::GaclManager::WMPGACL_PERSON_TYPE,
                           item, errors);

   if (errors.size() > 0) {
      edglog(error)<<"Removal of the gacl item failed: " << errors << "\n";
      throw AuthorizationException(__FILE__, __LINE__,
                                   "removeACLItem()", wmputilities::WMS_AUTHORIZATION_ERROR,
                                   "Removal of the gacl item failed:\n" + errors);
   }
   gaclmanager.saveGacl();

   edglog(debug)<<"removeACLItem successful"<<endl;

   GLITE_STACK_CATCH();
}

// getDelegatedProxyInfo and getJobProxyInfo
void
getProxyInfo(getProxyInfoResponse& getProxyInfo_response, const string& id,
             bool isjobid)
{
   GLITE_STACK_TRY("getProxyInfo()");
   edglog_fn("wmpoperations::getProxyInfo");

   initWMProxyOperation("getProxyInfo");
   if (isjobid) {
      security::do_authZ_jobid("getProxyInfo", id);
   } else {
      security::do_authZ("getProxyInfo", id);
   }

   string proxy;
   if (isjobid) {
      edglog(info)<<"Job Id: "<<id<<endl;
      // Checking delegation id
      if (id == "") {
         edglog(error)<<"Provided Job Id not valid"<<endl;
         throw JobOperationException(__FILE__, __LINE__,
                                     "getProxyInfo()", wmputilities::WMS_INVALID_ARGUMENT,
                                     "Provided Job Id not valid");
      }
      proxy = wmputilities::getJobDelegatedProxyPath(id);
      security::checkProxyExistence(proxy, id);
      edglog(debug)<<"Job proxy: "<<proxy<<endl;
   } else {
      edglog(info)<<"Delegation Id: "<<id<<endl;
      // Checking delegation id
      if (id == "") {
         edglog(error)<<"Provided Delegation Id not valid"<<endl;
         throw JobOperationException(__FILE__, __LINE__,
                                     "getProxyInfo()", wmputilities::WMS_INVALID_ARGUMENT,
                                     "Provided Delegation Id not valid");
      }
      proxy = security::getDelegatedProxyPath(id, wmputilities::getDN_SSL());
      edglog(debug)<<"Delegated Proxy: "<<proxy<<endl;
   }
   security::VOMSAuthN vomsproxy = security::VOMSAuthN(proxy);
   getProxyInfo_response.items = vomsproxy.getProxyInfo();

   edglog(debug)<<"getProxyInfo successful"<<endl;

   GLITE_STACK_CATCH();
}

void
checkPerusalFlag(JobId *jid, string& delegatedproxy, bool checkremotepeek)
{
   GLITE_STACK_TRY("checkPerusalFlag()");
   edglog_fn("wmpoperations::checkPerusalFlag");

   // not exposed: no security checks

   WMPEventLogger wmplogger(wmputilities::getEndpoint());
   wmplogger.setLBProxy(conf.isLBProxyAvailable(), wmputilities::getDN_SSL());
   wmplogger.setUserProxy(delegatedproxy);
   wmplogger.init_and_set_logging_job("", 0, jid);

   string jdlpath;
   jdlpath = wmputilities::getJobJDLExistingStartPath(*jid);

   edglog(debug)<<"jdlpath: "<<jdlpath<<endl;
   string type;
   boost::shared_ptr<JobAd> jad;
   string parent = wmplogger.retrieveRegJobEvent(jid->toString()).parent;
   if (!wmputilities::fileExists(jdlpath)) {
      // Checking if the job is a sub-node
      if (parent.empty()) {
         // JDL to start should be present, something went wrong
         throw JobOperationException(__FILE__, __LINE__,
                                     "checkPerusalFlag()", wmputilities::WMS_IS_FAILURE,
                                     "couldn't retrieve parent jobid for " + jid->toString());
      }

      // Job is a sub-node, getting JDL from parent JDL
      JobId parentjid(parent);
      jdlpath = wmputilities::getJobJDLExistingStartPath(parentjid);
      boost::shared_ptr<WMPExpDagAd> dag(new WMPExpDagAd(wmputilities::readTextFile(jdlpath)));
      boost::shared_ptr<NodeAd> jad(new NodeAd(dag->getNode(*jid)));

      // Supported sub-nodes are standard job, setting type = job
      type = JDL_TYPE_JOB;

   } else {
      // Job is standard job or a compound job parent
      if (!parent.empty()) {
         JobId parentjid(parent);
         jdlpath = wmputilities::getJobJDLExistingStartPath(parentjid);
      }
      boost::shared_ptr<Ad> ad(new Ad(wmputilities::readTextFile(jdlpath)));
      if (ad->hasAttribute(JDL::TYPE)) {
         type = glite_wms_jdl_toLower(ad->getString(JDL::TYPE));
      } else {
         type = JDL_TYPE_JOB;
      }
      if (type == JDL_TYPE_JOB) {
         jad.reset(new JobAd(*(ad->ad())));
      }
   }
   edglog(debug) <<"Type: "<<type<<endl;
   if (type == JDL_TYPE_JOB) {
      jad->setLocalAccess(false);

      if (jad->hasAttribute(JDL::PU_FILE_ENABLE)) {
         if (!jad->getBool(JDL::PU_FILE_ENABLE)) {
            edglog(debug)<<"Perusal not enabled for this job"<<endl;
            throw JobOperationException(__FILE__, __LINE__,
                                        "checkPerusalFlag()",
                                        wmputilities::WMS_OPERATION_NOT_ALLOWED,
                                        "Perusal not enabled for this job");
         }
      } else {
         edglog(debug)<<"Perusal not enabled for this job"<<endl;
         throw JobOperationException(__FILE__, __LINE__,
                                     "checkPerusalFlag()",
                                     wmputilities::WMS_OPERATION_NOT_ALLOWED,
                                     "Perusal not enabled for this job");
      }

      // Checking for attribute JDL::PU_FILES_DEST_URI value
      if (checkremotepeek) {
         if (jad->hasAttribute(JDL::PU_FILES_DEST_URI)) {
            string protocol = conf.getDefaultProtocol();
            string port = (conf.getDefaultPort() != 0)
                          ? (":" + boost::lexical_cast<std::string>(
                                conf.getDefaultPort()))
                          : "";
            string serverhost = getServerHost();
            string uri = jad->getString(JDL::PU_FILES_DEST_URI);

            string tofind = "://" + serverhost;
            std::string::size_type pos = uri.find(tofind);
            if (pos == string::npos) {
               edglog(debug)<<"Remote perusal peek URI set:\n"
                            <<uri<<endl;
               throw JobOperationException(__FILE__, __LINE__,
                                           "checkPerusalFlag()",
                                           wmputilities::WMS_OPERATION_NOT_ALLOWED,
                                           "Remote perusal peek URI set:\n" + uri);
            }
            string uriprotocol = uri.substr(0, pos);
            edglog(debug)<<"URI protocol: "<<uriprotocol<<endl;
            string uripath = uri.substr(pos + tofind.length(),
                                        uri.length() - 1);
            edglog(debug)<<"URI path: "<<uripath<<endl;
            pos = uripath.find("/");
            // Looking for port
            if (pos == string::npos) {
               edglog(debug)<<"Remote perusal peek URI set:\n"
                            <<uri<<endl;
               throw JobOperationException(__FILE__, __LINE__,
                                           "checkPerusalFlag()",
                                           wmputilities::WMS_OPERATION_NOT_ALLOWED,
                                           "Remote perusal peek URI set:\n" + uri);
            }
            uripath = uripath.substr(pos, uripath.length() - 1);
            edglog(debug)<<"URI path: "<<uripath<<endl;

            string serverpath;
            if ((uriprotocol == "https") || (uriprotocol == "http")) {
               // No document root added
               serverpath = wmputilities::getPeekDirectoryPath(*jid, false);
            } else {
               serverpath = wmputilities::getPeekDirectoryPath(*jid);
            }
            edglog(debug)<<"Server path: "<<serverpath<<endl;
            if (uripath != serverpath) {
               edglog(debug)<<"Remote perusal peek URI set:\n"
                            <<uri<<endl;
               throw JobOperationException(__FILE__, __LINE__,
                                           "checkPerusalFlag()",
                                           wmputilities::WMS_OPERATION_NOT_ALLOWED,
                                           "Remote perusal peek URI refers to server host, but "
                                           "the path is not manged by WMProxy:\n" + uri);
            }
         }
      }
   } else {
      edglog(debug)<<"Perusal service not available for dag or collection "
                   "type"<<endl;
      throw JobOperationException(__FILE__, __LINE__,
                                  "checkPerusalFlag()", wmputilities::WMS_OPERATION_NOT_ALLOWED,
                                  "Perusal service not available for dag or collection type");
   }

   GLITE_STACK_CATCH();
}

void
enableFilePerusal(enableFilePerusalResponse& enableFilePerusal_response,
                  const string& job_id, StringList * fileList)
{
   GLITE_STACK_TRY("enableFilePerusal()");
   edglog_fn("wmpoperations::enableFilePerusal");

   edglog(debug)<<"Operation requested for job: "<<job_id<<endl;
   initWMProxyOperation("enableFilePerusal");
   security::do_authZ_jobid("enableFilePerusal", job_id);

   JobId jid (job_id);
   // TODO delegation Id may be returned somehow when checking security...
   string delegatedproxy = wmputilities::getJobDelegatedProxyPath(jid);
   checkPerusalFlag(&jid, delegatedproxy, false);

   string filename = wmputilities::getPeekDirectoryPath(jid) + FILE_SEPARATOR
                     + PERUSAL_FILE_2_PEEK_NAME;

   // getting maxPerusalFiles attribute from configuration file
   unsigned int maxPerusalFiles = conf.getMaxPerusalFiles();
   unsigned int size = fileList->Item->size();

   if ( size <= maxPerusalFiles ) {
      if (size != 0) {
         fstream file2peek(filename.c_str(), ios::out);
         for (unsigned int i = 0; i < size; i++) {
            file2peek << (*(fileList->Item))[i]+"\n";
         }
         file2peek.close();
      } else {
         // Removing file2peek file if needed
         if (wmputilities::fileExists(filename)) {
            remove(filename.c_str());
         }
      }
      edglog(debug)<<"enableFilePerusal successful"<<endl;
   } else {
      throw JobOperationException(__FILE__, __LINE__, "wmpoperations::enableFilePerusal()",
                                  wmputilities::WMS_OPERATION_NOT_ALLOWED, "The maximum number of perusal files is reached");

   }

   GLITE_STACK_CATCH();
}

void
getPerusalFiles(getPerusalFilesResponse& getPerusalFiles_response,
                const string& job_id, const string& fileName, bool allChunks,
                const string& protocol)
{
   GLITE_STACK_TRY("getPerusalFiles()");
   edglog_fn("wmpoperations::getPerusalFiles");

   edglog(debug)<<"Operation requested for job: "<<job_id<<endl;
   initWMProxyOperation("getPerusalFiles");
   security::do_authZ_jobid("getPerusalFiles", job_id);

   JobId jid(job_id);

   if (fileName == "") {
      throw JobOperationException(__FILE__, __LINE__,
                                  "wmpoperations::getPerusalFiles()", wmputilities::WMS_INVALID_ARGUMENT,
                                  "Provided file name not valid");
   }

   if (!wmputilities::fileExists(wmputilities::getJobJDLStartedPath(jid))) {
      edglog(error)<<"The job is not started"<<endl;
      throw JobOperationException(__FILE__, __LINE__,
                                  "getPerusalFiles()", wmputilities::WMS_OPERATION_NOT_ALLOWED,
                                  "the job is not started");
   }
   // TODO delegation Id may be returned somehow when checking security...
   string delegatedproxy= wmputilities::getJobDelegatedProxyPath(jid);
   checkPerusalFlag(&jid, delegatedproxy, true);

   string peekdir = wmputilities::getPeekDirectoryPath(jid) + FILE_SEPARATOR;
   const boost::filesystem::path p(peekdir, boost::filesystem::native);
   vector<string> found;
   glite::wms::wmproxy::commands::list_files(p, found);

   vector<string> *jobdiruris = getJobDirectoryURIsVector(conf.getProtocols(),
                                conf.getDefaultProtocol(), conf.getDefaultPort(), conf.getHTTPSPort(),
                                job_id, protocol, "peek/");

   unsigned int jobdirsize = jobdiruris->size();

   vector<string> good;
   vector<string> returnvector;
   string currentfilename;

   vector<string>::iterator iter = found.begin();
   vector<string>::iterator const end = found.end();
   for (; iter != end; ++iter) {
      currentfilename = wmputilities::getFileName(*iter);
      if (currentfilename.find(fileName + ".") == 0) {
         edglog(debug)<<"Good perusal file: "<<*iter<<endl;
         good.push_back(*iter);
      }
      if (allChunks) {
         if (currentfilename.find(fileName + PERUSAL_DATE_INFO_SEPARATOR)
               == 0) {
            edglog(debug)<<"Good old global perusal file: "<<*iter<<endl;
            for (unsigned int i = 0; i < jobdirsize; i++) {
               returnvector.push_back((*jobdiruris)[i] + currentfilename);
            }
         }
      }
   }

   //unsigned int size = good.size();
   if (good.size()) {
      // Sorting vector
      sort(good.begin(), good.end());

      string tempfile = peekdir + TEMP_PERUSAL_FILE_NAME;

      long filesize = 0;
      long totalfilesize = 0;

      string startdate = good[0].substr(good[0].rfind(".") + 1,
                                        good[0].length() - 1);
      string enddate = startdate;

      fstream outfile(tempfile.c_str(), ios::out);
      if (!outfile.good()) {
         edglog(severe)<<tempfile<<": !outfile.good()"<<endl;
         throw FileSystemException(__FILE__, __LINE__,
                                   "getPerusalFiles()", WMS_FILE_SYSTEM_ERROR, "Unable to open "
                                   "perusal temporary file\n(please contact server administrator)");
      }

      string filetoreturn;
      vector<string>::iterator iter = good.begin();
      vector<string>::iterator const end = good.end();
      for (; iter != end; ++iter) {
         filesize = wmputilities::computeFileSize(*iter);
         filetoreturn = fileName
                        + PERUSAL_DATE_INFO_SEPARATOR + startdate
                        + PERUSAL_DATE_INFO_SEPARATOR + enddate;
         if ((totalfilesize + filesize) > FILE_TRANSFER_SIZE_LIMIT) {
            outfile.close();
            rename(tempfile.c_str(), (peekdir + filetoreturn).c_str());

            for (unsigned int i = 0; i < jobdirsize; i++) {
               returnvector.push_back((*jobdiruris)[i] + filetoreturn);
            }

            outfile.open(tempfile.c_str(), ios::out);
            if (!outfile.good()) {
               edglog(severe)<<tempfile<<": !outfile.good()"<<endl;
               throw FileSystemException(__FILE__, __LINE__,
                                         "getPerusalFiles()", WMS_FILE_SYSTEM_ERROR, "Unable to "
                                         "open perusal temporary file\n(please contact server "
                                         "administrator)");
            }
            totalfilesize = 0;
            enddate = (*iter).substr((*iter).rfind(".") + 1,
                                     (*iter).length() - 1);
            startdate = enddate;
         } else {
            enddate = (*iter).substr((*iter).rfind(".") + 1,
                                     (*iter).length() - 1);
         }
         ifstream infile((*iter).c_str());
         if (!infile.good()) {
            edglog(severe)<<*iter<<": !infile.good()"<<endl;
            throw FileSystemException(__FILE__, __LINE__,
                                      "getPerusalFiles()", WMS_FILE_SYSTEM_ERROR, "Unable to open "
                                      "perusal input file\n(please contact server administrator)");
         }
         outfile << infile.rdbuf();
         infile.close();
         remove((*iter).c_str());
         totalfilesize += filesize;
      }
      outfile.close();
      filetoreturn = fileName
                     + PERUSAL_DATE_INFO_SEPARATOR + startdate
                     + PERUSAL_DATE_INFO_SEPARATOR + enddate;
      rename(tempfile.c_str(), (peekdir + filetoreturn).c_str());

      for (unsigned int i = 0; i < jobdirsize; i++) {
         returnvector.push_back((*jobdiruris)[i] + filetoreturn);
      }
   }

   getPerusalFiles_response = returnvector;

   edglog(debug)<<"getPerusalFiles successful"<<endl;

   GLITE_STACK_CATCH();
}

void
getTransferProtocols(getTransferProtocolsResponse& getTransferProtocols_response)
{
   GLITE_STACK_TRY("getTransferProtocols()");
   edglog_fn("wmpoperations::getTransferProtocols");
   initWMProxyOperation("getTransferProtocols");
   security::do_authZ("getTransferProtocols");

   getTransferProtocols_response.protocols = new StringList;
   vector<string> *protocols = new vector<string>();

   vector<pair<string, int> > serverprotocols = conf.getProtocols();
   unsigned int size = serverprotocols.size();
   for (unsigned int i = 0; i < size; i++) {
      protocols->push_back(serverprotocols[i].first);
   }
   protocols->push_back("https");
   getTransferProtocols_response.protocols->Item = protocols;

   edglog(debug)<<"Transfer protocols retrieved successful"<<endl;
   GLITE_STACK_CATCH();
}


void
getJobStatusOp(getJobStatusResponse& getJobStatus_response, const string& job_id)
{
   GLITE_STACK_TRY("getJobStatus()");
   edglog_fn("wmpoperations::getJobStatus");

   initWMProxyOperation("getJobStatus");
   security::do_authZ("getJobStatusOp");

   // TODO status flag could be parametrized
   bool status_flag=true;
   // PERFORM Job actual Status request
   WMPEventLogger wmplogger(wmputilities::getEndpoint());
   JobStatus status = wmplogger.getStatus(status_flag);
   // fill in job Status structure:

   getJobStatus_response.jobStatus = new JobStatusStructType;
   getJobStatus_response.jobStatus->status = status.name();
   getJobStatus_response.jobStatus->jobid  = job_id;
   // TODO children?

   edglog(debug)<<"JobStatus retrieved successful"<<endl;
   GLITE_STACK_CATCH();
}
