
#include <boost/filesystem/operations.hpp> 
#include <boost/filesystem/exception.hpp>
#include <boost/progress.hpp>
#include <boost/scoped_ptr.hpp>

#include "purger.h"

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/LineParserExceptions.h"

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
namespace jobid         = glite::wmsutils::jobid;

using namespace std;

utilities::LineOption  options[] = {
  { 'l', 1,             "log-file",	    "\t logs any information into the specified file." },
  { 't', 1,		"threshold",	    "\t sets the purging threshold to the specified number of seconds." },
  { 'p', 1,		"staging-path",	    "\t defines the sandbox staging path." },
  { 'a', 1,		"allocated-limit"   "\t defines the percentange of allocated blocks which triggers the purging." }, 
  { 'b', no_argument,   "brute-rm"          "\t brute-force directory removal." },
  { 'f', no_argument,   "fake-rm"           "\t does not perform any directory removal." },
  { 'e', no_argument,   "enable-progress",  "\t enable the progress indicator." }, 
  { 'q', no_argument,   "quiet",            "\t does not create any log file (any settings specified with -l will be ignored)." }
};

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
    try { 	
      if ( fs::is_directory( *itr ) &&
	   itr->leaf().substr(0,prefix.length()) == prefix ) {
	path_found.push_back( *itr );
	if ( recursive && find_directories( *itr, prefix, path_found ) ) return true;
      }
    } 
    catch( fs::filesystem_error& e)
      {
	std::cerr << e.what() << std::endl;
      }	
  }
  return false;
}

int main( int argc, char* argv[])
{
  vector<utilities::LineOption>             optvec( options, options + sizeof(options)/sizeof(utilities::LineOption) );
  utilities::LineParser                     options( optvec, utilities::ParserData::zero_args );
  string log_file, staging_path;
  int allocated_limit, purge_threshold;
  bool fake_rm = false;

  try {
    options.parse( argc, argv );
   
    if (options.is_present('p')) staging_path.assign ( options['p'].getStringValue() );
    else {
    	char* env_var;
	if ((env_var=getenv("GLITE_WMS_TMP"))) staging_path.assign( string(env_var) );
	else {
		cerr << "Unable to set sandbox dir from the environment: GLITE_WMS_TMP not defined..." << endl;
		return 0;
	}
    }
    
    allocated_limit     = options.is_present('a') ? options['a'].getIntegerValue(): 0;
        if( allocated_limit ) {
    	struct statfs fs_stat;
    	if( !statfs(staging_path.c_str(), &fs_stat) ) {
		int allocated = (int) ceil( (1.0 - (fs_stat.f_bfree / (double) fs_stat.f_blocks)) * 100.0);

		if( allocated < allocated_limit ) {
			return 0;
		}
	}
    }
    
    fake_rm = options.is_present('f');
    purge_threshold = options.is_present('t') ? options['t'].getIntegerValue() : 604800;	    
    
 
    if( !options.is_present('l') && options.is_present('e') ) {
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
    else if (options.is_present('l')) log_file.assign( options['l'].getStringValue() );
    
    if( options.is_present('q') ) log_file.assign("/dev/null");
    
    if(options.is_present('l') || options.is_present('e')) logger::threadsafe::edglog.open( log_file.c_str(), logger::info );
    else logger::threadsafe::edglog.open( std::cout, logger::info ); 
    
    	std::vector<fs::path> found_path;
    	fs::path from_path( staging_path, fs::system_specific); 
    	find_directories(from_path, "https", found_path, true);
	
    	std::auto_ptr<boost::progress_display> show_progress;
    	if( options.is_present('e') ) show_progress.reset( new boost::progress_display(found_path.size()) );
	
    	for(std::vector<fs::path>::iterator it = found_path.begin(); it != found_path.end(); it++ ) {
		
		bool purge_done = wl::purger::purgeStorageEx( *it, purge_threshold, fake_rm );
		if( options.is_present('b') && ! purge_done ) {
		      try {
			logger::threadsafe::edglog << it -> leaf() << " -> forcing removal" << std::endl;      
			fs::remove_all( *it );
		      }
		      catch( fs::filesystem_error& fse )
		      {
			logger::threadsafe::edglog << fse.what() << std::endl;
		      }
		}
      	if( options.is_present('e') ) ++(*show_progress);
	}
  }
  catch( utilities::LineParsingError &error ) {
    cerr << error << endl;
    exit( error.return_code() );
  }
  catch( exception& e) {
    cerr << e.what() << endl;
  }
  catch( ... ) {
    cerr << "Uncaught exception..." << endl;
  }
  exit(-1);
}
