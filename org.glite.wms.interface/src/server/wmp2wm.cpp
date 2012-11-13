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
// File: wmp2wm.cpp
// Author: Marco Pappalardo
// Author: Giuseppe Avellino <egee@datamat.it>
//

#include "wmp2wm.h"
#include "responsestruct.h"

// Event logger
#include "eventlogger/eventlogger.h"

// Utilities
#include "utilities/utils.h"

// Commands
#include "commands/listjobmatch.h"

// Logging
#include "utilities/logging.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#include "glite/wms/common/utilities/edgstrstream.h"

// WMProxy exceptions
#include "utilities/wmpexceptions.h"
#include "utilities/wmpexception_codes.h"

// Common Utilities
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/common/utilities/wm_commands.h"

#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/PrivateAttributes.h"

// Global variables for configuration
extern std::string dispatcher_type_global;
extern std::string filelist_global;
// Listmatch classad attributes
const char * LISTMATCH_REASON = "reason";
const char * LISTMATCH_MATCH_RESULT = "match_result";

const std::string LISTMATCH_REASON_OK = "ok";
const std::string LISTMATCH_REASON_NO_MATCH = "no matching resources found";

// Dispatcher type possible choices
const std::string DISPATCHER_TYPE_FILELIST = "filelist";
const std::string DISPATCHER_TYPE_JOBDIR = "jobdir";


namespace logger       = glite::wms::common::logger;
namespace eventlogger  = glite::wms::wmproxy::eventlogger;
namespace wmsutilities = glite::wms::common::utilities;
namespace utils        = glite::wmsutils::classads;
namespace wmputilities = glite::wms::wmproxy::utilities;
namespace commands     = glite::wms::wmproxy::commands;

using namespace std;
using namespace glite::jdl;

namespace
{

void
f_forward(wmsutilities::JobDir jd, string const& ad)
{
   GLITE_STACK_TRY("f_forward()");
   edglog_fn("wmp2wm::f_forward");

   edglog(debug)<<"JobDir forwarding"<<endl;
   try {
      jd.deliver(ad);
   } catch (exception& ex) {
      throw wmputilities::FileSystemException(__FILE__, __LINE__,
                                              "wmp2wm::f_forward()", wmputilities::WMS_FILE_SYSTEM_ERROR,
                                              ex.what());
   }

   GLITE_STACK_CATCH();
}

string
toFormattedString(classad::ClassAd& classad)
{
   GLITE_STACK_TRY("toFormattedString()");

   string buffer = "";
   classad::PrettyPrint unp;
   unp.SetClassAdIndentation(1);
   unp.SetListIndentation(0);
   unp.Unparse(buffer, classad.Copy());
   return buffer;

   GLITE_STACK_CATCH();
}

}

namespace glite
{
namespace wms
{
namespace wmproxy
{
namespace server
{

WMP2WM::WMP2WM()
{
}

WMP2WM::~WMP2WM()
{
}

void
WMP2WM::init(const string& filename, eventlogger::WMPEventLogger *wmpeventlogger)
{
   GLITE_STACK_TRY("init()");
   edglog_fn("wmp2wm::init");

   this->wmpeventlogger = wmpeventlogger;
   // filename is the base_dir
   boost::filesystem::path base(filename, boost::filesystem::native);
   m_jobdir.reset(new wmsutilities::JobDir(base));
   edglog(debug)<<"wmp2wm initialization done"<<endl;
   GLITE_STACK_CATCH();
}

void
WMP2WM::submit(const string& jdl, const string& jdlpath)
{
   GLITE_STACK_TRY("submit()");
   edglog_fn("wmp2wm::submit");

   edglog(debug)<<"Forwarding Submit Request..."<<endl;

   classad::ClassAd * jdlAd(utils::parse_classad(jdl));
   classad::ClassAd convertedAd =
      wmsutilities::submit_command_create(*jdlAd);
   string convertedString = utils::unparse_classad(convertedAd);
   string command_str(convertedString);
   edglog(debug)<<"Converted string (written in filelist): "
                <<toFormattedString(convertedAd)<<endl;
   string regjdl = (jdlpath != "") ? jdlpath : jdl;
   try {
      f_forward(*m_jobdir, command_str);
      wmpeventlogger->logEvent(eventlogger::WMPEventLogger::LOG_ENQUEUE_OK, "",
                               true, true, filelist_global.c_str(), regjdl.c_str());
      edglog(debug)<<"LB Logged jdl/path: "<<jdl<<endl;
      edglog(debug)<<"Submit EnQueued OK"<<endl;
   } catch (exception& e) {
      // LogEnQueued FAIL if exception occurs
      wmpeventlogger->logEvent(eventlogger::WMPEventLogger::LOG_ENQUEUE_FAIL,
                               e.what(), true, true, "", regjdl.c_str());
      edglog(critical)<<"Submit EnQueued FAIL"<< e.what() << endl;
   }
   edglog(debug)<<"Submit Forwarded"<<endl;
   GLITE_STACK_CATCH();
}

void
WMP2WM::cancel(const string& jobid, const string& seq_code)
{
   GLITE_STACK_TRY("cancel()");
   edglog_fn("wmp2wm::cancel");

   edglog(debug)<<"Forwarding Cancel Request..."<<endl;

   classad::ClassAd jdlAd = wmsutilities::cancel_command_create(jobid);
   jdlAd.DeepInsertAttr(jdlAd.Lookup("arguments"), JDL::LB_SEQUENCE_CODE,
                        seq_code);
   string convertedString = utils::unparse_classad(jdlAd);

   edglog(debug)<<"Converted string (written in filelist): "
                <<toFormattedString(jdlAd)<<endl;

   string command_ad(convertedString);
   f_forward(*m_jobdir, command_ad);
   edglog(debug)<<"Cancel Forwarded"<<endl;

   GLITE_STACK_CATCH();
}

string
computePipePath(const string& listmatch_path)
{
   GLITE_STACK_TRY("computePipePath()");
   edglog_fn("wmp2wm::computePipePath");

   edglog(info)<<"Inserting ListMatch Pipe Name..."<<endl;

   char time_string[20];
   wmsutilities::oedgstrstream s;
   struct timeval tv;
   struct tm* ptm;
   long milliseconds;

   // Obtain the time of day, and convert it to a tm struct
   gettimeofday (&tv, NULL);
   ptm = localtime (&tv.tv_sec);

   //Format the date and time, down to a single second
   strftime(time_string, sizeof (time_string), "%Y%m%d%H%M%S", ptm);
   milliseconds = tv.tv_usec / 1000;
   s<<listmatch_path<<"/"<<getpid()<<"."<<string(time_string)
    <<milliseconds;
   return s.str();

   GLITE_STACK_CATCH();
}

string
insertUserProxy(classad::ClassAd& convertedAd, const string& pipe_path,
                const string& listmatchpath, const string& credentials_file)
{
   GLITE_STACK_TRY("insertUserProxy()");
   edglog_fn("wmp2wm::insertUserProxy");

   string local_path;
   string cmd_name;

   string::size_type idx = pipe_path.rfind("/");
   if (idx != string::npos) {
      local_path = pipe_path.substr(idx + 1);
   }
   local_path.assign(listmatchpath + "/user.proxy." + local_path);
   wmputilities::fileCopy(credentials_file, local_path);

   edglog(debug)<<"Inserting user proxy path: "<<local_path<<endl;
   convertedAd.DeepInsertAttr(static_cast<classad::ClassAd*>(
                                 convertedAd.Lookup("arguments"))->Lookup("ad"), JDLPrivate::USERPROXY,
                              local_path);

   return local_path;

   GLITE_STACK_CATCH();
}

void
WMP2WM::match(const string& jdl, const string& filel, const string& proxy,
              void * result)
{
   GLITE_STACK_TRY("match()");
   edglog_fn("wmp2wm::match");

   edglog(debug)<<"Forwarding Match Request..."<<endl;

   classad::ClassAd* reqAd(utils::parse_classad(jdl));

   // Third argument -1 means unlimited result
   // If you want to include Broker Info, set latest argument to true
   classad::ClassAd convertedAd =
      wmsutilities::match_command_create(*reqAd, filel, -1, false);
   string pipepath = computePipePath(filel);
   convertedAd.DeepInsertAttr(convertedAd.Lookup("arguments"), "file",
                              pipepath);
   string local_path = insertUserProxy(convertedAd, pipepath, filel, proxy);

   string convertedString = utils::unparse_classad(convertedAd);
   edglog(debug)<<"Converted string (written in filelist): "
                <<toFormattedString(convertedAd)<<endl;
   string command_ad(convertedString);

   f_forward(*m_jobdir, command_ad);
   edglog(debug)<<"Match Forwarded"<<endl;

   string resultlist = commands::listjobmatchex(local_path, pipepath);
   edglog(debug)<<"Match result: "<<resultlist<<endl;
   /* Listmatch returned classad, eg:
   [
        match_result =
           {
              "lxb2022.cern.ch:2119/blah-lsf-jra1_high,3",
              "lxb2022.cern.ch:2119/blah-lsf-jra1_low,3",
              "lxb2077.cern.ch:2119/blah-pbs-infinite,3",
              "lxb2077.cern.ch:2119/blah-pbs-long,3",
              "lxb2077.cern.ch:2119/blah-pbs-short,3"
           };
        reason = "ok"
    ]*/

   string errormsg = "";
   string reason = "";
   jdl::Ad ad;
   if ((resultlist != "")) {
      ad.fromString(resultlist);
      if (!ad.hasAttribute(LISTMATCH_REASON)) {
         errormsg = "Error during matchmaking: no error reason";
      } else if (((reason = ad.getString(LISTMATCH_REASON)) != LISTMATCH_REASON_OK)
                 && (reason != LISTMATCH_REASON_NO_MATCH)) {
         errormsg = "Error during matchmaking: " + reason;
      } else if (!ad.hasAttribute(LISTMATCH_MATCH_RESULT)) {
         errormsg = "Error during matchmaking: no match result attribute";
      }
   } else {
      errormsg = "Error during matchmaking: empty result list";
   }
   if (errormsg != "") {
      edglog(error)<<errormsg<<endl;
      throw wmputilities::FileSystemException(__FILE__, __LINE__,
                                              "wmp2wm::match()", wmputilities::WMS_FILE_SYSTEM_ERROR,
                                              errormsg + "\n(please contact server administrator)");
   }

   // TBC If reason ok can I assume attribute LISTMATCH_MATCH_RESULT
   // is present?? (even if empty)
   vector<StringAndLongType*> *file = new vector<StringAndLongType*>;
   StringAndLongType *item = NULL;
   vector<std::string> items = ad.getStringValue(LISTMATCH_MATCH_RESULT);

   string couple;
   string ce_id;
   long int rank;
   std::string::size_type pos = 0;
   for (unsigned int i = 0; i < items.size(); i++) {
      couple = items[i];
      pos = couple.rfind(",");
      if (pos != string::npos) {
         ce_id = couple.substr(0, pos);
         rank = atol(couple.substr(pos + 1, string::npos).c_str());
         item = new StringAndLongType();
         item->name = ce_id;
         item->size = rank;
         file->push_back(item);
      }
   }

   StringAndLongList *list = new StringAndLongList();
   list->file = file;
   ((jobListMatchResponse*)result)->CEIdAndRankList = list;

   GLITE_STACK_CATCH();
}


} // namespace server
} // namespace wmproxy
} // namespace wms
} // namespace glite


