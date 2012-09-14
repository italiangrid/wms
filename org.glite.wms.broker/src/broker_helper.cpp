// File: broker_helper.cpp
// Author: Francesco Giacomini

// $Id$

#include <fstream>
#include <stdexcept>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/timer.hpp>

#include <classad_distribution.h>

#include "glite/wms/broker/resolver.h"
#include "glite/wms/broker/match.h"
#include "Helper_exceptions.h"
#include "exceptions.h"
#include "brokerinfo.h"

#include "glite/wms/classad_plugin/classad_plugin_loader.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"

#include "glite/wms/common/logger/logging.h"

#include "glite/wmsutils/classads/classad_utils.h"

//#include "glite/wms/helper/HelperFactory.h"
//#include "glite/wms/helper/exceptions.h"

#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

#ifndef GLITE_WMS_DONT_HAVE_GPBOX
#include "gpbox_utils.h"
#endif

namespace fs            = boost::filesystem;
namespace jobid         = glite::wmsutils::jobid;
namespace configuration = glite::wms::common::configuration;
namespace jdl           = glite::jdl;
namespace utils         = glite::wmsutils::classads;

#ifndef GLITE_WMS_DONT_HAVE_GPBOX
namespace helper	      = glite::wms::broker::gpbox;
#endif

namespace glite {
namespace wms {
namespace broker {

using namespace helper;

namespace {

std::string const helper_id("BrokerHelper");

boost::shared_ptr<classad::ClassAd>
f_resolve_simple(
  boost::shared_ptr<classad::ClassAd> ad,
  std::string const& ce_id
)
{
  boost::shared_ptr<classad::ClassAd> result;

  static boost::regex  expression( "(.+/[^\\-]+-([^\\-]+))-(.+)" );
  boost::smatch        pieces;
  std::string gcrs, type, name;
  if (boost::regex_match(ce_id, pieces, expression)) {
    gcrs.assign(pieces[1].first, pieces[1].second);
    type.assign(pieces[2].first, pieces[2].second);
    name.assign(pieces[3].first, pieces[3].second);

    result.reset(new classad::ClassAd(*ad));
    jdl::set_globus_resource_contact_string(*result, gcrs);
    jdl::set_queue_name(*result, name);
    try {
      std::string junk = jdl::get_lrms_type(*result);
    } catch ( glite::jdl::CannotGetAttribute const& e) {
      jdl::set_lrms_type(*result, type);
    }
   
    jdl::set_ce_id(*result, ce_id);

    // TODO catch jdl::CannotSetAttribute
  } else {
    throw glite::wms::helper::InvalidAttributeValue(
      jdl::JDL::SUBMIT_TO,
      ce_id,
      "match (.+/[^\\-]+-([^\\-]+))-(.+)",
      helper_id
    );
  }

  return result;
}

std::string
flatten_requirements (
  configuration::Configuration const& config,
  classad::ClassAd const *job_ad,
  classad::ClassAd const *ce_ad
)
{
  const configuration::WMConfiguration* WM_conf = config.wm();
  assert(WM_conf);

  std::string result; // Empty string causes the attribute not to be set.

  std::vector<std::string> reqs_to_forward(WM_conf->ce_forward_parameters());

  // In order to forward the required attributes, these
  // have to be *removed* from the CE ad that is used
  // for flattening.
  classad::ClassAd* local_ce_ad(new classad::ClassAd(*ce_ad));
  classad::ClassAd* local_job_ad(new classad::ClassAd(*job_ad));

  std::vector<std::string>::const_iterator cur_req;
  std::vector<std::string>::const_iterator req_end = reqs_to_forward.end();
  for (cur_req = reqs_to_forward.begin();
       cur_req != req_end; cur_req++)
   {
     local_ce_ad->Remove(*cur_req);
     // Don't care if it doesn't succeed. If the attribute is
     // already missing, so much the better.
   }

  // Now, flatten the Requirements expression of the Job Ad
  // with whatever info is left from the CE ad.
  // Recipe received from Nick Coleman, ncoleman@cs.wisc.edu
  // on Tue, 8 Nov 2005
  classad::MatchClassAd mad;
  mad.ReplaceLeftAd( local_job_ad );
  mad.ReplaceRightAd( local_ce_ad );

  classad::ExprTree *req = mad.GetLeftAd()->Lookup( "Requirements" );
  classad::ExprTree *flattened_req = 0;
  classad::Value fval;

  if( ! ( mad.GetLeftAd()->Flatten( req, fval, flattened_req ) ) ) {
        // Error while flattening. Result is undefined.
        return result;
  }
  
  // No remaining requirement. Result is undefined.
  if (!flattened_req) return result;

  // The resulting requirements need to be passed via a
  // submit file, so we turn them into a string.

  classad::PrettyPrint res_unp; 

  res_unp.Unparse(result, flattened_req);
  return result;
}

boost::shared_ptr<classad::ClassAd>
f_resolve_mm(boost::shared_ptr<classad::ClassAd> ad)
try {
  boost::shared_ptr<classad::ClassAd> result;
  classad::ClassAd& input_ad = *ad;

  std::vector< previous_match > previous_matches;
  {
    classad::ExprList* pm_expr_list;
    bool exists = input_ad.EvaluateAttrList(
                     "edg_previous_matches_ex",
                     pm_expr_list
                  );
    if( exists ) {
      classad::ExprList::iterator it = pm_expr_list->begin();
      classad::ExprList::const_iterator e = pm_expr_list->end();
      for( ; it != e; it++){
         std::string ce_id;
         int pm_time;
         bool check_ce =
           static_cast<classad::ClassAd*>(*it)->
             EvaluateAttrString("ce_id", ce_id);
         bool check_pm_time =
           static_cast<classad::ClassAd*>(*it)->
             EvaluateAttrInt("timestamp", pm_time);
         if( check_ce && check_pm_time )
            previous_matches.push_back(
              previous_match( ce_id, pm_time )
            );
      }

    }

  }
  DataInfo data_info;
  MatchTable matches;

  if( input_ad.Lookup("InputData") || input_ad.Lookup("DataRequirements") )
    match(input_ad, matches, data_info, previous_matches);
  else
    match(input_ad, matches, previous_matches);

  if (matches.empty()) {
    throw NoCompatibleCEs();
  }

  configuration::Configuration const* const config
    = configuration::Configuration::instance();
  assert(config);

#ifndef GLITE_WMS_DONT_HAVE_GPBOX
  std::string dg_jobid_str(jdl::get_edg_jobid(input_ad));
  jobid::JobId dg_jobid(dg_jobid_str);

  std::string PBOX_host_name(config->wm()->pbox_host_name());

  if (!PBOX_host_name.empty()) {
    if (!gpbox::interact(
      *config,
      dg_jobid,
      PBOX_host_name,
      matches
    ))
      GLITE_LOG_ACCESS_UTILS<<"Error during gpbox interaction";
  }

  if (matches.empty()) {
    GLITE_LOG_ACCESS_UTILS<<"Empty CE list after gpbox screening";
    throw NoCompatibleCEs();
  }
#endif

  MatchTable::const_iterator ce_it;

  // If fuzzy_rank is true in the request ad we have
  // to use the stochastic selector...
  bool use_fuzzy_rank = false;
  if (jdl::get_fuzzy_rank(input_ad, use_fuzzy_rank) && use_fuzzy_rank) 
  {
    double factor = 1;
    try {
      factor = jdl::get_fuzzy_factor(input_ad);
    } 
    catch(...)
    {
    }
    ce_it = FuzzySelector(factor)(matches);
  } else {
    ce_it = MaxRankSelector()(matches);
  }

  classad::ClassAd const& ce_ad = *ce_it->ce_ad;
  // TODO: is the following ce_id the same as ce_it->ce_id?
  std::string const ce_id(
    utils::evaluate_attribute(ce_ad,"GlueCEUniqueID")
  );

  // Add the .Brokerinfo files to the InputSandbox
  bool input_sandbox_exists = false;
  bool wmpinput_sandbox_base_uri_exists = false;
  
  fs::path brokerinfo_path(
    jdl::get_input_sandbox_path(input_ad) + "/.BrokerInfo",
    fs::native
  );

  std::vector<std::string> ISB;
   
  jdl::get_input_sandbox(input_ad, ISB, input_sandbox_exists);
  std::string WMPInputSandboxBaseURI(jdl::get_wmpinput_sandbox_base_uri(
                                             input_ad, 
                                             wmpinput_sandbox_base_uri_exists)
                                    );

  if (wmpinput_sandbox_base_uri_exists) {
    ISB.push_back(WMPInputSandboxBaseURI+"/input/.BrokerInfo");
  }
  else {
    ISB.push_back(".BrokerInfo");
  }
   
  std::ofstream BIfilestream(brokerinfo_path.native_file_string().c_str());

  if (!BIfilestream) {
    throw helper::CannotCreateBrokerinfo(brokerinfo_path);
  }

  boost::scoped_ptr<classad::ClassAd> biAd(
    create_brokerinfo(input_ad, ce_ad, data_info)
  );
  classad::ExprTree const* DACexpr = input_ad.Lookup("DataAccessProtocol");
  if (DACexpr) {
    biAd->Insert("DataAccessProtocol", DACexpr->Copy());
  }
  BIfilestream << *biAd << std::endl;

  if (!BIfilestream) {
    throw CannotCreateBrokerinfo(brokerinfo_path);
  }

  result.reset(new classad::ClassAd(input_ad));

  jdl::set_input_sandbox(*result, ISB);

  jdl::set_ce_id(*result, ce_id);

  try {
    std::string flatten_result = flatten_requirements(
      *config, 
      &input_ad, 
      &ce_ad
    );
    // Set attribute only if it's not empty, so as not to upset 
    // condor_submit.
    if (!flatten_result.empty()) {
      jdl::set_remote_remote_ce_requirements(
        *result,
         flatten_result
      );
    }
  } catch (...) {
    // Let's leave remote_remote_requirements undefined if
    // anything went wrong.
  }

  try {

    jdl::set_globus_resource_contact_string(
      *result,
      utils::evaluate_attribute(ce_ad, "GlobusResourceContactString")
    );
    jdl::set_queue_name(
      *result,
      utils::evaluate_attribute(ce_ad, "QueueName")
    );
    jdl::set_lrms_type(
      *result,
      utils::evaluate_attribute(ce_ad, "LRMSType")
    );
    jdl::set_ce_id(
      *result,
      utils::evaluate_attribute(ce_ad, "CEid")
    );

  } catch (utils::InvalidValue const& e) {

    GLITE_LOG_ACCESS_ERROR<<e.what() << " for CE id " << ce_id;

    throw glite::wms::helper::HelperError("BrokerHelper");

  }

  return result;

} catch (ISNoResultError const& e) {

  throw NoAvailableCEs(
    "The user is not authorized on any resource currently registered in "
    + e.host()
  );

} catch (InformationServiceError const& e) {

  throw NoAvailableCEs(e.what());
  
} catch (RankingError const& e) {
  
  throw NoAvailableCEs(e.what());
  
} catch (jdl::CannotGetAttribute const& e) {

  throw glite::wms::helper::CannotGetAttribute(e, helper_id);

} catch (jdl::CannotSetAttribute const& e) {

  throw glite::wms::helper::CannotSetAttribute(e, helper_id);

} catch( jobid::JobIdException& jide ) {

  GLITE_LOG_ACCESS_ERROR<< jide.what(); 
  throw glite::wms::helper::InvalidAttributeValue(jdl::JDL::JOBID,
                                      "unknown",
                                      "valid jobid",
                                      helper_id);

} catch( fs::filesystem_error& fse ) {

    GLITE_LOG_ACCESS_ERROR<< fse.what();
    throw glite::wms::helper::FileSystemError(helper_id, fse);

}

} // {anonymous}

boost::shared_ptr<classad::ClassAd>
broker_helper_resolve(boost::shared_ptr<classad::ClassAd> ad)
{
  bool submit_to_exists = false;
  std::string ce_id = jdl::get_submit_to(*ad, submit_to_exists);

  boost::shared_ptr<classad::ClassAd> result(
    submit_to_exists
    ? f_resolve_simple(ad, ce_id) 
    : f_resolve_mm(ad)
  );

  return result;
}

}}}
