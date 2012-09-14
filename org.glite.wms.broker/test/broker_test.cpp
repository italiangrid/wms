#include "glite/wms/ism/ism.h"
#include "glite/wms/ism/ii-purchaser.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"

#include "glite/wms/common/configuration/exceptions.h"

#include "ResourceBroker.h"
#include "maximize_files_strategy.h"
#include "simple_strategy.h"
#include "stochastic_selector.h"
#include "glite/jdl/JobAdManipulation.h"

#include "matchmaking.h"
#include "exceptions.h"
#include "glite/jdl/ManipulationExceptions.h"

#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>

#include "glite/wms/common/logger/logger_utils.h"

#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/LineParserExceptions.h"
#include "glite/wms/common/utilities/scope_guard.h"


#include <glite/wmsutils/classads/classad_utils.h>

#include "glite/wms/classad_plugin/classad_plugin_loader.h"
#include "brokerinfo.h"

#include <classad_distribution.h>
#include <boost/progress.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "dynamic_library.h"
#include "ism_utils.h"
#include "signal_handling.h"

namespace logger = glite::wms::common::logger;

using namespace std;
using namespace glite;
using namespace glite::wms;
using namespace glite::wms::ism;
using namespace glite::wms::common::utilities;
using namespace glite::wmsutils::classads;
using namespace glite::wms::common::configuration;
using namespace glite::wms::broker;

typedef boost::shared_ptr<glite::wms::manager::server::DynamicLibrary> DynamicLibraryPtr;
typedef boost::shared_ptr<ism::purchaser> PurchaserPtr;

#define edglog(level) logger::threadsafe::edglog << logger::setlevel(logger::level)
#define edglog_fn(name) logger::StatePusher    pusher(logger::threadsafe::edglog, #name);

LineOption  options[] = {
    { 'c', 1, "conf_file", "\t use conf_file as configuration file. glite_wms.conf is the default" },
    { 'j', 1, "jdl_file", "\t use jdl_file as input file." },
    { 'R', 1, "repeat", "\t repeat n times the matchmaking process." },
    { 'w', 1, "wait", "\t time to wait before starting the actual MM." },
    { 'C', no_argument, "show-ce-ad", "\t show computing elements classad representation" },
    { 'B', no_argument, "show-brokerinfo-ad", "\t show brokerinfo classad representation" },
    { 'q', no_argument, "quiet", "\t do not log anything" },
    { 'S', no_argument, "show-se-ad", "\t show storage elements classad representation" },
    { 'v', no_argument, "verbose",     "\t be verbose" },
    { 'l', no_argument, "verbose",     "\t be verbose on log file" },
    { 'r', no_argument, "show-rank", "\t show rank value." }
};

void
show_slice_content(MutexSlicePtr const& mt_slice)
{
  Mutex::scoped_lock l(mt_slice->mutex);
  Slice::const_iterator it = mt_slice->slice->begin();
  Slice::const_iterator const e = mt_slice->slice->end();
  for( ; it != e; ++it) cout << *it << endl;
/*
  copy(
    mt_slice->slice->begin(),
    mt_slice->slice->end(),
    ostream_iterator<slice_type::value_type>(cout, "\n")
  );
*/
}

int main(int argc, char* argv[])
{

  std::vector<LineOption> optvec( options, options + sizeof(options)/sizeof(LineOption) );
  LineParser options( optvec, 0 );

  string conf_file;
  size_t time_2_wait;
  string req_file;

  Ism the_ism;
  set_ism(&the_ism);

  try {
     options.parse( argc, argv );
     conf_file.assign( options.is_present('c') ? options['c'].getStringValue() : "glite_wms.conf" );
     Configuration conf(conf_file.c_str(), ModuleType::network_server);
     NSConfiguration const* const ns_config(conf.ns());

     time_2_wait = options.is_present('w') ? options['w'].getIntegerValue() : 20;

#ifndef GLITE_WMS_HAVE_SYSLOG_LOGGING
     if( options.is_present('v') && !options.is_present('l'))   logger::threadsafe::edglog.open(std::clog, glite::wms::common::logger::debug);
                     
     if( options.is_present('l') ) logger::threadsafe::edglog.open(ns_config->log_file(), glite::wms::common::logger::debug);
     else {
        if( options.is_present('q') ) {
          logger::threadsafe::edglog.open(
            "/dev/null",
            glite::wms::common::logger::debug
          );
        } else {
          logger::threadsafe::edglog.open(
            std::clog, 
            glite::wms::common::logger::debug
          );
        }
     }
#else
     boost::details::pool::singleton_default<
        logger::wms_log
     >::instance().init(
        logger::wms_log::SYSLOG,
        (logger::wms_log::level)ns_config->log_level()
     );
#endif
     
     
     if( ! options.is_present('j') )
     {
        Error( "an input file with the jdl must be passed");
        return -1;
     }
     else
        req_file.assign(options['j'].getStringValue());

     if (!glite::wms::manager::server::signal_handling()) {
       Error("cannot initialize signal handling");
       return -1;
     }

     glite::wms::manager::main::ISM_Manager ism_manager;
     sleep(time_2_wait);

    if(options.is_present('C')){
      for_each(the_ism.computing.begin(), the_ism.computing.end(),
       show_slice_content
      );
    }

    if(options.is_present('S')){
      for_each(the_ism.storage.begin(), the_ism.storage.end(),
       show_slice_content
      );
    }



     Debug("-Reading-JDL-------------------------------------------------");

     ifstream fin(req_file.c_str());    
     if( !fin ) { 
        Warning("cannot open jdl file");
        return 0;
     }

     classad::ClassAdParser parser;
     boost::scoped_ptr<classad::ClassAd> reqAd(parser.ParseClassAd(fin));

     try {

        boost::scoped_ptr<classad::ClassAd> result;
      
        std::string vo(jdl::get_virtual_organisation(*reqAd.get()));
      
        bool input_data_exists = false;
        std::vector<std::string> input_data;
        jdl::get_input_data(*reqAd.get(), input_data, input_data_exists);

        glite::wms::broker::ResourceBroker rb;
        if (input_data_exists) {
          rb.changeStrategy( maximize_files() );
          Debug("-Using-RBMaximizeFiles-implementation------------------------");
        }
        // If fuzzy_rank is true in the request ad we have
        // to use the stochastic selector...
        bool use_fuzzy_rank = false;
        if (jdl::get_fuzzy_rank(*(reqAd.get()), use_fuzzy_rank) && use_fuzzy_rank) {
          rb.changeSelector(stochastic_selector());
          Debug("-Using-sthochastic-rank-selections---------------------------");
         
        }

        size_t n = options.is_present('R') ? options['R'].getIntegerValue() : 0; 
        size_t matches = n;
        boost::timer t0;
        do {  
          boost::shared_ptr<matchtable> suitable_CEs;
          boost::shared_ptr<filemapping> filemapping;
          boost::shared_ptr<storagemapping> storagemapping;
          boost::tie(suitable_CEs,filemapping,storagemapping) = rb.findSuitableCEs(reqAd.get());
        
          Debug(" ----- SUITABLE CEs -------");

          matchtable::iterator ces_it = suitable_CEs->begin();
          matchtable::iterator const ces_end = suitable_CEs->end();

          while( ces_it != ces_end ) {
             Debug(
                ces_it->get<broker::Id>() << 
                std::string(
                  options.is_present('r') ?
                  ", "+boost::lexical_cast<string>(ces_it->get<broker::Rank>()) 
                  : ""
                )
             );
             ++ces_it;
          }

          matchtable::const_iterator best_ce_it;
          if ( suitable_CEs->empty() ) {
            Error("no suitable ce found");      
          }
          else {
            Debug(".....trying selectBestCE");
            best_ce_it = rb.selectBestCE(*suitable_CEs);
          }

          Debug(" ----- BEST CE -------");
          Debug(boost::tuples::get<broker::Id>(*best_ce_it));
          
          if (options.is_present('B')) {
            Debug(" ----- BROKEINFO AD -------");
            boost::scoped_ptr<classad::ClassAd> biAd(
              glite::wms::broker::make_brokerinfo_ad(
               filemapping,storagemapping,
               *boost::tuples::get<broker::Ad>(*best_ce_it)
              )
            );
            Debug(*biAd.get());
          }
       } while(n--);
       Debug(matches << " matchmaking iterations performed in " << t0.elapsed() << " seconds.");
     }
     catch (InformationServiceError const& e) {
     
       Error("matchmaking::InformationServiceError: "
                 <<e.what());
     
     } catch (RankingError const& e) {
     
       Error("matchmaking::RankingError: "
                 << e.what());
     
     } catch (jdl::CannotGetAttribute const& e) {
     
       Error("jdl::CannotGetAttribute: "
                 << e.what());
     
     } catch (jdl::CannotSetAttribute const& e) {
     
       Error("jdl::CannotSetAttribute: "
                 << e.what());

     }

     Debug("---------------------------------------------------");
     Debug("END");
  }
  catch ( LineParsingError &er ) {
    Error( er.what() );
    exit( er.return_code() );
  }
  catch( CannotConfigure &er ) {
    Error( er.what() );
  }
  catch ( glite::wms::manager::server::CannotLoadDynamicLibrary &ex ) {
    Error("CannotLoadDynamicLibrary");
    Error(ex.error());
    exit( -1 );
  }
  catch( glite::wms::manager::server::CannotLookupSymbol &ex ) {
    Error("CannotLookupSymbol");
    Error(ex.error());
    exit( -1 );
  }
  catch( ... ) {
    Error("Uncaught exception...");
  }	 
  return 0;


}

