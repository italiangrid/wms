
#include <boost/filesystem/operations.hpp> 
#include <boost/filesystem/exception.hpp>
#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>

#include "purger.h"

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

#include "glite/wms/common/logger/logger_utils.h"

#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/LineParserExceptions.h"
#include "glite/wms/common/configuration/Configuration.h"

#include <iostream>                        // for cout
#include <string>
#include <vector>

#include <time.h>
#include <fstream>
#include <sys/vfs.h>

namespace fs            = boost::filesystem;
namespace wl	        = glite::wms;
namespace logger	= glite::wms::common::logger;
namespace utilities     = glite::wms::common::utilities;
namespace configuration = glite::wms::common::configuration;
namespace jobid         = glite::wmsutils::jobid;
namespace po            = boost::program_options;

using namespace std;

bool find_directories( const fs::path & from_path,
		       const std::string &prefix,
		       std::vector<fs::path>& path_found,
		       bool recursive = false ) 

{
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

int main( int argc, char* argv[])
{
  string log_file, staging_path, conf_file;
  int allocated_limit, purge_threshold;
  bool fake_rm = false;

  try {
    po::options_description desc("Usage");
    desc.add_options()
      ("help,h", "display this help and exit")
      ("conf-file,c", po::value<std::string>(), "configuration file")
#ifndef GLITE_WMS_HAVE_SYSLOG_LOGGING
      ("log-file,l", po::value<std::string>(),  "logs any information into the specified file")
#else
      ("syslog,s", "logs any information on syslog")
#endif
      ("threshold,t", po::value<int>(), "sets the purging threshold to the specified number of seconds.")
      ("staging-path,p", po::value<std::string>(), "defines the sandbox staging path.")
      ("allocated-limit,a", po::value<int>(), "defines the percentange of allocated blocks which triggers the purging.")
      ("brute-rm,b", "brute-force directory removal.")
      ("fake-rm,f", "does not perform any directory removal.")
      ("quiet,q", "does not create any log file (any settings specified with -l will be ignored)")
    ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
      options(desc).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
      cout << desc << '\n';
      return -1;
    }

    if ( vm.count("staging-path") ) staging_path.assign ( vm["staging-path"].as<std::string>() );
    else {
    	char* env_var;
	if ((env_var=getenv("GLITE_WMS_TMP"))) staging_path.assign( string(env_var)+"/SandboxDir" );
	else {
		cerr << "Unable to set sandbox dir from the environment: GLITE_WMS_TMP not defined..." << endl;
		return 0;
	}
    }
    allocated_limit = vm.count("allocated-limit") ? vm["allocated-limit"].as<int>(): 0;
    if( allocated_limit ) {
    	struct statfs fs_stat;
    	if( !statfs(staging_path.c_str(), &fs_stat) ) {
		int allocated = (int) ceil( (1.0 - (fs_stat.f_bfree / (double) fs_stat.f_blocks)) * 100.0);

		if( allocated < allocated_limit ) {
			return 0;
		}
	}
    }
    
    fake_rm = vm.count("fake-rm");
    purge_threshold = vm.count("threshold") ? vm["threshold"].as<int>() : 604800;
    conf_file = vm.count("conf-file") ? vm["conf-file"].as<std::string>() : "glite_wms.conf";
   
    configuration::Configuration config(conf_file,
      configuration::ModuleType::network_server);
  
#ifndef GLITE_WMS_HAVE_SYSLOG_LOGGING 
    if( !vm.count("log-file") ) {
#else
    if( !vm.count("syslog") ) { 
#endif
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
	log_file.assign( log_path + string("/edg-wl-purgeStorage-") + string(str_time) + string(".log") );
    }
#ifndef GLITE_WMS_HAVE_SYSLOG_LOGGING
    else if ( vm.count("log-file") ) log_file.assign( vm["log-file"].as<std::string>() );
#endif
    
    if( vm.count("quiet") ) log_file.assign("/dev/null");

#ifndef GLITE_WMS_HAVE_SYSLOG_LOGGING
    if( vm.count("log-file") )
       logger::threadsafe::edglog.open( log_file.c_str(), logger::info );
#else
    if( vm.count("syslog") )
       boost::details::pool::singleton_default<
          logger::wms_log
       >::instance().init(
                     logger::wms_log::SYSLOG,
                     logger::wms_log::INFO
       );
#endif
    else 
#ifndef GLITE_WMS_HAVE_SYSLOG_LOGGING
       logger::threadsafe::edglog.open( std::cout, logger::info ); 
#else
       boost::details::pool::singleton_default<
         logger::wms_log
       >::instance().init(
                       logger::wms_log::STDOUT,
                       logger::wms_log::INFO
       );
#endif
    
    	std::vector<fs::path> found_path;
    	fs::path from_path( staging_path, fs::native); 
    	find_directories(from_path, "https", found_path, true);
	
    	for(std::vector<fs::path>::iterator it = found_path.begin(); it != found_path.end(); it++ ) {
		
		bool purge_done = wl::purger::purgeStorageEx( *it, purge_threshold, fake_rm );
                if( vm.count("staging-path")&& ! purge_done ) {
		      try {
#ifndef GLITE_WMS_HAVE_SYSLOG_LOGGING
			logger::threadsafe::edglog << it->native_file_string() << " -> forcing removal" << std::endl;
#else
                        Info(it->native_file_string() << " -> forcing removal");
#endif
			fs::remove_all( *it );
		      }
		      catch( fs::filesystem_error& fse )
		      {
#ifndef GLITE_WMS_HAVE_SYSLOG_LOGGING
			logger::threadsafe::edglog << fse.what() << std::endl;
#else
                        Error(fse.what());
#endif
		      }
		}
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
