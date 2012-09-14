#include <dlfcn.h>
#include "glite/wms/ism/ism.h"
#include "glite/wms/ism/purchaser/ism-ii-g2-purchaser.h"
#include "glite/wms/ism/purchaser/ism-ii-purchaser.h"

//#include "Helper_matcher.h"

#include "Helper.h"

#include "glite/wms/helper/broker/exceptions.h"
//#include "broker/Helper.h"

#include "glite/wms/helper/exceptions.h"

#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"

#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/LineParserExceptions.h"

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"
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

#define edglog(level) logger::threadsafe::edglog << logger::setlevel(logger::level)
#define edglog_fn(name) logger::StatePusher    pusher(logger::threadsafe::edglog, #name);

namespace logger = glite::wms::common::logger;

using namespace std;
using namespace glite;
using namespace glite::wms;
using namespace glite::wms::ism;
using namespace glite::wms::ism::purchaser;
using namespace glite::wms::common::utilities;
using namespace glite::wmsutils::classads;
using namespace glite::wms::common::configuration;
using namespace glite::wms::helper::broker;

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
    { 'b', no_argument, "verbose",     "\t broker helper" },
    { 'd', 1, "dump_file", "\t load ism from dump" }
};

namespace {
//glite::wms::classad_plugin::classad_plugin_loader init;
boost::mutex f_output_mutex;
}


void load_dump(string const& filename)
{
    std::ifstream src(filename.c_str());
    if (!src.good()) {
        Warning("Unable to load ISM status from dump file: " << filename << "\n");
        return;
    }

    int errors = 0;
    while(!src.eof()) {
      boost::scoped_ptr<classad::ClassAd> ad;
      string id;
      try {
        ad.reset(parse_classad(src));
        ad->EvaluateAttrString("id", id);
      }
      catch (CannotParseClassAd&) {
        ++errors;
        continue;
      }

      int ut = std::time(0);
      const classad::ClassAd *i=evaluate_attribute(*ad,"info");

      boost::shared_ptr<classad::ClassAd> info(static_cast<classad::ClassAd*>(i->Copy()));
        info->SetParentScope(0);
      ism_type* the_se_ism = &get_ism(se); 
        if(info->Lookup("GlueSEUniqueID")) {
          the_se_ism->insert(
            make_ism_entry(id, ut, info, boost::function<bool(int&, ad_ptr)>())
          );
        }
        else
        if(info->Lookup("GlueCEUniqueID")) {

         ism_type* the_ce_ism = &get_ism(ce); 
          // Check the type of puchaser which has generated the info
          string purchased_by;
          info->EvaluateAttrString("PurchasedBy",purchased_by);
          if (purchased_by==string("ism_ii_purchaser")) {

            insert_aux_requirements(info);
            expand_glueceid_info(info);

            the_ce_ism->insert(
              make_ism_entry(id, ut, info, boost::function<bool(int&, ad_ptr)>())
            );
          }
          else if (purchased_by==string("ism_ii_g2_purchaser")) {
             the_ce_ism->insert(
              make_ism_entry(id, ut, info, boost::function<bool(int&, ad_ptr)>())
            );
          }
          else if (purchased_by==string("ism_cemon_purchaser")) {
            the_ce_ism->insert(
              make_ism_entry(id, ut, info, boost::function<bool(int&, ad_ptr)>())
            );
          }
          else if (purchased_by==string("ism_rgma_purchaser")) {
            the_ce_ism->insert(
              make_ism_entry(id, ut, info, boost::function<bool(int&, ad_ptr)>())
            );
          }
          else if (purchased_by==string("ism_cemon_asynch_purchaser")) {
            the_ce_ism->insert(make_ism_entry(id, ut, info));
          }
        }
      }
      Warning("Unable to load " << errors << " entries from " << filename << "\n");

}


struct broker_helper_call
{
  classad::ClassAd* reqAd;
  broker_helper_call(classad::ClassAd* ad) : reqAd(ad) {}
  void operator()() {
    boost::scoped_ptr<classad::ClassAd> result;
    result.reset(glite::wms::helper::Helper("BrokerHelper").resolve(reqAd));
    {
      boost::mutex::scoped_lock l(f_output_mutex);
      edglog(info)<< *result << endl;
    }
  }
};

namespace {
ism_type the_ism[4];
ism_mutex_type the_ism_mutex[2];
}
 
int main(int argc, char* argv[])
{

       void* libHandle = dlopen ("libglite_wms_helper_broker_ism.so", RTLD_NOW);
       if (!libHandle) {
          std::cerr<<"cannot load wms_helper_broker_ism lib "<<std::endl;
          std::cerr<<"dlerror returns: " << dlerror()<<std::endl;
          return -1;
       }

  set_ism(the_ism,the_ism+2,the_ism_mutex,ce);
  set_ism(the_ism,the_ism+2,the_ism_mutex,se);

  std::vector<LineOption> optvec( options, options + sizeof(options)/sizeof(LineOption) );
  LineParser options( optvec, 0 );

  string conf_file;
  string req_file;

  try {
     options.parse( argc, argv );
     conf_file.assign( options.is_present('c') ? options['c'].getStringValue() : "glite_wms.conf" );
     Configuration conf(conf_file.c_str(), ModuleType::network_server);
     NSConfiguration const* const ns_config(conf.ns());
     WMConfiguration const* const wm_config(conf.wm());
     if( options.is_present('v') && !options.is_present('l'))
       logger::threadsafe::edglog.open(std::clog, glite::wms::common::logger::debug); 

     if( options.is_present('l') ) logger::threadsafe::edglog.open(ns_config->log_file(),glite::wms::common::logger::debug);
     if( ! options.is_present('j') )
     {
        edglog(error) << "an input file with the jdl must be passed"<< endl;
        return -1;
     }
     else
        req_file.assign(options['j'].getStringValue());

     if(!options.is_present('d')) {

       bool const glue20_purchasing_is_enabled(
         wm_config->enable_ism_ii_glue20_purchasing()
       );
       bool const glue13_purchsing_is_enabled(
         wm_config->enable_ism_ii_glue13_purchasing()
       );

       if (glue13_purchsing_is_enabled) {
         ism_ii_purchaser icp_g13(
            ns_config->ii_contact(), ns_config->ii_port(),
            ns_config->ii_dn(), ns_config->ii_timeout(), std::string(), false, once
         );
         icp_g13();
       }
       if (glue20_purchasing_is_enabled) {
         ism_ii_g2_purchaser icp_g20(
            ns_config->ii_contact(), ns_config->ii_port(),
            "o=glue", ns_config->ii_timeout(), std::string(), false, once
         );
         icp_g20();
       }
    }
    else{
       string dump_file(
         options['d'].getStringValue()
       );
       load_dump(dump_file);
    }

    if(options.is_present('C')){
 
      ism_mutex_type::scoped_lock l(get_ism_mutex(ce));
      for (ism_type::iterator pos=get_ism(ce).begin();
        pos!= get_ism(ce).end(); ++pos) {

          classad::ClassAd  ad_ism_dump;
          ad_ism_dump.InsertAttr("id", pos->first);
          ad_ism_dump.InsertAttr("update_time", 
                    boost::tuples::get<0>(pos->second));
          ad_ism_dump.InsertAttr("expiry_time", 
                    boost::tuples::get<1>(pos->second));
          ad_ism_dump.Insert("info", 
                    boost::tuples::get<2>(pos->second).get()->Copy());

          edglog(debug) << ad_ism_dump <<std::endl;
       }

    }

    if(options.is_present('S')){

      ism_mutex_type::scoped_lock l(get_ism_mutex(se));
      for (ism_type::iterator pos=get_ism(se).begin();
        pos!= get_ism(se).end(); ++pos) {


          classad::ClassAd  ad_ism_dump;
          ad_ism_dump.InsertAttr("id", pos->first);
          ad_ism_dump.InsertAttr("update_time",
                    boost::tuples::get<0>(pos->second));
          ad_ism_dump.InsertAttr("expiry_time",
                    boost::tuples::get<1>(pos->second));
          ad_ism_dump.Insert("info",
                    boost::tuples::get<2>(pos->second).get()->Copy());

          edglog(debug) << ad_ism_dump <<std::endl;
       }

    }

    ifstream fin(req_file.c_str());
    if( !fin ) {
       edglog(warning) << "cannot open jdl file" << endl;
       return 0;
    }

//    boost::scoped_ptr<classad::ClassAd> result;
    classad::ClassAdParser parser;
    boost::scoped_ptr<classad::ClassAd> reqAd(parser.ParseClassAd(fin));

    try {

       if (options.is_present('m')){
          boost::scoped_ptr<classad::ClassAd> result;
          reqAd->InsertAttr("include_brokerinfo", true);
          reqAd->InsertAttr("number_of_results", 10);


          //result.reset(glite::wms::helper::matcher::Helper().resolve(reqAd.get()));
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
                edglog(debug) <<"CE_ID: "<< ce_id << endl;
                
             }
          }
       }

       if (options.is_present('b')) {
          boost::scoped_ptr<classad::ClassAd> result;
          reqAd->InsertAttr("include_brokerinfo", true);
          reqAd->InsertAttr("number_of_results", 10);

          //result.reset(glite::wms::helper::broker::Helper().resolve(reqAd.get()));
          result.reset(glite::wms::helper::Helper("BrokerHelper").resolve(reqAd.get()));

          edglog(info)<< *result << endl;


       }
   } 
    catch (glite::wms::helper::HelperError const& e) {
       edglog(error) << "HelperError exception caught: "<<
                     e.what()<< endl;
    }

  }
  catch ( LineParsingError &er ) {
     edglog(error)<< "LineParsingError exception caght" << endl;
  }
  catch( CannotConfigure &er ) {
    edglog(error) << "CannotConfigure exception caght" << endl;
  }

  return 0;
}

