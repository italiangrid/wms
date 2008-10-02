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
#include "jobcontrol_namespace.h"
#include "common/SignalChecker.h"

#include "JobControllerClientJD.h"
#include "JobControllerExceptions.h"

using namespace std;
USING_COMMON_NAMESPACE;

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

JobControllerClientJD::JobControllerClientJD() :
  JobControllerClientImpl(),
  jccjd_currentGood(false),
  jccjd_current(),
  jccjd_request(),
  jccjd_queue()
{
  const configuration::JCConfiguration       *config = configuration::Configuration::instance()->jc();
  const fs::path                              listname( config->input(), fs::native );
  logger::StatePusher                         pusher( clog, "JobControllerClientJD::JobControllerClientJD()" );

  try {
    this->jccjd_jd.reset(new utilities::JobDir(listname));
    clog << logger::setlevel( logger::info ) << "Created jobdir queue object.\n";
 
   // check if there are "old" requests not yet managed
    utilities::JobDir::iterator b, e;
    boost::tie(b, e) = this->jccjd_jd->old_entries();

    for ( ; b != e; ++b) {
      this->jccjd_queue.insert(std::make_pair(std::time(0), *b));
      this->jccjd_current = this->jccjd_queue.begin()->second;
      this->jccjd_currentGood = true;
    }
  } catch(utilities::JobDirError &error) {
    clog << logger::setlevel(logger::fatal)
      << "Error while setting jobdir.\nReason: " << error.what() << '\n';

    throw CannotCreate( error.what() );
  }
}

void JobControllerClientJD::extract_next_request()
{
  logger::StatePusher pusher(clog, "JobControllerClientJD::extract_next_request()");
  jccommon::SignalChecker::instance()->throw_on_signal();

  utilities::JobDir::iterator b, e;
  boost::tie(b, e) = this->jccjd_jd->new_entries();
      
  if (b != e) {
    for ( ; b != e; ++b) {
      this->jccjd_queue.insert(
        std::make_pair(std::time(0),
        this->jccjd_jd->set_old(*b)));
//] = this->jccjd_jd->set_old(*b);
    }
    this->jccjd_current = this->jccjd_queue.begin()->second;
    this->jccjd_currentGood = true;
    return;
  }

  // even if no new events were found, older requests
  // can be queued
  if (this->jccjd_queue.empty()) {
    this->jccjd_currentGood = false;
  } else {
    this->jccjd_current = this->jccjd_queue.begin()->second;
    this->jccjd_currentGood = true;
  }

  return;
}

std::string const JobControllerClientJD::get_current_request_name() const {
  return this->jccjd_current.native_file_string();
}

void JobControllerClientJD::release_request()
{
  if (this->jccjd_currentGood) {	
    fs::remove(this->jccjd_current);
    std::multimap<
      std::time_t,
      boost::filesystem::path
    >::iterator it, it_end = this->jccjd_queue.end();
    for (it = this->jccjd_queue.begin(); it != it_end; ++it) {
      if (it->second == this->jccjd_current) {
        this->jccjd_queue.erase(it);
        break;
      }
    }

    this->jccjd_queue.erase(it);
    if (this->jccjd_queue.empty()) {
      this->jccjd_currentGood = false;
    }
  }
}

const Request *JobControllerClientJD::get_current_request()
{
  if (!this->jccjd_currentGood) {
    return 0;
  }

  fs::ifstream is(this->jccjd_current);
  classad::ClassAdParser parser;
  boost::shared_ptr<classad::ClassAd> command_ad(
    parser.ParseClassAd(is)
  );
  
  this->jccjd_request.reset(*command_ad);
  if (command_ad.get()) {
    return &this->jccjd_request;
  } else {
    return 0;
  }
}

} // end namespace controller
} JOBCONTROL_NAMESPACE_END;
