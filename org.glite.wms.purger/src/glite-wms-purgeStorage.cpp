
#include <boost/filesystem/operations.hpp> 
#include <boost/filesystem/exception.hpp>
#include <boost/program_options.hpp>

#include "lb_utils.h"
#include "purger.h"

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

#include "glite/wms/common/logger/logger_utils.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMPConfiguration.h"

#include <iostream>                        // for cout
#include <string>
#include <vector>

#include <time.h>
#include <fstream>
#include <sys/vfs.h>

namespace fs            = boost::filesystem;
namespace wl	        = glite::wms;
namespace logger	= glite::wms::common::logger;
namespace configuration = glite::wms::common::configuration;
namespace jobid         = glite::wmsutils::jobid;
namespace po            = boost::program_options;

using namespace std;
namespace {

const configuration::Configuration* f_conf = 0;

bool 
find_directories( 
  const fs::path & from_path,
  const std::string &prefix,
  std::vector<fs::path>& path_found,
  bool recursive = false 
) {
  if ( !fs::exists( from_path ) ) return false;
  fs::directory_iterator end_itr; // default construction yields past-the-end
  for ( fs::directory_iterator itr( from_path );
        itr != end_itr;
        ++itr ) {
    if (fs::exists(*itr))
    try { 	
      if (fs::is_directory( *itr )) { 
	   if (itr->leaf().substr(0,prefix.length()) == prefix) path_found.push_back( *itr );
	   else if (recursive && find_directories( *itr, prefix, path_found )) return true;
      }
    } 
    catch( fs::filesystem_error& e) {
	std::cerr << e.what() << std::endl;
    }	
  }
  return false;
}

std::string 
get_staging_path()
{
  if (!f_conf) {
    f_conf = configuration::Configuration::instance();
    assert(f_conf);
  }
  static std::string const sandbox_staging_path(
    f_conf->wp()->sandbox_staging_path()
  );
  return sandbox_staging_path;
}
}

int main( int argc, char* argv[])
{
  string log_file;
  int allocated_limit;

  try {
    po::options_description desc("Usage");
    desc.add_options()
      ("help,h", "display this help and exit")
      ("conf-file,c", po::value<std::string>(), "configuration file")
      ("log-file,l", po::value<std::string>(),  "logs any information into the specified file")
      ("threshold,t", po::value<int>(), "sets the purging threshold to the specified number of seconds.")
      ("allocated-limit,a", po::value<int>(), "defines the percentange of allocated blocks which triggers the purging.")
      ("skip-threshold-checking", "does not perform the threshold check before purging.") 
      ("skip-status-checking", "does not perform any status checking before purging.")
      ("force-orphan-node-removal", "force removal of orphan dag nodes.")
    ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
      options(desc).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
      cout << desc << '\n';
      return -1;
    }
  
    configuration::Configuration config(
      (vm.count("conf-file") ? vm["conf-file"].as<std::string>() : "glite_wms.conf"),
      configuration::ModuleType::workload_manager
    );
 
    allocated_limit = vm.count("allocated-limit") ? vm["allocated-limit"].as<int>(): 0;
    if ( allocated_limit ) {
      struct statfs fs_stat;
      if( !statfs(get_staging_path().c_str(), &fs_stat) ) {
        int allocated = (int) ceil( (1.0 - (fs_stat.f_bfree / (double) fs_stat.f_blocks)) * 100.0);
        if( allocated < allocated_limit ) {
          return 0;
	}
      }
    }
    
    wl::purger::Purger thePurger;
    
    thePurger.threshold(
      vm.count("threshold") ? vm["threshold"].as<int>() : 604800
    ).
    skip_threshold_checking(
      vm.count("skip-threshold-checking")
    ).
    skip_status_checking(
      vm.count("skip-status-checking") 
    ).
    force_orphan_node_removal(
      vm.count("force-orphan-node-removal")
    ).
    log_using(
      edg_wll_LogClearTIMEOUT
    );

    if( !vm.count("log-file") ) {
	char* env_var;
	string log_path;
	if ((env_var=getenv("GLITE_WMS_TMP"))) log_path.assign( string(env_var) );
	else {
		cerr << "Unable to set logfile path from the environment: GLITE_WMS_TMP not defined..." << endl;
		return 0;
	}
	char str_time [64];	
	time_t now;
	time(&now);
	strftime(str_time, 64, "%a-%d-%b-%H:%M:%s-%Y", localtime(&now));
	log_file.assign( log_path + string("/glite-wms-purgeStorage-") + string(str_time) + string(".log") );
    }
    else if ( vm.count("log-file") ) log_file.assign( vm["log-file"].as<std::string>() );
    
    if( vm.count("log-file") )
       logger::threadsafe::edglog.open( log_file.c_str(), logger::info );
    else 
       logger::threadsafe::edglog.open( std::cout, logger::info ); 
    
   std::vector<fs::path> found_path;
   fs::path from_path( get_staging_path(), fs::native); 
   find_directories(from_path, "https", found_path, true);

   std::vector<fs::path>::const_iterator i = found_path.begin();
   std::vector<fs::path>::const_iterator const e = found_path.end();
   
   for( ; i != e ; ++i) {
     jobid::JobId id(jobid::from_filename( i->leaf())); 
     thePurger( id );
   }
  }
  catch (boost::program_options::unknown_option& e) {
    cerr << e.what() << endl;
  }
  catch( exception& e) {
    cerr << e.what() << endl;
  }
  catch( ... ) {
    cerr << "Uncaught exception..." << endl;
  }
  exit(-1);
}
