// Author: Francesco Giacomini
// Author: Salvatore Monforte 

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

// $Id: Helper_matcher_ism.cpp,v 1.2.2.7.2.2.6.2 2012/09/12 10:02:12 mcecchi Exp $

#include <fstream>
#include <stdexcept>
#include <boost/scoped_ptr.hpp>
#include <boost/timer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/regex.hpp>

#include <classad_distribution.h>

#include "glite/wms/helper/exceptions.h"
#include "glite/wms/helper/HelperFactory.h"

#include "exceptions.h"
#include "brokerinfo.h"
#include "mm_utility.h"
#include "mm_exceptions.h"
#include "matchmaker.h"
#include "RBSimpleISMImpl.h"
#include "RBMaximizeFilesISMImpl.h"
#include "Helper_matcher_ism.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/logger/logger_utils.h"

#include "glite/wmsutils/classads/classad_utils.h"

#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/PrivateAttributes.h"
#include "glite/jdl/ManipulationExceptions.h"
#include "glite/jdl/JobAdManipulation.h"

namespace fs            = boost::filesystem;
namespace logger        = glite::wms::common::logger;
namespace configuration = glite::wms::common::configuration;
namespace requestad = glite::jdl;
namespace utils = glite::wmsutils::classads;
namespace matchmaking = glite::wms::matchmaking;

namespace glite {
namespace wms {
namespace helper {
namespace matcher {

namespace {

std::string const helper_id("MatcherHelper");

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

  boost::tuple<
    boost::shared_ptr<matchmaking::matchtable>,
    boost::shared_ptr<brokerinfo::FileMapping>,
    boost::shared_ptr<brokerinfo::StorageMapping>
  > brokering_result;
  std::string mm_error;

  try {
    brokering_result = rb.findSuitableCEs(&input_ad);
    boost::shared_ptr<matchmaking::matchtable>& suitableCEs(
      boost::tuples::get<0>(brokering_result)
    );
    std::string vo(requestad::get_virtual_organisation(input_ad));    
    std::vector<classad::ExprTree*> hosts;
    bool include_brokerinfo = false;
    input_ad.EvaluateAttrBool("include_brokerinfo", include_brokerinfo);
    int number_of_results = -1;
    input_ad.EvaluateAttrInt("number_of_results", number_of_results);

    if (!suitableCEs->empty() ) {
      matchmaking::matchvector suitableCEs_vector(
        suitableCEs->begin(),
        suitableCEs->end()
      );

      std::stable_sort(
        suitableCEs_vector.begin(),
        suitableCEs_vector.end(),
        matchmaking::rank_greater_than_comparator()
      );

      matchmaking::matchvector::const_iterator it(
        suitableCEs_vector.begin()
      );
      matchmaking::matchvector::const_iterator const end(
        suitableCEs_vector.end()
      );
      for (int i = 0;
           it != end && (number_of_results == -1 || i < number_of_results);
           ++it, ++i) {

        std::auto_ptr<classad::ClassAd> ceinfo(new classad::ClassAd);
        string const ce_id(
          utils::evaluate_attribute(*matchmaking::getAd(it->second), "GlueCEUniqueID")
        );
        ceinfo->InsertAttr("ce_id", ce_id);
        ceinfo->InsertAttr("rank", matchmaking::getRank(it->second));

        if (include_brokerinfo) {
           std::string const ce_id(
             utils::evaluate_attribute(
               *matchmaking::getAd(it->second),
               "GlueCEUniqueID"
             )
           );
           ceinfo->Insert(
             "brokerinfo",
             brokerinfo::create_brokerinfo(
               input_ad,
               *matchmaking::getAd(it->second),
               brokerinfo::DataInfo(
                 boost::tuples::get<1>(brokering_result),
                 boost::tuples::get<2>(brokering_result)
               )  
             )
          );
        }
        hosts.push_back(ceinfo.get());
        ceinfo.release();
      }

      result->InsertAttr("reason", std::string("ok"));
      result->Insert("match_result", classad::ExprList::MakeExprList(hosts));
    }





  } catch (matchmaking::ISNoResultError const& e) {
    result->InsertAttr(
      "reason",
      "The user is not authorized on any resource currently registered in "
      + e.host()
    );

  } catch (matchmaking::InformationServiceError const& e) {
    result->InsertAttr("reason", std::string(e.what()));

  } catch (matchmaking::RankingError const& e) {
    result->InsertAttr("reason", std::string(e.what()));

  } catch (requestad::CannotGetAttribute const& e) {
    result->InsertAttr(
      "reason",
      "Attribute " + e.parameter() + " does not exist or has the wrong type"
    );

  } catch (requestad::CannotSetAttribute const& e) {
    result->InsertAttr("reason", "Cannot set attribute " + e.parameter());

  } catch (boost::filesystem::filesystem_error const& e) {
    result->InsertAttr("reason", std::string(e.what()));
  }
  return result;
}

}

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
  boost::shared_ptr<std::string> jwt
) const
{
  std::auto_ptr<classad::ClassAd> result = f_resolve_do_match(*input_ad);

  return result.release();
}

}}}}
