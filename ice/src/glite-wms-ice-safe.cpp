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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sys/wait.h>
#include <sys/types.h>          // getpid(), getpwnam()
#include <unistd.h>             // getpid()
#include <pwd.h>                // getpwnam()
#include <cerrno>

#include "common/src/configuration/ICEConfiguration.h"
#include "common/src/configuration/WMConfiguration.h"
#include "common/src/configuration/CommonConfiguration.h"
#include "iceUtils/IceConfManager.h"

#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <libgen.h> // for dirname

/* workaround for gsoap 2.7.13 */
#include "glite/ce/cream-client-api-c/cream_client_soapH.h"
SOAP_NMAC struct Namespace namespaces[] = {};

using namespace std;
namespace po = boost::program_options;
namespace iceUtil = glite::wms::ice::util;

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
 
//____________________________________________________________________
int main( int argc, char *argv[]) {

  string opt_pid_file;
  string opt_conf_file;
  //static const char* method_name = "glite-wms-ice::main() - ";
  
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

  iceUtil::IceConfManager::init( opt_conf_file );
  try{
    iceUtil::IceConfManager::instance();
  }
  catch(iceUtil::ConfigurationManager_ex& ex) {
    cerr << "glite-wms-ice-safe::main() - ERROR: " << ex.what() << endl;
    exit(1);
  }

  glite::wms::common::configuration::Configuration* conf = iceUtil::IceConfManager::instance()->getConfiguration();

  string logfile = conf->ice()->logfile();

  string logpath = dirname( (char*)logfile.c_str() );

  string consolelog = logpath + "/ice_console.log";

  if ( vm.count("daemon") ) {
    /*ofstream pid_file(opt_pid_file.c_str());
    if (!pid_file) {
      cerr << "the pid file " << opt_pid_file << " is not writable\n";
      return -1;
    }*/
    if (daemon(0, 0)) {
      cerr << "cannot become daemon (errno = "<< errno << "): " << strerror(errno) << "\n";
      return -1;
    }
    //pid_file << ::getpid();
  }
  

  //----------------------------------------------
//  if( conf->ice( )->input( ) == "jobdir" ) {
    string jobdirpath( conf->ice( )->input( ) );
    string tmp = "/bin/mv ";
    tmp += jobdirpath + "/old/* " + jobdirpath + "/new/ >/dev/null 2>&1";

    cout << "Executing [" << tmp << "]" << endl;
    
    system( tmp.c_str() );
//  }
 
  string BASEPATH = "/usr/bin";

  const char* pathenv = ::getenv("LOCATION_BIN");

  if( pathenv )
    BASEPATH = pathenv;
 
  if( argc>=3 ) {
  
    string tmp;
    char* mem = ::getenv( "MAX_ICE_MEM" );
    if(mem) {
        
      tmp = "MAX_ICE_MEM=\"";
      tmp += boost::lexical_cast<string>(mem) + "\" " + BASEPATH + "/glite-wms-ice --conf " + opt_conf_file;
    
    }
    else {
      tmp += BASEPATH + "/glite-wms-ice --conf " + opt_conf_file + " " + consolelog + " 2>&1";
    }
    
    
    
    while(true) {
      int ret = ::system( tmp.c_str( ) );
      int wret = WEXITSTATUS(ret);
      
      /** 
       * Sleeping 5 minutes should ensure that the kernel has a sufficient time
       * to close a previously used bind-port. This to prevent an not
       * understood problem with soap_bind that causes a crash if the port is temporarily
       * unavailable
       */
      if( 2 == wret) { sleep(300); continue; }
      exit(wret);
    }
  } 
}
