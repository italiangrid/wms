/* Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/utilities/boost_fs_add.h"
#include "glite/wms/common/utilities/streamdescriptor.h"
#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/LineParserExceptions.h"
#include "jobcontrol_namespace.h"

#include "JobStatusExtractor.h"

USING_COMMON_NAMESPACE;
USING_JOBCONTROL_NAMESPACE;
using namespace std;
namespace fs = boost::filesystem;

namespace {

RenameLogStreamNS_ts( ts );
RenameLogStreamNS( elog );

utilities::LineOption options[] = {
  { 'c', 1, "condor-id",     "\t\tSelects the job based on its condor id." },
  { 'e', 1, "job-id",        "\t\tSelects the job based on its job id." },
  { 'C', 1, "configuration", "\t\tUse an alternate configuration file." },
};

inline int signal_aware_fcntl( int fd, int cmd, struct flock *fl )
{
  int    res;

  do {
    res = fcntl(fd, cmd, fl);
  } while( (res == -1) && (errno == EINTR) ); // We have been interrupted by a signal, just retry...

  return res;
}

class DescriptorLock {
public:
  DescriptorLock( int fd, bool lock = true );

  ~DescriptorLock( void );

  int lock( void );
  int unlock( void );

  inline bool locked( void ) { return( this->dl_locked ); }

protected:
  bool     dl_locked;
  int      dl_fd;
};

class FstreamLock : public DescriptorLock {
public:
  FstreamLock( const std::fstream &fs, bool lock = true );
  FstreamLock( const std::ifstream &ifs, bool lock = true );
  FstreamLock( const std::ofstream &ofs, bool lock = true );
  FstreamLock( const std::filebuf &fb, bool lock = true );

  ~FstreamLock( void );

  inline int descriptor( void ) { return( this->dl_fd ); }
};

DescriptorLock::DescriptorLock( int fd, bool lock ) : dl_locked( false ), dl_fd( fd )
{
  if( lock ) this->lock();
}

DescriptorLock::~DescriptorLock( void )
{
  if( this->dl_locked ) this->unlock();
}

FstreamLock::FstreamLock( const fstream &fs, bool lock ) : DescriptorLock( utilities::streamdescriptor(fs), lock )
{}

FstreamLock::FstreamLock( const ifstream &fs, bool lock ) : DescriptorLock( utilities::streamdescriptor(fs), lock )
{}

FstreamLock::FstreamLock( const ofstream &fs, bool lock ) : DescriptorLock( utilities::streamdescriptor(fs), lock )
{}

FstreamLock::FstreamLock( const filebuf &fb, bool lock ) : DescriptorLock( utilities::bufferdescriptor(fb), lock )
{}

FstreamLock::~FstreamLock( void )
{}

int DescriptorLock::lock( void )
{
  int             res = 0;
  struct flock    fc;

  if( !this->dl_locked ) {
    fc.l_whence = SEEK_SET;
    fc.l_start = 0;
    fc.l_len = 0;

    fc.l_type = F_WRLCK;
    res = signal_aware_fcntl( this->dl_fd, F_SETLKW, &fc );

    this->dl_locked = (res == 0);
  }

  return( res );
}

int DescriptorLock::unlock( void )
{
  int              res = 0;
  struct flock     fc;

  if( this->dl_locked ) {
    fc.l_whence = SEEK_SET;
    fc.l_start = fc.l_len = 0;

    fc.l_type = F_UNLCK;
    res = signal_aware_fcntl( this->dl_fd, F_SETLKW, &fc );

    this->dl_locked = (res != 0);
  }

  return( res );
}
}

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
      utilities::create_parents( logpath.branch_path() );

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
	     FstreamLock loglock( logfile ); // Lock the "external" log file againist other LM-parts

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
  catch( utilities::CannotCreateParents const& error ) {
    cerr << "Cannot create parent path." << endl
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
