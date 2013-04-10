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
#include <iostream>
#include <fstream>
#include <sys/types.h>          // getpid(), getpwnam()
#include <unistd.h>             // getpid(), usleep
#include <pwd.h>                // getpwnam()
#include <cstdio>               // popen()
#include <cstdlib>              // atoi()
#include <csignal>
#include <getopt.h>
#include <cerrno>
#include <vector>

#include "common/src/configuration/ICEConfiguration.h"
#include "common/src/configuration/WMConfiguration.h"
#include "common/src/configuration/CommonConfiguration.h"

#include "utils/CreamJob.h"
#include "db/GetAllJobs.h"
#include "db/GetJobByGid.h"
#include "db/Transaction.h"
#include "utils/CreamProxyMethod.h"
#include "utils/IceConfManager.h"
#include "utils/IceUtils.h"

#include "glite/ce/cream-client-api-c/JobFilterWrapper.h"
#include "glite/ce/cream-client-api-c/ResultWrapper.h"
#include "glite/ce/cream-client-api-c/VOMSWrapper.h"

#include <boost/scoped_ptr.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

#include <list>

/* workaround for gsoap 2.7.13 */
#include "glite/ce/cream-client-api-c/cream_client_soapH.h"
SOAP_NMAC struct Namespace namespaces[] = {};

using namespace std;
//using namespace glite::ce::cream_client_api;
using namespace glite::wms::ice;

//namespace iceUtil   = glite::wms::ice::util;
//namespace iceDb     = glite::wms::ice::db;
//namespace po        = boost::program_options;
//namespace fs        = boost::filesystem;
//namespace api_util  = glite::ce::cream_client_api::util;
//namespace cream_api = glite::ce::cream_client_api::soap_proxy;

bool get_job( const string& gid, util::CreamJob& );
void get_all_jobs( list< util::CreamJob >& jobs );

int main(int argc, char*argv[]) 
{

  for( int i=0; i<100000; ++i) {
    //string S = glite::wms::ice::util::IceUtils::to_string( 4723453.23F );
    string S = boost::lexical_cast<string>( 4723453.23424 );
  }
  return 0;
  char*   confile = "glite_wms.conf";
  bool    cancel_all = false;
  string  gridjobid;
  int     option_index = 0;
  bool    log_abort = false;
  string  abort_reason;
  char    c;
  bool    from_file = false;
  string  inputlist;

  while(1) {
    static struct option long_options[] = {
      {"conf", 1, 0, 'c'},
      {"all", 0, 0, 'a'},
      {"log-abort", 1, 0, 'l'},
      {"from-file", 1, 0, 'f'},
      {0, 0, 0, 0}
    };
    c = getopt_long(argc, argv, "c:al:f:", long_options, &option_index);
    if ( c == -1 )
      break;
    
    switch(c) {
    case 'c':
      confile = optarg;
      break;
    case 'a':
      cancel_all = true;
      break;
    case 'l':
      log_abort = true;
      abort_reason = optarg;
      break;
    case 'f':
      from_file = true;
      inputlist = optarg;
      break;
    default:
      cerr << "Type " << argv[0] << " -h for help" << endl;
      exit(1);
    }
  }
  
  if( ( argv[optind] && from_file ) ||
      ( argv[optind] && cancel_all) ||
      ( cancel_all   && from_file ) )
    {
      cerr << "Only one of --all, --from-file <pathfile>, <gridjobid> "
	   << "must be specified in the command line" << endl;
      return 1;
    }

  if (!argv[optind] && !from_file && !cancel_all )
    {
      cerr << "Must specify one of the options --all, --from-file <pathfile>, <gridjobid>" << endl;
      return 1;
    }

  if( argv[optind] )
    gridjobid = string(argv[optind]);

  util::IceConfManager::init( confile );
  try{
    util::IceConfManager::instance();
  }
  catch(util::ConfigurationManager_ex& ex) {
    cerr << "glite-wms-ice-rm::main - ERROR: " << ex.what() << endl;
    exit(1);
  }
  
  std::list< util::CreamJob > jobList;
  //iceUtil::iceLBLogger *lb_logger( iceUtil::iceLBLogger::instance() );
  
  /**
     If specified by the user
     read gridjobids to cancel from file
  */
  if( from_file ) {
    
    FILE* in = fopen( inputlist.c_str(), "r" );
    if(!in) {
      cerr << "Error opening [" << inputlist << "]: " 
	   << strerror( errno ) << endl;
      return 1;
    }
    fclose( in );
    ifstream is(inputlist.c_str(), ios_base::in );
    string linestring;
    
    while( is >> linestring ) {
    
      util::CreamJob aJob;
      if( get_job( linestring, aJob ) )
        jobList.push_back( aJob );
      else
        cerr << "GriJobID ["<< linestring
	     << "] is not found in the ICE database. Skipping cancel for it..."
	     << endl;
    }
  }
  
  if( cancel_all ) {   
    get_all_jobs( jobList );       
  } 

  if( !gridjobid.empty() ) {
    util::CreamJob aJob;
    if( get_job( gridjobid, aJob ) )
      jobList.push_back( aJob );
    else {
      cerr << "GriJobid "<< gridjobid << " is not found in the ICE database." << endl;
      return 1;
    }
  }
  
  std::list< util::CreamJob >::iterator jit;
  jit = jobList.begin( );
  while( jit != jobList.end( ) ) {
    
    vector<glite::ce::cream_client_api::soap_proxy::JobIdWrapper> toCancel;

    toCancel.push_back( glite::ce::cream_client_api::soap_proxy::JobIdWrapper(jit->cream_jobid(), 
									      jit->cream_address(), 
									      std::vector<glite::ce::cream_client_api::soap_proxy::JobPropertyWrapper>())
			);
    
    glite::ce::cream_client_api::soap_proxy::JobFilterWrapper req( toCancel, vector<string>(), -1, -1, "", "");
    glite::ce::cream_client_api::soap_proxy::ResultWrapper res;

    cout << "Sending JobCancel to CREAM for job [" 
	 << jit->grid_jobid() << "] -> [" << jit->complete_cream_jobid() << "]" << endl;
    
    glite::ce::cream_client_api::soap_proxy::VOMSWrapper V( jit->user_proxyfile(),  !::getenv("GLITE_WMS_ICE_DISABLE_ACVER") );

    if( !V.IsValid( ) ) {
      cerr <<"*** For job [" << jit->grid_jobid() << "] -> ["
	   << jit->complete_cream_jobid() << "]"
	   << " the proxyfile ["
	   << jit->user_proxyfile() 
	   << "] is not valid: "
	   << V.getErrorMessage()
	   << ". Skipping cancellation of this job. "
	   << endl;
    } else {
      try {
        util::CreamProxy_Cancel( jit->cream_address(), jit->user_proxyfile(), &req, &res ).execute( 3 );
	cout << "CANCELLED JOB ["<< jit->grid_jobid() << "]" << endl;
      } catch(exception& ex) {
        cerr << "*** Error cancelling job [" << jit->grid_jobid() << "]: " << ex.what() << endl;
      }
    }
    
    ++jit;
  } 
  return 0;
}

//_______________________________________________________________________________
bool get_job( const string& gid, util::CreamJob& target )
{
  //list<string> fields_to_get;
  //list<vector<string> > result;
  //list<pair<string, string> > clause;
  
  //  fields_to_get.push_back("creamjobid");	//0
  // fields_to_get.push_back("creamurl");          //1
  //fields_to_get.push_back("userproxy");		//2
  //fields_to_get.push_back("gridjobid");         //3
  
  //clause.push_back( make_pair("gridjobid", gid ) );
  
  //iceDb::GetFields getter( fields_to_get, clause, result, "glite-wms-ice-rm::get_job" );
  db::GetJobByGid getter( gid, "glite-wms-ice-rm::get_job" );
  db::Transaction tnx( false, false );
  tnx.execute( &getter );
  
  if( !getter.found() ) return false;

  //  if(result.begin() == result.end()) return false;
  
  target = getter.get_job();//*result.begin( );
  
  return true;
}

//_______________________________________________________________________________
void get_all_jobs( list< util::CreamJob >& jobs )
{
//   list<string> fields_to_get;
//   list<vector<string> > result;
  
//   fields_to_get.push_back("creamjobid");	//0
//   fields_to_get.push_back("creamurl");          //1
//   fields_to_get.push_back("userproxy");		//2
//   fields_to_get.push_back("gridjobid");		//3
  
  
  //iceDb::GetFields getter( fields_to_get, list<pair<string,string> >(), result, "glite-wms-ice-rm::get_all_jobs" );
  db::GetAllJobs getter( &jobs, 0, 0, "glite-wms-ice-rm::get_all_jobs", false );
  db::Transaction tnx( false, false );
  tnx.execute( &getter );
  
  //  if(result.begin() == result.end()) return;
  
  //  list<vector<string> >::iterator jit = result.begin( );
  //  while( jit != result.end( ) )
  //  {
  //    jobs.push_back( *jit );
  //    
  //    ++jit;
  //  }
}
