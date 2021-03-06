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
#include <exception>
#include <iostream>
#include <vector>
#include <memory>

#include "glite/wms/common/configuration/Configuration.h"
#include "common/src/configuration/JCConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"
#include "glite/wms/common/utilities/LineParser.h"
#include "common/src/utilities/LineParserExceptions.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "common/src/process/process.h"
#include "jobcontrol_namespace.h"
#include "common/LockFile.h"

#include "ControllerLoop.h"
#include "exceptions.h"

USING_COMMON_NAMESPACE;
USING_JOBCONTROL_NAMESPACE;
using namespace std;
RenameLogStreamNS( elog );

namespace {

utilities::LineOption  options[] = {
  { 'C', no_argument, "check",           "\t\tOnly performs the program initialization." },
  { 'c', 1,           "configuration",   "\t\tUse an alternate configuration file." },
  { 'f', no_argument, "foreground",      "\t\tRun the server in the current process" },
  { 'k', 1,           "lockfile",        "\t\tPath for the lockfile.\n"
    "\t\tWill override the one given in the configuration file.\n" },
  { 'l', no_argument, "disable_logging", "\t\tDisables loggings to the L&B." },
  { 'r', no_argument, "allow_root",      "\t\tAllow running as root user." },
  { 'V', no_argument, "version",         "\t\tShow program version." },
  { 'v', no_argument, "verbose",         "\t\tRun the server in verbose mode." }
};

int run_instance( const string &conffile, const utilities::LineParser &options, 
		  auto_ptr<jccommon::LockFile> &lockfile, daemons::ControllerLoop::run_code_t &code )
{
  int                            res = 0;
  string                         lockfilename;
  configuration::Configuration   main_configuration( conffile, "JobController" );
  daemons::ControllerLoop        controller_loop( options );
  logger::StatePusher            pusher( elog::cedglog, "::run_instance()" );

  if( lockfile.get() == NULL ) {
    lockfilename.assign( options.is_present('k') ? options['k'].getStringValue() : main_configuration.jc()->lock_file() );

    if( lockfilename.length() == 0 ) {
      elog::cedglog << logger::setlevel( logger::fatal )
		    << "Lockfile path not given, nor in the configuration and as option (-k)." << endl;

      res = 1;
    }
    else lockfile.reset( new jccommon::LockFile(lockfilename) );
  }

  if( (lockfile.get() != NULL) && !lockfile->good() ) {
    if( !lockfile->error() ) 
      elog::cedglog << logger::setlevel( logger::fatal ) << "Stale lockfile found, program cannot start..." << endl;
    else
      elog::cedglog << logger::setlevel( logger::fatal ) << "I/O error while creating lockfile..." << endl;

    res = 1;
  }
  else if( lockfile->good() ) {
    if( !options.is_present('C') ) {
      if( !options.is_present('f') ) {
	if( options.is_present('v') )
	  clog << "Detaching daemon..." << endl;

	process::Process::self()->make_daemon(); // Do the dirty job in a detached process
	lockfile->reset_pid();
      }

      code = controller_loop.run();
      res = static_cast<int>( code );
    }
  }

  return res;
}

} // Anonymous namespace

int main( int argn, char *argv[] )
{
  int                                       res = 0;
  daemons::ControllerLoop::run_code_t       code = daemons::ControllerLoop::do_nothing;
  string                                    conffile;
  auto_ptr<jccommon::LockFile>              lockfile;
  vector<utilities::LineOption>             optvec( options, options + sizeof(options)/sizeof(utilities::LineOption) );
  utilities::LineParser                     options( optvec, utilities::ParserData::zero_args );

  try {
    options.parse( argn, argv );

    if( options.is_present('V') ) {
      clog << "European DataGrid JobController daemon v." << daemons::ControllerLoop::version() << endl
	   << "Built at " << daemons::ControllerLoop::compile_date() << ", " << daemons::ControllerLoop::compile_time();

      if( (daemons::ControllerLoop::build_user() != NULL) && (daemons::ControllerLoop::build_host() != NULL) )
	clog << "\nBy: " << daemons::ControllerLoop::build_user() << "@" << daemons::ControllerLoop::build_host();

      clog << endl << endl;
    }
    else {
      conffile.assign( options.is_present('c') ? options['c'].getStringValue() : "glite_wms_jc.conf" );

      do {
	res = run_instance( conffile, options, lockfile, code );
      } while( code == daemons::ControllerLoop::reload );
    }
  }
  catch( utilities::LineParsingError &error ) {
    clog << error << endl;
    res = error.return_code();
  }
  catch( configuration::CannotConfigure &error ) {
    clog << error << endl;
    res = 1;
  }
  catch( daemons::CannotStart &error ) {
    clog << "Cannot create the Loop object: " << error.reason() << endl;
    res = 1;
  }
  catch( exception &standard ) {
    clog << logger::setfunction( "::main()" ) << logger::setlevel( logger::null )
	 << "Caught a standard exception." << endl
	 << "What = " << standard.what() << endl;

    res = 1;
  }
  catch( ... ) {
    clog << logger::setfunction( "::main()" ) << logger::setlevel( logger::null )
	 << "Caught an unknown exception !!!" << endl;

    res = 1;
  }

  if( lockfile.get() != NULL )
    lockfile->remove();

  return res;
}
