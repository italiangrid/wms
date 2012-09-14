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
#include "glite/wms/ism/purchaser/ism-file-purchaser.h"
#include "glite/wms/ism/purchaser/ism-cemon-purchaser.h"
#include "glite/wms/ism/purchaser/ism-rgma-purchaser.h"
#include "glite/wms/ism/purchaser/ism-ii-purchaser.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/LineParserExceptions.h"

#include <classad_distribution.h>
#include <boost/scoped_ptr.hpp>
#include <vector>

using namespace std;
using namespace glite::wms::ism;
using namespace glite::wms::ism::purchaser;
using namespace glite::wms::common::utilities;

namespace logger = glite::wms::common::logger::threadsafe;

LineOption  options[] = {
    { 's', no_argument, "show-ad",     "\t display the complete classad description of any entry." },
    { 'v', no_argument, "verbose",     "\t be verbose." }
};

namespace {
  ism_type the_ism[2];
  ism_mutex_type the_ism_mutex[2];
  boost::function<bool(int&, ad_ptr)> 
    create_dummy_update_fn() {
      return boost::function<bool(int&, ad_ptr)>();
    }
}

extern "C" 
void set_purchaser_entry_update_fns
(
 purchaser::ii::create_entry_update_fn_t*,
 purchaser::cemon::create_entry_update_fn_t*,
 purchaser::rgma::create_entry_update_fn_t*
);


int main(int argc, char* argv[]) {

  set_ism(the_ism,the_ism_mutex,ce);
  set_ism(the_ism,the_ism_mutex,se);

  std::vector<LineOption> optvec( options, options + sizeof(options)/sizeof(LineOption) );
  LineParser options( optvec, 1 );
  string dump_file;
  try {
    options.parse( argc, argv );
    dump_file.assign( options.get_arguments()[0] );

    if( options.is_present('v') ) logger::edglog.open(std::clog, glite::wms::common::logger::debug);
    
    ism_file_purchaser icp(dump_file, once);
    set_purchaser_entry_update_fns(
      create_dummy_update_fn,
      create_dummy_update_fn,
      create_dummy_update_fn
    );     
    icp();
    ism_mutex_type::scoped_lock _(get_ism_mutex(ce));
    ism_mutex_type::scoped_lock __(get_ism_mutex(se));

    for (ism_type::iterator pos=get_ism(ce).begin();
      pos!= get_ism(ce).end(); ++pos) {

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
  catch( ... ) {
    cout << "Uncaught exception..." << endl;
  }
}
