#include <iostream>
#include <string>
#include <fstream>
#include <classad_distribution.h>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

#include "glite/wms/ism/ism.h"
#include "glite/wms/brokerinfo/storage_utils.h"
#include "glite/wms/brokerinfo/brokerinfo.h"
#include "glite/wms/broker/RBMaximizeFilesISMImpl.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/LineParserExceptions.h"
#include "glite/jobid/JobId.h"
#include <glite/wmsutils/classads/classad_utils.h>

namespace logger = glite::wms::common::logger;

using namespace std;
using namespace glite::wms::common::utilities;
using namespace glite::wmsutils::classads;
using namespace glite::wms::common::configuration;

#define edglog(level) logger::threadsafe::edglog << logger::setlevel(logger::level)
#define edglog_fn(name) logger::StatePusher    pusher(logger::threadsafe::edglog, #name);

class run_in_loop
{
  boost::function<void()> m_function;
  int m_period;

public:
  run_in_loop(boost::function<void()> const& f, int s)
    : m_function(f), m_period(s)
  { }
  void operator()()
  {
    while (true) {
      m_function();
      sleep(m_period);
    }
  }
};

class BrokerInfo {
  glite::wms::broker::RBMaximizeFilesISMImpl& m_bi;
  classad::ClassAd* m_ad_ptr;
  int m_retries;
public:
  BrokerInfo(glite::wms::broker::RBMaximizeFilesISMImpl& bi, classad::ClassAd* ad_ptr, int retries)
    : m_bi(bi), m_ad_ptr(ad_ptr), m_retries(retries) { }
  void operator()() {
    for (int i = 0; i < m_retries; ++i) {
       m_bi.findSuitableCEs(m_ad_ptr);
    }
  }
};


LineOption  options[] = {
    { 'c', 1, "conf_file", "\t use conf_file as configuration file. glite_wms.conf is the default" },
    { 'j', 1, "jdl_file", "\t use jdl_file as input file." },
    { 't', 1, "num_threads", "\t number of threads performing concurrent findSuitableCEs() calls" },
    { 'n', 1, "number", "\t number of retries" },
//    { 'v', no_argument, "verbose",     "\t be verbose" },
    { 'l', no_argument, "verbose",     "\t be verbose on log file" }
};

int main(int argc, char* argv[])
{
  std::vector<LineOption> optvec( options, options + sizeof(options)/sizeof(LineOption) );
  LineParser options( optvec, 0 );

  string conf_file;
  string req_file;

  try {
    options.parse( argc, argv );
    conf_file.assign( options.is_present('c') ? options['c'].getStringValue() : "glite_wms.conf" );
    Configuration conf(conf_file.c_str(), ModuleType::network_server);
    NSConfiguration const* const ns_config(conf.ns());
    if (options.is_present('l')) {
      logger::threadsafe::edglog.open(
        ns_config->log_file(), glite::wms::common::logger::debug);
    } else { 
      logger::threadsafe::edglog.open(
        std::clog, glite::wms::common::logger::debug);
    }

    int num_threads = 1;
    if (options.is_present('t')) {
      num_threads = options['t'].getIntegerValue();
    }
    if (! options.is_present('j')) {
      edglog(error) << "an input file with the jdl must be passed"<< endl;
      return -1;
    } else {
      req_file.assign(options['j'].getStringValue());
    }

    int count = 1;
    if( options.is_present('n') )  count = options['n'].getIntegerValue();

    edglog_fn("Catalog access unit test");
    edglog(debug) << "Running with broker model RBMaximizeFilesISMImpl" << endl;
    edglog(debug) << "reading from " << req_file << endl;

    glite::wms::ism::ism_type ism_1[2];
    glite::wms::ism::ism_type ism_2[2];
    glite::wms::ism::ism_mutex_type ism_mutex[2];

    glite::wms::ism::set_ism(ism_1, ism_2, ism_mutex, glite::wms::ism::ce);
    glite::wms::ism::set_ism(ism_1, ism_2, ism_mutex, glite::wms::ism::se);
    // do an asynchronous purchasing, otherwise MM won't return any entries

    ifstream ifs(req_file.c_str());    
    if (ifs) { 
      classad::ClassAdParser parser;
      classad::ClassAd *reqAd = parser.ParseClassAd(ifs, false);

      if (reqAd) {
        glite::wms::broker::RBMaximizeFilesISMImpl bi;
        BrokerInfo bi_(bi, reqAd, count);
        boost::thread_group bi_threads;

        for (int i = 0; i < num_threads; ++i) {
          bi_threads.add_thread(
            new boost::thread(
              run_in_loop(boost::bind(&BrokerInfo::operator(), bi_), 1)));
        }
        bi_threads.join_all();
        delete reqAd;
      } else {
        edglog_fn("reqAd=0, likely to be a parsing error");
        edglog(debug) << *reqAd << std::endl;
      }
    } else {
      edglog(warning) << "cannot open jdl file" << endl;
    }
    edglog(debug) << "END"<< endl;
  } catch ( LineParsingError &er ) {
    cerr << er << endl;
    exit( er.return_code() );
  } catch( CannotConfigure &er ) {
    cerr << er << endl;
  } catch( ... ) {
    cout << "Uncaught exception..." << endl;
  }	 

  return 0;
}
