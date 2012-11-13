// File: submission_utils.cpp
// Author: Francesco Giacomini
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

// $Id: submission_utils.cpp,v 1.1.2.1.2.4.2.1.2.1.2.3.2.2 2011/12/01 11:51:44 mcecchi Exp $

#include "submission_utils.h"
#include <classad_distribution.h>
#include "lb_utils.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/jobid/JobId.h"
#include "glite/wms/common/utilities/manipulation.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"

namespace fs = boost::filesystem;
namespace configuration = glite::wms::common::configuration;
namespace jobid = glite::jobid;
namespace jdl = glite::jdl;
namespace utilities = glite::wms::common::utilities;

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {

int get_max_shallow_count()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );

  return config.wm()->max_shallow_retry_count();
}

int get_max_retry_count()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );

  return config.wm()->max_retry_count();
}

std::string get_token_file()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );

  return config.wm()->token_file();
}

fs::path sandbox_dir()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  std::string const path_str = config.ns()->sandbox_staging_path();

  return fs::path(path_str, fs::native);
}

}

job_type_t job_type(classad::ClassAd const& jdl)
{
  std::string type;
  classad::ClassAd* nodes;
  classad::ExprList* dependencies;
  if (jdl.EvaluateAttrString("Type", type) &&
    type == "dag" &&
    jdl.EvaluateAttrClassAd("Nodes", nodes) &&
    nodes->EvaluateAttrList("dependencies", dependencies)
  ) {
    if (dependencies->size()) {
      return dag;
    };
    return collection;
  }
  return single;
}

fs::path get_cancel_token(jobid::JobId const& id)
{
  fs::path token_for_cancel(sandbox_dir());
  token_for_cancel /= fs::path(utilities::get_reduced_part(id), fs::native);
  token_for_cancel /= fs::path(utilities::to_filename(id), fs::native);
  token_for_cancel /= fs::path("wm_cancel_token", fs::native);
  return token_for_cancel;
}

fs::path get_reallyrunning_token(jobid::JobId const& id, int replans_count)
{
  fs::path result(sandbox_dir());
  result /= fs::path(utilities::get_reduced_part(id), fs::native);
  result /= fs::path(utilities::to_filename(id), fs::native);
  result /= fs::path(
    get_token_file() + '_' + boost::lexical_cast<std::string>(replans_count)
    , fs::native
  );

  return result;
}

// in practice the actual retry limit for the deep count is
// max(0, min(job_retry_count, max_retry_count))
// and similarly for the shallow count
// max(0, min(job_shallow_count, max_shallow_count))

void check_shallow_count(classad::ClassAd const& jdl, int count)
{
  // check against the job max shallow count
  bool valid = false;
  int job_shallow_count(
    jdl::get_shallow_retry_count(jdl, valid)
  );
  if (!valid || job_shallow_count < 0) {
    job_shallow_count = 0;
  }
  if (count >= job_shallow_count) {
    throw HitJobShallowCount(job_shallow_count);
  }

  // check against the system max shallow count
  int max_shallow_count = get_max_shallow_count();
  if (max_shallow_count < 0) {
    max_shallow_count = 0;
  }
  if (count >= max_shallow_count) {
    throw HitMaxShallowCount(max_shallow_count);
  }
}

void check_deep_count(classad::ClassAd const& jdl, int count)
{
  // check against the job max retry count
  bool valid = false;
  int job_retry_count(jdl::get_retry_count(jdl, valid));
  if (!valid || job_retry_count < 0) {
    job_retry_count = 0;
  }
  if (count >= job_retry_count) {
    throw HitJobRetryCount(job_retry_count);
  }

  // check against the system max retry count
  int max_retry_count = get_max_retry_count();
  if (count >= max_retry_count) {
    throw HitMaxRetryCount(max_retry_count);
  }
}

int get_job_shallow_count(classad::ClassAd const& jdl)
{
  bool valid = false;
  int result = jdl::get_shallow_retry_count(jdl, valid);
  if (!valid) {
    result = 0;
  }
  return result;
}

int match_retry_period()
{
  return configuration::Configuration::instance()->wm()->match_retry_period();
}

bool shallow_resubmission_is_enabled(classad::ClassAd const& jdl)
{
  return get_job_shallow_count(jdl) != -1;
}

void create_token(fs::path const& p)
{
  fs::ofstream _(p);
  if (!_) {
    throw CannotCreateToken(p.native_file_string(), errno);
  } else {
    Debug("created token " << p.native_file_string());
  }
}

boost::shared_ptr<X509> read_proxy(std::string const& proxy_file)
{
  std::FILE* rfd = std::fopen(proxy_file.c_str(), "r");
  if (!rfd) {
    throw MissingProxy(proxy_file);
  }
  boost::shared_ptr<std::FILE> fd(rfd, std::fclose);
  ::X509* rcert = ::PEM_read_X509(rfd, 0, 0, 0);
  if (!rcert) {
    throw InvalidProxy(proxy_file);
  }
  boost::shared_ptr<X509> cert(rcert, ::X509_free);
  return cert;
}

bool is_proxy_expired(jobid::JobId const& id)
{
  std::string proxy_file = get_user_x509_proxy(id);
  return X509_cmp_current_time(X509_get_notAfter(read_proxy(proxy_file))) <= 0;
}

bool is_proxy_expired(boost::shared_ptr<X509>& cert, jobid::JobId const& id)
{
  if (cert) {
    bool expired = X509_cmp_current_time(X509_get_notAfter(cert)) <= 0;
    if (expired) {
      std::string proxy_file = get_user_x509_proxy(id);
      cert = read_proxy(proxy_file);
      return X509_cmp_current_time(X509_get_notAfter(cert)) <= 0;
    } else {
      return false;
    }
  } else {
    std::string proxy_file = get_user_x509_proxy(id);
    cert = read_proxy(proxy_file);
    return X509_cmp_current_time(X509_get_notAfter(cert)) <= 0;
  }
}

}}}} // glite::wms::manager::server
