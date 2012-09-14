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
#include "glite/wms/ism/purchaser/ism-ii-purchaser.h"
#include "glite/wms/ism/purchaser/ism-rgma-purchaser.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/logger_utils.h"
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
//    { 'b', no_argument, "black-list",  "\t use configuration file black-list." },
    { 's', no_argument, "show-ad",     "\t display the complete classad description of any entry." },
    { 'v', no_argument, "verbose",     "\t be verbose" },
    { 'l', no_argument, "verbose",     "\t be verbose on log file" }
};
// if both -v and -l are used, -l has priority

namespace {
ism_type the_ism[2];
ism_mutex_type the_ism_mutex[2];
}
int main(int argc, char* argv[]) {

  set_ism(the_ism,the_ism_mutex,ce);
  set_ism(the_ism,the_ism_mutex,se);


  std::vector<LineOption> optvec( options, options + sizeof(options)/sizeof(LineOption) );
  LineParser options( optvec, 0 );
  string conf_file;
  try {
    options.parse( argc, argv );
    conf_file.assign( options.is_present('c') ? options['c'].getStringValue() : "glite_wms.conf" );

    Configuration config( conf_file.c_str(), ModuleType::workload_manager);

    NSConfiguration const* const ns_config(config.ns());
    WMConfiguration const* const wm_config(config.wm());

    if( options.is_present('v') && !options.is_present('l'))   logger::edglog.open(std::clog, glite::wms::common::logger::debug);

    if( options.is_present('l') ) logger::edglog.open(ns_config->log_file(), glite::wms::common::logger::debug);
    
    //ism_rgma_purchaser icp(ns_config->ii_timeout(), once);
    ism_rgma_purchaser icp(wm_config->rgma_query_timeout(), once);

//    if( options.is_present('b') ) icp.skip_predicate(is_in_black_list(wm_config->ism_black_list()));

    icp();


    ism_mutex_type::scoped_lock ce_l(get_ism_mutex(glite::wms::ism::ce));

    for (ism_type::iterator pos=get_ism(glite::wms::ism::ce).begin();
      pos!= get_ism(glite::wms::ism::ce).end(); ++pos) {

      if (options.is_present('s')) {
      
        classad::ClassAd  ad_ism_dump;
        ad_ism_dump.InsertAttr("id", pos->first);
        ad_ism_dump.InsertAttr("update_time", boost::tuples::get<0>(pos->second));
        ad_ism_dump.InsertAttr("expiry_time", boost::tuples::get<1>(pos->second));
        ad_ism_dump.Insert("info", boost::tuples::get<2>(pos->second).get()->Copy());
        Debug( ad_ism_dump );
//to be deleted
Debug("-----------------------------------------------------------------");
Debug("-----------------------------------------------------------------");
     }
     else {
        Debug( pos->first );
//to be deleted
Debug("-----------------------------------------------------------------");
     }
    }

    ism_mutex_type::scoped_lock se_l(get_ism_mutex(glite::wms::ism::se));

    for (ism_type::iterator pos=get_ism(glite::wms::ism::se).begin();
      pos!= get_ism(glite::wms::ism::se).end(); ++pos) {

      if (options.is_present('s')) {

        classad::ClassAd  ad_ism_dump;
        ad_ism_dump.InsertAttr("id", pos->first);
        ad_ism_dump.InsertAttr("update_time", boost::tuples::get<0>(pos->second));
        ad_ism_dump.InsertAttr("expiry_time", boost::tuples::get<1>(pos->second));
        ad_ism_dump.Insert("info", boost::tuples::get<2>(pos->second).get()->Copy());
        Debug( ad_ism_dump );
//to be deleted
Debug("-----------------------------------------------------------------");
Debug("-----------------------------------------------------------------");
     }
     else {
        Debug( pos->first );
//to be deleted
Debug("-----------------------------------------------------------------");
     }
    }
    

  } 
  catch ( LineParsingError &er ) {
    cerr << er << endl;
//    Debug(er);
    exit( er.return_code() );
  }
  catch( CannotConfigure &er ) {
    cerr << er << endl;
//    Debug(er);
  }
  catch( ... ) {
    cout << "Uncaught exception..." << endl;
//    Debug("Uncaught exception...");
  }
}
