#include <iostream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/tuple/tuple.hpp>

namespace fs = boost::filesystem;

#include <classad_distribution.h>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/logger/manipulators.h"
#include "../jobcontrol_namespace.h"
#include "../common/SignalChecker.h"

#include "JobControllerClientJD.h"
#include "JobControllerExceptions.h"

using namespace std;
USING_COMMON_NAMESPACE;

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

JobControllerClientJD::JobControllerClientJD( void ) : JobControllerClientImpl(),
						       jccjd_currentGood( false ), jccjd_current(), 
                                                       jccjd_request(), jccjd_queue()
{
  const configuration::JCConfiguration       *config = configuration::Configuration::instance()->jc();
  const fs::path                              listname( config->input(), fs::native );
  logger::StatePusher                         pusher( clog, "JobControllerClientJD::JobControllerClientJD()" );

  try {
    this->jccjd_jd=  new utilities::JobDir( listname );
    clog << logger::setlevel( logger::info ) << "Create jobdir queue object." << endl;
 
   // Check if there are "old" requests not managed
    utilities::JobDir::iterator b, e;
    boost::tie(b, e) = this->jccjd_jd->old_entries();

    for ( ; b != e; ++b)
      this->jccjd_queue.push( *b );    

  }
  catch( utilities::JobDirError &error ) {
    clog << logger::setlevel( logger::fatal )
	 << "Error while setting jobdir." << endl
	 << "Reason: " << error.what() << endl;

    throw CannotCreate( error.what() );
  }
}

JobControllerClientJD::~JobControllerClientJD( void )
{
  delete this->jccjd_jd;
}

void JobControllerClientJD::extract_next_request( void )
{
  logger::StatePusher         pusher( clog, "JobControllerClientJD::get_next_request()" );

  clog << logger::setlevel( logger::info ) << "Looking for new requests..." << endl;
  jccommon::SignalChecker::instance()->throw_on_signal();

  if( this->jccjd_queue.empty() ) { // look for new requests 

    while( true ) {
      utilities::JobDir::iterator b, e;
      boost::tie(b, e) = this->jccjd_jd->new_entries();		
      
      for ( ; b != e; ++b)  
	this->jccjd_queue.push( this->jccjd_jd->set_old( *b ) );
      
      if( this->jccjd_queue.empty() ) {
        sleep( 2 ); // wait for new requests
        jccommon::SignalChecker::instance()->throw_on_signal();
      } else { // we have found some requests
        break;
      }
    }
  }

  this->jccjd_current = this->jccjd_queue.front();
  this->jccjd_currentGood = true;

  return;
}

void JobControllerClientJD::release_request( void )
{
  if( this->jccjd_currentGood ) {	
    fs::remove( this->jccjd_current );	
    this->jccjd_queue.pop();
    this->jccjd_currentGood = false;
  }

  return;
}

const Request *JobControllerClientJD::get_current_request( void )
{
   classad::ClassAdParser parser;

   fs::ifstream is( this->jccjd_current );
   boost::shared_ptr<classad::ClassAd> command_ad( parser.ParseClassAd(is) );
         
   this->jccjd_request.reset( *command_ad );

   clog << logger::setlevel( logger::debug ) << "Got new request..." << endl;

   return &this->jccjd_request;
}

} // end namespace controller

} JOBCONTROL_NAMESPACE_END;
