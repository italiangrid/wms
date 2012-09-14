#include <dlfcn.h>
#include "glite/wms/ism/ism.h"
#include "glite/wms/ism/ii-purchaser.h"
#include "glite/wms/ism/rgma-purchaser.h"

//#include "Helper_matcher.h"
//#include "Helper.h"
//#include "Helper_exceptions.h"

#include "glite/wms/helper/Helper.h"

#include "glite/wms/helper/exceptions.h"

#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"

#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/LineParserExceptions.h"

#include "glite/wms/common/logger/logger_utils.h"

#include "glite/wms/classad_plugin/classad_plugin_loader.h"

#include <glite/wmsutils/classads/classad_utils.h>
#include <classad_distribution.h>

#include <boost/scoped_ptr.hpp>
#include <boost/array.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "dynamic_library.h"
#include "ism_utils.h"
#include "signal_handling.h"



#define edglog(level) logger::threadsafe::edglog << logger::setlevel(logger::level)
#define edglog_fn(name) logger::StatePusher    pusher(logger::threadsafe::edglog, #name);

namespace logger = glite::wms::common::logger;

using namespace std;
using namespace glite;
using namespace glite::wms;
using namespace glite::wms::ism;
using namespace glite::wms::common::utilities;
using namespace glite::wmsutils::classads;
using namespace glite::wms::common::configuration;
//using namespace glite::wms::broker;

LineOption  options[] = {
    { 'c', 1, "conf_file", "\t use conf_file as configuration file. glite_wms.conf is the default" },
    { 'j', 1, "jdl_file", "\t use jdl_file as input file." },
    { 'r', 1, "repeat", "\t repeat n times the matchmaking process." },
    { 'w', 1, "wait", "\t time to wait before starting the actual MM." },
    { 'C', no_argument, "show-ce-ad", "\t show computing elements classad representation" },
    { 'B', no_argument, "show-brokerinfo-ad", "\t show brokerinfo classad representation" },
    { 'q', no_argument, "quiet", "\t do not log anything" },
    { 'S', no_argument, "show-se-ad", "\t show storage elements classad representation" },
    { 'v', no_argument, "verbose",     "\t be verbose" },
    { 'l', no_argument, "verbose",     "\t be verbose on log file" },
    { 'm', no_argument, "verbose",     "\t matcher helper" },
    { 'b', no_argument, "verbose",     "\t broker helper" }
};


void
show_slice_content(MutexSlicePtr const& mt_slice)
{
  Mutex::scoped_lock l(mt_slice->mutex);
  Slice::const_iterator it = mt_slice->slice->begin();
  Slice::const_iterator const e = mt_slice->slice->end();
  for( ; it != e; ++it) cout  << *it << endl;
/*
  copy(
    mt_slice->slice->begin(),
    mt_slice->slice->end(),
    ostream_iterator<slice_type::value_type>(cout, "\n")
  );
*/
}
/*
typedef boost::shared_ptr<DynamicLibrary> DynamicLibraryPtr;

DynamicLibraryPtr make_dll(std::string const& lib_name)
{
  return DynamicLibraryPtr(new DynamicLibrary(lib_name));
}
*/


int main(int argc, char* argv[])
{
  boost::array<std::string,3> plugins = { { "gangmatch", "fqan", "lb_rank" } };
  std::for_each(plugins.begin(), plugins.end(), glite::wms::classad_plugin::init());
/*
  try{
    std::string const dll_name("libglite_wms_broker_helper.so");
    DynamicLibraryPtr dll(make_dll(dll_name));
  }
  catch( const CannotLoadDynamicLibrary& ex){
	std::cout <<"suca" <<std::endl;
     Error(ex.filename());
     Error(ex.error());
  }  
*/
         void* libHandle = dlopen ("libglite_wms_broker_helper.so", RTLD_NOW);
         if (!libHandle) {
            std::cerr<<"cannot load wms_broker_helper lib "<<std::endl;
            std::cerr<<"dlerror returns: " << dlerror()<<std::endl;
            return -1;
         }




  std::vector<LineOption> optvec( options, options + sizeof(options)/sizeof(LineOption) );
  LineParser options( optvec, 0 );

  string conf_file;
  string req_file;

  try {
     options.parse( argc, argv );
     conf_file.assign( options.is_present('c') ? options['c'].getStringValue() : "glite_wms.conf" );
     Configuration conf(conf_file.c_str(), ModuleType::network_server);
     NSConfiguration const* const ns_config(conf.ns());


#ifndef GLITE_WMS_HAVE_SYSLOG_LOGGING
     if( options.is_present('v') && !options.is_present('l'))
       logger::threadsafe::edglog.open(std::clog, glite::wms::common::logger::debug); 

     if( options.is_present('l') ) logger::threadsafe::edglog.open(ns_config->log_file(),glite::wms::common::logger::debug);
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


    SlicePtr ce_slice_ptr( new Slice );
    SlicePtr se_slice_ptr( new Slice );

    MutexSlicePtr ce_mt_slice ( new MutexSlice );
    MutexSlicePtr se_mt_slice ( new MutexSlice );

    ce_mt_slice->slice = ce_slice_ptr;
    se_mt_slice->slice = se_slice_ptr;

    Ism the_ism;

    the_ism.computing.push_back(ce_mt_slice);
    the_ism.storage.push_back(se_mt_slice);

    set_ism( &the_ism );

    ii_purchaser icp(ce_mt_slice, se_mt_slice,
        ns_config->ii_contact(), ns_config->ii_port(),
        ns_config->ii_dn(), ns_config->ii_timeout()
    );
//    rgma_purchaser icp(ce_mt_slice, se_mt_slice);

    icp();

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

/*
    glite::wms::manager::server::signal_handling();

    glite::wms::manager::main::ISM_Manager ism_manager;     

    sleep(15);
*/

    ifstream fin(req_file.c_str());
    if( !fin ) {
       Warning("cannot open jdl file");
       return 0;
    }

    boost::scoped_ptr<classad::ClassAd> result;
    classad::ClassAdParser parser;
    boost::scoped_ptr<classad::ClassAd> reqAd(parser.ParseClassAd(fin));

    try {
       if (options.is_present('m')){
          boost::scoped_ptr<classad::ClassAd> result;
          reqAd->InsertAttr("include_brokerinfo", true);
          reqAd->InsertAttr("number_of_results", 10);

          //result.reset(glite::wms::broker::helper::matcher::Helper().resolve(reqAd.get()));
          result.reset(glite::wms::helper::Helper("MatcherHelper").resolve(reqAd.get()));

          //edglog(debug)<< *result << endl;
          classad::ExprList* expr_list;
          bool check = result->EvaluateAttrList("match_result", expr_list);
          if ( check ) {
             classad::ExprList::iterator list_it = expr_list->begin();
             classad::ExprList::const_iterator list_end = expr_list->end();
             for ( ; list_it < list_end; list_it++ ){
                string ce_id;
                (static_cast<classad::ClassAd*>(*list_it))->EvaluateAttrString("ce_id", ce_id);
                Debug( ce_id );
             }
          }
       }

       if (options.is_present('b')) {
          boost::scoped_ptr<classad::ClassAd> result;
          reqAd->InsertAttr("include_brokerinfo", true);
          reqAd->InsertAttr("number_of_results", 10);

          //result.reset(glite::wms::broker::helper::Helper().resolve(reqAd.get()));
          result.reset(glite::wms::helper::Helper("BrokerHelper").resolve(reqAd.get()));

          Debug(*result);


       }


    } 
    catch (glite::wms::helper::HelperError const& e) {
       Error("HelperError exception caught: "<<
                     e.what());
    }

  }
  catch ( LineParsingError &er ) {
     Error("LineParsingError exception caght");
  }
  catch( CannotConfigure &er ) {
     Error("CannotConfigure exception caght");
  }

  return 0;
}

