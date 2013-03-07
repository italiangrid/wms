/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/*
 * File: listmatch.cpp
 * Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
 * Author: Cinzia Di Giusto <Cinzia.DiGiusto@cnaf.infn.it>
 * Author: Francesco Giacomini
 * Author: Francesco Prelz
 */

#include <iostream>
#include <fstream>

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

#include "listmatch.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/common/utilities/scope_guard.h"

#include "glite/wms/helper/Helper.h"
#include "glite/wms/helper/exceptions.h"

namespace utilities = glite::wms::common::utilities;
namespace ca = glite::wmsutils::classads;

namespace glite {
namespace wms {
namespace manager {
namespace server {

bool match(
  classad::ClassAd const& jdl,
  std::string const& result_file,
  int number_of_results,
  bool include_brokerinfo
)
{
  if (result_file.empty() || result_file[0] != '/') {
    return false;
  }

  std::auto_ptr<classad::ClassAd> result;

  try {

    classad::ClassAd ad(jdl);
    ad.InsertAttr("include_brokerinfo", include_brokerinfo);
    ad.InsertAttr("number_of_results", number_of_results);
    result.reset(glite::wms::helper::Helper("MatcherHelper").resolve(&ad));

    classad::ExprTree *et = result->Lookup ("match_result");
    assert(ca::is_expression_list(et));
    classad::ExprList& match_result = *static_cast<classad::ExprList*>(et);

    if (match_result.size() != 0) {
      std::vector<classad::ExprTree*> list;
      match_result.GetComponents(list);

      std::vector<classad::ExprTree*> newlist;
      if (number_of_results == -1) {
        number_of_results = match_result.size();
      }

      int const n_results = std::min(number_of_results, match_result.size());
      for (int i = 0; i < n_results; ++i) {
        assert(ca::is_classad(list[i]));
        std::auto_ptr<classad::ExprTree> element(list[i]->Copy());

        if (!include_brokerinfo) {

          //translate the answer in this format
          /* [
           *   reason = ok;
           *   match_result = {"host1,rank1", "host2,rank2", ...}
           * ]
           */
          classad::ClassAd* ca = static_cast<classad::ClassAd*>(element.get());
          std::string host = ca::evaluate_attribute(*ca, "ce_id");
          double rank = ca::evaluate_attribute(*ca, "rank");

          classad::Value value;
          value.SetStringValue(host + ',' + boost::lexical_cast<std::string>(rank));


          element.reset(classad::Literal::MakeLiteral(value));
        }
        newlist.push_back(element.release());
      }

      result->Insert("match_result", classad::ExprList::MakeExprList(newlist));
    }
  } catch (glite::wms::helper::HelperError const& e) {
    // We need to chime into the communication pipe in this case.
    result.reset(new classad::ClassAd);
    result->InsertAttr("reason", std::string("error accessing broker helper"));
    result->InsertAttr("match_result", new classad::ExprList);
  }

  classad::PrettyPrint result_unparser;
  std::string          result_string;
  result_unparser.Unparse(result_string, result.get());

  int const fd = open(result_file.c_str(), O_WRONLY | O_NONBLOCK);
  if (fd == -1) {
    return false;
  }
  utilities::scope_guard close_fd(boost::bind(::close, fd));

  int const max_retries = 7;
  int retry_count = 0;
  int backoff_time = 1;
  int const write_size = result_string.size();
  int written_size = 0;
  char const* const c_result_string = result_string.c_str();

  while (written_size != write_size) {
    int n_write(
      write(fd, c_result_string + written_size, write_size - written_size)
    );

    if (n_write > 0 && n_write <= (write_size - written_size)) {
      written_size += n_write;
      backoff_time = 1;
      retry_count = 0;
    } else if (n_write == 0) {
      sleep(backoff_time);
      backoff_time *= 2;
      ++retry_count;
    } else {
      if (errno == EAGAIN && retry_count < max_retries) {
        sleep(backoff_time);
        backoff_time *= 2;
        ++retry_count;
      } else {
        break;
      }
    }
  }

  return written_size == write_size;
}

}}}} // glite::wms::manager::server
