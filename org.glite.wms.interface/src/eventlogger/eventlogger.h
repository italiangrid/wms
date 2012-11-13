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
// File: eventlogger.h
// Author: Giuseppe Avellino <egee@datamat.it>
//


#ifndef GLITE_WMS_WMPROXY_WMPEVENTLOGGER_H
#define GLITE_WMS_WMPROXY_WMPEVENTLOGGER_H

#include <vector>

#include <boost/scoped_ptr.hpp>
#include "expdagad.h"
#include "glite/jdl/JobAd.h"
#include "glite/jobid/JobId.h"
#include "glite/lb/consumer.h"
#include "glite/lb/JobStatus.h"

namespace glite {
namespace wms {
namespace wmproxy {
namespace eventlogger {

struct regJobEvent {
   std::string instance;
   std::string jdl;
   std::string parent;
};

class WMPEventLogger
{
public:
   enum event_name {
      LOG_ACCEPT,
      LOG_REFUSE,
      LOG_CANCEL,
      LOG_CLEAR,
      LOG_ABORT,
      LOG_ENQUEUE_START,
      LOG_ENQUEUE_OK, LOG_ENQUEUE = LOG_ENQUEUE_OK,
      LOG_ENQUEUE_FAIL,
      LOG_USER_TAG
   };

   WMPEventLogger(const std::string& endpoint);
   virtual ~WMPEventLogger() throw();
   // Initializer
   void init_and_set_logging_job(const std::string& nsHost, int nsPort,
             glite::jobid::JobId *id);

   // Enable/Disable/Retrieve settings for  LB Proxy
   void setLBProxy(bool value, std::string dn = "");
   bool getLBProxy();

   // Enable/Disable/Retrieve settings for Bulk Match Making
   void setBulkMM(bool value);
   bool getBulkMM();

   // Sequence code
   char * getSequence();
   void setSequenceCode(const std::string& seqcode);
   void incrementSequenceCode();

   // Proxy renewal
   char * registerProxyRenewal(const std::string& proxy_path,
                               const std::string& my_proxy_server,
                               glite::jobid::JobId const *id = 0);
   void unregisterProxyRenewal();

   // Register methods
   bool registerJob(glite::jdl::JobAd *ad, glite::jobid::JobId const* const id, const std::string& path);
   bool registerDag(glite::jobid::JobId const* const id, WMPExpDagAd *ad, const std::string& path);
   void registerSubJobs(WMPExpDagAd *ad, edg_wlc_JobId *subjobs);
   std::vector<std::string> generateSubjobsIds(glite::jobid::JobId const* const j, int res_num);
   void logListener(const char* host, int port);

   // Event logging
   void logEvent(event_name event, const char *reason, bool retry, bool test,
                 const char *file_queue = NULL, const char *jdl = NULL);
   int logAbortEventSync(char *reason);

   void logUserTag(std::string name, const std::string& value);
   void logUserTags(classad::ClassAd *userTags);
   void logUserTags(std::vector<std::pair<std::string,
                    classad::ExprTree*> > userTags);

   void setLoggingJob(const std::string& jid, const char *seq_code = NULL);

   void setUserProxy(const std::string& proxy);

   regJobEvent retrieveRegJobEvent(const std::string& jobid_str);

   std::pair<std::string, regJobEvent> isStartAllowed();

   bool isAborted(std::string& reason);

   std::string getLastEventSeqCode();

   /**
    * Gets the status of the context job
    * @param childreninfo if set to true, subjobs info are also returned (if any)
    * @return the status of the context job
    */
   glite::lb::JobStatus getStatus(bool childreninfo);

   edg_wlc_JobId *m_subjobs;
private:

   edg_wll_Context ctx_;
   glite::jobid::JobId *id_;
   std::string dest_uri;
   std::string lb_host;
   int lb_port;
   std::string server;
   std::string delegatedproxy;

   bool m_lbProxy_b;
   bool m_bulkMM_b;

   std::string error_message(const std::string& api, int exitcode = 0);

   /**
   * check LB exit code and determine whether is worth retrying
   * code LB logging result
   * with_hp log with the host certificate (or not, boolean)
   * lap number of retries
   */
   void testAndLog(int& code, bool& with_hp, int& lap);

   /**
   * Perform desired query
   * check LB exit code
   * @return  LB query exit code (0=success)
   */
   int testAndQuery(edg_wll_QueryRec *jc, edg_wll_QueryRec *ec, edg_wll_Event ** events);
   int logEvent(event_name event, const char *reason,
                const char *file_queue = NULL, const char *jdl = NULL);
   void logEvent(event_name event, const char *reason, bool retry,
                 const char *file_queue = NULL, const char *jdl = NULL);

   void randomsleep();
};

}}}}

#endif // GLITE_WMS_WMPROXY_WMPEVENTLOGGER_H
