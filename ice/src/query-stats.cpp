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

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <getopt.h>

#include "glite/ce/cream-client-api-c/job_statuses.h"

#include "db/Transaction.h"
#include "db/GetStats.h"
#include "utils/IceConfManager.h"

#include "boost/algorithm/string.hpp"
#include "boost/regex.hpp"

/* workaround for gsoap 2.7.13 */
#include "glite/ce/cream-client-api-c/cream_client_soapH.h"
SOAP_NMAC struct Namespace namespaces[] = {};

using namespace std;
namespace iceUtil = glite::wms::ice::util;
namespace db = glite::wms::ice::db;
void printhelp( void ) {
  cout << "USAGE: queryDb --conf|-c <WMS CONFIGURATION FILE> [options]" << endl;
  cout << endl << "options: " << endl;
  cout << "  --from-date|-f\tSet the lower time limit to collect the stats from" << endl;
  cout << "  --to-date|t\tSet the upper time limit to collect the stats to" << endl;
  cout << "  --help|-h\t\tPrint this help" << endl;
}


int main( int argc, char* argv[] )
{
  string           conf_file( "glite_wms.conf" );
  int              option_index( 0 );
  
  string           fromdate;
  string           todate;
  
  time_t           from = 0;
  time_t           to = time(0);
  
  while( 1 ) {
    int c;
    static struct option long_options[] = {
      {"help", 0, 0, 'h'},
      {"conf", 1, 0, 'c'},
      {"from-date", 1, 0, 'f'},
      {"to-date",1,0,'t'},
      {0, 0, 0, 0}
    };
    c = getopt_long(argc, argv, "hc:f:t:", long_options, &option_index);
    
    if ( c == -1 )
      break;
    switch(c) {
    case 'h':
      printhelp();
      exit(0);
      break;
    case 'c':
      conf_file = string(optarg);
      break;
    case 'f':
      fromdate = string(optarg);
      break;
    case 't':
      todate = string(optarg);
      break;
    default:
      cerr << "Type " << argv[0] << " --help|-h for an help" << endl;
      exit(1);
    }
  }

  
  iceUtil::IceConfManager::init( conf_file );
  try{
    iceUtil::IceConfManager::instance();
  }
  catch(iceUtil::ConfigurationManagerException& ex) {
    cerr << ex.what() << endl;
    exit(1);
  }

  vector<pair<time_t, int> > result;

  boost::regex pattern;
  pattern = "^([0-9]{4})-([0-9]{1,2})-([0-9]{1,2})\\s([0-9][0-9]:[0-9][0-9]:[0-9][0-9])\\b";
  boost::cmatch what;

  if(!todate.empty()) {
    if( !boost::regex_match(todate.c_str(), what, pattern) ) {
      cerr << "Specified to date is wrong; must have the format 'YYYY-MM-DD HH:mm:ss'. Stop" << endl;
      exit(1);
    }
    struct tm tmp;
    strptime(todate.c_str(), "%Y-%m-%d %T", &tmp);
    to = mktime(&tmp);
  }
    
  if(!fromdate.empty()) {
    if( !boost::regex_match(fromdate.c_str(), what, pattern) ) {
      cerr << "Specified from date is wrong; must have the format 'YYYY-MM-DD HH:mm:ss'. Stop" << endl;
      exit(1);
    }
    struct tm tmp;
    strptime(fromdate.c_str(), "%Y-%m-%d %T", &tmp);
    from = mktime(&tmp);
  }

//  cout << "from="<<from << " - to=" << to << endl; 

  db::GetStats getter( result, from, to, "queryStats::main" );
  
  db::Transaction tnx(true, false);
  
  tnx.execute( &getter );
  
  map<int, unsigned long long> states;
  
  for(vector<pair<time_t, int> >::const_iterator it=result.begin();
      it != result.end();
      ++it)
      {
        states[it->second]++;
      }
      
  for(map<int, unsigned long long>::const_iterator it=states.begin();
      it!=states.end();
      ++it)
      {
        cout << "JOB_"<<glite::ce::cream_client_api::job_statuses::job_status_str[it->first]<<"="<<it->second<<endl;
      }

}
