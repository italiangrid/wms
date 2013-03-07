// File: Helper.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>

// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

// $Id: Helper_ism.cpp,v 1.22.2.19.2.2.2.6.2.6.4.1.2.1.2.2 2012/09/12 10:02:12 mcecchi Exp $

#include <fstream>
#include <stdexcept>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/function_output_iterator.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include <boost/timer.hpp>

#include <classad_distribution.h>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/logger/logger_utils.h"

#include "glite/wmsutils/classads/classad_utils.h"

#include "glite/wms/helper/HelperFactory.h"
#include "glite/wms/helper/exceptions.h"

#include <glite/wms/ism/ism.h>

#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"

#include "matchmaker.h"
#include "exceptions.h"
#include "mm_exceptions.h"
#include "Helper.h"
#include "RBSimpleISMImpl.h"
#include "RBMaximizeFilesISMImpl.h"
#include "brokerinfo.h"
#include "classad_plugin_loader.h"

namespace ba            = boost::algorithm;
namespace fs            = boost::filesystem;
namespace logger        = glite::wms::common::logger;
namespace configuration = glite::wms::common::configuration;
namespace requestad     = glite::jdl;
namespace utils         = glite::wmsutils::classads;
namespace matchmaking   = glite::wms::matchmaking;

#define edglog(level) logger::threadsafe::edglog << logger::setlevel(logger::level)
#define edglog_fn(name) logger::StatePusher    pusher(logger::threadsafe::edglog, #name);

namespace glite {
namespace wms {
namespace helper {
namespace broker {

namespace {

std::string const GlueCEInfoHostName = "GlueCEInfoHostName";
std::string const GlueCEInfoApplicationDir = "GlueCEInfoApplicationDir";
std::string const DataAccessProtocol = "DataAccessProtocol";
std::string const GlueCEUniqueID = "GlueCEUniqueID";

std::string const helper_id("BrokerHelper");

//FIXME: should be moved back to ckassad_plugin if/when only this DL is used
//FIXME: for matchmaking.
glite::wms::classad_plugin::classad_plugin_loader init;

helper::HelperImpl* create_helper()
{
  return new Helper;
}

struct Register
{
  Register()
  {
    helper::HelperFactory::instance()->register_helper(helper_id, create_helper);
  }
  ~Register()
  {
    helper::HelperFactory::instance()->unregister_helper(helper_id);
  }
};

Register const r;

std::string const f_output_file_suffix(".rbh");

std::string
unparse_expression_value(
classad::ClassAd const& ad, std::string const& expression
)
{
  std::string result;
  classad::Value v;
  ad.EvaluateExpr(expression, v);
  classad::ClassAdUnParser unparser;
  unparser.Unparse(result, v);
  return result;
}

struct string_appender
{
    string_appender(std::string& s, const std::string& sep) : m_str(&s), m_sep(sep) {}
    void operator()(const std::string& x) const {
        if(!x.empty()) *m_str += x + m_sep;
    }
    std::string* m_str;
    const std::string& m_sep;
};

std::string translate_to_RSL(classad::ClassAd &ctx, classad::ExprTree* e)
{
  ctx.Insert("ad", e);
  boost::shared_ptr<void> ctx_guard_ad (
    static_cast<void*>(0), 
    boost::bind(&classad::ClassAd::Remove, boost::ref(ctx), "ad")
  );

  classad::Value v;
  bool predicate = false;
  if ( (ctx.EvaluateExpr("ad.value", v) && v.IsUndefinedValue()) || 
    (ctx.EvaluateExpr("ad.requires", v) && !v.IsUndefinedValue() &&
    v.IsBooleanValue(predicate) && !predicate) ) {
    return std::string();
  }
  return 
    ba::trim_copy_if(
     unparse_expression_value(ctx, "ad.name"),ba::is_any_of("\"")
    ) + "=" + 
    ba::trim_copy_if(
     unparse_expression_value(ctx, "ad.value"),ba::is_any_of("\"")
    );
}

std::string translate_to_CREAM(classad::ClassAd &ctx, classad::ExprTree* e)
{
  ctx.Insert("ad", e);
  boost::shared_ptr<void> ctx_guard_ad (
    static_cast<void*>(0),
    boost::bind(&classad::ClassAd::Remove, boost::ref(ctx), "ad")
  );

  classad::Value v;
  bool predicate = false;
  if ( (ctx.EvaluateExpr("ad.value", v) && v.IsUndefinedValue()) || 
    (ctx.EvaluateExpr("ad.requires", v) && !v.IsUndefinedValue() &&
    v.IsBooleanValue(predicate) && !predicate) ) {
    return std::string();
  }
  return 
    ba::trim_copy_if(
     unparse_expression_value(ctx, "ad.name"),ba::is_any_of("\"")
    ) + "==" + unparse_expression_value(ctx, "ad.value");
}

std::auto_ptr<classad::ClassAd>
f_resolve_simple(classad::ClassAd const& input_ad, std::string const& ce_id)
{
  std::auto_ptr<classad::ClassAd> result;
  static boost::regex  expression( "(([^\\:]+):[0-9]+/[^\\-]+-([^\\-]+))-(.+)" );
  boost::smatch        pieces;
  std::string gcrs, host, type, name;
  if (boost::regex_match(ce_id, pieces, expression)) {
    gcrs.assign(pieces[1].first, pieces[1].second);
    host.assign(pieces[2].first, pieces[2].second);
    type.assign(pieces[3].first, pieces[3].second);
    name.assign(pieces[4].first, pieces[4].second);

    result.reset(new classad::ClassAd(input_ad));
    requestad::set_globus_resource_contact_string(*result, gcrs);
    requestad::set_queue_name(*result, name);
    try {
      std::string junk = requestad::get_lrms_type(*result);
    } catch (glite::jdl::CannotGetAttribute const& e) {
      requestad::set_lrms_type(*result, type);
    }
   
    requestad::set_ce_id(*result, ce_id);
    requestad::set_ceinfo_host_name(*result, host);
  } else {
    throw helper::InvalidAttributeValue(requestad::JDL::SUBMIT_TO,
                                        ce_id,
                                        "match (.+/[^\\-]+-([^\\-]+))-(.+)",
                                        helper_id);
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
  bool data_requirements_exist = false;

  std::vector<std::string> input_data;

  requestad::get_input_data(input_ad, input_data, input_data_exists);
  requestad::get_data_requirements(input_ad, data_requirements_exist);

  if (input_data_exists  || data_requirements_exist) {
    rb.changeImplementation(
      boost::shared_ptr<glite::wms::broker::ResourceBroker::Impl>(
        new glite::wms::broker::RBMaximizeFilesISMImpl()
      )
    );
  } 

  // If fuzzy_rank is true in the request ad we have
  // to use the stochastic selector...
  bool use_fuzzy_rank = false;
  if (requestad::get_fuzzy_rank(input_ad, use_fuzzy_rank) && use_fuzzy_rank) {
    rb.changeSelector("stochasticRankSelector");
    bool change_fuzzy_factor;
    double fuzzy_factor = requestad::get_fuzzy_factor(input_ad, change_fuzzy_factor);
    if (change_fuzzy_factor) {
      glite::wms::broker::RBSelectionSchema::FuzzyFactor = fuzzy_factor;
    }
  }
  boost::tuple<
    boost::shared_ptr<matchmaking::matchtable>,
    boost::shared_ptr<brokerinfo::FileMapping>,
    boost::shared_ptr<brokerinfo::StorageMapping>
  > brokering_result(
    rb.findSuitableCEs(&input_ad)
  );
  boost::shared_ptr<matchmaking::matchtable>& suitable_CEs(
    boost::tuples::get<0>(brokering_result)
  );
  if (suitable_CEs->empty()) {
    throw NoCompatibleCEs();
  }

  configuration::Configuration const* const config
    = configuration::Configuration::instance();
  assert(config);

  matchmaking::matchtable::const_iterator ce_it = rb.selectBestCE(*suitable_CEs);
 
  std::string const ce_id(
    utils::evaluate_attribute(*matchmaking::getAd(ce_it->second), GlueCEUniqueID)
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
  
  boost::shared_ptr<brokerinfo::FileMapping> fm = boost::tuples::get<1>(brokering_result);
  boost::shared_ptr<brokerinfo::StorageMapping> sm = boost::tuples::get<2>(brokering_result);
  
  boost::scoped_ptr<classad::ClassAd> biAd(
    brokerinfo::create_brokerinfo(
      input_ad,
      *matchmaking::getAd(ce_it->second),
      brokerinfo::DataInfo(fm,sm)
    )
  );
  classad::ExprTree const* DACexpr = input_ad.Lookup(DataAccessProtocol);
  if (DACexpr) {
    biAd->Insert(DataAccessProtocol, DACexpr->Copy());
  }
  BIfilestream << *biAd << std::endl;

  if (!BIfilestream) {
    throw CannotCreateBrokerinfo(brokerinfo_path);
  }

  result.reset(new classad::ClassAd(input_ad));

  requestad::set_input_sandbox(*result, ISB);

  requestad::set_ce_id(*result, ce_id);
  matchmaking::matchinfo const& ce_info = ce_it->second;
  classad::ClassAd const* ce_ad = matchmaking::getAd(ce_info).get();

  try {
    std::string flatten_result = flatten_requirements(
                                   *config, 
                                   &input_ad, 
                                    ce_ad
                                 );
    // Set attribute only if it's not empty, so as not to upset 
    // condor_submit.
    if (!flatten_result.empty()) {

      boost::regex const cream_ce_id(".+/cream-.+");
      bool const is_cream_ce = boost::regex_match(ce_id, cream_ce_id);

      if (is_cream_ce) {

        requestad::set_ce_requirements(
          *result,
          flatten_result
        );
      } else {
        requestad::set_remote_remote_ce_requirements(
          *result,
          flatten_result
        );
      }
    }
  } catch (...) {
    // Let's leave remote_remote_requirements undefined if
    // anything went wrong.
  }

  classad::ClassAd ctx;

  ctx.Insert("ce", matchmaking::getAd(ce_info).get());
  ctx.Insert("jdl", const_cast<classad::ClassAd*>(&input_ad));

  boost::shared_ptr<void> ctx_guard_ce (
    static_cast<void*>(0),
    boost::bind(&classad::ClassAd::Remove, boost::ref(ctx), "ce")
  );
  boost::shared_ptr<void> ctx_guard_jdl (
    static_cast<void*>(0),
    boost::bind(&classad::ClassAd::Remove, boost::ref(ctx), "jdl")
  );

  const configuration::WMConfiguration* WM_conf = config->wm();

  classad::ExprList* e = dynamic_cast<classad::ExprList*>(
    WM_conf->propagate_to_lrms()
  );
  std::vector<classad::ExprTree*> ads;
  if (e) {

    e->GetComponents(ads);
 
    boost::regex const cream_ce_id(".+/cream-.+");
    bool const is_cream_ce = boost::regex_match(ce_id, cream_ce_id);

    if (is_cream_ce) {

      std::string s;
      std::transform(
        ads.begin(), ads.end(),
        boost::make_function_output_iterator(string_appender(s,"&&")),
        boost::bind(translate_to_CREAM, ctx, _1)
      );
      std::string r;
      try {
        r = requestad::get_ce_requirements(*result);
      } catch (...) {}
      if (!r.empty()) r.append("&&");
      r.append(ba::trim_right_copy_if(s,ba::is_any_of("&")));
      requestad::set_ce_requirements(*result,r);
    }
    else {
      std::string s;
      std::transform(
        ads.begin(), ads.end(),
        boost::make_function_output_iterator(string_appender(s,",")),
        boost::bind(translate_to_RSL, ctx, _1)
      );
      result->InsertAttr("RSLArguments", ba::trim_right_copy_if(s,ba::is_any_of(",")));
    }
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
    requestad::set_ceinfo_host_name(
      *result,
      utils::evaluate_attribute(*ce_ad, GlueCEInfoHostName)
    );

  } catch (utils::InvalidValue const& e) {

    edglog(error) << e.what() << " for CE id " << ce_id << std::endl;

    throw helper::HelperError("BrokerHelper");

  }

  std::string attr;
  bool checkAttr =
    ce_ad->EvaluateAttrString(GlueCEInfoApplicationDir, attr);

  if( checkAttr ){
    bool check;
    requestad::set_ce_application_dir(
      *result,
      attr,
      check
    );
  }


  return result;

} catch (matchmaking::ISNoResultError const& e) {

  throw NoAvailableCEs(
    "The user is not authorized on any resource currently registered in "
    + e.host()
  );

} catch (matchmaking::InformationServiceError const& e) {

  throw NoAvailableCEs(e.what());
  
} catch (matchmaking::RankingError const& e) {
  
  throw NoAvailableCEs(e.what());
  
} catch (requestad::CannotGetAttribute const& e) {

  throw helper::CannotGetAttribute(e, helper_id);

} catch (requestad::CannotSetAttribute const& e) {

  throw helper::CannotSetAttribute(e, helper_id);

} catch( fs::filesystem_error& fse ) {

    edglog( error ) << fse.what() << std::endl;
    throw helper::FileSystemError(helper_id, fse.what());

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
Helper::resolve(
  classad::ClassAd const* input_ad,
  boost::shared_ptr<std::string> m_jw_template
) const
{
  bool submit_to_exists = false;
  std::string ce_id = requestad::get_submit_to(*input_ad, submit_to_exists);

  std::auto_ptr<classad::ClassAd> result
    = submit_to_exists ? f_resolve_simple(*input_ad, ce_id) 
                       : f_resolve_mm(*input_ad);

  return result.release();
}

}}}} // glite::wms::helper::broker

