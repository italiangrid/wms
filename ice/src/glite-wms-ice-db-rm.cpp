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
#include <exception>

#include "common/src/configuration/ICEConfiguration.h"
#include "common/src/configuration/WMConfiguration.h"
#include "common/src/configuration/CommonConfiguration.h"

#include "utils/CreamJob.h"
#include "db/GetJobByGid.h"
#include "db/Transaction.h"
#include "db/RemoveJobByGid.h"
#include "utils/IceConfManager.h"
#include "utils/DNProxyManager.h"
#include "utils/IceUtils.h"


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
using namespace glite::wms::ice;

bool get_job( const string& gid, util::CreamJob& );
void remove_job_fromdb( const util::CreamJob& );

int main(int argc, char*argv[]) 
{

  char*   confile = "glite_wms.conf";
  string  gridjobid;
  int     option_index = 0;
  string  abort_reason;
  char    c;
  bool    from_file = false;
  string  inputlist;

  while(1) {
    static struct option long_options[] = {
      {"conf", 1, 0, 'c'},
      {"help", 0, 0, 'h'},
      {"from-file", 1, 0, 'f'},
      {0, 0, 0, 0}
    };
    c = getopt_long(argc, argv, "c:hf:", long_options, &option_index);
    if ( c == -1 )
      break;
    
    switch(c) {
    case 'c':
      confile = optarg;
      break;
    case 'f':
      from_file = true;
      inputlist = optarg;
      break;
    case 'h':
      cout << endl << "Safely remove Job(s) (identified by Grid Job ID(s)) from ICE's database" << endl
           << endl << "  Usage: " << argv[0] << " [-c <conf_file>] [--from-file <input_file>] GridJobID"<<endl
      	   << endl << "  - If not specified -c <conf_file> the default will be used"<< endl<<"    (glite_wms.conf) in order to determine the path of ICE's database"
	   << endl << "  - Argument GridJobID and option --from-file <input_file>" << endl<<"    are mutually exclusive"
	   << endl << "  - If --from-file is specified the list of Grid JobIDs to remove"<<endl<<"    will be retrieved from <input_file> (the IDs must be newline separated"
	   << endl << endl;
      exit(0);
      break;
    default:
      cerr << "Type " << argv[0] << " -h for help" << endl;
      exit(1);
    }
  }
  
  if( argv[optind] && from_file ) 
    {
      cerr << "Only one of --from-file <pathfile> or <gridjobid> "
	   << "must be specified in the command line" << endl;
      return 1;
    }

  if (!argv[optind] && !from_file )
    {
      cerr << "Must specify a list of Grid JobIDs as arguments or option --from-file <pathfile>" << endl;
      return 1;
    }

  if( argv[optind] )
    gridjobid = string(argv[optind]);

  util::IceConfManager::init( confile );
  try{
    util::IceConfManager::instance();
  }
  catch(util::ConfigurationManagerException& ex) {
    cerr << "glite-wms-ice-db-rm::main - ERROR: " << ex.what() << endl;
    exit(1);
  }
  
  cout << "Will use database " << util::IceConfManager::instance()->getConfiguration()->ice()->persist_dir( ) << "/ice.db ..."<< endl<< endl; 
  
  std::list< util::CreamJob > jobList;
  
  /**
     If specified by the user
     read gridjobids, to delete from DB, from file
  */
  if( from_file ) {
    
    if( !boost::filesystem::exists( boost::filesystem::path( inputlist , boost::filesystem::native ) ))
    {
      cerr << "Error opening file [" << inputlist << "]: No such file or directory" << endl;
      exit(1);
    }    
    try {
      ifstream is(inputlist.c_str(), ios_base::in );
      
      if(!is) {
        perror( (string("Error opening file [")+ inputlist + "]").c_str( ) ) ;
	exit(1);
      }
      
      string linestring;
    
      while( is >> linestring ) {
    
        util::CreamJob aJob;
        if( get_job( linestring, aJob ) )
          jobList.push_back( aJob );
        else
          cerr << "GriJobID ["<< linestring
	       << "] is not found in the ICE database. Skipping..."
	       << endl;
      }
    } catch(std::exception& ex ) {
      cerr << ex.what() << endl;
    }
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
  for( ; jit != jobList.end( ); ++jit ) {
    
    cout << "Removing from ICE's DB job [" 
	 << jit->grid_jobid() << "] -> [" << jit->complete_cream_jobid() << "]" << endl;

    remove_job_fromdb( *jit );
    
  } 
  return 0;
}

//_______________________________________________________________________________
bool get_job( const string& gid, util::CreamJob& target )
{

  db::GetJobByGid getter( gid, "glite-wms-ice-db-rm::get_job" );
  db::Transaction tnx( false, false );
  tnx.execute( &getter );
  
  if( !getter.found() ) return false;
  
  target = getter.get_job();
  
  return true;
}

//_______________________________________________________________________________
void remove_job_fromdb( const util::CreamJob& aJob ) {
  if( aJob.proxy_renewable() )
      util::DNProxyManager::getInstance()->decrementUserProxyCounter( aJob.user_dn(), aJob.myproxy_address() );
      
  {
    db::RemoveJobByGid remover( aJob.grid_jobid(), "glite-wms-ice-db-rm::remove_job_fromdb" );
    db::Transaction tnx(false, false);
    tnx.execute( &remover );
  }
}
