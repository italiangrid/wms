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
#include "glite/ce/monitor-client-api-c/CEMonitorBinding.nsmap"
#include "glite/wms/ism/ism.h"
#include "glite/wms/ism/purchaser/ism-ii-purchaser.h"
#include "glite/wms/ism/purchaser/ism-cemon-asynch-purchaser.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/LineParserExceptions.h"

#include <vector>
#include <string>
#include <exception>
#include <csignal>

static std::sig_atomic_t f_received_quit_signal;

extern "C" {
  void signal_handler(int)
  {
    f_received_quit_signal = 1;
  }
}

void signal_handling_init()
{
  std::signal(SIGPIPE, SIG_IGN);
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);
  std::signal(SIGQUIT, signal_handler);
}

bool received_quit_signal()
{
  return f_received_quit_signal != 0;
}

using namespace std;
using namespace glite::wms::ism;
using namespace glite::wms::ism::purchaser;
using namespace glite::wms::common::utilities;

namespace logger      = glite::wms::common::logger::threadsafe;

LineOption  options[] = {
    { 'p', 1, "port",      "\t specify the listening port." },
    { 'o', no_argument, "once"     "\t run the purchaser single shot." },
    { 's', no_argument, "show-ad", "\t display the complete classad description of any entry." },
    { 'v', no_argument, "verbose", "\t be verbose." }
};

int main(int argc, char* argv[])
{ 
  std::vector<LineOption> optvec( options, options + sizeof(options)/sizeof(LineOption) );
  LineParser options( optvec, 0 );
  int port(5120);
  exec_mode_t mode(loop);
  exit_predicate_type exit_predicate = exit_predicate_type();
  try {
    options.parse( argc, argv );
    if( options.is_present('v') ) logger::edglog.open(std::clog, glite::wms::common::logger::debug);
    if( options.is_present('p') ) port = options['p'].getIntegerValue();
    if( options.is_present('o') ) { mode = once; exit_predicate = received_quit_signal; }

    ism_cemon_asynch_purchaser icap("CE_MONITOR", port, mode, 30, exit_predicate);
    icap();
    ism_mutex_type::scoped_lock l(get_ism_mutex());

    for (ism_type::iterator pos=get_ism().begin();
      pos!= get_ism().end(); ++pos) {

      if (options.is_present('s')) {

        classad::ClassAd  ad_ism_dump;
        ad_ism_dump.InsertAttr("id", pos->first);
        ad_ism_dump.InsertAttr("update_time", boost::tuples::get<0>(pos->second));
        ad_ism_dump.InsertAttr("expiry_time", boost::tuples::get<1>(pos->second));
        ad_ism_dump.Insert("info", boost::tuples::get<2>(pos->second).get()->Copy());
        cout << ad_ism_dump;
     }
     else {
        cout << pos->first << endl;
     }
    }
  }
  catch ( LineParsingError &error ) {
    cerr << error << endl;
    exit( error.return_code() );
  }
  catch( std::exception& e ) {
    cout << e.what() << endl;
  }
  catch( ... ) {
    cout << "Uncaught exception..." << endl;
  }
  return 0;	
}
