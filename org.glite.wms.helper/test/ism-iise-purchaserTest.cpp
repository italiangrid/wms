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
#include "glite/wms/ism/ism.h"
#include "glite/wms/ism/purchaser/ism-iise-purchaser.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"
#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/LineParserExceptions.h"

#include <classad_distribution.h>
#include <boost/scoped_ptr.hpp>
#include <vector>

using namespace std;
using namespace glite::wms::ism;
using namespace glite::wms::ism::purchaser;
using namespace glite::wms::common::utilities;
using namespace glite::wms::common::configuration;

namespace logger = glite::wms::common::logger::threadsafe;

LineOption  options[] = {
    { 'c', 1, "conf_file", "\t use conf_file as configuration file." },
    { 'b', no_argument, "black-list",  "\t use configuration file black-list." },
    { 's', no_argument, "show-ad",     "\t display the complete classad description of any entry." },
    { 'v', no_argument, "verbose",     "\t be verbose." }
};

int main(int argc, char* argv[]) {

  std::vector<LineOption> optvec( options, options + sizeof(options)/sizeof(LineOption) );
  LineParser options( optvec, 0 );
  string conf_file;
  try {
    options.parse( argc, argv );
    conf_file.assign( options.is_present('c') ? options['c'].getStringValue() : "glite_wms.conf" );

    Configuration config( conf_file.c_str(), ModuleType::workload_manager);

    NSConfiguration const* const ns_config(config.ns());
    WMConfiguration const* const wm_config(config.wm());

    if( options.is_present('v') ) logger::edglog.open(std::clog, glite::wms::common::logger::debug);
    
    ism_iise_purchaser icp(ns_config->ii_contact(), ns_config->ii_port(),
        ns_config->ii_dn(),ns_config->ii_timeout(), once);

    if( options.is_present('b') ) icp.skip_predicate(is_in_black_list(wm_config->ism_black_list()));

    icp();
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
  catch( CannotConfigure &error ) {
    cerr << error << endl;
  }
  catch( ... ) {
    cout << "Uncaught exception..." << endl;
  }
}
