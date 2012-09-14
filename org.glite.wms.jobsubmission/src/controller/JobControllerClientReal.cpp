#include <iostream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

namespace fs = boost::filesystem;

#include <classad_distribution.h>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/logger/manipulators.h"
#include "jobcontrol_namespace.h"
#include "common/SignalChecker.h"

#include "JobControllerClientReal.h"
#include "JobControllerExceptions.h"

using namespace std;
USING_COMMON_NAMESPACE;

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

JobControllerClientReal::JobControllerClientReal( void ) : JobControllerClientImpl(),
							   jccr_currentGood( false ), jccr_current(), jccr_request(),
							   jccr_queue(), jccr_mutex(), jccr_extractor()
{
  const configuration::JCConfiguration       *config = configuration::Configuration::instance()->jc();
  fs::path                                    listname( config->input(), fs::native );
  logger::StatePusher                         pusher( clog, "JobControllerClientReal::JobControllerClientReal()" );

  try {
    this->jccr_queue.open( listname.native_file_string() );
    this->jccr_mutex.reset( new utilities::FileListDescriptorMutex(this->jccr_queue) );
    this->jccr_extractor.reset( this->jccr_queue );

    clog << logger::setlevel( logger::info ) << "Create file queue oject." << endl;
  }
  catch( utilities::FileContainerError &error ) {
    clog << logger::setlevel( logger::fatal )
	 << "Error while opening filelist." << endl
	 << "Reason: " << error.string_error() << endl;

    throw CannotCreate(error.string_error());
  }
}

JobControllerClientReal::~JobControllerClientReal()
{
  this->jccr_queue.close();
}

void JobControllerClientReal::extract_next_request()
{
  utilities::FileListLock     locker( *this->jccr_mutex, false );
  logger::StatePusher         pusher( clog, "JobControllerClientReal::get_next_request()" );

  clog << logger::setlevel( logger::info ) << "Waiting for requests..." << endl;
  jccommon::SignalChecker::instance()->throw_on_signal();

  while (true) {
    locker.lock();
    this->jccr_current = this->jccr_extractor.get_next();

    if (this->jccr_current != this->jccr_extractor->end()) {
      break;
    } else {
      locker.unlock();
      jccommon::SignalChecker::instance()->throw_on_signal();
    }
  }

  this->jccr_currentGood = true;
  this->jccr_request.reset(*this->jccr_current);
  clog << logger::setlevel( logger::debug ) << "Got new request...\n";

  return;
}

void JobControllerClientReal::release_request()
{
  if( this->jccr_currentGood ) {
    utilities::FileListLock     locker( *this->jccr_mutex );

    this->jccr_extractor.erase(this->jccr_current);
    this->jccr_currentGood = false;
  }

  return;
}

Request *JobControllerClientReal::get_current_request()
{
  return &this->jccr_request;
}

}} JOBCONTROL_NAMESPACE_END;
