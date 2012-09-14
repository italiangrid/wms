// File: Helper.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

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

#include "Helper.h"
#include "Helper_exceptions.h"
#include "simple_strategy.h"
#include "maximize_files_strategy.h"
#include "stochastic_selector.h"
#include "exceptions.h"
#include "storage_utils.h"

#include "glite/wms/classad_plugin/classad_plugin_loader.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"

#include "glite/wms/common/logger/logger_utils.h"

#include "glite/wmsutils/classads/classad_utils.h"

#include "glite/wms/helper/HelperFactory.h"
#include "glite/wms/helper/exceptions.h"

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
namespace logger        = glite::wms::common::logger;
namespace configuration = glite::wms::common::configuration;
namespace requestad     = glite::jdl;
namespace utils         = glite::wmsutils::classads;

#ifndef GLITE_WMS_DONT_HAVE_GPBOX
namespace helper	      = glite::wms::broker::gpbox;
#endif

namespace glite {
namespace wms {
namespace broker {
namespace helper {
namespace {

std::string const helper_id("BrokerHelper");

glite::wms::helper::HelperImpl* create_helper()
{
  return new Helper;
}

struct Register
{
  Register()
  {
    glite::wms::helper::HelperFactory::instance()->register_helper(
      helper_id, create_helper
    );
  }
  ~Register()
  {
    glite::wms::helper::HelperFactory::instance()->unregister_helper(
      helper_id
    );
  }
};

Register const r;

std::string const f_output_file_suffix(".rbh");

std::auto_ptr<classad::ClassAd>
f_resolve_simple(classad::ClassAd const& input_ad, std::string const& ce_id)
{
  std::auto_ptr<classad::ClassAd> result;

  static boost::regex  expression( "(.+/[^\\-]+-([^\\-]+))-(.+)" );
  boost::smatch        pieces;
  std::string gcrs, type, name;
  if (boost::regex_match(ce_id, pieces, expression)) {
    gcrs.assign(pieces[1].first, pieces[1].second);
    type.assign(pieces[2].first, pieces[2].second);
    name.assign(pieces[3].first, pieces[3].second);

    result.reset(new classad::ClassAd(input_ad));
    requestad::set_globus_resource_contact_string(*result, gcrs);
    requestad::set_queue_name(*result, name);
    try {
      std::string junk = requestad::get_lrms_type(*result);
    } catch ( glite::jdl::CannotGetAttribute const& e) {
      requestad::set_lrms_type(*result, type);
    }
   
    requestad::set_ce_id(*result, ce_id);

    // TODO catch requestad::CannotSetAttribute
  } else {
    throw glite::wms::helper::InvalidAttributeValue(
      requestad::JDL::SUBMIT_TO,
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

std::auto_ptr<classad::ClassAd>
f_resolve_mm(classad::ClassAd const& input_ad)
try {
  std::auto_ptr<classad::ClassAd> result;

  std::string vo(requestad::get_virtual_organisation(input_ad));

  glite::wms::broker::ResourceBroker rb;
  
  bool input_data_exists = false;
  bool data_requirements_exists = false;

  std::vector<std::string> input_data;
  requestad::get_input_data(input_ad, input_data, input_data_exists);
  requestad::get_data_requirements(input_ad, data_requirements_exists);
  
  if (input_data_exists || data_requirements_exists) 
  {
    rb.changeStrategy( maximize_files() );
  } 

  // If fuzzy_rank is true in the request ad we have
  // to use the stochastic selector...
  bool use_fuzzy_rank = false;
  if (requestad::get_fuzzy_rank(input_ad, use_fuzzy_rank) && use_fuzzy_rank) 
  {
    double factor = 1;
    try {
      factor = requestad::get_fuzzy_factor(input_ad);
    } 
    catch(...)
    {
    }
    rb.changeSelector(stochastic_selector(factor));
  }

  boost::tuple<
    boost::shared_ptr<matchtable>,
    boost::shared_ptr<filemapping>,
    boost::shared_ptr<storagemapping>
  > brokering_result(
    rb.findSuitableCEs(&input_ad)
  );
  boost::shared_ptr<matchtable>& suitable_CEs(
    boost::tuples::get<0>(brokering_result)
  );
  if (suitable_CEs->empty()) {
    throw NoCompatibleCEs();
  }

  configuration::Configuration const* const config
    = configuration::Configuration::instance();
  assert(config);

#ifndef GLITE_WMS_DONT_HAVE_GPBOX
  std::string dg_jobid_str(requestad::get_edg_jobid(input_ad));
  jobid::JobId dg_jobid(dg_jobid_str);

  std::string PBOX_host_name(config->wm()->pbox_host_name());

  if (!PBOX_host_name.empty()) {
    if (!gpbox::interact(
      *config,
      dg_jobid,
      PBOX_host_name,
      *suitable_CEs
    ))
      Info("Error during gpbox interaction");
  }

  if (suitable_CEs->empty()) {
    Info("Empty CE list after gpbox screening");
    throw NoCompatibleCEs();
  }
#endif

  matchtable::const_iterator ce_it = rb.selectBestCE(*suitable_CEs);
  classad::ClassAd const& ceAd = *boost::tuples::get<Ad>(*ce_it);
  std::string const ce_id(
    utils::evaluate_attribute(ceAd,"GlueCEUniqueID")
  );

  // Add the .Brokerinfo files to the InputSandbox
  bool input_sandbox_exists = false;
  bool wmpinput_sandbox_base_uri_exists = false;
  
  fs::path brokerinfo_path(
    requestad::get_input_sandbox_path(input_ad) + "/.BrokerInfo",
    fs::native
  );

  std::vector<std::string> ISB;
   
  requestad::get_input_sandbox(input_ad, ISB, input_sandbox_exists);
  std::string WMPInputSandboxBaseURI(requestad::get_wmpinput_sandbox_base_uri(
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
    throw CannotCreateBrokerinfo(brokerinfo_path);
  }

  boost::scoped_ptr<classad::ClassAd> biAd(
    make_brokerinfo_ad(
      boost::tuples::get<1>(brokering_result),
      boost::tuples::get<2>(brokering_result),
      ceAd
    )
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

  requestad::set_input_sandbox(*result, ISB);

  requestad::set_ce_id(*result, ce_id);
  matchinfo const& ce_info = *ce_it;
  classad::ClassAd const* ce_ad = boost::tuples::get<Ad>(ce_info).get();

  try {
    std::string flatten_result = flatten_requirements(
                                   *config, 
                                   &input_ad, 
                                    ce_ad
                                 );
    // Set attribute only if it's not empty, so as not to upset 
    // condor_submit.
    if (!flatten_result.empty()) {
      requestad::set_remote_remote_ce_requirements(
        *result,
         flatten_result
      );
    }
  } catch (...) {
    // Let's leave remote_remote_requirements undefined if
    // anything went wrong.
  }

  try {

    requestad::set_globus_resource_contact_string(
      *result,
      utils::evaluate_attribute(*ce_ad, "GlobusResourceContactString")
    );
    requestad::set_queue_name(
      *result,
      utils::evaluate_attribute(*ce_ad, "QueueName")
    );
    requestad::set_lrms_type(
      *result,
      utils::evaluate_attribute(*ce_ad, "LRMSType")
    );
    requestad::set_ce_id(
      *result,
      utils::evaluate_attribute(*ce_ad, "CEid")
    );

  } catch (utils::InvalidValue const& e) {

    Error(e.what() << " for CE id " << ce_id);

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
  
} catch (requestad::CannotGetAttribute const& e) {

  throw glite::wms::helper::CannotGetAttribute(e, helper_id);

} catch (requestad::CannotSetAttribute const& e) {

  throw glite::wms::helper::CannotSetAttribute(e, helper_id);

} catch( jobid::JobIdException& jide ) {

  Error( jide.what() );
  throw glite::wms::helper::InvalidAttributeValue(requestad::JDL::JOBID,
                                      "unknown",
                                      "valid jobid",
                                      helper_id);

} catch( fs::filesystem_error& fse ) {

    Error( fse.what() );
    throw glite::wms::helper::FileSystemError(helper_id, fse);

}

} // {anonymous}

std::string
Helper::id() const
{
  return helper_id;
}

std::string
Helper::output_file_suffix() const
{
  return f_output_file_suffix;
}

classad::ClassAd*
Helper::resolve(classad::ClassAd const* input_ad) const
{
  bool submit_to_exists = false;
  std::string ce_id = requestad::get_submit_to(*input_ad, submit_to_exists);

  std::auto_ptr<classad::ClassAd> result
    = submit_to_exists ? f_resolve_simple(*input_ad, ce_id) 
                       : f_resolve_mm(*input_ad);

  return result.release();
}

}}}}

