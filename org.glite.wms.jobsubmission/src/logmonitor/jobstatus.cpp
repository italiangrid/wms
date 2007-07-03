#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/utilities/boost_fs_add.h"
#include "glite/wms/common/utilities/fstreamlock.h"
#include "glite/wms/common/utilities/streamdescriptor.h"
#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/LineParserExceptions.h"
#include "jobcontrol_namespace.h"

#include "JobStatusExtractor.h"

USING_COMMON_NAMESPACE;
USING_JOBCONTROL_NAMESPACE;
using namespace std;
namespace fs = boost::filesystem;

RenameLogStreamNS_ts( ts );
RenameLogStreamNS( elog );

utilities::LineOption options[] = {
  { 'c', 1, "condor-id",     "\t\tSelects the job based on its condor id." },
  { 'd', 1, "dag-id",        "\t\tSets the Job-Id of the DAG job if known." },
  { 'e', 1, "job-id",        "\t\tSelects the job based on its job id." },
  { 'C', 1, "configuration", "\t\tUse an alternate configuration file." },
};

int main( int argn, char *argv[] )
{
  const configuration::LMConfiguration      *lmconfig;

  int                                        res = 0, jobstatus;
  ofstream                                   logfile;
  string                                     conffile, errors;
  auto_ptr<configuration::Configuration>     conf;
  auto_ptr<logmonitor::JobStatusExtractor>   extractor;
  vector<utilities::LineOption>              optvec( options, options + sizeof(options) / sizeof(utilities::LineOption) );
  utilities::LineParser                      options( optvec, utilities::ParserData::zero_args );

  try {
    options.parse( argn, argv );

    conffile.assign( options.is_present('C') ? options['C'].getStringValue() : "glite_wms.conf" );
    conf.reset( new configuration::Configuration(conffile, "LogMonitor") ); // LogMonitor is the most similar program...

    lmconfig = conf->lm();
    fs::path logpath(lmconfig->external_log_file(), fs::native);

    if( !fs::exists(logpath) ) {
      fs::create_parents( logpath.branch_path() );

      utilities::create_file( logpath.native_file_string().c_str() ); // GCC 3.x has a strange behaviour with non existsne files...
    }

    if( !options.is_present('c') && !options.is_present('e') ) {
      options.usage( cerr );
      cerr << "You must specify at least one between --condor-id and --job-id.\n";

      res = 1;
    }
    else if( options.is_present('c') && options.is_present('e') ) {
      options.usage( cerr );
      cerr << "You cannot specify --condor-id with --job-id.\n";

      res = 1;
    }
    else {
      logfile.open( logpath.native_file_string().c_str(), ios::app );
      if( logfile.good() ) {
	utilities::FstreamLock  loglock( logfile ); // Lock the "external" log file againist other LM-parts

	elog::cedglog.open( logfile );
	ts::edglog.unsafe_attach( elog::cedglog ); // Attach edglog to the right stream

	extractor.reset( new logmonitor::JobStatusExtractor(options) );

	jobstatus = extractor->get_job_status( errors );
	cout << jobstatus << endl;
	cerr << errors << endl;
      }
      else {
	cerr << "Cannot open output log file \"" << logpath.native_file_string() << "\", aborting..." << endl;

	res = 1;
      }
    }
  }
  catch( utilities::LineParsingError &error ) {
    cerr << error << endl;
    res = error.return_code();
  }
  catch( fs::filesystem_error &error ) {
    cerr << "Got an error during filesystem usage." << endl
	 << error.what() << endl;

    res = 1;
  }
  catch( exception &error ) {
    cerr << "Got an uncaught standard exception." << endl
	 << error.what() << endl;

    res = 1;
  }
  catch( ... ) {
    cerr << "Something has been thrown, but I don't know what !!!" << endl;

    res = 1;
  }

  return res;
}
