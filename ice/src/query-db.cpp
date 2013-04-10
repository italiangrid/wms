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

#include <list>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <getopt.h>

#include "glite/ce/cream-client-api-c/job_statuses.h"

#include "db/Transaction.h"
#include "db/GetAllJobs.h"
#include "utils/IceConfManager.h"
#include "utils/CreamJob.h"

#include "boost/algorithm/string.hpp"

/* workaround for gsoap 2.7.13 */
#include "glite/ce/cream-client-api-c/cream_client_soapH.h"
SOAP_NMAC struct Namespace namespaces[] = {};

using namespace std;
using namespace glite::wms::ice;

void printhelp( void ) {
  cout << "USAGE: glite-wms-ice-query-db --conf|-c <WMS CONFIGURATION FILE> [options]" << endl;
  cout << endl << "options: " << endl;
  cout << "  --verbose|-v\t\tVerbose output (print each db's record" << endl;
  cout << "  --status-filter|-s\tSelect only records in which the status column is one"<<endl
       << "  \t\t\tof those specified as option argument; more states can"<<endl
       << "  \t\t\tbe ',' separated and they must be:"<<endl;
  cout << "\t\t\tREGISTERED"<<endl;
  cout << "\t\t\tPENDING"<<endl;
  cout << "\t\t\tIDLE"<<endl;
  cout << "\t\t\tRUNNING"<<endl;
  cout << "\t\t\tREALLY_RUNNING"<<endl;
  cout << "\t\t\tCANCELLED"<<endl;
  cout << "\t\t\tHELD"<<endl;
  cout << "\t\t\tABORTED"<<endl;
  cout << "\t\t\tDONE_OK"<<endl;
  cout << "\t\t\tDONE_FAILED"<<endl;
  cout << "\t\t\tUNKNOWN"<<endl;
  cout << "\t\t\tPURGED"<<endl<<endl;
  cout << "  --userdn|-u\t\tPrint the USERDN column of the job table" << endl;
  cout << "  --creamjobid|-C\tPrint the CREAM JOB ID column of the job table" << endl;
  cout << "  --gridjobid|-G\tPrint the GRID JOB ID column of the job table" << endl;
  cout << "  --userproxy|-p\tPrint the USER PROXY column of the job table" << endl;
  cout << "  --cream-url|-r\tPrint the CREAM URL column of the job table" << endl;
  cout << "  --myproxy-url|-m\tPrint the MYPROXY URL column of the job table" << endl;
  cout << "  --status|-S\t\tPrint the STATUS column of the job table" << endl;
  cout << "  --lease-id|-L\t\tPrint the LEASE-ID column of the job table" << endl;
  cout << "  --delegation-id|-D\tPrint the DELEGATION-ID column of the job table" << endl;
  cout << "  --worker-node|-w\tPrint the WORKER-NODE column of the job table" << endl;
  cout << "  --help|-h\t\tPrint this help" << endl;
}


int main( int argc, char* argv[] )
{
  string           conf_file( "glite_wms.conf" );
  string           states("");
  string           sqlquery("");
  bool             all_status(true);
  bool             debug( false );
  bool             verbose( false );
  int              option_index( 0 );

  bool wn       = false;
  bool status   = false;
  bool lease    = false;
  bool did      = false;
  bool curl     = false;
  bool myurl    = false;
  bool ccid     = false;
  bool gid      = false;
  bool seq_code = false;
  bool jdl      = false;
  bool modif_jdl= false;
  bool userdn   = false;
  bool pxfile   = false;

  while( 1 ) {
    int c;
    static struct option long_options[] = {
      {"help", 0, 0, 'h'},
      {"status-filter", 1, 0, 's'},
      {"conf", 1, 0, 'c'},
      /*****************************/
      {"userdn", 0, 0, 'u'},
      {"creamjobid",0,0,'C'},
      {"gridjobid",0,0,'G'},
      {"userproxy",0,0,'p'},
      {"cream-url",0,0,'r'},
      {"myproxy-url",0,0,'m'},
      {"status",0,0,'S'},
      {"lease-id",0,0,'L'},
      {"delegation-id",0,0,'D'},
      {"worker-node",0,0,'w'},
      {"verbose", 0, 0, 'v'},
      {"jdl", 0, 0, 'j'},
      {"modified-jdl", 0, 0, 'J'},
      {"sequence-code", 0, 0, 'q'},
      {0, 0, 0, 0}
    };
    c = getopt_long(argc, argv, "hs:c:uCGprmSLDwvjJq", long_options, &option_index);
    
    if ( c == -1 )
      break;
    switch(c) {
     case 'q':
       //fields_to_retrieve.push_back( glite::wms::ice::util::CreamJob::sequence_code_field() );
       seq_code = true;
       break;
     case 'j':
       //fields_to_retrieve.push_back( glite::wms::ice::util::CreamJob::jdl_field() );
       jdl = true;
       break;
     case 'J':
       //fields_to_retrieve.push_back( glite::wms::ice::util::CreamJob::modified_jdl_field() );
       modif_jdl = true;
       break;

    case 'h':
      printhelp();
      exit(0);
      break;
    case 'd':
      debug = true;
      break;
    case 's':
      states = string(optarg);
      all_status = false;
      break;
    case 'c':
      conf_file = string(optarg);
      break;
    case 'u':
      //      fields_to_retrieve.push_back( glite::wms::ice::util::CreamJob::user_dn_field() );
      userdn = true;
      break;
    case 'C':
      //      fields_to_retrieve.push_back( glite::wms::ice::util::CreamJob::complete_cream_jobid_field() );
      ccid = true;
      break;
    case 'G':
      //      fields_to_retrieve.push_back( glite::wms::ice::util::CreamJob::grid_jobid_field() );
      gid = true;
      break;
    case 'p':
      //      fields_to_retrieve.push_back( glite::wms::ice::util::CreamJob::user_proxyfile_field() );
      pxfile = true;
      break;
    case 'r':
      //      fields_to_retrieve.push_back( glite::wms::ice::util::CreamJob::cream_address_field() );
      curl = true;
      break;
    case 'm':
      //      fields_to_retrieve.push_back( glite::wms::ice::util::CreamJob::myproxy_address_field() );
      myurl = true;
      break;
    case 'S':
      //      status_pos = fields_to_retrieve.size();
      //      fields_to_retrieve.push_back( glite::wms::ice::util::CreamJob::status_field() );
      status = true;
      break;
    case 'L':
      //      fields_to_retrieve.push_back( glite::wms::ice::util::CreamJob::lease_id_field() );
      lease = true;
      break;
    case 'D':
      //      fields_to_retrieve.push_back( glite::wms::ice::util::CreamJob::delegation_id_field() );
      did = true;
      break;
    case 'w':
      //      fields_to_retrieve.push_back( glite::wms::ice::util::CreamJob::worker_node_field() );
      wn = true;
      break;
    case 'v':
      verbose = true;
      break;
    default:
      cerr << "Type " << argv[0] << " --help|-h for an help" << endl;
      exit(1);
    }
  }

  /**
   * Load valid job states strings
   */
  set<string> valid_states;
  for( int i=0; i<=11; ++i)
    valid_states.insert(glite::ce::cream_client_api::job_statuses::job_status_str[i]);
    
  /**
   * Check options
   */
  if(verbose) {
    bool at_least_one = curl || myurl || status || ccid || seq_code || lease || did || modif_jdl || userdn || pxfile || gid || wn || seq_code;
  
    if( !at_least_one) {
      cerr << "Options --verbose|-v requires at least one of the options "
           << "--userdn,--creamjobid,--gridjobid,--userproxy,--cream-url,"
	   << "--myproxy-url,--status,--lease-id,--delegation-id,"
	   << "--worker-node,--jdl,--modified-jdl,--sequence-code" << endl;
      exit(1);
    }
  }
  
  util::IceConfManager::init( conf_file );
  try{
    util::IceConfManager::instance();
  }
  catch(util::ConfigurationManager_ex& ex) {
    cerr << ex.what() << endl;
    exit(1);
  }

  list<util::CreamJob> result, filtered;

  db::GetAllJobs getter( &result, 0, 0, "queryDb::main", false );

  db::Transaction tnx(true,false);
  tnx.execute( &getter );
  
  set<string> filter_states;
  
  
  vector<string> SplitVec;
  boost::split( SplitVec, states, boost::is_any_of(","), boost::token_compress_on );
  
  //cout << "len=" << SplitVec.size() << endl;
  
  if(!states.empty()) {
  for(vector<string>::const_iterator sit = SplitVec.begin();
      sit != SplitVec.end();
      ++sit )
      {
        if(valid_states.find( *sit )==valid_states.end() ) {
	  cerr << "Invalid status name [" << *sit << "]."<<endl<<endl<<"Valid status names are: REGISTERED, PENDING, IDLE, RUNNING, REALLY-RUNNING, CANCELLED, HELD, DONE-OK, DONE-FAILED, PURGED, ABORTED, UNKNOWN"<<endl;
	  exit(1);
	}      
        filter_states.insert( *sit );
      }
  }
  
  if(states.empty()) {
    filtered = result;
  } else {
    
    list< util::CreamJob >::const_iterator it = result.begin();
    for( ; it!=result.end(); ++it ) {
      string currentStatus = glite::ce::cream_client_api::job_statuses::job_status_str[it->status( )];
      if(filter_states.find( currentStatus ) != filter_states.end()) {
        filtered.push_back( *it );
      }
    }
  }
  //bool at_least_one = curl || myurl || status || ccid || seq_code || lease || did || modif_jdl || userdn || pxfile || gid || wn || seq_code;
  
  if(verbose) {
    for( list< util::CreamJob >::const_iterator it=filtered.begin();
	 it != filtered.end();
	 ++it)
      {
	if( curl ) 
	  cout << "[" << it->cream_address( ) << "]";
	if( myurl )
	  cout << "[" << it->myproxy_address( ) << "]";
	if( status )
	  cout << "[" << glite::ce::cream_client_api::job_statuses::job_status_str[it->status( )] << "]";
	if( ccid )
	  cout << "[" << it->complete_cream_jobid( ) << "]";
	if( gid ) 
	  cout << "[" << it->grid_jobid( ) << "]";
	if(seq_code)
	  cout << "[" << it->sequence_code( ) << "]";
	if( lease )
	  cout << "[" << it->lease_id( ) << "]";
	if( did )
	  cout << "[" << it->delegation_id( ) << "]";
	if( jdl )
	  cout << "[" << it->jdl( ) << "]";
	if( modif_jdl )
	  cout << "[" << it->modified_jdl( ) << "]";
	if( userdn )
	  cout << "[" << it->user_dn( ) << "]" ;
	if( pxfile )
	  cout << "[" << it->user_proxyfile( ) << "]" ;
	if( wn )
	  cout << "[" << it->worker_node( ) << "]";
	
	cout << endl;
      }
    cout << endl << "------------------------------------------------" << endl;
  }
  cout << filtered.size() << " item(s) found" << endl;

}
