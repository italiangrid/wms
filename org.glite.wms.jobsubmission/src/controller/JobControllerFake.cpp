#include <iostream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <classad_distribution.h>

#include "glite/jdl/convert.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"

#include "glite/wmsutils/jobid/JobId.h"

#include "JobControllerFake.h"
#include "JobControllerExceptions.h"
#include "SubmitAd.h"
#include "SubmitAdExceptions.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

JobControllerFake::JobControllerFake( void ) {}

JobControllerFake::~JobControllerFake( void ) {}

int JobControllerFake::submit( const classad::ClassAd *pad )
try {
  ofstream              ofs;
  SubmitAd              ad( pad );
  logger::StatePusher   pusher( elog::cedglog, "JobControllerFake::submit(...)" );

  elog::cedglog << logger::setlevel( logger::null )
		<< "Got request for submission of job " << ad.job_id() << endl
		<< "Original classad: " << *pad << endl
		<< "Modified classad: " << ad.classad() << endl
		<< "Writing condor submit file: " << ad.submit_file() << endl;

  ofs.open( ad.submit_file().c_str() );

  if( ofs.good() ) {
    glite::jdl::to_submit_stream( ofs, ad );

    ofs.close();

    elog::cedglog << "File successfully created." << endl;
  }
  else
    elog::cedglog << "Cannot open condor submit file." << endl;

  return 0;
}
catch( SubmitAdException &error ) { throw CannotExecute( error.error() ); }

bool JobControllerFake::cancel( const glite::wmsutils::jobid::JobId &id, const char *logfile )
{
  logger::StatePusher     pusher( elog::cedglog, "JobControllerFake::cancel(...)" );

  elog::cedglog << logger::setlevel( logger::null )
		<< "Got request for cancellation of job." << endl
		<< "JOB Id = " << id.toString() << endl;

  return true;
}

bool JobControllerFake::cancel( int condorid, const char *logfile )
{
  logger::StatePusher     pusher( elog::cedglog, "JobControllerFake::cancel(...)" );

  elog::cedglog << logger::setlevel( logger::null )
		<< "Got request for cancellation of job." << endl
		<< "condor Id = " << condorid << endl;

  return true;
}

size_t JobControllerFake::queue_size( void )
{
  return 0;
}

}; // namespace controller

} JOBCONTROL_NAMESPACE_END;
