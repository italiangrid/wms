/* LICENSE:
Copyright (c) Members of the EGEE Collaboration. 2010. 
See http://www.eu-egee.org/partners/ for details on the copyright
holders.  

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

   http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. 
See the License for the specific language governing permissions and 
limitations under the License.

END LICENSE */

#include "classad_distribution.h"
#include "utils/IceConfManager.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "common/src/configuration/ICEConfiguration.h"
#include "utils/Request_source.h"
#include "utils/Request.h"
#include "utils/Request_source_jobdir.h"

#include <string>
#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include <unistd.h>
#include <exception>
#include <fstream>
#include <sys/time.h>

#include <boost/program_options.hpp>
#include <boost/format.hpp>
//#include <boost/algorithm/string.hpp>

/* workaround for gsoap 2.7.13 */
#include "glite/ce/cream-client-api-c/cream_client_soapH.h"
SOAP_NMAC struct Namespace namespaces[] = {};

namespace utils = glite::wms::common::utilities;
namespace iceUtil = glite::wms::ice::util;
using namespace std;
namespace po = boost::program_options;

int getRandom(double);

  /**
   * Usage: putFL <filelist> <jdlfile>
   *
   */

int main(int argc, char* argv[]) {

    string opt_conf_file;
    string opt_ad;

    po::positional_options_description p;
    p.add("ad", -1);

    po::options_description desc("Usage");
    desc.add_options()
        ("help", "display this help and exit")
        (
         "conf",
         po::value<string>(&opt_conf_file)->default_value("glite_wms.conf"),
         "configuration file"
         )
        (
         "noproc", "Do not process the input classad (put it as-is in ICE filelist)"
         )
        ( 
         "ad",
         po::value<string>(&opt_ad)->default_value("ad.jdl"),
         "ClassAd file to put into the filelist"
         )
        ;
    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), vm);
    } catch( std::exception& ex ) {
        cerr << "There was an error parsing the command line. "
             << "Error was: " << ex.what() << endl
             << "Type " << argv[0] 
             << " --help for the list of available options"
             << endl;
        exit( 1 );
    }
    po::notify(vm);

    if ( vm.count("help") ) {
        cout << desc << endl;
        return 0;
    }
    
    //
    // Init ICE conf file
    iceUtil::IceConfManager::init( opt_conf_file );
    try{
        iceUtil::IceConfManager::instance();
    }
    catch(iceUtil::ConfigurationManager_ex& ex) {
        cerr << "putFL::main() - ERROR: " << ex.what() << endl;
        exit(1);
    }
    
    // int j, howmany;
    //    utils::FileList<string> fl;

    iceUtil::Request_source* input_queue = new iceUtil::Request_source_jobdir( iceUtil::IceConfManager::instance()->getConfiguration()->ice()->input(), true);//( iceUtil::Request_source_factory::make_source_input_ice() );

    ifstream is( opt_ad.c_str() );
    string a_line;
    string Buf;

    while(is.peek()!=EOF) {
        std::getline(is, a_line, '\n');
        Buf += a_line;
    }

    is.close();

    string request; // request to put in the filelist

    if ( vm.count("noproc") ) {
        request = Buf;
    } else {        
        classad::ClassAdParser parser;
        classad::ClassAd *ad = parser.ParseClassAd( Buf.c_str() );
        
        if(!ad) {
            cerr << "Error parsing classad: " << Buf <<endl;
            exit(1);
        }
        struct timeval T;
        gettimeofday(&T, 0);
        
        ad->InsertAttr("edg_jobid", boost::str( boost::format( "https://grid005.pd.infn.it:9000/000%1%.%2%" ) % time(NULL) % T.tv_usec ) );
        
        classad::ClassAdUnParser unp;
        
        Buf.clear();
        unp.Unparse(Buf, ad);
        
        request = "[arguments = [ jobad = " + Buf
            + " ]; command = \"Reschedule\"; protocol = \"1.0.0\" ]";
    }


    string filelist_name( iceUtil::IceConfManager::instance()->getConfiguration()->ice()->input() );

    cout << "Adding JDL <" << request << "> to input queue " << filelist_name << endl;

    input_queue->put_request( request );

}


//______________________________________________________________________________
int getRandom(double max) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  srand((unsigned int)tv.tv_usec);
  int j= 1 + (int)(max*rand()/(RAND_MAX+1.0));
  return j;
}
