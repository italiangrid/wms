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

//
// File: wmproxy.cpp
// Author: Giuseppe Avellino <egee@datamat.it>
//

#include <string>
#include <signal.h>  // sig_atomic
#include "soapH.h"

// gSOAP
#include "WMProxy.nsmap"
#include "wmproxyserve.h"
#include "soapWMProxyObject.h"

// Boost singleton
#include <boost/pool/detail/singleton.hpp>

// Logging
#include "utilities/logging.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

// Configuration
#include "glite/wms/common/configuration/ModuleType.h"
#include "glite/wms/common/configuration/exceptions.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"

#include "configuration.h"
#include "gsoapfaultmanipulator.h"


#include "utilities/utils.h" // waitForSeconds()

// Exceptions
#include "glite/wmsutils/exception/Exception.h"

#include "signalhandler.h"

// Global variable for configuration
WMProxyConfiguration conf;

// Global variables for configuration attributes (ENV dependant)
std::string filelist_global;

namespace logger        = glite::wms::common::logger;
namespace wmsexception  = glite::wmsutils::exception;
namespace wmputilities  = glite::wms::wmproxy::utilities;
namespace configuration = glite::wms::common::configuration;

using namespace std;
namespace server = glite::wms::wmproxy::server;

const string opt_conf_file("glite_wms.conf");

void
sendFault(WMProxyService& proxy, const string& method, const string& msg, int code)
{
   setSOAPFault(&proxy, code, method, time(NULL), code, msg);
   soap_print_fault(&proxy, stderr);
   soap_send_fault(&proxy);
   soap_destroy(&proxy);
   soap_end(&proxy);
   soap_done(&proxy);
}

int
main(int argc, char* argv[])
{
   server::initsignalhandler();
   try {
      extern WMProxyConfiguration conf;
      conf = boost::details::pool::singleton_default<WMProxyConfiguration>
             ::instance();

      fstream edglog_stream;
      conf.init(opt_conf_file,
                configuration::ModuleType::workload_manager_proxy);
      string log_file = conf.wmp_config->log_file();
      // Checking for log file
      if (!log_file.empty()) {
         if (!ifstream(log_file.c_str())) {
            ofstream(log_file.c_str());
         }
         edglog_stream.open(log_file.c_str(), ios::in | ios::out
                            | ios::ate);
      }
      if (edglog_stream) {
         logger::threadsafe::edglog.open(edglog_stream,
                                         static_cast<logger::level_t>(conf.wmp_config->log_level()));
      }

      edglog_fn("wmproxy::main");
      edglog(info)
            <<"------- Starting Server Instance -------"
            <<endl;

      extern string filelist_global;
      filelist_global
         = configuration::Configuration::instance()->wm()->input();

      server::servedrequestcount_global = 0;

      edglog(info) << "WM proxy serving process started" << endl;
      edglog(info) <<"---------------------------------------" <<endl;

      //openlog("glite_wms_wmproxy_server", LOG_PID || LOG_CONS, LOG_DAEMON);

      server::WMProxyServe proxy;
      proxy.serve();

      //closelog();
      edglog(info)<<"Exiting WM proxy serving process ..."<<endl;

   } catch (configuration::CannotOpenFile& file) {
      string msg = "Cannot open file: " + string(file.what());
      edglog(fatal)<<msg<<endl;
      WMProxyService proxy;
      sendFault(proxy, "main", msg, -5);
      return 1;
   } catch (configuration::CannotConfigure& error) {
      string msg = "Cannot configure: " + string(error.what());
      edglog(fatal)<<msg<<endl;
      WMProxyService proxy;
      sendFault(proxy, "main", msg, -4);
      return 1;
   } catch (wmsexception::Exception& exc) {
      string msg = "Exception caught: " + string(exc.what());
      edglog(fatal)<<msg<<endl;
      WMProxyService proxy;
      sendFault(proxy, "main", msg, -3);
      return 1;
   } catch (exception& ex) {
      string msg = "Standard Exception caught: " + string(ex.what());
      edglog(fatal)<<msg<<endl;
      WMProxyService proxy;
      sendFault(proxy, "main", msg, -2);
      return 1;
   } catch (...) {
      string msg = "Uncaught exception";
      edglog(fatal)<<msg<<endl;
      WMProxyService proxy;
      sendFault(proxy, "main", msg, -1);
      return 1;
   }

   return 0;
}
