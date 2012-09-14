// File: Helper_matcher_ism.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <fstream>
#include <stdexcept>
#include <boost/scoped_ptr.hpp>
#include <boost/timer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/regex.hpp>

#include <classad_distribution.h>

#include "Helper_matcher.h"
#include "Helper_exceptions.h"
#include "storage_utils.h"
#include "exceptions.h"
#include "matchmaking.h"

#include "glite/wms/helper/exceptions.h"
#include "glite/wms/helper/HelperFactory.h"

#include "simple_strategy.h"
#include "maximize_files_strategy.h"

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"

#include "glite/wms/common/logger/logger_utils.h"

#include "glite/wmsutils/classads/classad_utils.h"

#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/PrivateAttributes.h"
#include "glite/jdl/ManipulationExceptions.h"

#ifndef GLITE_WMS_DONT_HAVE_GPBOX
#include "gpbox_utils.h"
#endif

namespace fs            = boost::filesystem;
namespace jobid         = glite::wmsutils::jobid;
namespace logger        = glite::wms::common::logger;
namespace configuration = glite::wms::common::configuration;
namespace requestad     = glite::jdl;
namespace utils		= glite::wmsutils::classads;
namespace wmshlp	= glite::wms::helper;
namespace glite {
namespace wms {
namespace broker {
namespace helper {
namespace matcher {

namespace {

std::string const helper_id("MatcherHelper");

wmshlp::HelperImpl* create_helper()
{
  return new Helper;
}

struct Register
{
  Register()
  {
    wmshlp::HelperFactory::instance()->register_helper(helper_id, create_helper);
  }
  ~Register()
  {
    wmshlp::HelperFactory::instance()->unregister_helper(helper_id);
  }
};

Register const r;

std::string const f_output_file_suffix(".rbh");

/* Answer format:
 * 1) if no error occurs
 * [
 *   reason =ok;
 *   match_result={
 *                  [
 *                    ce_id=...;
 *                    rank=...;
 *                    brokerinfo=...;
 *                  ],
 *                  [...],
 *                  ...
 *                }
 * ]
 * 2) if error occurs
 * [
 *   reason = error reason;
 *   match_result={}
 * ]
 */

std::auto_ptr<classad::ClassAd>
f_resolve_do_match(classad::ClassAd const& input_ad)
{
  std::auto_ptr<classad::ClassAd> result;
  result.reset(new classad::ClassAd);

  result->InsertAttr("reason", std::string("no matching resources found"));
  result->Insert("match_result", new classad::ExprList);
    
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

  boost::tuple<
    boost::shared_ptr<matchtable>,
    boost::shared_ptr<filemapping>,
    boost::shared_ptr<storagemapping>
  > brokering_result;
  std::string mm_error;
  
  try {
    brokering_result = rb.findSuitableCEs(&input_ad);
    boost::shared_ptr<matchtable>& suitableCEs(
      boost::tuples::get<0>(brokering_result)
    );
    std::string vo(requestad::get_virtual_organisation(input_ad));    
    std::vector<classad::ExprTree*> hosts;
    bool include_brokerinfo = false;
    input_ad.EvaluateAttrBool("include_brokerinfo", include_brokerinfo);
    int number_of_results = -1;
    input_ad.EvaluateAttrInt("number_of_results", number_of_results);

#ifndef GLITE_WMS_DONT_HAVE_GPBOX
    std::string x509_user_proxy_file_name(requestad::get_x509_user_proxy(input_ad)); 
    if (!suitableCEs->empty()) { 
      configuration::Configuration const* const config(
        configuration::Configuration::instance()
      );
      assert(config);
 
      std::string PBOX_host_name(config->wm()->pbox_host_name());
      if (!PBOX_host_name.empty())
      {
        if (!glite::wms::broker::gpbox::interact(
          *config,
          x509_user_proxy_file_name,
          PBOX_host_name,
          *suitableCEs
        ))
          Debug("Error during gpbox interaction");
      }
    }
#endif

    if (!suitableCEs->empty() ) {

      std::stable_sort(
        suitableCEs->begin(),
        suitableCEs->end(),
        rank_greater_than_comparator()
      );

      matchtable::const_iterator it(
        suitableCEs->begin()
      );
      matchtable::const_iterator const end(
        suitableCEs->end()
      );
      for (int i = 0;
           it != end && (number_of_results == -1 || i < number_of_results);
           ++it, ++i) {

        std::auto_ptr<classad::ClassAd> ceinfo(new classad::ClassAd);
        ceinfo->InsertAttr("ce_id", boost::tuples::get<Id>(*it));
        ceinfo->InsertAttr("rank", boost::tuples::get<Rank>(*it));
        
        if (include_brokerinfo) {
           std::string const ce_id(
             utils::evaluate_attribute(
               *boost::tuples::get<Ad>(*it),
               "GlueCEUniqueID"
             )
           );
           ceinfo->Insert(
             "brokerinfo",  
             make_brokerinfo_ad(
               boost::tuples::get<1>(brokering_result),
               boost::tuples::get<2>(brokering_result),
               *boost::tuples::get<Ad>(*it)
             )
          );
        }
        hosts.push_back(ceinfo.get());
        ceinfo.release();
      }
      
      result->InsertAttr("reason", std::string("ok"));    
      result->Insert("match_result", classad::ExprList::MakeExprList(hosts));           
    }
#ifndef GLITE_WMS_DONT_HAVE_GPBOX
    else {
      Info("Empty CE list after G-Pbox screening");
    }
#endif
  } catch (ISNoResultError const& e) {
    result->InsertAttr(
      "reason",
      "The user is not authorized on any resource currently registered in "
      + e.host()
    );

  } catch (InformationServiceError const& e) {
    result->InsertAttr("reason", std::string(e.what()));  

  } catch (RankingError const& e) {
    result->InsertAttr("reason", std::string(e.what()));  
 
  } catch (requestad::CannotGetAttribute const& e) {
    result->InsertAttr(
      "reason", 
      "Attribute " + e.parameter() + " does not exist or has the wrong type"
    ); 

  } catch (requestad::CannotSetAttribute const& e) {
    result->InsertAttr("reason", "Cannot set attribute " + e.parameter());

  } catch (jobid::JobIdException const& e) {
    result->InsertAttr("reason", std::string(e.what()));  

  } catch (boost::filesystem::filesystem_error const& e) {
    result->InsertAttr("reason", std::string(e.what()));  
  }
  return result;
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
  std::auto_ptr<classad::ClassAd> result = f_resolve_do_match(*input_ad);

  return result.release();
}

}}}}} // glite::wms::helper::matcher
