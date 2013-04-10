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
#include <sys/types.h>          // getpid(), getpwnam()
#include <unistd.h>             // getpid(), usleep
#include <pwd.h>                // getpwnam()
#include <cstdio>               // popen()
#include <cstdlib>              // atoi()
#include <csignal>

#include "glite/ce/cream-client-api-c/creamApiLogger.h"

#include "common/src/configuration/ICEConfiguration.h"
#include "common/src/configuration/WMConfiguration.h"
#include "common/src/configuration/CommonConfiguration.h"

#include "main/Main.h"
#include "db/GetJobByGid.h"
#include "db/Transaction.h"
#include "utils/IceConfManager.h"
#include "utils/DNProxyManager.h"
#define RUN_ON_LINUX
#include "segv_handler.h"

#include "glite/ce/cream-client-api-c/certUtil.h"

#include <boost/scoped_ptr.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>

#include <list>

/* workaround for gsoap 2.7.13 */
#include "glite/ce/cream-client-api-c/cream_client_soapH.h"
SOAP_NMAC struct Namespace namespaces[] = {};

using namespace std;
using namespace glite::ce::cream_client_api;
using namespace glite::wms::ice;

//namespace iceUtil   = glite::wms::ice::util;
namespace po        = boost::program_options;
namespace fs        = boost::filesystem;
namespace api_util  = glite::ce::cream_client_api::util;
namespace cream_api = glite::ce::cream_client_api::soap_proxy;

#define MAX_ICE_MEM 550000LL

long long check_my_mem( const pid_t pid ) throw();

/*
void sigusr1_handle(int x) { 
  exit(2);
}

void sigusr2_handle(int x) { 
  exit(3);
}
*/
void sigpipe_handle(int x) { 
/*  CREAM_SAFE_LOG(util::creamApiLogger::instance()->getLogger()->debugStream() 
		 << "glite-wms-ice::sigpipe_handle: Captured SIGPIPE. x argument=["
		 << x 
		 << "]" );*/
}

// change the uid and gid to those of user no-op if user corresponds
// to the current effective uid only root can set the uid (this
// function was originally taken from
// org.glite.wms.manager/src/daemons/workload_manager.cpp
bool set_user(std::string const& user)
{
    uid_t euid = ::geteuid();
 
    if (euid == 0 && user.empty()) {
        return false;
    }
 
    ::passwd* pwd(::getpwnam(user.c_str()));
    if (pwd == 0) {
        return false;
    }
    ::uid_t uid = pwd->pw_uid;
    ::gid_t gid = pwd->pw_gid;
 
    return
        euid == uid
        || (euid == 0 && ::setgid(gid) == 0 && ::setuid(uid) == 0);
}
 

int main(int argc, char*argv[]) 
{
    string opt_pid_file;
    string opt_conf_file;

    int cache_dump_delay = 900;

    if( getenv("GLITE_WMS_ICE_CACHEDUMP_DELAY" ) ) {
      cache_dump_delay = atoi(getenv("GLITE_WMS_ICE_CACHEDUMP_DELAY" ));
      if(cache_dump_delay < 300)
	cache_dump_delay = 300;
    }

    static const char* method_name = "glite-wms-ice::main() - ";
    
    po::options_description desc("Usage");
    desc.add_options()
        ("help", "display this help and exit")
        (
         "daemon",
         po::value<string>(&opt_pid_file),
         "run in daemon mode and save the pid in this file"
         )
        (
         "conf",
         po::value<string>(&opt_conf_file)->default_value("glite_wms.conf"),
         "configuration file"
         )
        ;
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
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

    if ( vm.count("daemon") ) {
        ofstream pid_file(opt_pid_file.c_str());
        if (!pid_file) {
            cerr << "the pid file " << opt_pid_file << " is not writable\n";
            return -1;
        }
        if (daemon(0, 0)) {
            cerr << "cannot become daemon (errno = "<< errno << ")\n";
            return -1;
        }
        pid_file << ::getpid();
    }

    

    /**
     * - creates an ICE object
     * - initializes the job cache
     * - starts the async event consumer and status poller
     * - opens the WM's and the NS's filelist
     */    

    /*****************************************************************************
     * Initializes configuration manager (that in turn loads configurations)
     ****************************************************************************/
    glite::wms::ice::util::IceConfManager::init( opt_conf_file );
    try{
        glite::wms::ice::util::IceConfManager::instance();
    }
    catch(glite::wms::ice::util::ConfigurationManagerException& ex) {
        cerr << "glite-wms-ice::main() - ERROR: " << ex.what() << endl;
        exit(1);
    }

    glite::wms::common::configuration::Configuration* conf = glite::wms::ice::util::IceConfManager::instance()->getConfiguration();

    //
    // Change user id to become the "dguser" specified in the configuratoin file
    //
    string dguser( conf->common()->dguser() );
    if (!set_user(dguser)) {
        cerr << "glite-wms-ice::main() - ERROR: cannot set the user id to " 
             << dguser << endl;
        exit( 1 );
    }


    /*
     * Build all paths needed for files referred into the configuration file
     *
     * This code is taken from JC/LM (thanks to Ale)
     */
    std::list< fs::path > paths;

    try {
        paths.push_back(fs::path(conf->ice()->input(), fs::native));
        paths.push_back(fs::path(conf->ice()->persist_dir() + "/boh", fs::native));
        paths.push_back(fs::path(conf->ice()->logfile(), fs::native));
    } catch( ... ) {
        cerr << "glite-wms-ice::main() - ERROR: cannot create paths; "
             << "check ICE configuration file"
             << endl;
        exit( 1 );
    }
    
    for( std::list< fs::path >::iterator pathIt = paths.begin(); pathIt != paths.end(); ++pathIt ) {
        if( (!pathIt->native_file_string().empty()) && !fs::exists(pathIt->branch_path()) ) {
            try {
                fs::create_directories( pathIt->branch_path() );
            } catch( ... ) {
                cerr << "glite-wms-ice::main() - ERROR: cannot create path "
                     << pathIt->branch_path().string() 
                     << endl;
                exit( 1 );
            }
        }
    }

    /*****************************************************************************
     * Sets the log file
     ****************************************************************************/
    api_util::creamApiLogger* logger_instance = api_util::creamApiLogger::instance();
    log4cpp::Category* log_dev = logger_instance->getLogger();

    log_dev->setPriority( conf->ice()->ice_log_level() );
    logger_instance->setLogfileEnabled( conf->ice()->log_on_file() );
    logger_instance->setConsoleEnabled( conf->ice()->log_on_console() );
    logger_instance->setMaxLogFileSize( -1 );
    //logger_instance->setMaxLogFileRotations( conf->ice()->max_logfile_rotations() );
    string logfile = conf->ice()->logfile();
    string hostcert = conf->ice()->ice_host_cert();

    logger_instance->setLogFile(logfile.c_str());
    CREAM_SAFE_LOG(log_dev->debugStream() 
		   << "ICE VersionID is [" << ICE_VERSIONID << "] ProcessID=["
		   << ::getpid() << "]"
		   );
    cout << "Logfile is [" << logfile << "]" << endl;

    signal(SIGPIPE, sigpipe_handle);

    /*****************************************************************************
     * Gets the distinguished name from the host proxy certificate
     ****************************************************************************/

    CREAM_SAFE_LOG(
                   log_dev->infoStream()
                   << method_name
                   << "Host certificate is [" << hostcert << "]" 
                   
                   );



    /**
     * Now the cache is ready and filled with all job's information
     * Let's create the DNProxyManager that also load all DN->ProxyFile mappping
     * by scanning the cache
     */
    glite::wms::ice::util::DNProxyManager::getInstance();
    

    /*****************************************************************************
     * Initializes the database by invoking a fake query, and the ice manager
     ****************************************************************************/
    {
      //      list<pair<string, string> > params;
      //      params.push_back( make_pair("failure_reason", "" ));
      
      //      glite::wms::ice::db::UpdateJobByGid updater("FAKE QUERY TO INITIALISE DB", params, "glite-wms-ice::main");
      glite::wms::ice::db::GetJobByGid getter( "foo", "glite-wms-ice::main");
      glite::wms::ice::db::Transaction tnx( false, true );
      tnx.execute(&getter);
    }

    /**
     * Get Instance of IceCore component
     */
    glite::wms::ice::Main* iceManager( glite::wms::ice::Main::instance( ) );

    /**
     * Starts status poller and/or listener if specified in the config file
     */
    iceManager->startPoller( );  
    //iceManager->startJobKiller( );
    iceManager->startProxyRenewer( );


    /*****************************************************************************
     * Main loop that fetch requests from input filelist, submit/cancel the jobs,
     * removes requests from input filelist.
     ****************************************************************************/
     
    return iceManager->main_loop( );
   
    //return 0;
}
