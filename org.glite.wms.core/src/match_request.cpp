// File: request.cpp
// Authors: Marco Cecchi
//          Francesco Giacomini
//          Salvatore Monforte
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

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/utilities/wm_commands.h"
#include "glite/wms/common/utilities/input_reader.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/helper/exceptions.h"

#include "match_request.h"
#include "listmatch.h"

namespace jdl = glite::jdl;
namespace utilities = glite::wms::common::utilities;
namespace configuration = glite::wms::common::configuration;
namespace fs = boost::filesystem;

namespace glite {
namespace wms {
namespace manager {
namespace server {

MatchProcessor::MatchProcessor(boost::shared_ptr<MatchState> state)
  : m_state(state)
{ }

void MatchProcessor::operator()()
{
  std::auto_ptr<classad::ClassAd> ad(
    utilities::match_command_remove_ad(*m_state->command_ad));
  std::string filename = utilities::match_command_get_file(
    *m_state->command_ad
  );
  int number = utilities::match_command_get_number_of_results(
    *m_state->command_ad
  );
  bool brokerinfo = utilities::match_command_get_include_brokerinfo(
    *m_state->command_ad
  );

  Debug(
    "considering match " << m_state->id.toString()
    << ' ' << filename
    << ' ' << number
    << ' ' << brokerinfo
  );

  try {
    if (!match(*ad, filename, number, brokerinfo)) {

      Info("Failed match for " << m_state->id.toString());
    }
  } catch (MatchError& e) {

    Error("Match error: " + std::string(e.what()));
  }
}

}}}}
