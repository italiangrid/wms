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

#include <fnCall.h>
#include <classad_distribution.h>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "glite/wms/ism/ism.h"
#include "glite/lb/statistics.h"
#include "glite/lb/context.h"
#include "glite/lb/consumer.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMPConfiguration.h"

namespace ism = glite::wms::ism;
#ifdef WANT_NAMESPACES
using namespace classad;
#endif

namespace glite {
namespace wms {
namespace classad_plugin {

namespace {
enum stats_t {
  avg = 0,
  std_dev
};
}

double lb_statistics(std::string const& queue_id, stats_t mode, int seconds)
{
  std::string lbserver_fqdn(
    glite::wms::common::configuration::Configuration::instance()->wp()
      ->lbserver().front()
  );
  std::string lbserver_host;
  int lbserver_port = 9000;
  static boost::regex const regex("(.*):([[:digit:]]*).*");
  boost::smatch pieces;
  if (boost::regex_match(lbserver_fqdn, pieces, regex)) {
    lbserver_host.assign(pieces[1].first, pieces[1].second);
    lbserver_port = boost::lexical_cast<int>(
      std::string().assign(pieces[2].first, pieces[2].second)
    );
  }
  std::string lbserver;
  if (lbserver_host.empty()) {
    lbserver = lbserver_fqdn;
  } else {
    lbserver = lbserver_host;
  }

  edg_wll_Context ctx;
  edg_wll_InitContext(&ctx);
  edg_wll_SetParam(
    ctx,
    EDG_WLL_PARAM_QUERY_SERVER,
    lbserver.c_str()
  );
  edg_wll_SetParam(ctx, EDG_WLL_PARAM_QUERY_SERVER_PORT, lbserver_port);

  edg_wll_QueryRec group[2];
  group[0].attr = EDG_WLL_QUERY_ATTR_DESTINATION;
  group[0].op = EDG_WLL_QUERY_OP_EQUAL;
  group[0].value.c = const_cast<char*>(queue_id.c_str());
    
  group[1].attr = EDG_WLL_QUERY_ATTR_UNDEF;
    
  time_t now(::time(0));
  time_t to(now);
  time_t from(now - seconds);
  float* durations = 0;
  float* dispersions = 0;
  char** groups = 0;
  int from_res, to_res;
  if (!edg_wll_StateDurationsFromTo(
      ctx,
      group,
      EDG_WLL_JOB_SUBMITTED, // TODO transition SCHEDULED -> RUNNING not implemented yet, lb server side
      EDG_WLL_JOB_RUNNING,
      0,
      &from,
      &to,
      &durations,
      &dispersions,
      &groups,
      &from_res,
      &to_res)
  ) {
    if (mode == avg) {
      return durations[0];
    } else {
      return dispersions[0];
    }
  }
}

bool measured_response_time(
  const char* name,
  ArgumentList const& arguments,
  EvalState &state,
  Value& result)
{

  result.SetErrorValue();
  std::string queue_id;
  int mode = 0, seconds = 0;
  // we check to make sure that we are passed at least one argument
  if (arguments.size() <= 1) {
    return false;
  } else {
    Value arg[3];
    if (!(arguments[0]->Evaluate(state, arg[0]) && arg[0].IsStringValue(queue_id))) {
      return false;
    }
    if (!(arguments[1]->Evaluate(state, arg[1]) && arg[1].IsIntegerValue(mode))) {
      return false;
    }
    if (!(arguments[2]->Evaluate(state, arg[2]) && arg[2].IsIntegerValue(seconds))) {
      return false;
    }
  }

  if (mode) {
    result.SetRealValue(lb_statistics(queue_id, avg, seconds));
  } else {
    result.SetRealValue(lb_statistics(queue_id, std_dev, seconds));
  }

  return true;
}

}}}
