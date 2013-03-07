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

#include <classad_distribution.h>
#include <classad.h>

#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/exception.hpp>

#include "MatchingPipe_nb.h"

#include "glite/wms/common/utilities/edgstrstream.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/jdl/ManipulationExceptions.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/JDLAttributes.h"

#include "utilities/logging.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#include "utilities/wmpexception_codes.h"
#include "utilities/wmpexceptions.h"

namespace glite {
namespace wms {
namespace wmproxy {
namespace commands {

namespace logger        = glite::wms::common::logger;
namespace utilities     = glite::wms::common::utilities;
namespace utils      = glite::wmsutils::classads;
namespace requestad     = glite::jdl;
namespace wmputilities  = glite::wms::wmproxy::utilities;

using namespace std;

string
listjobmatchex(const string& credentials_file, string& pipepath)
{
   int iserror;
   std::string reason;
   std::string result;

   const std::string error_attribute("ERROR_ATTRIBUTE");
   const std::string reason_attribute("REASON_ATTRIBUTE");

   boost::scoped_ptr<classad::ClassAd> ad;
   std::vector<std::string> match_strings;
   std::vector<std::string> match_list;

   MatchingPipe_nb p(pipepath);
   try {
      if (p.open()) {
         result = p.read();
         p.close();
         // Removing User Proxy and Pipe
         remove(pipepath.c_str());
         remove(credentials_file.c_str());
      } else {
         edglog(severe) << "Could not open pipe: " << pipepath << std::endl;
      }
   } catch ( wmputilities::JobTimedoutException ex) {
      // Removing User Proxy and Pipe
      remove(pipepath.c_str());
      remove(credentials_file.c_str());
      edglog(severe) << "*****************************************************************" << result << std::endl;
      edglog(severe) << "* Exceptions caught while waiting for ListMatch result from WM."   << result << std::endl;
      edglog(severe) << "*****************************************************************" << result << std::endl;;
      throw wmputilities::JobTimedoutException (__FILE__, __LINE__, "jobListMatch()", wmputilities::WMS_OPERATION_TIMEDOUT,  "Timeout reached, command execution will be terminated now");
   } catch(std::string& ex) {
      // Removing User Proxy and Pipe
      remove(pipepath.c_str());
      remove(credentials_file.c_str());
      edglog(severe) << "*****************************************************************" << result << std::endl;
      edglog(severe) << "* Exceptions caught while waiting for ListMatch result from WM."   << result << std::endl;
      edglog(severe) << "* " << ex << std::endl;
      edglog(severe) << "*****************************************************************" << result << std::endl;
   } catch (...) {
      // Removing User Proxy and Pipe
      remove(pipepath.c_str());
      remove(credentials_file.c_str());
      edglog(severe) << "*****************************************************************" << result << std::endl;
      edglog(severe) << "* Exceptions caught while waiting for ListMatch result from WM."   << result << std::endl;
      edglog(severe) << "*****************************************************************" << result << std::endl;
   }


   try {
      ad.reset(utils::parse_classad(result) );
   } catch( utils::CannotParseClassAd& e ) {
      edglog(severe) << "Cannot Parse classAd: " << result << std::endl;
   }

   if (ad.get()->EvaluateAttrInt(error_attribute, iserror) ) {
      edglog( severe ) << "Error in listjobmatchex: Cannot evaluate int (error) attribute" << std::endl;
   }
   if (iserror) {
      if (ad.get()->EvaluateAttrString(reason_attribute, reason)) {
         match_list.push_back(reason);
      } else {
         match_list.push_back("Unknown reason");
      }
   }
   if (!utils::EvaluateAttrListOrSingle(*(ad.get()),"match_result", match_strings)) {
      edglog(critical) << "No Matching Resources found." << std::endl;
   }
   return result;
}

}}}}
