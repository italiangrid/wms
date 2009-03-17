#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/program_options.hpp>
#include <boost/function.hpp>

#include "lb_utils.h"
#include "purger.h"

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMPConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"

#include "glite/wms/common/logger/edglog.h"

#include <iostream>                        // for cout
#include <string>
#include <vector>
#include <cmath>
#include <time.h>
#include <fstream>
#include <sys/vfs.h>

namespace fs            = boost::filesystem;
namespace wl	        = glite::wms;
namespace configuration = glite::wms::common::configuration;
namespace jobid         = glite::wmsutils::jobid;
namespace po            = boost::program_options;
namespace logger	= glite::wms::common::logger;

namespace {

const configuration::Configuration* f_conf = 0;

bool
purge_directories(
  const fs::path& from_path,
  wl::purger::Purger& thePurger
) {

  static std::string const prefix("https");

  if (!fs::exists(from_path)) {
    return false;
  }
  fs::directory_iterator end_itr; // default construction yields past-the-end
  for (fs::directory_iterator itr( from_path );
       itr != end_itr;
       ++itr) {
    if (fs::exists(*itr))
    try { 	
      if (fs::is_directory( *itr ) && itr->leaf()!="lost+found") {
	   if (itr->leaf().substr(0,prefix.length()) == prefix) {
             jobid::JobId id(jobid::from_filename( itr->leaf()));
             thePurger(id);
           }
	   else if (purge_directories( *itr, thePurger )) return true;
      }
    }
    catch( fs::filesystem_error& e) {
	std::cerr << e.what() << std::endl;
    }	
  }
  return false;
}

bool
lb_type()
{
  if (!f_conf) {
    f_conf = configuration::Configuration::instance();
    assert(f_conf);
  }
  return f_conf->common()->lbproxy();
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
  try {
    po::options_description desc("Usage");
    desc.add_options()
      ("help,h", "display this help and exit")
      (
        "conf-file,c",
        po::value<std::string>(),
        "configuration file"
      )
      (
        "log-file,l",
        po::value<std::string>(),
        "logs any information into the specified file"
      )
      (
        "threshold,t",
        po::value<int>(),
        "sets the purging threshold to the specified number of seconds"
      )
      ( 
        "staging-path,p",
        po::value<std::string>(),
        "absolute path to sandbox staging directory"
      )
      (
        "allocated-limit,a",
        po::value<int>(),
        "defines the percentange of allocated blocks which triggers the"
        " purging"
      )
      (
        "skip-status-checking,s",
        "does not perform any status checking before purging"
      )
      (
        "force-orphan-node-removal,o", 
        "force removal of orphan dag nodes"
      )
      ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
      std::cout << desc << '\n';
      return EXIT_SUCCESS;
    }

    configuration::Configuration config(
      vm.count("conf-file")
      ? vm["conf-file"].as<std::string>()
      : "glite_wms.conf",
      configuration::ModuleType::workload_manager
    );

    std::string const staging_path(
      vm.count("staging-path")
      ?  vm["staging-path"].as<std::string>()
      : get_staging_path()
    );
    
    int const allocated_limit(
      vm.count("allocated-limit") ? vm["allocated-limit"].as<int>(): 0
    );
    if (allocated_limit) {
      struct statfs fs_stat;
      if (!statfs(staging_path.c_str(), &fs_stat)) {
        int const allocated(
          static_cast<int>(
            (1.0 - (static_cast<double>(fs_stat.f_bfree) / fs_stat.f_blocks))
            * 100
          )
        );
        if (allocated < allocated_limit) {
          return EXIT_SUCCESS;
	}
      }
    }

    bool have_lbproxy = lb_type();
    wl::purger::Purger thePurger(have_lbproxy);

    boost::function<int(edg_wll_Context)> log_clear;

    if (have_lbproxy) {
      log_clear = edg_wll_LogClearTIMEOUTProxy;
    } else {
      log_clear = edg_wll_LogClearTIMEOUT;
    }

    thePurger.threshold(
      vm.count("threshold") ? vm["threshold"].as<int>() : 0
    ).
    skip_status_checking(
      vm.count("skip-status-checking")
    ).
    force_orphan_node_removal(
      vm.count("force-orphan-node-removal")
    ).
    log_using(log_clear);
   
   if (vm.count("log-file")) {
     logger::threadsafe::edglog.open( vm["log-file"].as<std::string>(), logger::info);
   }
   else {
     logger::threadsafe::edglog.open( std::cout, logger::info);
   }
   std::vector<fs::path> found_path;
   fs::path from_path( staging_path, fs::native);
   
   purge_directories(
     from_path, 
     thePurger
   );

  }
  catch (boost::program_options::unknown_option const& e) {
    std::cerr<< e.what() << '\n';
  } catch (std::exception const& e) {
    std::cerr << e.what() << '\n';
  } catch (...) {
    std::cerr << "unknown exception\n";
  }
  return EXIT_FAILURE;
}
