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

//
// File: eventlogger.cpp
// Author: Giuseppe Avellino <egee@datamat.it>
//

#include "eventlogger.h"
#include "server/configuration.h"

// Boost
#include <boost/lexical_cast.hpp>

// Logging & Bookkeeping
#include "glite/lb/producer.h"
#include "glite/lb/Event.h"
#include "glite/lb/Job.h"
#include "glite/lb/context-int.h" // edg_wll_SetSequenceCode

// EDG log
#include "utilities/logging.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#include "utilities/wmpexception_codes.h"
#include "utilities/wmpexceptions.h"
#include "utilities/utils.h"

#include "glite/jdl/PrivateAttributes.h"
#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/jdl_attributes.h"
#include "glite/security/proxyrenewal/renewal.h"

#include "security/authorizer.h"

extern WMProxyConfiguration conf;

namespace glite {
namespace wms {
namespace wmproxy {
namespace eventlogger {

using namespace std;
using namespace glite::jdl; // DagAd
using namespace glite::jobid; //JobId
using namespace glite::wmsutils::exception; //Exception
using namespace glite::wms::wmproxy::utilities; //Exception codes

namespace jobid        = glite::jobid;
namespace logger       = glite::wms::common::logger;
namespace wmputilities = glite::wms::wmproxy::utilities;

// Environment variable for user defined log destination
const char * GLITE_WMS_LOG_DESTINATION = "GLITE_WMS_LOG_DESTINATION";

// LB Proxy default availability status
// BulkMM default availability status
const bool DEFAULT_BULK_MM = false;


// Environment variables for Proxy cert and key
const char * GLITE_HOST_KEY = "GLITE_HOST_KEY";
const char * GLITE_HOST_CERT = "GLITE_HOST_CERT";

// Port number for proxy renewal service
const int LB_RENEWAL_PORT = 7512;

// Number of log retries before giving up
const int LOG_RETRY_COUNT = 3;
const int QUERY_RETRY_COUNT = 3;

// Randomic value lower and upper limits for LB[Proxy] log retry on failure
const int LB_LOG_RETRY_LOWER_LIMIT = 30;
const int LB_LOG_RETRY_UPPER_LIMIT = 60;
const int LB_PROXY_LOG_RETRY_LOWER_LIMIT = 5;
const int LB_PROXY_LOG_RETRY_UPPER_LIMIT = 10;


WMPEventLogger::WMPEventLogger(const string& endpoint)
{
   edglog_fn("WMPEventLogger::WMPEventLogger");

   id_ = NULL;
   m_subjobs = NULL;
   lb_host = "";
   server = endpoint;
   delegatedproxy = "";
   m_lbProxy_b = conf.isLBProxyAvailable();
   m_bulkMM_b = DEFAULT_BULK_MM;

   if (edg_wll_InitContext(&ctx_)
         || (edg_wll_SetParam(ctx_, EDG_WLL_PARAM_SOURCE, EDG_WLL_SOURCE_WM_PROXY))
         || (endpoint.c_str()
             ? edg_wll_SetParamString(ctx_, EDG_WLL_PARAM_INSTANCE,
                                      endpoint.c_str())
             : false)) {
      string msg = error_message("LB initialization failed\n"
                                 "edg_wll_InitContext, edg_wll_SetParam[String]");
      throw LBException(__FILE__, __LINE__, "WMPEventLogger()",
                        WMS_IS_FAILURE, msg);
   }
};

WMPEventLogger::~WMPEventLogger() throw()
{
   edg_wll_FreeContext(ctx_);
}

void
WMPEventLogger::init_and_set_logging_job(
   const string& lb_host,
   int lb_port,
   jobid::JobId *id)
{
   GLITE_STACK_TRY("init()");
   edglog_fn("WMPEventlogger::init");

   id_ = id;
   setLoggingJob(id->toString());

   this->lb_host = lb_host;
   this->lb_port = lb_port;

   if (!getenv(GLITE_WMS_LOG_DESTINATION)) {
      if (!lb_host.empty()) {
         edglog(debug)<<"Setting LB log destination to: "<<lb_host<<endl;
         if (edg_wll_SetParamString(ctx_, EDG_WLL_PARAM_DESTINATION,
            lb_host.c_str())) {

            string msg = error_message("Setting parameter "
               "EDG_WLL_PARAM_DESTINATION failed");
            throw LBException(__FILE__, __LINE__, "init", WMS_IS_FAILURE, msg);
         }
         if (edg_wll_SetParam(ctx_, EDG_WLL_PARAM_DESTINATION_PORT, lb_port)) {
            string msg = error_message("Setting parameter "
               "EDG_WLL_PARAM_DESTINATION_PORT failed");
            throw LBException(__FILE__, __LINE__, "init", WMS_IS_FAILURE, msg);
         }
      }
   } else {
      edglog(debug)<<"GLITE_WMS_LOG_DESTINATION is set to: "
                   <<getenv(GLITE_WMS_LOG_DESTINATION)<<endl;
   }
   GLITE_STACK_CATCH();
}


// BULKMM
void WMPEventLogger::setBulkMM(bool value)
{
   GLITE_STACK_TRY("WMPEventLogger::setBulkMM(value)");
   edglog_fn("WMPEventLogger::setBulkMM");
   if (value) {
      edglog(debug)<<"Bulk MM Enabled" << endl;
   } else {
      edglog(debug)<<"Bluk MM Disabled" << endl;
   }
   m_bulkMM_b=value;
   GLITE_STACK_CATCH();

}
bool WMPEventLogger::getBulkMM()
{
   return m_bulkMM_b;
}

// LBProxy
void
WMPEventLogger::setLBProxy(bool value, std::string userdn)
{
   GLITE_STACK_TRY("setLBProxy()");
   edglog_fn("WMPEventlogger::setLBProxy");
   m_lbProxy_b = value;
   if (value) {
      edglog(debug)<<"Setting LBProxy to 'true'"<<endl;
      if (edg_wll_SetParam(ctx_, EDG_WLL_PARAM_LBPROXY_USER, userdn.c_str())) {
         edglog(critical)<<error_message("Parameter setting "
                                         "EDG_WLL_PARAM_LBPROXY_USER failed\nedg_wll_SetParam")<<endl;
      }
   } else {
      edglog(debug)<<"Setting LBProxy to 'false'"<<endl;
      if (edg_wll_SetParam(ctx_, EDG_WLL_PARAM_LBPROXY_USER, NULL)) {
         edglog(critical)<<error_message("Parameter setting "
                                         "EDG_WLL_PARAM_LBPROXY_USER failed\nedg_wll_SetParam")<<endl;
      }
   }

   GLITE_STACK_CATCH();
}

bool
WMPEventLogger::getLBProxy()
{
   return m_lbProxy_b;
}

char *
WMPEventLogger::getSequence()
{
   GLITE_STACK_TRY("getSequence()");
   return edg_wll_GetSequenceCode(ctx_);
   GLITE_STACK_CATCH();
};

void
WMPEventLogger::setSequenceCode(const string& seqcode)
{
   GLITE_STACK_TRY("setSequenceCode()");
   if (edg_wll_SetSequenceCode(ctx_, seqcode.c_str(), EDG_WLL_SEQ_NORMAL)) {
      string msg = error_message("Set sequence code failed\n"
                                 "edg_wll_SetSequenceCode");
      throw LBException(__FILE__, __LINE__, "setSequenceCode",
                        WMS_IS_FAILURE, msg);
   }
   GLITE_STACK_CATCH();
}

void
WMPEventLogger::incrementSequenceCode()
{
   GLITE_STACK_TRY("incrementSequenceCode()");
   if (edg_wll_IncSequenceCode(ctx_)) {
      string msg = error_message("Increment sequence code failed\n"
                                 "edg_wll_IncSequenceCode");
      throw LBException(__FILE__, __LINE__, "incrementSequenceCode",
                        WMS_IS_FAILURE, msg);
   }
   GLITE_STACK_CATCH();
}


/*********************
* LB  LOG/REGISTER METHODS
**********************/
char *
WMPEventLogger::registerProxyRenewal(const string& proxy_path,
                                     const string& my_proxy_server,
                                     glite::jobid::JobId const *id)
{
   GLITE_STACK_TRY("registerProxyRenewal()");
   edglog_fn("WMPEventLogger::registerProxyRenewal");

   edglog(debug)<<"Proxy path: "<<proxy_path<<endl;
   edglog(debug)<<"My Proxy Server: "<<my_proxy_server<<endl;

   char *renewal_proxy_path = NULL;
   int i = LOG_RETRY_COUNT;
   std::string jobid;
   if (id) {
      jobid = id->toString();
   } else {
      jobid = id_->toString();
   }

   int register_result=1;
   for (; (i > 0) && register_result; i--) {
      register_result = glite_renewal_RegisterProxy(
                           (char*)proxy_path.c_str(),
                           (char*)my_proxy_server.c_str(), LB_RENEWAL_PORT,
                           jobid.c_str(), EDG_WLPR_FLAG_UNIQUE,
                           &renewal_proxy_path);
      if (register_result) {
         edglog(severe)
               << error_message("Register job failed\nglite_renewal_RegisterProxy",register_result)
               << endl;
         randomsleep();
      }
   }

   if (i > 0) {
      // SUCCESS: calling edg_wll_SetParam (no sleep, local service)
      for (int j = 0;
            j < LOG_RETRY_COUNT
            && !edg_wll_SetParam(ctx_, EDG_WLL_PARAM_X509_PROXY,
                                 renewal_proxy_path);
            j++);
   } else {
      // glite_renewal_RegisterProxy failed
      for (int j = 0;
            j < LOG_RETRY_COUNT
            && !edg_wll_SetParam(ctx_, EDG_WLL_PARAM_X509_PROXY,
                                 proxy_path.c_str());
            j++);
      string msg = "Proxy renewal registration failed\n"
                   "glite_renewal_RegisterProxy, edg_wll_SetParam[Proxy]";
      throw LBException(__FILE__, __LINE__, "registerProxyRenewal()",
                        WMS_LOGGING_ERROR, msg);
   }
   return renewal_proxy_path;
   GLITE_STACK_CATCH();
}

void
WMPEventLogger::unregisterProxyRenewal()
{
   GLITE_STACK_TRY("unregisterProxyRenewal()");

   int i = LOG_RETRY_COUNT;
   for (; (i > 0)
         && glite_renewal_UnregisterProxy(id_->toString().c_str(), NULL);
         i--);
   if (i == 0) {
      // glite_renewal_UnregisterProxy FAILED
      edglog(error)
            <<error_message("Proxy renewal unregiser failed\nglite_renewal_UnregisterProxy")
            <<endl;
   }

   GLITE_STACK_CATCH();
}

bool
WMPEventLogger::registerJob(JobAd *jad, glite::jobid::JobId const* const jid, const string& path)
{
   GLITE_STACK_TRY("registerJob()");
   edglog_fn("WMPEventlogger::registerJob");

   char str_addr[1024];
   sprintf(str_addr, "%s", server.c_str());

   int register_result = 1;
   int i = LOG_RETRY_COUNT;
   if (m_lbProxy_b) {
      edglog(debug)<<"Registering job to LB Proxy..."<<endl;
      for (; (i > 0) && register_result; i--) {
         register_result = edg_wll_RegisterJobProxy(ctx_, jid->c_jobid(),
            EDG_WLL_JOB_SIMPLE, path.c_str(), str_addr, 0, NULL, NULL);
         if (register_result) {
            char *et, *ed;
            edg_wll_Error(ctx_, &et, &ed);
            edglog(severe) << "edg_wll_RegisterJobProxy returned: " << et <<
              " (" << ed << ')' << endl;
            randomsleep();
         }
      }
   } else {
      edglog(debug)<<"Registering job to LB..."<<endl;
      for (; (i > 0) && register_result; i--) {
         register_result = edg_wll_RegisterJobSync(ctx_, jid->c_jobid(),
            EDG_WLL_JOB_SIMPLE, path.c_str(), str_addr, 0, NULL, NULL);
         if (register_result) {
            char *et, *ed;
            edg_wll_Error(ctx_, &et, &ed);
            edglog(severe) << "edg_wll_RegisterJobProxy returned: " << et <<
              " (" << ed << ')' << endl;
            randomsleep();
         }
      }
   }
   if (register_result) {
      return false;
   } else {
      if (jad->hasAttribute(JDL::USERTAGS)) {
         logUserTags((classad::ClassAd*) jad->delAttribute(JDL::USERTAGS));
      }
      return true;
   }

   GLITE_STACK_CATCH();
}

void
WMPEventLogger::registerSubJobs(WMPExpDagAd *ad, edg_wlc_JobId *subjobs)
{
   GLITE_STACK_TRY("registerSubJobs()");
   edglog_fn("WMPEventlogger::registerSubJobs");

   // Prepare both jdls and subjobs for registering
   vector<string> jobids;
   vector<string> jdls = ad->getSubmissionStrings(&jobids);
   unsigned int jdlssize = jdls.size();
   if (jdlssize != jobids.size()) {
      string msg = "Number of nodes do not correspond to number of "
                   "inserted jobids";
      throw JobOperationException(__FILE__, __LINE__, "registerSubJobs()",
                                  WMS_IS_FAILURE, msg);
   }

   char **jdls_char = (char**) calloc(jdlssize + 1, sizeof(char*)); // same size for both arrays
   edg_wlc_JobId* jids_id = (edg_wlc_JobId*)calloc(jdlssize + 1, sizeof(edg_wlc_JobId));

   vector<string>::const_iterator iter = jdls.begin();
   vector<string>::const_iterator const end = jdls.end();
   vector<string>::iterator iter_jobid = jobids.begin();
   for (unsigned int i = 0; iter != end; ++iter, ++iter_jobid, ++i) {
      jdls_char[i] = (char*) malloc(sizeof(char) * (int(iter->size()) + 1));
      strcpy(jdls_char[i], iter->c_str());
      glite_jobid_parse(iter_jobid->c_str(), &jids_id[i]);
   }

   int i = LOG_RETRY_COUNT;
   bool register_success = false;
   if (m_lbProxy_b) {
      edglog(debug)<<"Registering DAG subjobs to LB Proxy..."<<endl;
      for (; i > 0; i--) {
         if(edg_wll_RegisterSubjobsProxy(ctx_, id_->c_jobid(), (const char **)jdls_char, server.c_str(), jids_id)) {
            char *et, *ed;
            edg_wll_Error(ctx_,&et,&ed);
            edglog(severe)<<"Register DAG subjobs failed, edg_wll_RegisterSubjobsProxy returned:" << et << " (" << ed << "), for jobid: " << id_->toString() << endl;
            randomsleep();
         } else {
            register_success = true;
            break;
         }
      }
   } else {
      edglog(debug)<<"Registering DAG subjobs to LB..."<<endl;
      for (; i > 0; i--) {
         if (edg_wll_RegisterSubjobs(ctx_, id_->c_jobid(), jdls_char, server.c_str(), jids_id)) {
            char *et, *ed;
            edg_wll_Error(ctx_, &et,&ed);
            edglog(severe)<<"Register DAG subjobs failed, edg_wll_RegisterSubjobs returned:" << et << " (" << ed << "), for jobid: " << id_->toString() << endl;
            randomsleep();
         } else {
            register_success = true;
            break;
         }
      }
   }

   for (unsigned int i = 0; i < jdlssize; ++i) {
      free(jdls_char[i]);
      glite_jobid_free(jids_id[i]);
   }
   free(jdls_char);
   free(jids_id);

   if (!register_success) {
      throw LBException(__FILE__, __LINE__, "registerSubJobs()",
                        WMS_LOGGING_ERROR, error_message("Register DAG subjobs failed\n"
                              "edg_wll_RegisterSubjobs[Proxy]"));
   }
   logUserTags(ad->getSubAttributes(JDL::USERTAGS));
   GLITE_STACK_CATCH();
}

vector<string>
WMPEventLogger::generateSubjobsIds(glite::jobid::JobId const* const j, int res_num)
{
   GLITE_STACK_TRY("generateSubjobsIds()");
   edglog_fn("WMPEventlogger::generateSubjobsIds");

   m_subjobs = NULL;
   if (edg_wll_GenerateSubjobIds(ctx_, j->c_jobid(), res_num, "WMPROXY", &m_subjobs)) {
      string msg = error_message("Job ID generation failed\n"
                                 "edg_wll_GenerateSubjobIds");
      throw LBException(__FILE__, __LINE__, "generateSubjobsIds()",
                        WMS_LOGGING_ERROR, msg);
   }
   vector<string> jobids;
   string id;
   for (int i = 0; i < res_num; i++) {
      id = string(edg_wlc_JobIdUnparse(m_subjobs[i]));
      jobids.push_back(id);
   }
   return jobids;

   GLITE_STACK_CATCH();
}

bool
WMPEventLogger::registerDag(
   glite::jobid::JobId const* const id,
   WMPExpDagAd *dag,
   const string& path)
{
   GLITE_STACK_TRY("registerDag()");
   edglog_fn("WMPEventlogger::registerDag");

   char str_addr[1024];
   sprintf(str_addr, "%s", server.c_str());

   int dagsize = dag->size();

   // Setting LB log sync timeout
   struct timeval timeout;
   timeout.tv_sec = 120 + dagsize; // seconds
   timeout.tv_usec = 0; // microseconds
   edglog(debug)<<"Setting LB log sync timeout to "<<timeout.tv_sec
                <<" seconds"<<endl;
   if (edg_wll_SetParamTime(ctx_, EDG_WLL_PARAM_LOG_SYNC_TIMEOUT, &timeout)) {
      edglog(error)
            << error_message("Unable to set LB log sync timeout\nedg_wll_SetParamTime")
            << endl;
   }

   // Registering as a Dag
   edg_wll_RegJobJobtype registration_type_i = EDG_WLL_REGJOB_DAG;
   string registration_type_s = "DAG";
   // check bulk MM condition:
   // 1) enable bulkMM is true inside WM configuration file
   // 2) dag has no dependencies
   if ( m_bulkMM_b) {
      if (dag->getDependenciesNumber()==0 ) {
         // Registering as a Collection
         registration_type_i = EDG_WLL_REGJOB_COLLECTION;
         registration_type_s = "COLLECTION";
         edglog(debug)<<"BulkMM activated, empty/no dependency found"<< endl;
      } else {
         edglog(debug)<<"BulkMM activated but dependency found"<< endl;
      }
   } else {
      edglog(debug)<<"BulkMM deactivated"<< endl ;
   }

   int register_result = 1;
   int i = LOG_RETRY_COUNT;
   if (m_lbProxy_b) {
      edglog(debug)<<"Registering "<< registration_type_s <<" to LB Proxy..."<<endl;
      for (; (i > 0) && register_result; i--) {
         register_result = edg_wll_RegisterJobProxy(ctx_, id->c_jobid(),
                           registration_type_i, path.c_str(), str_addr, dagsize,
                           "WMPROXY", &m_subjobs);
         if (register_result) {
            edglog(severe)<<error_message("Register " +registration_type_s + " failed\n"
                                          "edg_wll_RegisterJobProxy", register_result)<<endl;
            randomsleep();
         }
      }
   } else {
      edglog(debug)<<"Registering "<< registration_type_s <<"to LB..."<<endl;
      for (; (i > 0) && register_result; i--) {
         register_result = edg_wll_RegisterJobSync(ctx_, id->c_jobid(),
                           registration_type_i, path.c_str(), str_addr, dagsize,
                           "WMPROXY", &m_subjobs);
         if (register_result) {
            edglog(severe)<<error_message("Register " + registration_type_s + " failed\n"
                                          "edg_wll_RegisterJobSync", register_result)<<endl;
            randomsleep();
         }
      }
   }

   if (register_result) {
      return false;
   } else {
      // Logging parent user tags
      if (dag->hasAttribute(JDL::USERTAGS)) {
         logUserTags(dag->getAttributeAd(JDL::USERTAGS).ad());
      }
      // Logging children user tags
      //logUserTags(dag->getSubAttributes(JDL::USERTAGS));

      return true;
   }

   GLITE_STACK_CATCH();
}

// User Tags and JobId logs:
void
WMPEventLogger::logUserTag(string name, const string& value)
{
   GLITE_STACK_TRY("logUserTag()");
   edglog_fn("WMPEventlogger::logUserTag");
   boost::scoped_ptr<Ad> classad(new Ad());
   classad->setAttribute(name, value);
   logUserTags(classad->ad());
   GLITE_STACK_CATCH();
}

void
WMPEventLogger::logUserTags(vector<pair<string, classad::ExprTree*> > userTags)
{
   GLITE_STACK_TRY("logUserTags(vector<pair< string, ExprTree>>)");
   edglog_fn("WMPEventlogger::logUserTags");

   unsigned int size = userTags.size();
   for (unsigned int i = 0; i < size; i++) {
      if (userTags[i].second->GetKind() != classad::ExprTree::CLASSAD_NODE) {
         string msg = "Wrong UserTag value for " + userTags[i].first;
         throw LBException(__FILE__, __LINE__, "logUserTags()",
                           WMS_LOGGING_ERROR, msg);
      }
      setLoggingJob(userTags[i].first);
      logUserTags((classad::ClassAd*)(userTags[i].second));
   }
   setLoggingJob(id_->toString());

   GLITE_STACK_CATCH();
}

void
WMPEventLogger::logUserTags(classad::ClassAd* userTags)
{
   GLITE_STACK_TRY("logUserTags()");
   edglog_fn("WMPEventlogger::logUserTags");

   vector<pair<string, classad::ExprTree *> > vect;
   classad::Value val;
   string attrValue;
   userTags->GetComponents(vect);

   // Pointer to a function
   int (*fp) (_edg_wll_Context*, const char*, const char*);
   if (m_lbProxy_b) {
      edglog(debug)<<"Setting job to log to LB Proxy..."<<endl;
      fp = &edg_wll_LogUserTagProxy;
   } else {
      edglog(debug)<<"Setting job to log to LB..."<<endl;
      fp = &edg_wll_LogUserTag;
   }

   unsigned int size = vect.size();
   for (unsigned int i = 0; i < size; i++) {
      if (!userTags->EvaluateExpr(vect[i].second, val)) {
         throw LBException(__FILE__, __LINE__, "logUserTags()",
                           WMS_LOGGING_ERROR, "Unable to Parse Expression");
      }
      if (val.IsStringValue(attrValue)) {
         edglog(debug)<<"Logging user tag to LB[Proxy]: "<<vect[i].first<<endl;
         int j = LOG_RETRY_COUNT;
         int outcome = 1;
         for (; (j > 0) && outcome; j--) {
            outcome = fp(ctx_, (vect[i].first).c_str(), attrValue.c_str());
            if (outcome) {
               edglog(severe)<<error_message("Log user tag failed\n"
                                             "edg_wll_LogUserTag[Proxy]", outcome)<<endl;
               randomsleep();
            }
         }
         if (outcome) {
            string msg = error_message("Log user tag failed\n"
                                       "edg_wll_LogUserTag[Proxy]", outcome);
            throw LBException(__FILE__, __LINE__, "logUserTags()",
                              WMS_LOGGING_ERROR, msg);
         }
      }
   }
   GLITE_STACK_CATCH();
}

void
WMPEventLogger::setLoggingJob(const string& jid, const char* seq_code)
{
   GLITE_STACK_TRY("setLoggingJob()");
   edglog_fn("WMPEventlogger::setLoggingJob");

   glite::jobid::JobId jobid(jid);
   if (m_lbProxy_b) {
      edglog(debug)<<"Setting job for logging to LB Proxy..."<<endl;
      if (edg_wll_SetLoggingJobProxy(ctx_, jobid.c_jobid(), seq_code, getDN_SSL().c_str(), EDG_WLL_SEQ_NORMAL)) {
         string msg = error_message("Set logging job failed\n"
                                    "edg_wll_SetLoggingJobProxy");
         edglog(critical)<<msg<<endl;
         throw LBException(__FILE__, __LINE__, "setLoggingJob()",
                           WMS_LOGGING_ERROR, msg);
      }
   } else {
      edglog(debug)<<"Setting job for logging to LB..."<<endl;
      if (edg_wll_SetLoggingJob(ctx_, jobid.c_jobid(), seq_code,
                                EDG_WLL_SEQ_NORMAL)) {
         string msg = error_message("Set logging job failed\n"
                                    "edg_wll_SetLoggingJob");
         throw LBException(__FILE__, __LINE__, "setLoggingJob()",
                           WMS_LOGGING_ERROR, msg);
      }
   }

   GLITE_STACK_CATCH();
}

void
WMPEventLogger::logListener(const char* host, int port)
{
   GLITE_STACK_TRY("logListener()");
   edglog_fn("WMPEventlogger::logListener");
   edglog(debug)<<"Logging Listener event..."<<endl;
   int outcome = 1;
   int i = LOG_RETRY_COUNT;
   if (m_lbProxy_b) {
      edglog(debug)<<"Logging to LB Proxy..."<<endl;
      for (; (i > 0) && outcome; i--) {
         outcome = edg_wll_LogListenerProxy(ctx_, "InteractiveListener",
                                            host, (uint16_t) port);
         if (outcome) {
            edglog(severe)<<error_message("Register log listener failed\n"
                                          "edg_wll_LogListenerProxy", outcome)<<endl;
            randomsleep();
         }
      }
   } else {
      edglog(debug)<<"Logging to LB..."<<endl;
      for (; (i > 0) && outcome; i--) {
         outcome = edg_wll_LogListener(ctx_, "InteractiveListener",
                                       host, (uint16_t) port);
         if (outcome) {
            edglog(severe)<<error_message("Register log listener failed\n"
                                          "edg_wll_LogListener", outcome)<<endl;
            randomsleep();
         }
      }
   } // end switch LB normal

   if (outcome) {
      string msg = error_message("Register log listener failed\n"
                                 "edg_wll_LogListener[Proxy]", outcome);
      throw LBException(__FILE__, __LINE__, "logListener()",
                        WMS_LOGGING_ERROR, msg);
   }

   GLITE_STACK_CATCH();
}
int
WMPEventLogger::logAbortEventSync(char* reason)
{
   GLITE_STACK_TRY("logAbortEventSync()");

   edglog_fn("WMPEventlogger::logAbortEventSync");
   edglog(debug)<<"Logging Abort event (sync)"<<endl;
   //TODO Checks possibility to do it with LBProxy

   int outcome = 1;
   int i = LOG_RETRY_COUNT;
   if (m_lbProxy_b) {
      edglog(debug)<<"Logging to LB Proxy..."<<endl;
      for (; (i > 0) && outcome; i--) {
         outcome = edg_wll_LogEventProxy(ctx_, EDG_WLL_EVENT_ABORT,
                                         EDG_WLL_FORMAT_ABORT, reason);
         if (outcome) {
            edglog(severe)<<error_message("Register log abort failed\n"
                                          "edg_wll_LogEventProxy", outcome)<<endl;
            randomsleep();
         }
      }
   } else { // end switch LB PROXY
      edglog(debug)<<"Logging to LB..."<<endl;
      for (; (i > 0) && outcome; i--) {
         outcome = edg_wll_LogEventSync(ctx_, EDG_WLL_EVENT_ABORT,
                                        EDG_WLL_FORMAT_ABORT, reason);
         if (outcome) {
            edglog(severe)<<error_message("Register log sync abort failed\n"
                                          "edg_wll_LogEventSync", outcome)<<endl;
            randomsleep();
         }
      }
   } // end switch LB normal

   if (outcome) {
      string msg = error_message("Register log abort failed edg_wll_LogEventSync/Proxy", outcome);
      edglog(critical)<<msg<<endl;
   }

   return outcome;

   GLITE_STACK_CATCH();
}

void
WMPEventLogger::logEvent(event_name event, const char* reason, bool retry,
                         bool test, const char* file_queue, const char* jdl)
{
   GLITE_STACK_TRY("logEvent()");
   edglog_fn("WMPEventlogger::logEvent");
   if (!test) {
      logEvent(event, reason, retry, file_queue, jdl);
   }
   int res;
   bool with_hp = false;
   int lap = 0;
   do {
      // PERFORM the Requested operation (actual LB call)
      res = logEvent(event, reason, file_queue, jdl);
      testAndLog(res, with_hp, lap);
   } while (res != 0);
   GLITE_STACK_CATCH();
}

void
WMPEventLogger::setUserProxy(const string& proxy)
{
   GLITE_STACK_TRY("setUserProxy()");
   edglog_fn("WMPEventlogger::setUserProxy");

   this->delegatedproxy = proxy;
   if (!proxy.empty()) {
      try {
         security::checkProxyValidity(proxy);
      } catch (Exception& ex) {
         if (ex.getCode() != wmputilities::WMS_PROXY_EXPIRED) {
            // Problem with proxy (not expired)
            throw ex;
            // } else {
            // Delegated proxy expired - continue processing
            // TODO most commands cannot be performed anymore:
            // submit/cancel will fail anyway (to be tested)
            // output/ perusal may continue (to be tested)
         }

      }
      if (edg_wll_SetParam(ctx_, EDG_WLL_PARAM_X509_PROXY, proxy.c_str())) {
         string msg = error_message("Unable to set User Proxy for "
                                    "LB context\nedg_wll_SetParam");
         throw LBException(__FILE__, __LINE__, "setUserProxy()",
                           WMS_LOGGING_ERROR, msg);
      }
   } else {
      // Setting host proxy
      if (!getenv(GLITE_HOST_KEY) || !getenv(GLITE_HOST_CERT)) {
         throw AuthenticationException(__FILE__, __LINE__,
                                       "setJobLoggingProxy()", WMS_AUTHORIZATION_ERROR,
                                       "Unable to set User Proxy for LB context");
      } else {
         if (edg_wll_SetParam(ctx_, EDG_WLL_PARAM_X509_PROXY, NULL)
               || edg_wll_SetParam(ctx_, EDG_WLL_PARAM_X509_KEY,
                                   getenv(GLITE_HOST_KEY))
               || edg_wll_SetParam(ctx_, EDG_WLL_PARAM_X509_CERT,
                                   getenv(GLITE_HOST_CERT))) {
            string msg = error_message("Unable to set User Proxy for "
                                       "LB context\nedg_wll_SetParam");
            throw LBException(__FILE__, __LINE__, "setUserProxy()",
                              WMS_LOGGING_ERROR, msg);
         }
      }
   }

   GLITE_STACK_CATCH();
}

edg_wlc_JobId  wmpJobIdParse(const char* jobid_str)
{
   edg_wlc_JobId jobid;
   // parse the jobID string
   if (edg_wlc_JobIdParse(jobid_str, &jobid)) {
      throw LBException(__FILE__, __LINE__,
                        "wmpJobIdParse()", WMS_OPERATION_NOT_ALLOWED,
                        "Error during edg_wlc_JobIdParse");
   }
   return jobid;
}




/*********************
* LB  QUERY  METHODS
* called by checkPerusalFlag
**********************/

regJobEvent
WMPEventLogger::retrieveRegJobEvent(const string& jobid_str)
{
   GLITE_STACK_TRY("retrieveRegJobEvent()");
   edglog_fn("WMPEventlogger::retrieveRegJobEvent");

   // parse the jobID string
   edg_wlc_JobId jobid = wmpJobIdParse (jobid_str.c_str());

   regJobEvent event;
   event.instance = "";
   event.jdl = "";
   event.parent = "";

   edg_wll_Event * events = NULL;
   edg_wll_QueryRec jc[2];
   edg_wll_QueryRec ec[2];
   memset(jc, 0, sizeof jc);
   memset(ec, 0, sizeof ec);

   // job condition: JOBID = jobid
   jc[0].attr = EDG_WLL_QUERY_ATTR_JOBID;
   jc[0].op = EDG_WLL_QUERY_OP_EQUAL;
   jc[0].value.j = jobid;
   jc[1].attr = EDG_WLL_QUERY_ATTR_UNDEF;

   // event condition: Event type = REGJOB
   ec[0].attr = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
   ec[0].op = EDG_WLL_QUERY_OP_EQUAL;
   ec[0].value.i = EDG_WLL_EVENT_REGJOB;
   ec[1].attr = EDG_WLL_QUERY_ATTR_UNDEF;

   // Automatically performs query, check result, and retry if necessary
   if ( testAndQuery(jc, ec, &events) ) {
      // throw Exception
      throw LBException(__FILE__, __LINE__,"retrieveRegJobEvent()",
                        WMS_LOGGING_ERROR, "Unable to retrieve regjob event");
   }
   // Getting last event found (will be stored in events[i]
   int i = 0;
   while (events[i].type) {
      i++;
   }
   i--;
   // Preparing returning result instance starting from found Registered event
   if (events[i].regJob.src_instance) {
      event.instance = events[i].regJob.src_instance;
   }
   if (events[i].regJob.jdl) {
      event.jdl = events[i].regJob.jdl;
   }
   if (events[i].regJob.parent) {
      event.parent = string(edg_wlc_JobIdUnparse(events[i].regJob.parent));
   }
   for (int i = 0; events[i].type; i++) {
      edg_wll_FreeEvent(&events[i]);
   }
   return event;
   GLITE_STACK_CATCH();
}

string
WMPEventLogger::getLastEventSeqCode()
{
   GLITE_STACK_TRY("getLastEventSeqCode()");
   edglog_fn("WMPEventlogger::getLastEventSeqCode");


   // parse the jobID string
   edg_wlc_JobId jobid = wmpJobIdParse (id_->toString().c_str());
   edg_wll_Event * events = NULL;
   edg_wll_QueryRec jc[2];
   edg_wll_QueryRec ec[2];
   memset(jc, 0, sizeof jc);
   memset(ec, 0, sizeof ec);

   // job condition: JOBID = jobid
   jc[0].attr = EDG_WLL_QUERY_ATTR_JOBID;
   jc[0].op = EDG_WLL_QUERY_OP_EQUAL;
   jc[0].value.j = jobid;
   jc[1].attr = EDG_WLL_QUERY_ATTR_UNDEF;

   // event condition: Event SOURCE = NetworkServer
   ec[0].attr = EDG_WLL_QUERY_ATTR_SOURCE;
   ec[0].op = EDG_WLL_QUERY_OP_EQUAL;
   ec[0].value.i = EDG_WLL_SOURCE_NETWORK_SERVER;
   ec[1].attr = EDG_WLL_QUERY_ATTR_UNDEF;


   // Automatically performs query, check result, and retry if necessary
   if ( testAndQuery(jc, ec, &events) ) {
      // throw Exception
      throw LBException(__FILE__, __LINE__, "getLastEventSeqCode()",
                        WMS_LOGGING_ERROR, "Unable to get events for job: "+ id_->toString());
   }

   int i = 0;
   while (events[i].type) {
      i++;
   }
   i--;

   string seqcode = events[i].any.seqcode;

   for (int i = 0; events[i].type; i++) {
      edg_wll_FreeEvent(&events[i]);
   }

   return seqcode;

   GLITE_STACK_CATCH();
}

bool
WMPEventLogger::isAborted(string& reason)
{
   GLITE_STACK_TRY("isAborted()");
   edglog_fn("WMPEventlogger::isAborted");

   reason = "";


   // parse the jobID string
   edg_wlc_JobId jobid = wmpJobIdParse (id_->toString().c_str());

   edg_wll_Event * events = NULL;
   edg_wll_QueryRec jc[2];
   edg_wll_QueryRec ec[2];
   memset(jc, 0, sizeof jc);
   memset(ec, 0, sizeof ec);

   // job condition: JOBID = jobid
   jc[0].attr = EDG_WLL_QUERY_ATTR_JOBID;
   jc[0].op = EDG_WLL_QUERY_OP_EQUAL;
   jc[0].value.j = jobid;
   jc[1].attr = EDG_WLL_QUERY_ATTR_UNDEF;

   // event condition: Event type = CANCEL
   ec[0].attr = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
   ec[0].op = EDG_WLL_QUERY_OP_EQUAL;
   ec[0].value.i = EDG_WLL_EVENT_ABORT;
   ec[1].attr = EDG_WLL_QUERY_ATTR_UNDEF;

   // Automatically performs query, check result, and retry if necessary
   switch ( testAndQuery(jc, ec, &events) ) {
   case ENOENT:
      // No cancel event found
      return false;
   case 0:
      // SUCCESS!!
      break;
   default:
      // ALL other cases: Exeption
      throw LBException(__FILE__, __LINE__, "isAborted()",
                        WMS_LOGGING_ERROR, "Unable to query LB and LBProxy");
   }
   // fill reason (reference) parameter
   reason = events[0].abort.reason;
   for (int i = 0; events[i].type; i++) {
      edg_wll_FreeEvent(&events[i]);
   }
   return true;
   GLITE_STACK_CATCH();
}

pair<string, regJobEvent>
WMPEventLogger::isStartAllowed()
{
   GLITE_STACK_TRY("isStartAllowed()");
   edglog_fn("WMPEventlogger::isStartAllowed");

   // parse the jobID string
   edg_wlc_JobId jobid = wmpJobIdParse (id_->toString().c_str());

   edg_wll_Event * events = NULL;
   edg_wll_QueryRec jc[2];
   edg_wll_QueryRec ec[2];
   memset(jc, 0, sizeof jc);
   memset(ec, 0, sizeof ec);

   // job condition: JOBID = jobid
   jc[0].attr = EDG_WLL_QUERY_ATTR_JOBID;
   jc[0].op = EDG_WLL_QUERY_OP_EQUAL;
   jc[0].value.j = jobid;
   jc[1].attr = EDG_WLL_QUERY_ATTR_UNDEF;

   // event condition: Event SOURCE = NetworkServer
   ec[0].attr = EDG_WLL_QUERY_ATTR_SOURCE;
   ec[0].op = EDG_WLL_QUERY_OP_EQUAL;
   ec[0].value.i = EDG_WLL_SOURCE_NETWORK_SERVER;
   ec[1].attr = EDG_WLL_QUERY_ATTR_UNDEF;



   regJobEvent event;
   event.instance = "";
   event.jdl = "";
   event.parent = "";
   // Automatically performs query, check result, and retry if necessary
   if ( testAndQuery(jc, ec, &events) ) {
      // throw Exception
      throw LBException(__FILE__, __LINE__, "isStartAllowed()",
                        WMS_LOGGING_ERROR,"Unable to get events for job: "+ id_->toString());
   }

   bool flag = false;
   int i = 0;
   while (events[i].type) {
      edglog(debug)<<"Event type: "<<events[i].type<<endl;
      switch (events[i].type) {
      case EDG_WLL_EVENT_REGJOB:
         if (events[i].regJob.src_instance) {
            event.instance = events[i].regJob.src_instance;
         }
         if (events[i].regJob.jdl) {
            event.jdl = events[i].regJob.jdl;
         }
         if (events[i].regJob.parent) {
            event.parent
               = string(edg_wlc_JobIdUnparse(events[i].regJob.parent));
         }
         break;
      case EDG_WLL_EVENT_USERTAG:
         break;
      case EDG_WLL_EVENT_ACCEPTED:
      case EDG_WLL_EVENT_ENQUEUED:
         flag = true;
         break;
      case EDG_WLL_EVENT_ABORT:
         throw JobOperationException(__FILE__, __LINE__,
                                     "isStartAllowed()", WMS_OPERATION_NOT_ALLOWED,
                                     "job state (ABORTED) doesn't allow start operation");
         break;
      case EDG_WLL_EVENT_CANCEL:
         throw JobOperationException(__FILE__, __LINE__,
                                     "isStartAllowed()", WMS_OPERATION_NOT_ALLOWED,
                                     "job state (CANCELLED) doesn't allow start operation");
         break;
      default:
         throw JobOperationException(__FILE__, __LINE__,
                                     "isStartAllowed()", WMS_OPERATION_NOT_ALLOWED,
                                     "unable to complete the operation");
         break;
      }
      i++;
   }
   i--;

   // Only logs EDG_WLL_EVENT_REGJOB, EDG_WLL_EVENT_USERTAG,
   // EDG_WLL_EVENT_ACCEPTED & EDG_WLL_EVENT_ENQUEUED are present
   // Checking for last event type
   string seqcode = "";
   if (flag) {
      if ((events[i].type == EDG_WLL_EVENT_ENQUEUED)
            && (events[i].enQueued.result == EDG_WLL_ENQUEUED_FAIL)) {
         seqcode = string(events[i].enQueued.seqcode);
      } else {
         throw JobOperationException(__FILE__, __LINE__,
                                     "isStartAllowed()", WMS_OPERATION_NOT_ALLOWED,
                                     "job started or start operation already in progress");
      }
   } else {
      if (events[i].type == EDG_WLL_EVENT_REGJOB) {
         seqcode = string(events[i].regJob.seqcode);
      } else if (events[i].type == EDG_WLL_EVENT_USERTAG) {
         seqcode = string(events[i].userTag.seqcode);
      }
   }

   for (int i = 0; events[i].type; i++) {
      edg_wll_FreeEvent(&events[i]);
   }

   pair<string, regJobEvent> returnpair;
   returnpair.first = seqcode;
   returnpair.second = event;

   return returnpair;

   GLITE_STACK_CATCH();
}

glite::lb::JobStatus
WMPEventLogger::getStatus(bool childreninfo)
{
   GLITE_STACK_TRY("getStatus()");
   edglog_fn("WMPEventlogger::getStatus");

   // parse the jobID string
   edg_wlc_JobId jobid = wmpJobIdParse (id_->toString().c_str());

   edg_wll_JobStat * states = NULL;
   edg_wll_QueryRec jc[2];
   memset(jc, 0, sizeof jc);

   // job condition: JOBID = jobid
   jc[0].attr = EDG_WLL_QUERY_ATTR_JOBID;
   jc[0].op = EDG_WLL_QUERY_OP_EQUAL;
   jc[0].value.j = jobid;
   jc[1].attr = EDG_WLL_QUERY_ATTR_UNDEF;

   int error;
   int flag = EDG_WLL_STAT_CLASSADS;
   if (childreninfo) {
      flag = flag | EDG_WLL_STAT_CHILDREN;
   }

   if (m_lbProxy_b) {
      edglog(debug)<<"Querying LB Proxy..."<<endl;
      error = edg_wll_QueryJobsProxy(ctx_, jc, flag, NULL, &states);
      // OR operator not necessary, workaround for the LB bug #22442
      if ((error == ENOENT) || (states[0].state == EDG_WLL_JOB_UNDEF)) { // no events found
         edglog(debug)<<"No status found querying LB Proxy. Querying LB..."<<endl;
         error = edg_wll_QueryJobs(ctx_, jc, flag, NULL, &states);
      }
   } else { // end switch LB PROXY
      edglog(debug)<<"Querying LB..."<<endl;
      error = edg_wll_QueryJobs(ctx_, jc, flag, NULL, &states);
   } // end switch LB normal

   // OR operator not necessary, workaround for the LB bug #22442
   if (error || (states[0].state == EDG_WLL_JOB_UNDEF)) {
      string msg = error_message("Unable to get job status\n"
                                 "edg_wll_QueryJobs[Proxy]", error);
      throw LBException(__FILE__, __LINE__, "getStatus()",
                        WMS_LOGGING_ERROR,msg);
   }

   // Searching last state
   int i = 0;
   while (states[i].state) {
      i++;
   }
   i--;

   /*for (int i = 0; events[i].type; i++) {
      edg_wll_FreeStatus(&states[i]);
   }*/

   return glite::lb::JobStatus(states[i]);

   GLITE_STACK_CATCH();
}

//
//
// Private methods
//
//

string
WMPEventLogger::error_message(const string& message, int exitcode)
{
   GLITE_STACK_TRY("error_message()");

   char * msg = NULL;
   char * dsc = NULL;
   edg_wll_Error(ctx_, &msg, &dsc);

   string lb;
   if (!m_lbProxy_b) {
      if (getenv(GLITE_WMS_LOG_DESTINATION)) {
         lb = "LB server (ENV): " + string(getenv(GLITE_WMS_LOG_DESTINATION)) + "\n";
      } else if (!lb_host.empty()) {
         lb = "LB server: " + this->lb_host + ":"
              + boost::lexical_cast<string>(this->lb_port) + "\n";
      }
   }

   string error_message = string(lb + message
                                 + ((exitcode) ? "\nExit code: "
                                    + boost::lexical_cast<string>(exitcode) : ""));

   if (msg && dsc) {
      error_message += "\nLB[Proxy] Error: " + string(msg)
                       + "\n(" + string(dsc) + ")";
   } else {
      error_message += "\nLB[Proxy] Error not available "
                       "(empty messages)";
   }

   free(msg);
   free(dsc);
   return error_message;

   GLITE_STACK_CATCH();
}

void
WMPEventLogger::testAndLog(int& code, bool& with_hp, int& lap)
{
   GLITE_STACK_TRY("testAndLog()");
   edglog_fn("WMPEventlogger::testAndLog");

   if (code) {
      switch (code) {
      case EINVAL:
         edglog(critical)<<"Critical error in LB calls: EINVAL"<<endl;
         code = 0; // Don't retry...
         break;
      case EDG_WLL_ERROR_GSS:
         edglog(severe)<<"Severe error in SSL layer while communicating with LB daemons"<<endl;
         if (with_hp) {
            edglog(severe)<<"The log with the host certificate has just been done. Giving up"<<endl;
            code = 0; // Don't retry...
         } else {
            code = 0; // Don't retry.
         }
         break;
      default:
         if (++lap > LOG_RETRY_COUNT) {
            string msg = "Unable to complete operation: LB call retried "
                         + boost::lexical_cast<string>(lap - 1)
                         + " times, always failed";
            edglog(error)<<msg<<endl;
            //code = 0; // Don't retry anymore
            throw LBException(__FILE__, __LINE__,
                              "WMPEventLogger::testAndLog()", WMS_LOGGING_ERROR,
                              msg + "\n(please contact server administrator)");
         } else {
            randomsleep();
         }
         break;
      }
   }
   return;
   GLITE_STACK_CATCH();
}

int
WMPEventLogger::testAndQuery(edg_wll_QueryRec *jc, edg_wll_QueryRec *ec, edg_wll_Event **events)
{
   GLITE_STACK_TRY("testAndQuery()");
   edglog_fn("WMPEventlogger::testAndQuery");
   int result=1 ; // default is error
   int lap = 1;
   bool lbProxy_b = m_lbProxy_b;
   while (lap<=QUERY_RETRY_COUNT) {
      // PERFORM the Query:
      edglog(debug) << "LB query lap #"<< lap <<": ";
      if (lbProxy_b) {
         result = edg_wll_QueryEventsProxy(ctx_, jc, ec, events);
         if (result == ENOENT) { // no events found
            lbProxy_b= false; // Next cycle will no contact LBProxy
            edglog(debug)<<"No events found querying LB Proxy: querying LB"<<endl;
            result = edg_wll_QueryEvents(ctx_, jc, ec, events);
         }
      } else {
         result = edg_wll_QueryEvents(ctx_, jc, ec, events);
      }
      // Determine whether is worth retrying the query
      switch (result) {
      case 0:
         // SUCCESS!
         return result;
      case ENOENT:
         edglog(debug)<<"No (previous) events found querying LB" << endl;
         return result;  // Do not retry
      case EINVAL:
         edglog(critical)<<"Critical error in LB calls: EINVAL" << endl;
         return result;  // Do not retry
      case EDG_WLL_ERROR_SERVER_RESPONSE:
      case EDG_WLL_ERROR_DB_CALL:
      case EDG_WLL_ERROR_GSS:
      case EDG_WLL_ERROR_DNS:
      case EIO:
      case ETIMEDOUT:
         // May be temporarily error: break and Retry
         edglog(warning)<<"Temporarily error while contacting LB" << endl;
         edglog(warning)<<"edg_wll_QueryEvents Error Code: " << result;
         break;
      default:
         if (    result > GLITE_WMS_LOGGING_ERROR_BASE  &&
                 result < EDG_WLL_ERROR_COMPARE_EVENTS) {
            // LB returned codes lead to definite error - do not retry
            edglog(critical)<<"Critical WMS error code in LB call: "<< result << endl;
            return result;  // Do not retry
         } else {
            // All other system error may depend on temporarily problem
            // LB returned codes lead to definite error
            edglog(critical)<<"Critical generic error code in LB call: "<< result << endl;
            return result;
         }
         break;
      } // switch Result
      if (lap==QUERY_RETRY_COUNT) {
         // ALL queries returned with ERROR
         edglog(critical)<< "Unable to complete operation: LB query retried " <<
                         boost::lexical_cast<string>(lap) <<  " times, always failed";
         return result;  // Do not retry
      } else {
         lap++;
         randomsleep();
      }
   }    // end while (lap<=QUERY_RETRY_COUNT)
   return result;
   GLITE_STACK_CATCH();
}

int
WMPEventLogger::logEvent(event_name event, const char* reason,
                         const char* file_queue, const char* jdl)
{
   GLITE_STACK_TRY("logEvent()");
   edglog_fn("WMPEventlogger::logEvent");

   if (m_lbProxy_b) {
      edglog(debug)<<"Logging to LB Proxy..."<<endl;
      switch (event) {
      case LOG_ACCEPT: {
         // Log remote address
         string remoteHost="Host not available";
         if (getenv("REMOTE_HOST")) {
            remoteHost = string(getenv("REMOTE_HOST"));
         } else if (getenv("REMOTE_ADDR")) {
            remoteHost = string(getenv("REMOTE_ADDR"));
            if (getenv("REMOTE_PORT")) {
               remoteHost+= ":" + string(getenv("REMOTE_PORT"));
            }
         }

         edglog(debug)<<"Logging Accept event..."<<endl;
         return edg_wll_LogAcceptedProxy(ctx_, EDG_WLL_SOURCE_USER_INTERFACE,
                                         edg_wll_SourceToString(EDG_WLL_SOURCE_NETWORK_SERVER),
                                         remoteHost.c_str(), "");
         break;
      }
      case LOG_ENQUEUE_START:
         edglog(debug)<<"Logging Enqueue START event..."<<endl;
         return edg_wll_LogEnQueuedProxy(ctx_, file_queue, jdl, "START", "");
         break;
      case LOG_ENQUEUE_OK:
         edglog(debug)<<"Logging Enqueue OK event..."<<endl;
         return edg_wll_LogEnQueuedProxy(ctx_, file_queue, jdl, "OK", "");
         break;
      case LOG_ENQUEUE_FAIL:
         edglog(debug)<<"Logging Enqueue FAIL event..."<<endl;
         return edg_wll_LogEnQueuedProxy (ctx_, file_queue, jdl, "FAIL",
                                          reason);
         break;
      case LOG_ABORT:
         edglog(debug)<<"Logging Abort event..."<<endl;
         return edg_wll_LogAbortProxy(ctx_, reason);
         break;
      case LOG_REFUSE:
         edglog(debug)<<"Logging Refuse event..."<<endl;
         return edg_wll_LogRefusedProxy(ctx_, EDG_WLL_SOURCE_WM_PROXY,
                                        server.c_str(), "", "");
         break;
      case LOG_CANCEL:
         edglog(debug)<<"Logging Cancel event..."<<endl;
         return edg_wll_LogCancelREQProxy(ctx_, reason);
         break;
      case LOG_CLEAR:
         edglog(debug)<<"Logging Clear event..."<<endl;
         return edg_wll_LogClearProxy(ctx_, reason);
         break;
      case LOG_USER_TAG:
         edglog(debug)<<"Logging User Tag event..."<<endl;
         return edg_wll_LogCancelREQProxy(ctx_, reason);
         break;
      default:
         edglog(severe)<<"Warning: no event caught, not Logging"<<endl;
         return 1;
      }
   } else { // end switch LB PROXY
      edglog(debug)<<"Logging to LB..."<<endl;
      switch (event) {
      case LOG_ACCEPT: {
         // Log remote address
         string remoteHost="Host not available";
         if (getenv("REMOTE_ADDR")) {
            remoteHost = string(getenv("REMOTE_ADDR"));
            if (getenv("REMOTE_PORT")) {
               remoteHost+= ":" + string(getenv("REMOTE_PORT"));
            }
         }
         edglog(debug)<<"Logging Accept event..."<<endl;
         return edg_wll_LogAccepted(ctx_, EDG_WLL_SOURCE_USER_INTERFACE,
                                    remoteHost.c_str(), reason, ""); // reason is "From Client"
         break;
      }
      case LOG_REFUSE:
         edglog(debug)<<"Logging Refuse event..."<<endl;
         return edg_wll_LogRefused(ctx_, EDG_WLL_SOURCE_WM_PROXY,
                                   server.c_str(), "", "");
         break;
      case LOG_CANCEL:
         edglog(debug)<<"Logging Cancel event..."<<endl;
         return edg_wll_LogCancelREQ(ctx_, reason);
         break;
      case LOG_CLEAR:
         edglog(debug)<<"Logging Clear event..."<<endl;
         return edg_wll_LogClear(ctx_, reason);
         break;
      case LOG_ABORT:
         edglog(debug)<<"Logging Abort event..."<<endl;
         return edg_wll_LogAbort(ctx_, reason);
         break;
      case LOG_ENQUEUE_START:
         edglog(debug)<<"Logging Enqueue START event..."<<endl;
         return edg_wll_LogEnQueued(ctx_, file_queue, jdl, "START", "");
         break;
      case LOG_ENQUEUE_OK:
         edglog(debug)<<"Logging Enqueue OK event..."<<endl;
         return edg_wll_LogEnQueued(ctx_, file_queue, jdl, "OK", "");
         break;
      case LOG_ENQUEUE_FAIL:
         edglog(debug)<<"Logging Enqueue FAIL event..."<<endl;
         return edg_wll_LogEnQueued(ctx_, file_queue, jdl, "FAIL",
                                    reason);
         break;
      default:
         edglog(severe)<<"Warning: no event caught, not Logging"<<endl;
         return 1;
      }
   } // end switch LB normal
   // Unpredictable line: ERROR!
   edglog(critical)<<"logEvent Unpredictable line found!!!"<<endl;
   return 1;

   GLITE_STACK_CATCH();
}

void
WMPEventLogger::logEvent(event_name event, const char* reason, bool retry,
                         const char* file_queue, const char* jdl)
{
   GLITE_STACK_TRY("logEvent()");
   edglog_fn("WMPEventLogger::logEvent");
   edglog(debug)<<"Logging event code"<<event<<" request..."<<endl;

   int i = 0;
   int logged = 1;
   for (; i < LOG_RETRY_COUNT && !logged && retry; i++) {
      // PERFORM the Requested operation (actual LB call)
      logged = logEvent(event, reason, file_queue, jdl);
      edglog(debug)<<"logEvent exit code: "<<logged<<endl;
      if (!logged && (i < 2) && retry) {
         edglog(error)<<error_message("Error logging event\nedg_wll_Log<Event>REQ", logged)<<endl;
         randomsleep();
      }
   }
   if ((retry && (i >= LOG_RETRY_COUNT)) || (!retry && (i > 0)) ) {
      string msg = error_message("edg_wll_Log<Event>REQ", logged);
      throw LBException(__FILE__, __LINE__, "logEvent()",
                        WMS_LOGGING_ERROR, msg);
   }
   GLITE_STACK_CATCH();
}

void
WMPEventLogger::randomsleep()
{
   GLITE_STACK_TRY("randomsleep()");
   edglog_fn("WMPEventlogger::randomsleep");

   if (m_lbProxy_b) {
      int randomvalue = generateRandomNumber(LB_PROXY_LOG_RETRY_LOWER_LIMIT,
                                             LB_PROXY_LOG_RETRY_UPPER_LIMIT);
      edglog(debug)<<"Failed to contact LB. Sleeping for "<<randomvalue
                   <<" seconds before retry..."<<endl;
      sleep(randomvalue);
   } else {
      int randomvalue = generateRandomNumber(LB_LOG_RETRY_LOWER_LIMIT,
                                             LB_LOG_RETRY_UPPER_LIMIT);
      edglog(debug)<<"Failed to contact LB. Sleeping for "<<randomvalue
                   <<" seconds before retry..."<<endl;
      sleep(randomvalue);
   }

   GLITE_STACK_CATCH();
}

}}}}
